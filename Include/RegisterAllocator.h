#ifndef REGISTER_ALLOCATOR_H
#define REGISTER_ALLOCATOR_H

#include <set>
#include <vector>
#include <algorithm>

#include <ControlFlowGraph.h>

namespace VMIR {

class RegisterAllocator {
public:
    RegisterAllocator() = delete;
    RegisterAllocator(ControlFlowGraph* graph, uint32_t GPRegCount, uint32_t FPRegCount) : mGraph{graph}, mGPRegisterCount{GPRegCount}, mFPRegisterCount{FPRegCount} {
        for (uint32_t i = 0; i < mGPRegisterCount; ++i) {
            mFreeGPRegisters.emplace(i);
        }

        for (uint32_t i = 0; i < mFPRegisterCount; ++i) {
            mFreeFPRegisters.emplace(i);
        }
    }

    bool PerformRegisterAllocation();

    inline uint32_t GetGPRegisterCount() const { return mGPRegisterCount; }
    inline uint32_t GetFPRegisterCount() const { return mFPRegisterCount; }

    inline ControlFlowGraph* GetGraph() const { return mGraph; }

private:
    void ExpireOldIntervals(Value* valueIn);
    void SpillAtInterval(Value* valueIn);

    inline bool IsGPRegisterOccupied(GPRegisterLocation reg) const { return mFreeGPRegisters.find(reg) == mFreeGPRegisters.cend(); }
    inline bool IsGPRegisterFree(GPRegisterLocation reg) const { return mFreeGPRegisters.find(reg) != mFreeGPRegisters.cend(); }

    inline void SetGPRegisterOccupied(GPRegisterLocation reg) { mFreeGPRegisters.erase(reg); }
    inline void SetGPRegisterFree(GPRegisterLocation reg) { mFreeGPRegisters.insert(reg); }

    inline bool AreAllGPRegistersOccupied() const { return mFreeGPRegisters.empty(); }

    inline bool IsFPRegisterOccupied(FPRegisterLocation reg) const { return mFreeFPRegisters.find(reg) == mFreeFPRegisters.cend(); }
    inline bool IsFPRegisterFree(FPRegisterLocation reg) const { return mFreeFPRegisters.find(reg) != mFreeFPRegisters.cend(); }

    inline void SetFPRegisterOccupied(FPRegisterLocation reg) { mFreeFPRegisters.erase(reg); }
    inline void SetFPRegisterFree(FPRegisterLocation reg) { mFreeFPRegisters.insert(reg); }

    inline bool AreAllFPRegistersOccupied() const { return mFreeFPRegisters.empty(); }

    inline std::optional<GPRegisterLocation> GetFreeGPRegisterLocation() {
        if (AreAllGPRegistersOccupied()) {
            return std::nullopt;
        }
        return *mFreeGPRegisters.begin();
    }

    inline std::optional<FPRegisterLocation> GetFreeFPRegisterLocation() {
        if (AreAllFPRegistersOccupied()) {
            return std::nullopt;
        }
        return *mFreeFPRegisters.begin();
    }
    
    inline StackLocation GenerateNewStackLocation() { return StackLocation(mStackLocations++); }

    ControlFlowGraph* mGraph{nullptr};
    uint32_t mGPRegisterCount{};
    uint32_t mFPRegisterCount{};

    // I want always use a register with the smallest ID possible
    struct IncreasingGPRegisterIdComparator {
        bool operator() (GPRegisterLocation a, GPRegisterLocation b) const {
            return a.registerId < b.registerId;
        }
    };
    std::set<GPRegisterLocation, IncreasingGPRegisterIdComparator> mFreeGPRegisters{};

    struct IncreasingFPRegisterIdComparator {
        bool operator() (FPRegisterLocation a, FPRegisterLocation b) const {
            return a.registerId < b.registerId;
        }
    };
    std::set<FPRegisterLocation, IncreasingFPRegisterIdComparator> mFreeFPRegisters{};

    struct IncreasingEndComparator {
        bool operator() (Value* a, Value* b) const {
            return a->GetLiveInterval().end < b->GetLiveInterval().end;
        }
    };
    std::multiset<Value*, IncreasingEndComparator> mActiveValues{};

    uint32_t mStackLocations{0};
};

}   // namespace VMIR

#endif  // REGISTER_ALLOCATOR_H
