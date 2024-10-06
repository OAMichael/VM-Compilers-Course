#include <IRBuilder.h>

namespace VMIR {

IRBuilder* IRBuilder::mInstancePtr = nullptr;
IRBuilder::IRBuilder() {}


Function* IRBuilder::CreateFunction(const std::string& name) {
    return CreateFunction(ValueType::Void, name);
}

Function* IRBuilder::CreateFunction(const ValueType retType, const std::string& name) {
    Function* func = new Function(retType, name);
    mFunctions.push_back(func);
    return func;
}

Function* IRBuilder::CreateFunction(const std::vector<ValueType>& argsTypes, const std::string& name) {
    return CreateFunction(ValueType::Void, argsTypes, name);
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


InstructionAdd* IRBuilder::CreateAdd() {
    return CreateAdd(nullptr, nullptr, nullptr, nullptr);
}

InstructionAdd* IRBuilder::CreateAdd(Value* input1, Value* input2, Value* output) {
    return CreateAdd(nullptr, input1, input2, output);
}

InstructionAdd* IRBuilder::CreateAdd(BasicBlock* parentBasicBlock) {
    return CreateAdd(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionAdd* IRBuilder::CreateAdd(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionAdd* inst = new InstructionAdd(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionSub* IRBuilder::CreateSub() {
    return CreateSub(nullptr, nullptr, nullptr, nullptr);
}

InstructionSub* IRBuilder::CreateSub(Value* input1, Value* input2, Value* output) {
    return CreateSub(nullptr, input1, input2, output);
}

InstructionSub* IRBuilder::CreateSub(BasicBlock* parentBasicBlock) {
    return CreateSub(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionSub* IRBuilder::CreateSub(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionSub* inst = new InstructionSub(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionMul* IRBuilder::CreateMul() {
    return CreateMul(nullptr, nullptr, nullptr, nullptr);
}

InstructionMul* IRBuilder::CreateMul(Value* input1, Value* input2, Value* output) {
    return CreateMul(nullptr, input1, input2, output);
}

InstructionMul* IRBuilder::CreateMul(BasicBlock* parentBasicBlock) {
    return CreateMul(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionMul* IRBuilder::CreateMul(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionMul* inst = new InstructionMul(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionDiv* IRBuilder::CreateDiv() {
    return CreateDiv(nullptr, nullptr, nullptr, nullptr);
}

InstructionDiv* IRBuilder::CreateDiv(Value* input1, Value* input2, Value* output) {
    return CreateDiv(nullptr, input1, input2, output);
}

InstructionDiv* IRBuilder::CreateDiv(BasicBlock* parentBasicBlock) {
    return CreateDiv(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionDiv* IRBuilder::CreateDiv(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionDiv* inst = new InstructionDiv(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionRem* IRBuilder::CreateRem() {
    return CreateRem(nullptr, nullptr, nullptr, nullptr);
}

InstructionRem* IRBuilder::CreateRem(Value* input1, Value* input2, Value* output) {
    return CreateRem(nullptr, input1, input2, output);
}

InstructionRem* IRBuilder::CreateRem(BasicBlock* parentBasicBlock) {
    return CreateRem(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionRem* IRBuilder::CreateRem(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionRem* inst = new InstructionRem(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionAnd* IRBuilder::CreateAnd() {
    return CreateAnd(nullptr, nullptr, nullptr, nullptr);
}

InstructionAnd* IRBuilder::CreateAnd(Value* input1, Value* input2, Value* output) {
    return CreateAnd(nullptr, input1, input2, output);
}

InstructionAnd* IRBuilder::CreateAnd(BasicBlock* parentBasicBlock) {
    return CreateAnd(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionAnd* IRBuilder::CreateAnd(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionAnd* inst = new InstructionAnd(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionOr* IRBuilder::CreateOr() {
    return CreateOr(nullptr, nullptr, nullptr, nullptr);
}

InstructionOr* IRBuilder::CreateOr(Value* input1, Value* input2, Value* output) {
    return CreateOr(nullptr, input1, input2, output);
}

InstructionOr* IRBuilder::CreateOr(BasicBlock* parentBasicBlock) {
    return CreateOr(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionOr* IRBuilder::CreateOr(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionOr* inst = new InstructionOr(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionXor* IRBuilder::CreateXor() {
    return CreateXor(nullptr, nullptr, nullptr, nullptr);
}

InstructionXor* IRBuilder::CreateXor(Value* input1, Value* input2, Value* output) {
    return CreateXor(nullptr, input1, input2, output);
}

InstructionXor* IRBuilder::CreateXor(BasicBlock* parentBasicBlock) {
    return CreateXor(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionXor* IRBuilder::CreateXor(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionXor* inst = new InstructionXor(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionShl* IRBuilder::CreateShl() {
    return CreateShl(nullptr, nullptr, nullptr, nullptr);
}

InstructionShl* IRBuilder::CreateShl(Value* input1, Value* input2, Value* output) {
    return CreateShl(nullptr, input1, input2, output);
}

InstructionShl* IRBuilder::CreateShl(BasicBlock* parentBasicBlock) {
    return CreateShl(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionShl* IRBuilder::CreateShl(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionShl* inst = new InstructionShl(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionShr* IRBuilder::CreateShr() {
    return CreateShr(nullptr, nullptr, nullptr, nullptr);
}

InstructionShr* IRBuilder::CreateShr(Value* input1, Value* input2, Value* output) {
    return CreateShr(nullptr, input1, input2, output);
}

InstructionShr* IRBuilder::CreateShr(BasicBlock* parentBasicBlock) {
    return CreateShr(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionShr* IRBuilder::CreateShr(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionShr* inst = new InstructionShr(id, input1, input2, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionLoad* IRBuilder::CreateLoad() {
    return CreateLoad(nullptr, nullptr, nullptr);
}

InstructionLoad* IRBuilder::CreateLoad(Value* loadPtr, Value* output) {
    return CreateLoad(nullptr, loadPtr, output);
}

InstructionLoad* IRBuilder::CreateLoad(BasicBlock* parentBasicBlock) {
    return CreateLoad(parentBasicBlock, nullptr, nullptr);
}

InstructionLoad* IRBuilder::CreateLoad(BasicBlock* parentBasicBlock, Value* loadPtr, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionLoad* inst = new InstructionLoad(id, loadPtr, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (loadPtr != nullptr) {
        loadPtr->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionStore* IRBuilder::CreateStore() {
    return CreateStore(nullptr, nullptr, nullptr);
}

InstructionStore* IRBuilder::CreateStore(Value* storePtr, Value* input) {
    return CreateStore(nullptr, storePtr, input);
}

InstructionStore* IRBuilder::CreateStore(BasicBlock* parentBasicBlock) {
    return CreateStore(parentBasicBlock, nullptr, nullptr);
}

InstructionStore* IRBuilder::CreateStore(BasicBlock* parentBasicBlock, Value* storePtr, Value* input) {
    InstructionId id = mInstructions.size();
    InstructionStore* inst = new InstructionStore(id, storePtr, input);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (storePtr != nullptr) {
        storePtr->AddUser(inst);
    }
    if (input != nullptr) {
        input->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionJump* IRBuilder::CreateJump() {
    return CreateJump(nullptr, nullptr);
}

InstructionJump* IRBuilder::CreateJump(BasicBlock* parentBasicBlock) {
    return CreateJump(parentBasicBlock, nullptr);
}

InstructionJump* IRBuilder::CreateJump(BasicBlock* parentBasicBlock, BasicBlock *jumpBB) {
    InstructionId id = mInstructions.size();
    InstructionJump* inst = new InstructionJump(id, jumpBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        if (jumpBB != nullptr) {
            parentBasicBlock->SetSuccessor(jumpBB);
            jumpBB->AddPredecessor(parentBasicBlock);
        }
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionBeq* IRBuilder::CreateBeq() {
    return CreateBeq(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBeq* IRBuilder::CreateBeq(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBeq(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBeq* IRBuilder::CreateBeq(BasicBlock* parentBasicBlock) {
    return CreateBeq(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBeq* IRBuilder::CreateBeq(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionId id = mInstructions.size();
    InstructionBeq* inst = new InstructionBeq(id, input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        if (trueBB != nullptr) {
            parentBasicBlock->SetTrueSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->SetFalseSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionBne* IRBuilder::CreateBne() {
    return CreateBne(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBne* IRBuilder::CreateBne(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBne(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBne* IRBuilder::CreateBne(BasicBlock* parentBasicBlock) {
    return CreateBne(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBne* IRBuilder::CreateBne(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionId id = mInstructions.size();
    InstructionBne* inst = new InstructionBne(id, input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        if (trueBB != nullptr) {
            parentBasicBlock->SetTrueSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->SetFalseSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionBgt* IRBuilder::CreateBgt() {
    return CreateBgt(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBgt* IRBuilder::CreateBgt(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBgt(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBgt* IRBuilder::CreateBgt(BasicBlock* parentBasicBlock) {
    return CreateBgt(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBgt* IRBuilder::CreateBgt(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionId id = mInstructions.size();
    InstructionBgt* inst = new InstructionBgt(id, input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        if (trueBB != nullptr) {
            parentBasicBlock->SetTrueSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->SetFalseSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionBlt* IRBuilder::CreateBlt() {
    return CreateBlt(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBlt* IRBuilder::CreateBlt(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBlt(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBlt* IRBuilder::CreateBlt(BasicBlock* parentBasicBlock) {
    return CreateBlt(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBlt* IRBuilder::CreateBlt(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionId id = mInstructions.size();
    InstructionBlt* inst = new InstructionBlt(id, input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        if (trueBB != nullptr) {
            parentBasicBlock->SetTrueSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->SetFalseSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionBge* IRBuilder::CreateBge() {
    return CreateBge(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBge* IRBuilder::CreateBge(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBge(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBge* IRBuilder::CreateBge(BasicBlock* parentBasicBlock) {
    return CreateBge(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBge* IRBuilder::CreateBge(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionId id = mInstructions.size();
    InstructionBge* inst = new InstructionBge(id, input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        if (trueBB != nullptr) {
            parentBasicBlock->SetTrueSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->SetFalseSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionBle* IRBuilder::CreateBle() {
    return CreateBle(nullptr, nullptr, nullptr, nullptr, nullptr);
}

InstructionBle* IRBuilder::CreateBle(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    return CreateBle(nullptr, input1, input2, trueBB, falseBB);
}

InstructionBle* IRBuilder::CreateBle(BasicBlock* parentBasicBlock) {
    return CreateBle(parentBasicBlock, nullptr, nullptr, nullptr, nullptr);
}

InstructionBle* IRBuilder::CreateBle(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB) {
    InstructionId id = mInstructions.size();
    InstructionBle* inst = new InstructionBle(id, input1, input2, trueBB, falseBB);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
        if (trueBB != nullptr) {
            parentBasicBlock->SetTrueSuccessor(trueBB);
            trueBB->AddPredecessor(parentBasicBlock);
        }
        if (falseBB != nullptr) {
            parentBasicBlock->SetFalseSuccessor(falseBB);
            falseBB->AddPredecessor(parentBasicBlock);
        }
    }
    if (input1 != nullptr) {
        input1->AddUser(inst);
    }
    if (input2 != nullptr) {
        input2->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionCall* IRBuilder::CreateCall() {
    return CreateCall(nullptr, nullptr, nullptr);
}

InstructionCall* IRBuilder::CreateCall(Function* function) {
    return CreateCall(nullptr, function, nullptr);
}

InstructionCall* IRBuilder::CreateCall(Function* function, Value* ret) {
    return CreateCall(nullptr, function, ret);
}

InstructionCall* IRBuilder::CreateCall(Function* function, const std::vector<Value*>& args) {
    return CreateCall(nullptr, function, nullptr, args);
}

InstructionCall* IRBuilder::CreateCall(Function* function, Value* ret, const std::vector<Value*>& args) {
    return CreateCall(nullptr, function, ret, args);
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock) {
    return CreateCall(parentBasicBlock, nullptr, nullptr);
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock, Function* function) {
    return CreateCall(parentBasicBlock, function, nullptr);
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock, Function* function, Value* ret) {
    InstructionId id = mInstructions.size();
    InstructionCall* inst = new InstructionCall(id, function, ret);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (ret != nullptr) {
        ret->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock, Function* function, const std::vector<Value*>& args) {
    return CreateCall(parentBasicBlock, function, nullptr, args);
}

InstructionCall* IRBuilder::CreateCall(BasicBlock* parentBasicBlock, Function* function, Value* ret, const std::vector<Value*>& args) {
    InstructionId id = mInstructions.size();
    InstructionCall* inst = new InstructionCall(id, function, ret, args);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    for (auto* a : args) {
        a->AddUser(inst);
    }
    if (ret != nullptr) {
        ret->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionRet* IRBuilder::CreateRet() {
    return CreateRet(nullptr, nullptr);
}

InstructionRet* IRBuilder::CreateRet(Value* output) {
    return CreateRet(nullptr, output);
}

InstructionRet* IRBuilder::CreateRet(BasicBlock* parentBasicBlock) {
    return CreateRet(parentBasicBlock, nullptr);
}

InstructionRet* IRBuilder::CreateRet(BasicBlock* parentBasicBlock, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionRet* inst = new InstructionRet(id, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (output != nullptr) {
        output->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionAlloc* IRBuilder::CreateAlloc() {
    return CreateAlloc(nullptr, nullptr, 1);
}

InstructionAlloc* IRBuilder::CreateAlloc(Value* output) {
    return CreateAlloc(nullptr, output, 1);
}

InstructionAlloc* IRBuilder::CreateAlloc(Value* output, const size_t count) {
    return CreateAlloc(nullptr, output, count);
}

InstructionAlloc* IRBuilder::CreateAlloc(BasicBlock* parentBasicBlock) {
    return CreateAlloc(parentBasicBlock, nullptr, 1);
}

InstructionAlloc* IRBuilder::CreateAlloc(BasicBlock* parentBasicBlock, Value* output) {
    return CreateAlloc(parentBasicBlock, output, 1);
}

InstructionAlloc* IRBuilder::CreateAlloc(BasicBlock* parentBasicBlock, Value* output, const size_t count) {
    InstructionId id = mInstructions.size();
    InstructionAlloc* inst = new InstructionAlloc(id, output, count);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionPhi* IRBuilder::CreatePhi() {
    return CreatePhi(nullptr, {}, nullptr);
}

InstructionPhi* IRBuilder::CreatePhi(const std::set<Value*>& inputs, Value* output) {
    return CreatePhi(nullptr, inputs, output);
}

InstructionPhi* IRBuilder::CreatePhi(BasicBlock* parentBasicBlock) {
    return CreatePhi(parentBasicBlock, {}, nullptr);
}

InstructionPhi* IRBuilder::CreatePhi(BasicBlock* parentBasicBlock, const std::set<Value*>& inputs, Value* output) {
    InstructionId id = mInstructions.size();
    InstructionPhi* inst = new InstructionPhi(id, inputs, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    for (auto* i : inputs) {
        i->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


IRBuilder::~IRBuilder() {
    Cleanup();
}


void IRBuilder::PrintDebug(std::ostream& out) const {
    for (auto* f : mFunctions) {
        out << "Function: " << f->GetName() << std::endl;

        out << "    Predecessors:" << std::endl;
        for (const auto* bb : mBasicBlocks) {
            if (bb->GetParentFunction() != f) {
                continue;
            }

            for (const auto* pred : bb->GetPredecessors()) {
                out << "        " << pred->GetName() << " -> " << bb->GetName() << std::endl;
            }
        }
        out << "\n";

        out << "    Successors:" << std::endl;
        for (const auto* bb : mBasicBlocks) {
            if (bb->GetParentFunction() != f) {
                continue;
            }

            auto [trueSucc, falseSucc] = bb->GetSuccessors();
            if (trueSucc) {
                out << "        " << bb->GetName() << " -> " << trueSucc->GetName() << std::endl;
            }
            if (falseSucc) {
                out << "        " << bb->GetName() << " -> " << falseSucc->GetName() << std::endl;
            }
        }
        out << "\n";

        out << "    Users:" << std::endl;
        for (const auto* v : mValuesWithData.at(f)) {
            for (const auto* i : v->GetUsers()) {
                out << "        " << v->GetValueStr() << " -> [" << i->GetAsString() << "]" << std::endl;
            }
        }
        for (const auto* v : mValues.at(f)) {
            for (const auto* i : v->GetUsers()) {
                out << "        " << v->GetValueStr() << " -> [" << i->GetAsString() << "]" << std::endl;
            }
        }
        out << "\n";

        out << "    Producers:" << std::endl;
        for (const auto* v : mValuesWithData.at(f)) {
            Instruction* prod = v->GetProducer();
            if (prod != nullptr) {
                out << "        " << v->GetValueStr() << " -> [" << prod->GetAsString() << "]" << std::endl;
            }
            else {
                out << "        " << v->GetValueStr() << " -> null" << std::endl;
            }
        }
        for (const auto* v : mValues.at(f)) {
            Instruction* prod = v->GetProducer();
            if (prod != nullptr) {
                out << "        " << v->GetValueStr() << " -> [" << prod->GetAsString() << "]" << std::endl;
            }
            else {
                out << "        " << v->GetValueStr() << " -> null" << std::endl;
            }
        }
    }
}

}   // namespace VMIR
