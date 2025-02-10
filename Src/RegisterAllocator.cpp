#include <RegisterAllocator.h>
#include <IRBuilder.h>

#include <ranges>

namespace ranges = std::ranges;
namespace views = ranges::views;


namespace VMIR {

bool RegisterAllocator::PerformRegisterAllocation() {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();
    LivenessAnalyzer* livenessAnalyzer = IrBuilder->GetOrCreateLivenessAnalyzer(mGraph);

    if (!livenessAnalyzer->IsAnalysisDone() && !livenessAnalyzer->PerformLivenessAnalysis()) {
        return false;
    }

    struct IncreasingStartComparator {
        bool operator() (Value* a, Value* b) const {
            return a->GetLiveInterval().start < b->GetLiveInterval().start;
        }
    };

    std::multiset<Value*, IncreasingStartComparator> increasingStartValues{};
    for (auto* bb : mGraph->GetBasicBlocks()) {
        auto* inst = bb->Front();
        while (inst) {
            Value* instOutput = inst->GetOutput();
            if (instOutput) {
                increasingStartValues.insert(instOutput);
            }
            inst = inst->GetNext();
        }
    }

    /*
    // TODO: Should I account for function argument values? Or their locations are assured by calling convention?
    const auto& funcArgs = mGraph->GetEntryBasicBlock()->GetParentFunction()->GetArgs();
    for (const auto* v : funcArgs) {
        increasingStartValues.insert(v);
    }
    */

    for (auto* v : increasingStartValues) {
        ExpireOldIntervals(v);
        if ((v->IsIntegralValueType() && AreAllGPRegistersOccupied()) || (v->IsFloatingPointValueType() && AreAllFPRegistersOccupied())) {
            SpillAtInterval(v);
        }
        else {
            if (v->IsIntegralValueType()) {
                std::optional<GPRegisterLocation> regLocation = GetFreeGPRegisterLocation();
                v->SetLocation(regLocation.value());
                SetGPRegisterOccupied(regLocation.value());
            }
            else {
                std::optional<FPRegisterLocation> regLocation = GetFreeFPRegisterLocation();
                v->SetLocation(regLocation.value());
                SetFPRegisterOccupied(regLocation.value());
            }
            mActiveValues.insert(v);
        }
    }

    return true;
}


void RegisterAllocator::ExpireOldIntervals(Value* valueIn) {
    auto it = mActiveValues.begin();
    auto end = mActiveValues.end();
    while (it != end) {
        Value* i = valueIn;
        Value* j = *it;

        // Skip values with diffrent type (GP/FP)
        if (j->IsIntegralValueType() != i->IsIntegralValueType()) {
            ++it;
            continue;
        }

        if (j->GetLiveInterval().end > i->GetLiveInterval().start) {
            return;
        }

        it = mActiveValues.erase(it);

        Location jLocation = j->GetLocation();
        if (i->IsIntegralValueType()) {
            GPRegisterLocation registerLocation = std::get<GPRegisterLocation>(jLocation);
            SetGPRegisterFree(registerLocation);
        }
        else {
            FPRegisterLocation registerLocation = std::get<FPRegisterLocation>(jLocation);
            SetFPRegisterFree(registerLocation);
        }
    }
}


void RegisterAllocator::SpillAtInterval(Value* valueIn) {
    // Find value in mActiveValues with the same type (GP/FP) and furthest end
    auto view = mActiveValues | views::reverse | views::filter([valueIn](auto&& v){ return v->IsIntegralValueType() == valueIn->IsIntegralValueType(); }) | views::take(0);
    Value* spill = *view.begin();
    if (spill->GetLiveInterval().end > valueIn->GetLiveInterval().end) {
        Location spillLoc = spill->GetLocation();
        valueIn->SetLocation(spillLoc);

        StackLocation newSpillLoc = GenerateNewStackLocation();
        spill->SetLocation(newSpillLoc);

        mActiveValues.erase(spill);
        mActiveValues.insert(valueIn);
    }
    else {
        StackLocation stackLocation = GenerateNewStackLocation();
        valueIn->SetLocation(stackLocation);
    }
}

}   // namespace VMIR
