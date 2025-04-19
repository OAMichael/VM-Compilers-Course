#include <IRBuilder.h>

#include <iomanip>

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
    BasicBlockId id = GenerateNewBasicBlockId();
    BasicBlock* bb = new BasicBlock(id, parentFunction, name);
    if (parentFunction != nullptr) {
        parentFunction->AppendBasicBlock(bb);
    }
    mBasicBlocks.emplace(id, bb);
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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


InstructionAshr* IRBuilder::CreateAshr() {
    return CreateAshr(nullptr, nullptr, nullptr, nullptr);
}

InstructionAshr* IRBuilder::CreateAshr(Value* input1, Value* input2, Value* output) {
    return CreateAshr(nullptr, input1, input2, output);
}

InstructionAshr* IRBuilder::CreateAshr(BasicBlock* parentBasicBlock) {
    return CreateAshr(parentBasicBlock, nullptr, nullptr, nullptr);
}

InstructionAshr* IRBuilder::CreateAshr(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output) {
    InstructionId id = GenerateNewInstructionId();
    InstructionAshr* inst = new InstructionAshr(id, input1, input2, output);
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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
    InstructionId id = GenerateNewInstructionId();
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

InstructionRet* IRBuilder::CreateRet(Value* returnValue) {
    return CreateRet(nullptr, returnValue);
}

InstructionRet* IRBuilder::CreateRet(BasicBlock* parentBasicBlock) {
    return CreateRet(parentBasicBlock, nullptr);
}

InstructionRet* IRBuilder::CreateRet(BasicBlock* parentBasicBlock, Value* returnValue) {
    InstructionId id = GenerateNewInstructionId();
    InstructionRet* inst = new InstructionRet(id, returnValue);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (returnValue != nullptr) {
        returnValue->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionAlloc* IRBuilder::CreateAlloc() {
    return CreateAlloc(nullptr, nullptr, ValueType::Unknown, 0);
}

InstructionAlloc* IRBuilder::CreateAlloc(Value* output, ValueType type, size_t count) {
    return CreateAlloc(nullptr, output, type, count);
}

InstructionAlloc* IRBuilder::CreateAlloc(BasicBlock* parentBasicBlock) {
    return CreateAlloc(parentBasicBlock, nullptr, ValueType::Unknown, 0);
}

InstructionAlloc* IRBuilder::CreateAlloc(BasicBlock* parentBasicBlock, Value* output, ValueType type, size_t count) {
    InstructionId id = GenerateNewInstructionId();
    InstructionAlloc* inst = new InstructionAlloc(id, output, type, count);
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

InstructionPhi* IRBuilder::CreatePhi(const std::list<Value*>& inputs, Value* output) {
    return CreatePhi(nullptr, inputs, output);
}

InstructionPhi* IRBuilder::CreatePhi(BasicBlock* parentBasicBlock) {
    return CreatePhi(parentBasicBlock, {}, nullptr);
}

InstructionPhi* IRBuilder::CreatePhi(BasicBlock* parentBasicBlock, const std::list<Value*>& inputs, Value* output) {
    InstructionId id = GenerateNewInstructionId();
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


InstructionMv* IRBuilder::CreateMv() {
    return CreateMv(nullptr, nullptr, nullptr);
}

InstructionMv* IRBuilder::CreateMv(Value* input, Value* output) {
    return CreateMv(nullptr, input, output);
}

InstructionMv* IRBuilder::CreateMv(BasicBlock* parentBasicBlock) {
    return CreateMv(parentBasicBlock, nullptr, nullptr);
}

InstructionMv* IRBuilder::CreateMv(BasicBlock* parentBasicBlock, Value* input, Value* output) {
    InstructionId id = GenerateNewInstructionId();
    InstructionMv* inst = new InstructionMv(id, input, output);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input != nullptr) {
        input->AddUser(inst);
    }
    if (output != nullptr) {
        output->SetProducer(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionNullCheck* IRBuilder::CreateNullCheck() {
    return CreateNullCheck(nullptr, nullptr);
}

InstructionNullCheck* IRBuilder::CreateNullCheck(Value* input) {
    return CreateNullCheck(nullptr, input);
}

InstructionNullCheck* IRBuilder::CreateNullCheck(BasicBlock* parentBasicBlock) {
    return CreateNullCheck(parentBasicBlock, nullptr);
}

InstructionNullCheck* IRBuilder::CreateNullCheck(BasicBlock* parentBasicBlock, Value* input) {
    InstructionId id = GenerateNewInstructionId();
    InstructionNullCheck* inst = new InstructionNullCheck(id, input);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (input != nullptr) {
        input->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


InstructionBoundsCheck* IRBuilder::CreateBoundsCheck() {
    return CreateBoundsCheck(nullptr, nullptr, nullptr);
}

InstructionBoundsCheck* IRBuilder::CreateBoundsCheck(Value* inputPtr, Value* inputArray) {
    return CreateBoundsCheck(nullptr, inputPtr, inputArray);
}

InstructionBoundsCheck* IRBuilder::CreateBoundsCheck(BasicBlock* parentBasicBlock) {
    return CreateBoundsCheck(parentBasicBlock, nullptr, nullptr);
}

InstructionBoundsCheck* IRBuilder::CreateBoundsCheck(BasicBlock* parentBasicBlock, Value* inputPtr, Value* inputArray) {
    InstructionId id = GenerateNewInstructionId();
    InstructionBoundsCheck* inst = new InstructionBoundsCheck(id, inputPtr, inputArray);
    if (parentBasicBlock != nullptr) {
        parentBasicBlock->AppendInstruction(inst);
    }
    if (inputPtr != nullptr) {
        inputPtr->AddUser(inst);
    }
    if (inputArray != nullptr) {
        inputArray->AddUser(inst);
    }
    mInstructions.insert({id, inst});
    return inst;
}


IRBuilder::~IRBuilder() {
    Cleanup();
}


Value* IRBuilder::CopyValue(Value* src) {
    if (!src) {
        return nullptr;
    }

    // Use the same constant value for every function
    if (src->HasValue()) {
        return src;
    }

    return CreateValue(src->GetValueType());
}


Instruction* IRBuilder::CopyInstruction(Instruction* src) {
    if (!src) {
        return nullptr;
    }

    switch (src->GetType()) {
        default:
        case InstructionType::Unknown:      return nullptr;
        case InstructionType::Add:          return CreateAdd();
        case InstructionType::Sub:          return CreateSub();
        case InstructionType::Mul:          return CreateMul();
        case InstructionType::Div:          return CreateDiv();
        case InstructionType::Rem:          return CreateRem();
        case InstructionType::And:          return CreateAnd();
        case InstructionType::Or:           return CreateOr();
        case InstructionType::Xor:          return CreateXor();
        case InstructionType::Shl:          return CreateShl();
        case InstructionType::Shr:          return CreateShr();
        case InstructionType::Ashr:         return CreateAshr();
        case InstructionType::Load:         return CreateLoad();
        case InstructionType::Store:        return CreateStore();
        case InstructionType::Jump:         return CreateJump();
        case InstructionType::Beq:          return CreateBeq();
        case InstructionType::Bne:          return CreateBne();
        case InstructionType::Bgt:          return CreateBgt();
        case InstructionType::Blt:          return CreateBlt();
        case InstructionType::Bge:          return CreateBge();
        case InstructionType::Ble:          return CreateBle();
        case InstructionType::Call:         return CreateCall();
        case InstructionType::Ret:          return CreateRet();
        case InstructionType::Alloc:        return CreateAlloc();
        case InstructionType::Phi:          return CreatePhi();
        case InstructionType::Mv:           return CreateMv();
        case InstructionType::NullCheck:    return CreateNullCheck();
        case InstructionType::BoundsCheck:  return CreateBoundsCheck();
    }
}


BasicBlock* IRBuilder::CopyBasicBlock(BasicBlock* src) {
    if (!src) {
        return nullptr;
    }

    std::string name = "<" + src->GetName() + "_COPY>";
    return CreateBasicBlock(src->GetName());
}


Function* IRBuilder::CopyFunction(Function* src) {
    if (!src) {
        return nullptr;
    }

    std::unordered_map<Value*, Value*> valuesMap{};
    std::unordered_map<Instruction*, Instruction*> instructionMap{};
    std::unordered_map<BasicBlock*, BasicBlock*> basicBlocksMap{};

    const auto& srcArgs = src->GetArgs();

    ValueType retType = src->GetReturnType();
    std::vector<ValueType> argsTypes{};
    for (auto* arg : srcArgs) {
        argsTypes.push_back(arg->GetValueType());
    }
    std::string name = "<" + src->GetName() + "_COPY>";

    Function* dst = CreateFunction(retType, argsTypes, name);

    const auto& dstArgs = dst->GetArgs();
    for (size_t i = 0; i < srcArgs.size(); ++i) {
        valuesMap.emplace(srcArgs[i], dstArgs[i]);
    }

    for (BasicBlock* srcBB : src->GetBasicBlocks()) {
        BasicBlock* dstBB = CopyBasicBlock(srcBB);
        basicBlocksMap.emplace(srcBB, dstBB);

        Instruction* srcInst = srcBB->Front();
        while (srcInst) {
            Instruction* dstInst = CopyInstruction(srcInst);
            instructionMap.emplace(srcInst, dstInst);

            dstBB->AppendInstruction(dstInst);
            srcInst = srcInst->GetNext();
        }

        dst->AppendBasicBlock(dstBB);
    }

    dst->SetEntryBasicBlock(basicBlocksMap[src->GetEntryBasicBlock()]);


    auto GetOrCopyValue = [&valuesMap, this](Value* src) {
        // Use the same constant value for every function
        if (src->HasValue()) {
            return src;
        }

        if (!valuesMap.contains(src)) {
            Value* dst = CopyValue(src);
            valuesMap.emplace(src, dst);
        }
        return valuesMap[src];
    };


    for (BasicBlock* srcBB : src->GetBasicBlocks()) {
        BasicBlock* dstBB = basicBlocksMap[srcBB];

        if (srcBB->GetTrueSuccessor() != nullptr) {
            dstBB->SetTrueSuccessor(basicBlocksMap[srcBB->GetTrueSuccessor()]);
        }
        if (srcBB->GetFalseSuccessor() != nullptr) {
            dstBB->SetFalseSuccessor(basicBlocksMap[srcBB->GetFalseSuccessor()]);
        }

        for (auto* srcPred : srcBB->GetPredecessors()) {
            dstBB->AddPredecessor(basicBlocksMap[srcPred]);
        }

        Instruction* srcInst = srcBB->Front();
        while (srcInst) {
            Instruction* dstInst = instructionMap[srcInst];

            if (srcInst->IsArithmetic()) {
                InstructionArithmetic* srcArith = static_cast<InstructionArithmetic*>(srcInst);
                InstructionArithmetic* dstArith = static_cast<InstructionArithmetic*>(dstInst);

                Value* srcInput1 = srcArith->GetInput1();
                Value* srcInput2 = srcArith->GetInput2();
                Value* srcOutput = srcArith->GetOutput();

                Value* dstInput1 = GetOrCopyValue(srcInput1);
                Value* dstInput2 = GetOrCopyValue(srcInput2);
                Value* dstOutput = GetOrCopyValue(srcOutput);

                dstArith->SetInput1(dstInput1);
                dstArith->SetInput2(dstInput2);
                dstArith->SetOutput(dstOutput);

                dstInput1->AddUser(dstArith);
                dstInput2->AddUser(dstArith);
                dstOutput->SetProducer(dstArith);
            }
            else if (srcInst->GetType() == InstructionType::Load) {
                InstructionLoad* srcLoad = static_cast<InstructionLoad*>(srcInst);
                InstructionLoad* dstLoad = static_cast<InstructionLoad*>(dstInst);

                Value* srcLoadPtr = srcLoad->GetLoadPtr();
                Value* srcOutput  = srcLoad->GetOutput();

                Value* dstLoadPtr = GetOrCopyValue(srcLoadPtr);
                Value* dstOutput  = GetOrCopyValue(srcOutput);

                dstLoad->SetLoadPtr(dstLoadPtr);
                dstLoad->SetOutput(dstOutput);

                dstLoadPtr->AddUser(dstLoad);
                dstOutput->SetProducer(dstLoad);
            }
            else if (srcInst->GetType() == InstructionType::Store) {
                InstructionStore* srcStore = static_cast<InstructionStore*>(srcInst);
                InstructionStore* dstStore = static_cast<InstructionStore*>(dstInst);

                Value* srcStorePtr = srcStore->GetStorePtr();
                Value* srcInput    = srcStore->GetInput();

                Value* dstStorePtr = GetOrCopyValue(srcStorePtr);
                Value* dstInput    = GetOrCopyValue(srcInput);

                dstStore->SetStorePtr(dstStorePtr);
                dstStore->SetInput(dstInput);

                dstStorePtr->AddUser(dstStore);
                dstInput->AddUser(dstStore);
            }
            else if (srcInst->GetType() == InstructionType::Jump) {
                InstructionJump* srcJump = static_cast<InstructionJump*>(srcInst);
                InstructionJump* dstJump = static_cast<InstructionJump*>(dstInst);

                BasicBlock* srcJumpBasicBlock = srcJump->GetJumpBasicBlock();
                BasicBlock* dstJumpBasicBlock = basicBlocksMap[srcJumpBasicBlock];

                dstJump->SetJumpBasicBlock(dstJumpBasicBlock);
            }
            else if (srcInst->IsBranch()) {
                InstructionBranch* srcBranch = static_cast<InstructionBranch*>(srcInst);
                InstructionBranch* dstBranch = static_cast<InstructionBranch*>(dstInst);

                Value* srcInput1 = srcBranch->GetInput1();
                Value* srcInput2 = srcBranch->GetInput2();

                Value* dstInput1 = GetOrCopyValue(srcInput1);
                Value* dstInput2 = GetOrCopyValue(srcInput2);

                dstBranch->SetInput1(dstInput1);
                dstBranch->SetInput2(dstInput2);

                dstInput1->AddUser(dstBranch);
                dstInput2->AddUser(dstBranch);


                BasicBlock* srcTrueBasicBlock  = srcBranch->GetTrueBasicBlock();
                BasicBlock* srcFalseBasicBlock = srcBranch->GetFalseBasicBlock();

                BasicBlock* dstTrueBasicBlock  = basicBlocksMap[srcTrueBasicBlock];
                BasicBlock* dstFalseBasicBlock = basicBlocksMap[srcFalseBasicBlock];

                dstBranch->SetTrueBasicBlock(dstTrueBasicBlock);
                dstBranch->SetFalseBasicBlock(dstFalseBasicBlock);
            }
            else if (srcInst->GetType() == InstructionType::Call) {
                InstructionCall* srcCall = static_cast<InstructionCall*>(srcInst);
                InstructionCall* dstCall = static_cast<InstructionCall*>(dstInst);

                dstCall->SetFunction(srcCall->GetFunction());

                Value* srcReturnValue = srcCall->GetReturnValue();
                if (srcReturnValue) {
                    Value* dstReturnValue = GetOrCopyValue(srcReturnValue);

                    dstCall->SetReturnValue(dstReturnValue);
                    dstReturnValue->SetProducer(dstCall);
                }

                const auto& srcArgs = srcCall->GetArguments();
                for (size_t i = 0; i < srcArgs.size(); ++i) {
                    Value* srcArg = srcArgs[i];
                    Value* dstArg = GetOrCopyValue(srcArg);

                    dstCall->SetArgument(i, dstArg);
                    dstArg->AddUser(dstCall);
                }
            }
            else if (srcInst->GetType() == InstructionType::Ret) {
                InstructionRet* srcRet = static_cast<InstructionRet*>(srcInst);
                InstructionRet* dstRet = static_cast<InstructionRet*>(dstInst);

                Value* srcReturnValue = srcRet->GetReturnValue();
                if (srcReturnValue) {
                    Value* dstReturnValue = GetOrCopyValue(srcReturnValue);
                    dstRet->SetReturnValue(dstReturnValue);
                    dstReturnValue->AddUser(dstRet);
                }
            }
            else if (srcInst->GetType() == InstructionType::Phi) {
                InstructionPhi* srcPhi = static_cast<InstructionPhi*>(srcInst);
                InstructionPhi* dstPhi = static_cast<InstructionPhi*>(dstInst);

                Value* srcOutput = srcPhi->GetOutput();
                Value* dstOutput = GetOrCopyValue(srcOutput);

                dstPhi->SetOutput(dstOutput);
                dstOutput->SetProducer(dstPhi);

                for (Value* srcInput : srcPhi->GetInputs()) {
                    Value* dstInput = GetOrCopyValue(srcInput);

                    dstPhi->AddInput(dstInput);
                    dstInput->AddUser(dstPhi);
                }
            }
            else if (srcInst->GetType() == InstructionType::Mv) {
                InstructionMv* srcMv = static_cast<InstructionMv*>(srcInst);
                InstructionMv* dstMv = static_cast<InstructionMv*>(dstInst);

                Value* srcInput  = srcMv->GetInput();
                Value* srcOutput = srcMv->GetOutput();

                Value* dstInput  = GetOrCopyValue(srcInput);
                Value* dstOutput = GetOrCopyValue(srcOutput);

                dstMv->SetInput(dstInput);
                dstMv->SetOutput(dstOutput);

                dstInput->AddUser(dstMv);
                dstOutput->SetProducer(dstMv);
            }
            else if (srcInst->GetType() == InstructionType::NullCheck) {
                InstructionNullCheck* srcNullCheck = static_cast<InstructionNullCheck*>(srcInst);
                InstructionNullCheck* dstNullCheck = static_cast<InstructionNullCheck*>(dstInst);

                Value* srcInput = srcNullCheck->GetInput();
                Value* dstInput = GetOrCopyValue(srcInput);

                dstNullCheck->SetInput(dstInput);
                dstInput->AddUser(dstNullCheck);
            }
            else if (srcInst->GetType() == InstructionType::BoundsCheck) {
                InstructionBoundsCheck* srcBoundsCheck = static_cast<InstructionBoundsCheck*>(srcInst);
                InstructionBoundsCheck* dstBoundsCheck = static_cast<InstructionBoundsCheck*>(dstInst);

                Value* srcInputPtr   = srcBoundsCheck->GetInputPtr();
                Value* srcInputArray = srcBoundsCheck->GetInputArray();

                Value* dstInputPtr   = GetOrCopyValue(srcInputPtr);
                Value* dstInputArray = GetOrCopyValue(srcInputArray);

                dstBoundsCheck->SetInputPtr(dstInputPtr);
                dstBoundsCheck->SetInputArray(dstInputArray);

                dstInputPtr->AddUser(dstBoundsCheck);
                dstInputArray->AddUser(dstBoundsCheck);
            }

            srcInst = srcInst->GetNext();
        }
    }

    return dst;
}


bool IRBuilder::CompareFunctions(Function* lhs, Function* rhs) const {
    if (!lhs || !rhs) {
        return false;
    }

    // Check return type
    if (lhs->GetReturnType() != rhs->GetReturnType()) {
        return false;
    }

    std::unordered_map<Value*, Value*> valuesMap{};
    std::unordered_map<Instruction*, Instruction*> instructionMap{};
    std::unordered_map<BasicBlock*, BasicBlock*> basicBlocksMap{};

    // Check parameters
    const auto& lhsArgs = lhs->GetArgs();
    const auto& rhsArgs = rhs->GetArgs();
    if (lhsArgs.size() != rhsArgs.size()) {
        return false;
    }

    for (size_t i = 0; i < lhsArgs.size(); ++i) {
        Value* lhsArg = lhsArgs[i];
        Value* rhsArg = rhsArgs[i];

        if (lhsArg->GetValueType() != rhsArg->GetValueType()) {
            return false;
        }

        valuesMap.emplace(lhsArg, rhsArg);
    }

    // Check basic blocks count
    const auto& lhsBBs = lhs->GetBasicBlocks();
    const auto& rhsBBs = rhs->GetBasicBlocks();
    if (lhsBBs.size() != rhsBBs.size()) {
        return false;
    }

    for (size_t i = 0; i < lhsBBs.size(); ++i) {
        BasicBlock* lhsBB = lhsBBs[i];
        BasicBlock* rhsBB = rhsBBs[i];

        if (lhsBB->Size() != rhsBB->Size()) {
            return false;
        }

        basicBlocksMap.emplace(lhsBB, rhsBB);
    }

    // Check entry basic blocks
    BasicBlock* lhsEntry = lhs->GetEntryBasicBlock();
    BasicBlock* rhsEntry = rhs->GetEntryBasicBlock();
    if (!lhsEntry || !rhsEntry) {
        return false;
    }

    if (basicBlocksMap[lhsEntry] != rhsEntry) {
        return false;
    }

    // Check control flow and collect instructions and values
    for (const auto& bbPair : basicBlocksMap) {
        BasicBlock* lhsBB = bbPair.first;
        BasicBlock* rhsBB = bbPair.second;

        const auto& lhsBBTrueSucc = lhsBB->GetTrueSuccessor();
        const auto& rhsBBTrueSucc = rhsBB->GetTrueSuccessor();
        if ((lhsBBTrueSucc != nullptr) != (rhsBBTrueSucc != nullptr)) {
            return false;
        }
        if (lhsBBTrueSucc && basicBlocksMap[lhsBBTrueSucc] != rhsBBTrueSucc) {
            return false;
        }

        const auto& lhsBBFalseSucc = lhsBB->GetFalseSuccessor();
        const auto& rhsBBFalseSucc = rhsBB->GetFalseSuccessor();
        if ((lhsBBFalseSucc != nullptr) != (rhsBBFalseSucc != nullptr)) {
            return false;
        }
        if (lhsBBFalseSucc && basicBlocksMap[lhsBBFalseSucc] != rhsBBFalseSucc) {
            return false;
        }

        const auto& lhsBBPreds = lhsBB->GetPredecessors();
        const auto& rhsBBPreds = rhsBB->GetPredecessors();
        if (lhsBBPreds.size() != rhsBBPreds.size()) {
            return false;
        }

        for (auto* lhsBBPred : lhsBBPreds) {
            if (!rhsBBPreds.contains(basicBlocksMap[lhsBBPred])) {
                return false;
            }
        }


        Instruction* lhsInst = lhsBB->Front();
        Instruction* rhsInst = rhsBB->Front();
        while (lhsInst && rhsInst) {
            if (lhsInst->GetType() != rhsInst->GetType()) {
                return false;
            }

            Value* lhsOutput = lhsInst->GetOutput();
            Value* rhsOutput = rhsInst->GetOutput();
            if ((lhsOutput != nullptr) != (rhsOutput != nullptr)) {
                return false;
            }
            if (lhsOutput) {
                if (valuesMap.contains(lhsOutput)) {
                    if (valuesMap[lhsOutput] != rhsOutput) {
                        return false;
                    }
                }
                else {
                    valuesMap.emplace(lhsOutput, rhsOutput);
                }
            }

            std::vector<Value*> lhsInputs{};
            std::vector<Value*> rhsInputs{};
            lhsInst->PopulateInputs(lhsInputs);
            rhsInst->PopulateInputs(rhsInputs);

            if (lhsInputs.size() != rhsInputs.size()) {
                return false;
            }

            for (size_t i = 0; i < lhsInputs.size(); ++i) {
                Value* lhsInput = lhsInputs[i];
                Value* rhsInput = rhsInputs[i];
                if (valuesMap.contains(lhsInput)) {
                    if (valuesMap[lhsInput] != rhsInput) {
                        return false;
                    }
                }
                else {
                    valuesMap.emplace(lhsInput, rhsInput);
                }
            }

            instructionMap.emplace(lhsInst, rhsInst);

            lhsInst = lhsInst->GetNext();
            rhsInst = rhsInst->GetNext();
        }
    }


    // Check data flow
    for (const auto& valuesPair : valuesMap) {
        Value* lhsValue = valuesPair.first;
        Value* rhsValue = valuesPair.second;

        if (lhsValue->GetValueType() != rhsValue->GetValueType()) {
            return false;
        }

        if (lhsValue->HasValue() != rhsValue->HasValue()) {
            return false;
        }

        if (lhsValue->HasValue() && lhsValue != rhsValue) {
            return false;
        }

        Instruction* lhsProducer = lhsValue->GetProducer();
        Instruction* rhsProducer = rhsValue->GetProducer();
        if ((lhsProducer != nullptr) != (rhsProducer != nullptr)) {
            return false;
        }

        if (lhsProducer && instructionMap[lhsProducer] != rhsProducer) {
            return false;
        }

        const auto& lhsUsers = lhsValue->GetUsers();
        const auto& rhsUsers = rhsValue->GetUsers();
        if (lhsUsers.size() != rhsUsers.size()) {
            return false;
        }

        for (auto* lhsUser : lhsUsers) {
            BasicBlock* lhsUserBB = lhsUser->GetParentBasicBlock();
            if (lhsUserBB->GetParentFunction() != lhs) {
                continue;
            }
            if (!rhsUsers.contains(instructionMap[lhsUser])) {
                return false;
            }
        }
    }

    return true;
}


void IRBuilder::PrintIR(std::ostream& out) {
    for (auto* f : mFunctions) {
        f->Print(out);
        out << "\n\n";
    }
}

void IRBuilder::PrintDebug(std::ostream& out) {
    out << "Constants:\n";
    for (const auto& pair : mValuesWithData) {
        auto* v = pair.second;
        out << "    " << v->GetValueStr() << "\n";
    }
    out << "\n\n";

    for (auto* f : mFunctions) {
        const auto& args = f->GetArgs();

        out << "Function: " << f->GetName() << "\n";

        out << "    Args: [ ";
        for (auto* arg : args) {
            out << arg->GetValueStr() << " ";
        }
        out << "]\n\n";

        out << "    Predecessors:\n";
        for (const auto& pair : mBasicBlocks) {
            auto* bb = pair.second;
            if (bb->GetParentFunction() != f) {
                continue;
            }

            for (const auto* pred : bb->GetPredecessors()) {
                out << "        " << pred->GetName() << " -> " << bb->GetName() << "\n";
            }
        }
        out << "\n";

        out << "    Successors:\n";
        for (const auto& pair : mBasicBlocks) {
            auto* bb = pair.second;
            if (bb->GetParentFunction() != f) {
                continue;
            }

            for (auto* succ : bb->GetSuccessors()) {
                out << "        " << bb->GetName() << " -> " << succ->GetName() << "\n";
            }
        }
        out << "\n";

        out << "    Users:\n";
        for (const auto& pair : mValuesWithData) {
            auto* v = pair.second;
            for (auto* u : v->GetUsers()) {
                BasicBlock* bb = u->GetParentBasicBlock();
                if (!bb || bb->GetParentFunction() != f) {
                    continue;
                }
                out << "        " << v->GetValueStr() << " -> [" << u->GetAsString() << "]\n";
            }

        }
        for (const auto& pair : mValues) {
            auto* v = pair.second;
            for (auto* u : v->GetUsers()) {
                BasicBlock* bb = u->GetParentBasicBlock();
                if (!bb || bb->GetParentFunction() != f) {
                    continue;
                }
                out << "        " << v->GetValueStr() << " -> [" << u->GetAsString() << "]\n";
            }

        }
        out << "\n";

        out << "    Producers:\n";
        for (const auto& pair : mValues) {
            auto* v = pair.second;
            if (auto it = std::find(args.begin(), args.end(), v); it != args.end()) {
                size_t argNo = std::distance(args.begin(), it);
                out << "        " << v->GetValueStr() << " -> [Arg#" << argNo << "]\n";
            }
            else {
                Instruction* prod = v->GetProducer();
                if (prod != nullptr) {
                    BasicBlock* bb = prod->GetParentBasicBlock();
                    if (!bb || bb->GetParentFunction() != f) {
                        continue;
                    }
                    out << "        " << v->GetValueStr() << " -> [" << prod->GetAsString() << "]\n";
                }
            }
        }
        out << "\n";

        ControlFlowGraph* graph = GetOrCreateControlFlowGraph(f);
        if (!graph->IsDominatorTreeBuilt()) {
            graph->BuildDominatorTree();
        }

        out << "    Dominance relationship:\n";
        for (auto* bb : graph->GetBasicBlocks()) {
            if (bb->GetImmediateDominator() != nullptr) {
                out << "        idom(" << bb->GetName() << ") = " << bb->GetImmediateDominator()->GetName() << "\n";
            }
            else {
                out << "        idom(" << bb->GetName() << ") = null\n";
            }
        }
        out << "\n";

        for (auto* bb : graph->GetBasicBlocks()) {
            auto& allDominated = bb->GetDominatedBasicBlocks();
            out << "        " << bb->GetName() << " >> [";
            for (auto it = allDominated.begin(), end = allDominated.end(); it != end; ++it) {
                if (it != allDominated.begin()) {
                    out << ", ";
                }
                out << (*it)->GetName();
            }
            out << "]\n";
        }
        out << "\n";

        LivenessAnalyzer* livenessAnalyzer = GetOrCreateLivenessAnalyzer(graph);
        if (!livenessAnalyzer->IsAnalysisDone()) {
            livenessAnalyzer->PerformLivenessAnalysis();
        }

        out << "    Liveness:\n";
        out << "        Linear order:\n";
        for (const auto* bb : livenessAnalyzer->GetBasicBlocksLinearOrder()) {
            if (bb->GetParentFunction() != f) {
                continue;
            }

            out << "            " << bb->GetName() << ":\n";
            out << std::right << std::setw(70) << "live: " << bb->GetLiveRange().start << "\n";
            auto* inst = bb->Front();
            while (inst) {
                out << "                " << std::left << std::setw(44) << inst->GetAsString() << "    live: " << std::left << std::setw(8) << std::to_string(inst->GetLiveNumber()) + "," << "lin: " << inst->GetLinearNumber() << "\n";
                inst = inst->GetNext();
            }
            out << std::right << std::setw(70) << "live: " << bb->GetLiveRange().end << "\n";
        }
        out << "\n";

        out << "        Live intervals:\n";
        for (const auto& pair : mValues) {
            auto* v = pair.second;
            auto it = std::find(args.begin(), args.end(), v);
            if (it == args.end()) {
                Instruction* prod = v->GetProducer();
                if (!prod) {
                    continue;
                }

                BasicBlock* bb = prod->GetParentBasicBlock();
                if (!bb || bb->GetParentFunction() != f) {
                    continue;
                }
            }

            out << "            " << v->GetValueStr() << ": [" << v->GetLiveInterval().start << ", " << v->GetLiveInterval().end << ")\n";
        }
        out << "\n";

        if (auto it = mRegisterAllocators.find(graph); it != mRegisterAllocators.cend()) {
            RegisterAllocator* registerAllocator = it->second;

            out << "    Register Allocation (GPR = " << registerAllocator->GetGPRegisterCount() << ", FPR = " << registerAllocator->GetFPRegisterCount() << "):\n";
            for (const auto& pair : mValues) {
                auto* v = pair.second;

                // TODO: function argument values. Should I account for function argument values? Or their locations are assured by calling convention?
                Instruction* prod = v->GetProducer();
                if (!prod) {
                    continue;
                }

                BasicBlock* bb = prod->GetParentBasicBlock();
                if (!bb || bb->GetParentFunction() != f) {
                    continue;
                }

                Location loc = v->GetLocation();

                out << "        " << v->GetValueStr() << " -> ";
                if (std::holds_alternative<GPRegisterLocation>(loc)) {
                    out << "r" << std::get<GPRegisterLocation>(loc).registerId;
                }
                else if (std::holds_alternative<FPRegisterLocation>(loc)) {
                    out << "f" << std::get<FPRegisterLocation>(loc).registerId;
                }
                else {
                    out << "s" << std::get<StackLocation>(loc).stackLocationId;
                }
                out << "\n";
            }
            out << "\n";
        }
    }
}

}   // namespace VMIR
