#include <StaticInliningPass.h>
#include <IRBuilder.h>

namespace VMIR {

static const char STATIC_INLINING_PASS_NAME[] = "Static Inlining Pass";
static const size_t DEFAULT_INLINE_INSTRUCTION_COUNT_THRESHOLD = 10;

StaticInliningPass::StaticInliningPass() : Pass(STATIC_INLINING_PASS_NAME), mInlineInstructionCountThreshold(DEFAULT_INLINE_INSTRUCTION_COUNT_THRESHOLD) {}

void StaticInliningPass::Run(Function* func) {
    InlineCallees(func);
}

size_t StaticInliningPass::GetInlineInstructionCountThreshold() const {
    return mInlineInstructionCountThreshold;
}

void StaticInliningPass::SetInlineInstructionCountThreshold(size_t threshold) {
    mInlineInstructionCountThreshold = threshold;
};


void StaticInliningPass::InlineCallees(Function* func) {
    mFunctionsToProcess.insert(func);

    // Search for call instructions
    std::vector<InstructionCall*> calls{};
    for (const auto* bb : func->GetBasicBlocks()) {
        Instruction* inst = bb->Front();
        while (inst) {
            if (inst->GetType() == InstructionType::Call) {
                InstructionCall* instCall = static_cast<InstructionCall*>(inst);
                calls.push_back(instCall);
            }
            inst = inst->GetNext();
        }
    }

    for (auto* instCall : calls) {
        Function* callee = instCall->GetFunction();
        if (mFunctionsToProcess.contains(callee)) {
            continue;
        }

        // Resolve and check callee before inlining. Inline callee first before inlining the call
        InlineCallees(callee);
        if (callee->GetInstructionCount() <= mInlineInstructionCountThreshold) {
            InlineCall(instCall);
        }
    }

    mFunctionsToProcess.erase(func);
}


void StaticInliningPass::InlineCall(InstructionCall* instCall) const {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    BasicBlock* callBB = instCall->GetParentBasicBlock();
    auto callInputs = instCall->GetArguments();
    Value* callOutput = instCall->GetReturnValue();
    Function* caller = callBB->GetParentFunction();

    // Build callee graph: copy callee graph to insert it to caller graph (TODO: implement callee cache)
    Function* calleeOrig = instCall->GetFunction();
    Function* callee = IrBuilder->CopyFunction(calleeOrig);
    

    BasicBlock* callBBTrueSucc  = callBB->GetTrueSuccessor();
    BasicBlock* callBBFalseSucc = callBB->GetFalseSuccessor();


    Instruction* instAfterCall = instCall->GetNext();
    callBB->RemoveInstruction(instCall);
    IrBuilder->RemoveInstruction(instCall);
    for (size_t i = 0; i < callInputs.size(); ++i) {
        Value* callInput = callInputs[i];
        callInput->RemoveUser(instCall);
    }

    // Save instructions of the call BB after the call instruction
    std::vector<Instruction*> instsAfterCall{};
    while (instAfterCall) {
        Instruction* next = instAfterCall->GetNext();

        instsAfterCall.push_back(instAfterCall);
        callBB->RemoveInstruction(instAfterCall);

        instAfterCall = next;
    }


    // Handle the callee entry basic block
    BasicBlock* calleeEntry = callee->GetEntryBasicBlock();

    // Small optimization
    if (calleeEntry->GetPredecessors().empty()) {
        // If callee entry BB does not have any predecessors, then it means that the only entrance to it is via function call
        // In that case we can simply append callee entry BB instructions to the caller call BB
        Instruction* calleeEntryInst = calleeEntry->Front();
        while (calleeEntryInst) {
            Instruction* next = calleeEntryInst->GetNext();
            callBB->AppendInstruction(calleeEntryInst);
            calleeEntryInst = next;
        }

        // Fix callee entry BB control flow
        BasicBlock* calleeEntryTrueSucc  = calleeEntry->GetTrueSuccessor();
        BasicBlock* calleeEntryFalseSucc = calleeEntry->GetFalseSuccessor();

        callBB->SetTrueSuccessor(calleeEntryTrueSucc);
        callBB->SetFalseSuccessor(calleeEntryFalseSucc);

        if (calleeEntryTrueSucc) {
            calleeEntryTrueSucc->RemovePredecessor(calleeEntry);
            calleeEntryTrueSucc->AddPredecessor(callBB);
        }
        if (calleeEntryFalseSucc) {
            calleeEntryFalseSucc->RemovePredecessor(calleeEntry);
            calleeEntryFalseSucc->AddPredecessor(callBB);
        }
    }
    else {
        // If callee entry BB does have any predecessors, then there is a possible loop/branch in the callee
        // In that case we must append whole callee entry BB to the caller graph and create Jump to it
        caller->AppendBasicBlock(calleeEntry);

        InstructionJump* instJumpToCalleeEntry = IrBuilder->CreateJump();
        instJumpToCalleeEntry->SetJumpBasicBlock(calleeEntry);
        callBB->AppendInstruction(instJumpToCalleeEntry);

        calleeEntry->AddPredecessor(callBB);
        callBB->SetTrueSuccessor(calleeEntry);
        callBB->SetFalseSuccessor(nullptr);
    }


    // Save the callee return BBs and move the callee BBs to the caller graph
    std::vector<InstructionRet*> calleeRets{};
    for (auto* calleeBB : callee->GetBasicBlocks()) {
        Instruction* termInst = calleeBB->Back();
        if (termInst->GetType() == InstructionType::Ret) {
            InstructionRet* instRet = static_cast<InstructionRet*>(termInst);
            calleeRets.push_back(instRet);
        }

        if (calleeBB == calleeEntry) {
            continue;
        }

        caller->AppendBasicBlock(calleeBB);
    }


    // Propagate arguments: rebind all inputs if there are any
    for (size_t i = 0; i < callInputs.size(); ++i) {
        Value* callInput = callInputs[i];
        Value* calleeArg = callee->GetArg(i);

        auto calleeArgUsers = calleeArg->GetUsers();
        for (auto* user : calleeArgUsers) {
            if (user->IsArithmetic()) {
                InstructionArithmetic* instArith = static_cast<InstructionArithmetic*>(user);
                Value* userInput1 = instArith->GetInput1();
                Value* userInput2 = instArith->GetInput2();

                calleeArg->RemoveUser(instArith);
                callInput->AddUser(instArith);
                if (userInput1 == calleeArg) {
                    instArith->SetInput1(callInput);
                }
                if (userInput2 == calleeArg) {
                    instArith->SetInput2(callInput);
                }

            }
            else if (user->GetType() == InstructionType::Load) {
                InstructionLoad* instLoad = static_cast<InstructionLoad*>(user);
        
                calleeArg->RemoveUser(instLoad);
                callInput->AddUser(instLoad);
                instLoad->SetLoadPtr(callInput);
            }
            else if (user->GetType() == InstructionType::Store) {
                InstructionStore* instStore = static_cast<InstructionStore*>(user);
                Value* userInput1 = instStore->GetStorePtr();
                Value* userInput2 = instStore->GetInput();
        
                calleeArg->RemoveUser(instStore);
                callInput->AddUser(instStore);
                if (userInput1 == calleeArg) {
                    instStore->SetStorePtr(callInput);
                }
                if (userInput2 == calleeArg) {
                    instStore->SetInput(callInput);
                }
            }
            else if (user->IsBranch()) {
                InstructionBranch* instBranch = static_cast<InstructionBranch*>(user);
                Value* userInput1 = instBranch->GetInput1();
                Value* userInput2 = instBranch->GetInput2();
        
                calleeArg->RemoveUser(instBranch);
                callInput->AddUser(instBranch);
                if (userInput1 == calleeArg) {
                    instBranch->SetInput1(callInput);
                }
                if (userInput2 == calleeArg) {
                    instBranch->SetInput2(callInput);
                }
            }
            else if (user->GetType() == InstructionType::Call) {
                InstructionCall* instCall = static_cast<InstructionCall*>(user);
                auto args = instCall->GetArguments();
        
                calleeArg->RemoveUser(instCall);
                callInput->AddUser(instCall);
                for (size_t i = 0; i < args.size(); ++i) {
                    if (args[i] == calleeArg) {
                        instCall->SetArgument(i, callInput);
                    }
                }
            }
            else if (user->GetType() == InstructionType::Ret) {
                InstructionRet* instRet = static_cast<InstructionRet*>(user);
        
                calleeArg->RemoveUser(instRet);
                callInput->AddUser(instRet);
                instRet->SetReturnValue(callInput);
            }
            else if (user->GetType() == InstructionType::Mv) {
                InstructionMv* instMv = static_cast<InstructionMv*>(user);
        
                calleeArg->RemoveUser(instMv);
                callInput->AddUser(instMv);
                instMv->SetInput(callInput);
            }
        }
    }


    BasicBlock* postCallBB = nullptr;

    // Small optimization
    if (calleeRets.size() == 1) {
        // If the callee has only one return BB, then we can simply replace Ret with Mv there and append caller post call instructions there
        InstructionRet* calleeRet = calleeRets.front();
        BasicBlock* calleeRetBB = calleeRet->GetParentBasicBlock();

        // Rebind output if there is a return value
        Value* retValue = calleeRet->GetReturnValue();
        if (retValue) {
            InstructionMv* instMv = IrBuilder->CreateMv();
            instMv->SetInput(retValue);
            instMv->SetOutput(callOutput);
    
            retValue->RemoveUser(calleeRet);
            retValue->AddUser(instMv);

            callOutput->SetProducer(instMv);
    
            calleeRetBB->AppendInstruction(instMv);
        }

        calleeRetBB->RemoveInstruction(calleeRet);
        IrBuilder->RemoveInstruction(calleeRet);

        for (auto* inst : instsAfterCall) {
            calleeRetBB->AppendInstruction(inst);
        }

        postCallBB = calleeRetBB;
    }
    else {
        // If the callee has multiple return BBs, then we need to create another post call BB
        postCallBB = IrBuilder->CreateBasicBlock(caller, "PostCallTo_" + calleeOrig->GetName());

        // For all basic blocks of the callee which end with Ret instruction replace the instruction with Jump to post call BB
        std::vector<Value*> phiInputs{};
        for (auto* calleeRet : calleeRets) {
            BasicBlock* calleeRetBB = calleeRet->GetParentBasicBlock();

            Value* retValue = calleeRet->GetReturnValue();
            if (retValue) {
                phiInputs.push_back(retValue);
                retValue->RemoveUser(calleeRet);
            }

            InstructionJump* instJumpToPostCall = IrBuilder->CreateJump();
            instJumpToPostCall->SetJumpBasicBlock(postCallBB);
            calleeRetBB->SetSuccessor(postCallBB);
            postCallBB->AddPredecessor(calleeRetBB);

            calleeRetBB->RemoveInstruction(calleeRet);
            IrBuilder->RemoveInstruction(calleeRet);

            calleeRetBB->AppendInstruction(instJumpToPostCall);
        }

        // If there is a return value, then create Phi
        if (callOutput) {
            InstructionPhi* instPhi = IrBuilder->CreatePhi();
            for (auto* input : phiInputs) {
                instPhi->AddInput(input);
                input->AddUser(instPhi);
            }

            instPhi->SetOutput(callOutput);
            callOutput->SetProducer(instPhi);

            postCallBB->AppendInstruction(instPhi);
        }

        for (auto* inst : instsAfterCall) {
            postCallBB->AppendInstruction(inst);
        }
    }

    postCallBB->SetTrueSuccessor(callBBTrueSucc);
    postCallBB->SetFalseSuccessor(callBBFalseSucc);

    if (callBBTrueSucc) {
        callBBTrueSucc->RemovePredecessor(callBB);
        callBBTrueSucc->AddPredecessor(postCallBB);
    }
    if (callBBFalseSucc) {
        callBBFalseSucc->RemovePredecessor(callBB);
        callBBFalseSucc->AddPredecessor(postCallBB);
    }

    // TODO: Remove unused basic blocks (if any) and unused values (if any)
    IrBuilder->RemoveFunction(callee);
}

}   // namespace VMIR
