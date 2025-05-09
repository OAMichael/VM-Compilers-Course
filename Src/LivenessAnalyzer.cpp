#include <LivenessAnalyzer.h>
#include <LoopAnalyzer.h>
#include <IRBuilder.h>


namespace VMIR {

bool LivenessAnalyzer::PerformLivenessAnalysis() {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();
    mLoopAnalyzer = IrBuilder->GetOrCreateLoopAnalyzer(mGraph);

    if (!mLoopAnalyzer->IsLoopTreeBuilt()) {
        mLoopAnalyzer->BuildLoopTree();
    }

    // Do not perform any analysis if there is at least 1 irreducible loop
    if (mLoopAnalyzer->HasIrreducibleLoops()) {
        return false;
    }

    CreateBasicBlocksLinearOrder(mGraph->GetEntryBasicBlock());
    AssignLinearAndLiveNumbers();
    CalculateLiveRanges();

    for (auto* bb : mBBLinearOrder) {
        bb->SetUnmarked();
    }

    mIsAnalysisDone = true;
    return true;
}


bool LivenessAnalyzer::CheckIfBlockCanBeVisited(BasicBlock* block) {
    if (block == nullptr) {
        return false;
    }

    Loop* loop = block->GetLoop();
    if (loop->GetHeader() == block) {
        // block is a loop header representing a loop
        // Check that all external predecessors are visited
        const auto& latches = loop->GetLatches();
        for (auto* bb : block->GetPredecessors()) {
            if (std::find(latches.cbegin(), latches.cend(), bb) != latches.cend()) {
                continue;
            }
            if (!bb->IsMarked(BasicBlock::Marker::Black)) {
                return false;
            }
        }
    }
    else {
        // block is not a loop header. So, it is regular basic block
        // Check that all predecessors are visited
        for (auto* bb : block->GetPredecessors()) {
            if (!bb->IsMarked(BasicBlock::Marker::Black)) {
                return false;
            }
        }
    }

    return true;
}


void LivenessAnalyzer::VisitLoopBlock(BasicBlock* bb, std::set<BasicBlock*>& exitBlocks) {
    if (bb == nullptr || bb->IsMarked(BasicBlock::Marker::Black)) {
        return;
    }

    mBBLinearOrder.push_back(bb);
    bb->SetMarked(BasicBlock::Marker::Black);

    BasicBlock* trueSucc = bb->GetTrueSuccessor();
    BasicBlock* falseSucc = bb->GetFalseSuccessor();

    auto VisitSuccessor = [this, bb, &exitBlocks](BasicBlock* succ) {
        if (succ == nullptr || succ->IsMarked(BasicBlock::Marker::Black)) {
            return;
        }

        Loop* bbLoop = bb->GetLoop();
        Loop* succLoop = succ->GetLoop();
        if (succLoop == bbLoop) {
            // Successor from the same loop
            if (succ->GetLoop()->GetHeader() != succ) {
                VisitLoopBlock(succ, exitBlocks);
            }
        }
        else {
            // Successor is exit block (or block from one of the outer loops or from one of inner loops, erase later)
            exitBlocks.insert(succ);
        }
    };

    // Try false successor first and then true one. Then false again, since it can be allowed to visit after true one was visited
    if (falseSucc != nullptr && !falseSucc->IsMarked(BasicBlock::Marker::Black) && CheckIfBlockCanBeVisited(falseSucc)) {
        VisitSuccessor(falseSucc);
    }
    if (trueSucc != nullptr && !trueSucc->IsMarked(BasicBlock::Marker::Black) && CheckIfBlockCanBeVisited(trueSucc)) {
        VisitSuccessor(trueSucc);
    }
    if (falseSucc != nullptr && !falseSucc->IsMarked(BasicBlock::Marker::Black) && CheckIfBlockCanBeVisited(falseSucc)) {
        VisitSuccessor(falseSucc);
    }
}


void LivenessAnalyzer::VisitLoop(Loop* loop, std::set<BasicBlock*>& exitBlocks) {
    auto* header = loop->GetHeader();
    if (header->IsMarked(BasicBlock::Marker::Black)) {
        return;
    }

    // Collect loop by acquiring all strongly connected components one by one
    // This will be the first one (for innermost loops is only one)
    VisitLoopBlock(header, exitBlocks);

    // By this moment exitBlocks contain several blocks each one is one of two types:
    // - Real exit blocks out of the loop
    // - Blocks which are in other loop but it's inside of the current loop (for loops containing inner loops)
    //
    // Real exit blocks are processed later. Now, we have to process inner loops

    // While processing inner loops, blocks from current loop can appear in exitBlocks container
    // So, we must visit them as well. But after that other inner loops can appear and so on

    // Note that for both cases there is no external edges since loop is reducible. Thus, the iterations will eventually stop
    bool noInnerLoopsLeft = false;
    bool noFakeExitBlocksLeft = false;
    while (true) {
        // First, process all inner loops (and inner of inner and so on) which are found by this moment
        while (true) {
            // Account for linear order invariants
            BasicBlock* loopToVisit = nullptr;
            for (auto* exit : exitBlocks) {
                if (exit->GetLoop() != loop && loop->IsLoopInside(exit->GetLoop()) && CheckIfBlockCanBeVisited(exit)) {
                    loopToVisit = exit;
                    break;
                }
            }
            if (loopToVisit == nullptr) {
                noInnerLoopsLeft = true;
                break;
            }
            VisitLoop(loopToVisit->GetLoop(), exitBlocks);
            exitBlocks.erase(loopToVisit);
            noFakeExitBlocksLeft = false;
        }

        // Second, process all fake exit blocks which are actually blocks from current loop
        while (true) {
            // Account for linear order invariants
            BasicBlock* fakeExitToVisit = nullptr;
            for (auto* exit : exitBlocks) {
                if (exit->GetLoop() == loop && CheckIfBlockCanBeVisited(exit)) {
                    fakeExitToVisit = exit;
                    break;
                }
            }
            if (fakeExitToVisit == nullptr) {
                noFakeExitBlocksLeft = true;
                break;
            }
            VisitLoopBlock(fakeExitToVisit, exitBlocks);
            exitBlocks.erase(fakeExitToVisit);
            noInnerLoopsLeft = false;
        }

        if (noInnerLoopsLeft && noFakeExitBlocksLeft) {
            break;
        }
    }
}


void LivenessAnalyzer::CreateBasicBlocksLinearOrder(BasicBlock* entry) {
    // By this moment, we assume that graph has no irreducible loops
    if (entry == nullptr) {
        return;
    }

    // If entry is a loop header, go through it
    Loop* currLoop = entry->GetLoop();
    if (entry == currLoop->GetHeader()) {
        std::set<BasicBlock*> exitBlocks{};
        VisitLoop(currLoop, exitBlocks);

        // Visit all real exit blocks according to linear order
        while (!exitBlocks.empty()) {
            for (auto* exit : exitBlocks) {
                if (exit->IsMarked(BasicBlock::Marker::Black)) {
                    exitBlocks.erase(exit);
                    break;
                }
                if (CheckIfBlockCanBeVisited(exit)) {
                    CreateBasicBlocksLinearOrder(exit);
                    exitBlocks.erase(exit);
                    break;
                }
            }
        }
    }
    else {
        if (entry->IsMarked(BasicBlock::Marker::Black)) {
            return;
        }

        // Mark visited basic blocks as black
        mBBLinearOrder.push_back(entry);
        entry->SetMarked(BasicBlock::Marker::Black);

        // Explicitly differentiate false and true successors, because
        // we would like to process false successor first if it is possible
        BasicBlock* trueSucc = entry->GetTrueSuccessor();
        BasicBlock* falseSucc = entry->GetFalseSuccessor();

        // Try false successor first, then true successor
        if (falseSucc != nullptr && !falseSucc->IsMarked(BasicBlock::Marker::Black) && CheckIfBlockCanBeVisited(falseSucc)) {
            CreateBasicBlocksLinearOrder(falseSucc);
        }
        if (trueSucc != nullptr && !trueSucc->IsMarked(BasicBlock::Marker::Black) && CheckIfBlockCanBeVisited(trueSucc)) {
            CreateBasicBlocksLinearOrder(trueSucc);
        }

        // Try false successor again. We either visited it already, or it can be visited after true successor is visited
        if (falseSucc != nullptr && !falseSucc->IsMarked(BasicBlock::Marker::Black) && CheckIfBlockCanBeVisited(falseSucc)) {
            CreateBasicBlocksLinearOrder(falseSucc);
        }
    }
}


void LivenessAnalyzer::AssignLinearAndLiveNumbers() {
    uint64_t linearNumber = 0;
    uint64_t liveNumber = 0;
    for (auto* bb : mBBLinearOrder) {
        LiveRange& lr = bb->GetLiveRange();
        Instruction* inst = bb->Front();

        lr.start = liveNumber;
        liveNumber += kInstructionLiveDiffSpillFill;
        while (inst != nullptr) {
            inst->SetLinearNumber(linearNumber);
            ++linearNumber;

            if (inst->IsPhi()) {
                inst->SetLiveNumber(lr.start);
            }
            else {
                inst->SetLiveNumber(liveNumber);
                liveNumber += kInstructionLiveDiffSpillFill;
            }

            inst = inst->GetNext();
        }
        lr.end = liveNumber;
    }
}


void LivenessAnalyzer::CalculateLiveRanges() {
    std::unordered_map<BasicBlock*, std::set<Value*>> bbLivesets;
    for (auto it = mBBLinearOrder.rbegin(), end = mBBLinearOrder.rend(); it != end; ++it) {
        BasicBlock* bb = *it;
        auto& liveset = bbLivesets[bb];
        LiveRange& bbLiveRange = bb->GetLiveRange();

        // Calculate initial liveset for the block
        for (auto* succ : bb->GetSuccessors()) {
            // Union of livesets of the successors' livesets
            if (auto succIt = bbLivesets.find(succ); succIt != bbLivesets.end()) {
                auto& succLiveset = succIt->second;
                for (auto* value : succLiveset) {
                    liveset.insert(value);
                }
            }

            // Successors' phi real inputs
            auto* succInst = succ->Front();
            while (succInst != nullptr && succInst->IsPhi()) {
                const auto& phiInputs = static_cast<InstructionPhi*>(succInst)->GetInputs();
                for (auto* value : phiInputs) {
                    Instruction* producer = value->GetProducer();
                    if (producer != nullptr && producer->GetParentBasicBlock() == bb) {
                        liveset.insert(value);
                    }
                }
                succInst = succInst->GetNext();
            }
        }

        // For each value in the liveset append liverange of the block
        for (auto* value : liveset) {
            // Ignore potential live holes
            LiveInterval& valueLI = value->GetLiveInterval();
            valueLI.UniteWith(bbLiveRange);
        }

        // Reverse iterate over block non-phi instructions
        Instruction* bbInst = bb->Back();
        while (bbInst != nullptr && !bbInst->IsPhi()) {
            Value* instOutput = bbInst->GetOutput();
            if (instOutput) {
                // Shorten live interval of the value
                LiveInterval& bbValueLI = bbInst->GetLiveInterval();
                bbValueLI.start = bbInst->GetLiveNumber();

                // Minimal value liverange is [Ln, Ln + 2) (kInstructionLiveDiffSpillFill == 2)
                bbValueLI.UniteWith(LiveInterval(bbValueLI.start, bbValueLI.start + kInstructionLiveDiffSpillFill));

                // Remove the instruction from the liveset
                liveset.erase(instOutput);
            }

            // Iterate over instruction's inputs (it's pity we don't have unified interface for that)
            // Add input to liveset and append [BB_START, inst_live_num) to input live range
            const LiveRange liveRangeBBToInst = LiveRange(bbLiveRange.start, bbInst->GetLiveNumber());
            switch (bbInst->GetType()) {
                default: break;
                case InstructionType::Add:
                case InstructionType::Sub:
                case InstructionType::Mul:
                case InstructionType::Div:
                case InstructionType::Rem:
                case InstructionType::And:
                case InstructionType::Or:
                case InstructionType::Xor:
                case InstructionType::Shl:
                case InstructionType::Shr:
                case InstructionType::Ashr: {
                    InstructionArithmetic* instArith = static_cast<InstructionArithmetic*>(bbInst);

                    Value* input1 = instArith->GetInput1();
                    liveset.insert(input1);
                    input1->GetLiveInterval().UniteWith(liveRangeBBToInst);

                    Value* input2 = instArith->GetInput2();
                    liveset.insert(input2);
                    input2->GetLiveInterval().UniteWith(liveRangeBBToInst);

                    break;
                }
                case InstructionType::Load: {
                    InstructionLoad* instLoad = static_cast<InstructionLoad*>(bbInst);
                    Value* loadPtr = instLoad->GetLoadPtr();
                    liveset.insert(loadPtr);
                    loadPtr->GetLiveInterval().UniteWith(liveRangeBBToInst);
                    break;
                }
                case InstructionType::Store: {
                    InstructionStore* instStore = static_cast<InstructionStore*>(bbInst);

                    Value* storePtr = instStore->GetStorePtr();
                    liveset.insert(storePtr);
                    storePtr->GetLiveInterval().UniteWith(liveRangeBBToInst);

                    Value* input = instStore->GetInput();
                    liveset.insert(input);
                    input->GetLiveInterval().UniteWith(liveRangeBBToInst);

                    break;
                }
                case InstructionType::Beq:
                case InstructionType::Bne:
                case InstructionType::Bgt:
                case InstructionType::Blt:
                case InstructionType::Bge:
                case InstructionType::Ble: {
                    InstructionBranch* instBranch = static_cast<InstructionBranch*>(bbInst);

                    Value* input1 = instBranch->GetInput1();
                    liveset.insert(input1);
                    input1->GetLiveInterval().UniteWith(liveRangeBBToInst);

                    Value* input2 = instBranch->GetInput2();
                    liveset.insert(input2);
                    input2->GetLiveInterval().UniteWith(liveRangeBBToInst);

                    break;
                }
                case InstructionType::Call: {
                    InstructionCall* instCall = static_cast<InstructionCall*>(bbInst);
                    const auto& args = instCall->GetArguments();
                    for (auto* arg : args) {
                        liveset.insert(arg);
                        arg->GetLiveInterval().UniteWith(liveRangeBBToInst);
                    }
                    break;
                }
                case InstructionType::Ret: {
                    InstructionRet* instRet = static_cast<InstructionRet*>(bbInst);
                    Value* returnValue = instRet->GetReturnValue();
                    if (returnValue != nullptr) {
                        liveset.insert(returnValue);
                        returnValue->GetLiveInterval().UniteWith(liveRangeBBToInst);
                    }
                    break;
                }
                case InstructionType::Mv: {
                    InstructionMv* instMv = static_cast<InstructionMv*>(bbInst);
                    Value* input = instMv->GetInput();
                    liveset.insert(input);
                    input->GetLiveInterval().UniteWith(liveRangeBBToInst);
                    break;
                }
                case InstructionType::NullCheck: {
                    InstructionNullCheck* instNullCheck = static_cast<InstructionNullCheck*>(bbInst);
                    Value* input = instNullCheck->GetInput();
                    liveset.insert(input);
                    input->GetLiveInterval().UniteWith(liveRangeBBToInst);
                    break;
                }
                case InstructionType::BoundsCheck: {
                    InstructionBoundsCheck* instBoundsCheck = static_cast<InstructionBoundsCheck*>(bbInst);

                    Value* inputPtr = instBoundsCheck->GetInputPtr();
                    liveset.insert(inputPtr);
                    inputPtr->GetLiveInterval().UniteWith(liveRangeBBToInst);

                    Value* inputArray = instBoundsCheck->GetInputArray();
                    liveset.insert(inputArray);
                    inputArray->GetLiveInterval().UniteWith(liveRangeBBToInst);

                    break;
                }
            }

            bbInst = bbInst->GetPrev();
        }

        // After iterating over instructions remove phi in current block from liveset
        bbInst = bb->Front();
        while (bbInst != nullptr && bbInst->IsPhi()) {
            liveset.erase(bbInst->GetOutput());
            bbInst = bbInst->GetNext();
        }

        // If block is a loop header add live range of [loop_header.begin, loop_end) to all instruction in the liveset
        if (bb->GetLoop()->GetHeader() == bb) {
            LiveRange loopLiveRange{};
            loopLiveRange.start = bbLiveRange.start;

            for (auto* latch : bb->GetLoop()->GetLatches()) {
                loopLiveRange.end = std::max(loopLiveRange.end, latch->GetLiveRange().end);
            }

            for (auto* value : liveset) {
                LiveInterval& valueLI = value->GetLiveInterval();
                valueLI.UniteWith(loopLiveRange);
            }
        }
    }
}

}   // namespace VMIR
