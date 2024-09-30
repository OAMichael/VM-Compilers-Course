#include <IRBuilder.h>

namespace VMIR {

IRBuilder* IRBuilder::mInstancePtr = nullptr;
IRBuilder::IRBuilder() {}


Function* IRBuilder::CreateFunction(const std::string& name) {
    Function* func = new Function(name);
    mFunctions.push_back(func);
    return func;
}

Function* IRBuilder::CreateFunction(const ValueType retType, const std::string& name) {
    Function* func = new Function(retType, name);
    mFunctions.push_back(func);
    return func;
}

Function* IRBuilder::CreateFunction(const std::vector<ValueType>& argsTypes, const std::string& name) {
    Function* func = new Function(argsTypes, name);
    mFunctions.push_back(func);
    return func;
}

Function* IRBuilder::CreateFunction(const ValueType retType, const std::vector<ValueType>& argsTypes, const std::string& name) {
    Function* func = new Function(retType, argsTypes, name);
    mFunctions.push_back(func);
    return func;
}



BasicBlock* IRBuilder::CreateBasicBlock() {
    return CreateBasicBlock(nullptr, "");
}

BasicBlock* IRBuilder::CreateBasicBlock(Function* parentFunction) {
    return CreateBasicBlock(parentFunction, "");
}

BasicBlock* IRBuilder::CreateBasicBlock(const std::string& name) {
    return CreateBasicBlock(nullptr, name);
}

BasicBlock* IRBuilder::CreateBasicBlock(Function* parentFunction, const std::string& name) {
    BasicBlockId id = mBasicBlocks.size();
    BasicBlock* bb = new BasicBlock(parentFunction, id, name);
    if (parentFunction != nullptr) {
        parentFunction->AppendBasicBlock(bb);
    }
    mBasicBlocks.push_back(bb);
    return bb;
}

BasicBlock* IRBuilder::CreateBasicBlock(std::vector<Instruction*>& instructions) {
    return CreateBasicBlock(instructions, nullptr, "");
}

BasicBlock* IRBuilder::CreateBasicBlock(const std::vector<Instruction*>& instructions, Function* parentFunction) {
    return CreateBasicBlock(instructions, parentFunction, "");
}

BasicBlock* IRBuilder::CreateBasicBlock(const std::vector<Instruction*>& instructions, const std::string& name) {
    return CreateBasicBlock(instructions, nullptr, name);
}

BasicBlock* IRBuilder::CreateBasicBlock(const std::vector<Instruction*>& instructions, Function* parentFunction, const std::string& name) {
    BasicBlockId id = mBasicBlocks.size();
    BasicBlock* bb = new BasicBlock(instructions, parentFunction, id, name);
    if (parentFunction != nullptr) {
        parentFunction->AppendBasicBlock(bb);
    }
    for (auto& inst : instructions) {
        inst->SetParentBasicBlock(bb);
    }
    mBasicBlocks.push_back(bb);
    return bb;
}


InstructionAdd* IRBuilder::CreateAdd() {
    return CreateAdd(nullptr, nullptr, nullptr, nullptr);
}

InstructionAdd* IRBuilder::CreateAdd(const Value* input1, const Value* input2, const Value* output) {
    return CreateAdd(nullptr, input1, input2, output);
}

InstructionAdd* IRBuilder::CreateAdd(BasicBlock* parentBasicBlock) {
    return CreateAdd(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionAdd* IRBuilder::CreateAdd(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionAdd* inst = new InstructionAdd(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionSub* IRBuilder::CreateSub() {
    return CreateSub(nullptr, nullptr, nullptr, nullptr);
}

InstructionSub* IRBuilder::CreateSub(const Value* input1, const Value* input2, const Value* output) {
    return CreateSub(nullptr, input1, input2, output);
}

InstructionSub* IRBuilder::CreateSub(BasicBlock* parentBasicBlock) {
    return CreateSub(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionSub* IRBuilder::CreateSub(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionSub* inst = new InstructionSub(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionMul* IRBuilder::CreateMul() {
    return CreateMul(nullptr, nullptr, nullptr, nullptr);
}

InstructionMul* IRBuilder::CreateMul(const Value* input1, const Value* input2, const Value* output) {
    return CreateMul(nullptr, input1, input2, output);
}

InstructionMul* IRBuilder::CreateMul(BasicBlock* parentBasicBlock) {
    return CreateMul(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionMul* IRBuilder::CreateMul(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionMul* inst = new InstructionMul(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionDiv* IRBuilder::CreateDiv() {
    return CreateDiv(nullptr, nullptr, nullptr, nullptr);
}

InstructionDiv* IRBuilder::CreateDiv(const Value* input1, const Value* input2, const Value* output) {
    return CreateDiv(nullptr, input1, input2, output);
}

InstructionDiv* IRBuilder::CreateDiv(BasicBlock* parentBasicBlock) {
    return CreateDiv(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionDiv* IRBuilder::CreateDiv(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionDiv* inst = new InstructionDiv(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionRem* IRBuilder::CreateRem() {
    return CreateRem(nullptr, nullptr, nullptr, nullptr);
}

InstructionRem* IRBuilder::CreateRem(const Value* input1, const Value* input2, const Value* output) {
    return CreateRem(nullptr, input1, input2, output);
}

InstructionRem* IRBuilder::CreateRem(BasicBlock* parentBasicBlock) {
    return CreateRem(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionRem* IRBuilder::CreateRem(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionRem* inst = new InstructionRem(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionAnd* IRBuilder::CreateAnd() {
    return CreateAnd(nullptr, nullptr, nullptr, nullptr);
}

InstructionAnd* IRBuilder::CreateAnd(const Value* input1, const Value* input2, const Value* output) {
    return CreateAnd(nullptr, input1, input2, output);
}

InstructionAnd* IRBuilder::CreateAnd(BasicBlock* parentBasicBlock) {
    return CreateAnd(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionAnd* IRBuilder::CreateAnd(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionAnd* inst = new InstructionAnd(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionOr* IRBuilder::CreateOr() {
    return CreateOr(nullptr, nullptr, nullptr, nullptr);
}

InstructionOr* IRBuilder::CreateOr(const Value* input1, const Value* input2, const Value* output) {
    return CreateOr(nullptr, input1, input2, output);
}

InstructionOr* IRBuilder::CreateOr(BasicBlock* parentBasicBlock) {
    return CreateOr(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionOr* IRBuilder::CreateOr(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionOr* inst = new InstructionOr(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionXor* IRBuilder::CreateXor() {
    return CreateXor(nullptr, nullptr, nullptr, nullptr);
}

InstructionXor* IRBuilder::CreateXor(const Value* input1, const Value* input2, const Value* output) {
    return CreateXor(nullptr, input1, input2, output);
}

InstructionXor* IRBuilder::CreateXor(BasicBlock* parentBasicBlock) {
    return CreateXor(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionXor* IRBuilder::CreateXor(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionXor* inst = new InstructionXor(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionShl* IRBuilder::CreateShl() {
    return CreateShl(nullptr, nullptr, nullptr, nullptr);
}

InstructionShl* IRBuilder::CreateShl(const Value* input1, const Value* input2, const Value* output) {
    return CreateShl(nullptr, input1, input2, output);
}

InstructionShl* IRBuilder::CreateShl(BasicBlock* parentBasicBlock) {
    return CreateShl(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionShl* IRBuilder::CreateShl(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionShl* inst = new InstructionShl(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionShr* IRBuilder::CreateShr() {
    return CreateShr(nullptr, nullptr, nullptr, nullptr);
}

InstructionShr* IRBuilder::CreateShr(const Value* input1, const Value* input2, const Value* output) {
    return CreateShr(nullptr, input1, input2, output);
}

InstructionShr* IRBuilder::CreateShr(BasicBlock* parentBasicBlock) {
    return CreateShr(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionShr* IRBuilder::CreateShr(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionShr* inst = new InstructionShr(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionLoad* IRBuilder::CreateLoad() {
    return CreateLoad(nullptr, nullptr, nullptr);
}

InstructionLoad* IRBuilder::CreateLoad(const Value* loadPtr, const Value* output) {
    return CreateLoad(nullptr, loadPtr, output);
}

InstructionLoad* IRBuilder::CreateLoad(BasicBlock* parentBasicBlock) {
    return CreateLoad(parentBasicBlock, nullptr, nullptr);
}

InstructionLoad* IRBuilder::CreateLoad(BasicBlock* parentBasicBlock, const Value* loadPtr, const Value* output) {
    InstructionLoad* inst = new InstructionLoad(loadPtr, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionStore* IRBuilder::CreateStore() {
    return CreateStore(nullptr, nullptr, nullptr);
}

InstructionStore* IRBuilder::CreateStore(const Value* storePtr, const Value* input) {
    return CreateStore(nullptr, storePtr, input);
}

InstructionStore* IRBuilder::CreateStore(BasicBlock* parentBasicBlock) {
    return CreateStore(parentBasicBlock, nullptr, nullptr);
}

InstructionStore* IRBuilder::CreateStore(BasicBlock* parentBasicBlock, const Value* storePtr, const Value* input) {
    InstructionStore* inst = new InstructionStore(storePtr, input);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionJump* IRBuilder::CreateJump() {
    return CreateJump(nullptr, nullptr);
}

InstructionJump* IRBuilder::CreateJump(BasicBlock* parentBasicBlock) {
    return CreateJump(parentBasicBlock, nullptr);
}

InstructionJump* IRBuilder::CreateJump(BasicBlock* parentBasicBlock, BasicBlock *jumpBB) {
    InstructionJump* inst = new InstructionJump(jumpBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
        if (jumpBB != nullptr) {
            parentBasicBlock->AddSuccessor(jumpBB);
            jumpBB->AddPredecessor(parentBasicBlock);
        }
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionBeq* IRBuilder::CreateBeq() {
    return CreateBeq(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBeq* IRBuilder::CreateBeq(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBeq(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBeq* IRBuilder::CreateBeq(BasicBlock* parentBasicBlock) {
    return CreateBeq(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBeq* IRBuilder::CreateBeq(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionBeq* inst = new InstructionBeq(input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
        if (trueBB != nullptr) {
            parentBasicBlock->AddSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->AddSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionBne* IRBuilder::CreateBne() {
    return CreateBne(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBne* IRBuilder::CreateBne(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBne(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBne* IRBuilder::CreateBne(BasicBlock* parentBasicBlock) {
    return CreateBne(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBne* IRBuilder::CreateBne(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionBne* inst = new InstructionBne(input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
        if (trueBB != nullptr) {
            parentBasicBlock->AddSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->AddSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionBgt* IRBuilder::CreateBgt() {
    return CreateBgt(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBgt* IRBuilder::CreateBgt(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBgt(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBgt* IRBuilder::CreateBgt(BasicBlock* parentBasicBlock) {
    return CreateBgt(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBgt* IRBuilder::CreateBgt(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionBgt* inst = new InstructionBgt(input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
        if (trueBB != nullptr) {
            parentBasicBlock->AddSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->AddSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionBlt* IRBuilder::CreateBlt() {
    return CreateBlt(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBlt* IRBuilder::CreateBlt(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBlt(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBlt* IRBuilder::CreateBlt(BasicBlock* parentBasicBlock) {
    return CreateBlt(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBlt* IRBuilder::CreateBlt(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionBlt* inst = new InstructionBlt(input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
        if (trueBB != nullptr) {
            parentBasicBlock->AddSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->AddSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionBge* IRBuilder::CreateBge() {
    return CreateBge(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBge* IRBuilder::CreateBge(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBge(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBge* IRBuilder::CreateBge(BasicBlock* parentBasicBlock) {
    return CreateBge(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBge* IRBuilder::CreateBge(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionBge* inst = new InstructionBge(input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
        if (trueBB != nullptr) {
            parentBasicBlock->AddSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->AddSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionBle* IRBuilder::CreateBle() {
    return CreateBle(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBle* IRBuilder::CreateBle(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBle(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBle* IRBuilder::CreateBle(BasicBlock* parentBasicBlock) {
    return CreateBle(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBle* IRBuilder::CreateBle(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionBle* inst = new InstructionBle(input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
        if (trueBB != nullptr) {
            parentBasicBlock->AddSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->AddSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionCall* IRBuilder::CreateCall() {
    return CreateCall(nullptr, nullptr, nullptr);
}

InstructionCall* IRBuilder::CreateCall(const Function* function) {
    return CreateCall(nullptr, function, nullptr);
}

InstructionCall* IRBuilder::CreateCall(const Function* function, const Value* ret) {
    return CreateCall(nullptr, function, ret);
}

InstructionCall* IRBuilder::CreateCall(const Function* function, const std::vector<const Value*>& args) {
    return CreateCall(nullptr, function, nullptr, args);
}

InstructionCall* IRBuilder::CreateCall(const Function* function, const Value* ret, const std::vector<const Value*>& args) {
    return CreateCall(nullptr, function, ret, args);
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock) {
    return CreateCall(parentBasicBlock, nullptr, nullptr);
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock, const Function* function) {
    return CreateCall(parentBasicBlock, function, nullptr);
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock, const Function* function, const Value* ret) {
    InstructionCall* inst = new InstructionCall(function, ret);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock, const Function* function, const std::vector<const Value*>& args) {
    return CreateCall(parentBasicBlock, function, nullptr, args);
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock, const Function* function, const Value* ret, const std::vector<const Value*>& args) {
    InstructionCall* inst = new InstructionCall(function, ret, args);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionRet* IRBuilder::CreateRet() {
    return CreateRet(nullptr, nullptr);
}

InstructionRet* IRBuilder::CreateRet(const Value* output) {
    return CreateRet(nullptr, output);
}

InstructionRet* IRBuilder::CreateRet(BasicBlock* parentBasicBlock) {
    return CreateRet(parentBasicBlock, nullptr);
}

InstructionRet* IRBuilder::CreateRet(BasicBlock* parentBasicBlock, const Value* output) {
    InstructionRet* inst = new InstructionRet(output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionAlloc* IRBuilder::CreateAlloc() {
    return CreateAlloc(nullptr, nullptr, 1);
}

InstructionAlloc* IRBuilder::CreateAlloc(const Value* output) {
    return CreateAlloc(nullptr, output, 1);
}

InstructionAlloc* IRBuilder::CreateAlloc(const Value* output, const size_t count) {
    return CreateAlloc(nullptr, output, count);
}

InstructionAlloc* IRBuilder::CreateAlloc(BasicBlock* parentBasicBlock) {
    return CreateAlloc(parentBasicBlock, nullptr, 1);
}

InstructionAlloc* IRBuilder::CreateAlloc(BasicBlock* parentBasicBlock, const Value* output) {
    return CreateAlloc(parentBasicBlock, output, 1);
}

InstructionAlloc* IRBuilder::CreateAlloc(BasicBlock* parentBasicBlock, const Value* output, const size_t count) {
    InstructionAlloc* inst = new InstructionAlloc(output, count);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


InstructionPhi* IRBuilder::CreatePhi() {
    return CreatePhi(nullptr, nullptr, nullptr, nullptr);
}

InstructionPhi* IRBuilder::CreatePhi(const Value* input1, const Value* input2, const Value* output) {
    return CreatePhi(nullptr, input1, input2, output);
}

InstructionPhi* IRBuilder::CreatePhi(BasicBlock* parentBasicBlock) {
    return CreatePhi(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionPhi* IRBuilder::CreatePhi(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output) {
    InstructionPhi* inst = new InstructionPhi(input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        inst->SetParentBasicBlock(parentBasicBlock);
    }
    mInstructions.push_back(inst);
    return inst;
}


IRBuilder::~IRBuilder() {
    Cleanup();
}

}   // namespace VMIR
