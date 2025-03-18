#include <ConstantFoldingPass.h>
#include <IRBuilder.h>

namespace VMIR {

static const char CONSTANT_FOLDING_PASS_NAME[] = "Constant Folding Pass";

ConstantFoldingPass::ConstantFoldingPass() : Pass(CONSTANT_FOLDING_PASS_NAME) {}

void ConstantFoldingPass::Run(Function* func) const {
    for (auto* bb : func->GetBasicBlocks()) {
        Instruction* inst = bb->Front();
        while (inst) {
            inst = OptimizeInstructionAndGetNext(inst);
        }
    }
}


/*
    Optimize in a tree like structure:
    - If instruction is arithmetic with both inputs appeared to be constant, replace it with move with resulting constant
    - If instruction is Mv with constant (was or appeared to be such due to optimization above), then iterate over all its users recursively
        - If user is arithmetic with both inputs appeared to be constant, replace it with move with resulting constant
        - If user is Mv with constant (was or appeared to be such due to optimization above), then iterate over all its users recursively
            - ...
            - ...
                - ...
                - ...
*/
Instruction* ConstantFoldingPass::OptimizeInstructionAndGetNext(Instruction* inst) const {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    Instruction* currInst = inst;
    if (currInst->IsArithmetic()) {
        InstructionArithmetic* instArith = static_cast<InstructionArithmetic*>(currInst);
        currInst = OptimizeInstructionArithmetic(instArith);
    }

    // If current instruction is Mv with constant (was or appeared to be such due to optimization above),
    // then iterate over all users and inplace the constant (except for phi)
    Instruction* next = currInst->GetNext();
    if (currInst->GetType() == InstructionType::Mv) {
        InstructionMv* instMv = static_cast<InstructionMv*>(currInst);
        Value* inputMv = instMv->GetInput();
        Value* outputMv = instMv->GetOutput();
        if (inputMv->HasValue()) {
            BasicBlock* bb = currInst->GetParentBasicBlock();
            auto users = outputMv->GetUsers();
            for (Instruction* user : users) {
                // Do not touch phi at all
                if (user->GetType() != InstructionType::Phi) {
                    ReplaceMoveUserInputWithConstant(user, instMv);
                }

                // If user is arithmetic or move, then optimize it recursively
                if (user->IsArithmetic() || user->GetType() == InstructionType::Mv) {
                    OptimizeInstructionAndGetNext(user);
                }
            }

            next = instMv->GetNext();
            if (outputMv->GetUsers().empty()) {
                inputMv->RemoveUser(instMv);
                outputMv->SetProducer(nullptr);
                bb->RemoveInstruction(instMv);
                IrBuilder->RemoveInstruction(instMv);
            }
        }
    }

    return next;
}


Instruction* ConstantFoldingPass::OptimizeInstructionArithmetic(InstructionArithmetic* inst) const {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    Value* input1 = inst->GetInput1();
    Value* input2 = inst->GetInput2();
    Value* output = inst->GetOutput();
    if (!input1->HasValue() || !input2->HasValue()) {
        return inst;
    }

    BasicBlock* bb = inst->GetParentBasicBlock();
    Value* inputMv = GetOrCreateConstantForMove(input1, input2, inst->GetType());

    InstructionMv* instMv = IrBuilder->CreateMv();
    instMv->SetInput(inputMv);
    instMv->SetOutput(output);

    input1->RemoveUser(inst);
    input2->RemoveUser(inst);

    inputMv->AddUser(instMv);
    output->SetProducer(instMv);

    bb->InsertInstructionBefore(instMv, inst);
    bb->RemoveInstruction(inst);

    IrBuilder->RemoveInstruction(inst);

    return instMv;
}


Value* ConstantFoldingPass::GetOrCreateConstantForMove(Value* input1, Value* input2, InstructionType op) const {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    switch (input1->GetValueType()) {
        default: {
            return nullptr;
        }
        case ValueType::Int8: {
            int8_t a = input1->GetValue<int8_t>().value();
            int8_t b = input2->GetValue<int8_t>().value();
            int8_t res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<int8_t>(res);
        }
        case ValueType::Int16: {
            int16_t a = input1->GetValue<int16_t>().value();
            int16_t b = input2->GetValue<int16_t>().value();
            int16_t res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<int16_t>(res);
        }
        case ValueType::Int32: {
            int32_t a = input1->GetValue<int32_t>().value();
            int32_t b = input2->GetValue<int32_t>().value();
            int32_t res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<int32_t>(res);
        }
        case ValueType::Int64: {
            int64_t a = input1->GetValue<int64_t>().value();
            int64_t b = input2->GetValue<int64_t>().value();
            int64_t res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<int64_t>(res);
        }
        case ValueType::Uint8: {
            uint8_t a = input1->GetValue<uint8_t>().value();
            uint8_t b = input2->GetValue<uint8_t>().value();
            uint8_t res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<uint8_t>(res);
        }
        case ValueType::Uint16: {
            uint16_t a = input1->GetValue<uint16_t>().value();
            uint16_t b = input2->GetValue<uint16_t>().value();
            uint16_t res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<uint16_t>(res);
        }
        case ValueType::Uint32: {
            uint32_t a = input1->GetValue<uint32_t>().value();
            uint32_t b = input2->GetValue<uint32_t>().value();
            uint32_t res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<uint32_t>(res);
        }
        case ValueType::Uint64: {
            uint64_t a = input1->GetValue<uint64_t>().value();
            uint64_t b = input2->GetValue<uint64_t>().value();
            uint64_t res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<uint64_t>(res);
        }
        case ValueType::Float32: {
            float a = input1->GetValue<float>().value();
            float b = input2->GetValue<float>().value();
            float res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<float>(res);
        }
        case ValueType::Float64: {
            double a = input1->GetValue<double>().value();
            double b = input2->GetValue<double>().value();
            double res = PerformValueOperation(a, b, op);
            return IrBuilder->GetOrCreateValueWithData<double>(res);
        }
    }
}


void ConstantFoldingPass::ReplaceMoveUserInputWithConstant(Instruction* user, InstructionMv* instMv) const {
    Value* inputMv = instMv->GetInput();
    Value* outputMv = instMv->GetOutput();

    if (user->IsArithmetic()) {
        InstructionArithmetic* instArith = static_cast<InstructionArithmetic*>(user);
        Value* userInput1 = instArith->GetInput1();
        Value* userInput2 = instArith->GetInput2();

        outputMv->RemoveUser(instArith);
        inputMv->AddUser(instArith);
        if (userInput1 == outputMv) {
            instArith->SetInput1(inputMv);
        }
        if (userInput2 == outputMv) {
            instArith->SetInput2(inputMv);
        }
    }
    else if (user->GetType() == InstructionType::Load) {
        InstructionLoad* instLoad = static_cast<InstructionLoad*>(user);

        outputMv->RemoveUser(instLoad);
        inputMv->AddUser(instLoad);
        instLoad->SetLoadPtr(inputMv);
    }
    else if (user->GetType() == InstructionType::Store) {
        InstructionStore* instStore = static_cast<InstructionStore*>(user);
        Value* userInput1 = instStore->GetStorePtr();
        Value* userInput2 = instStore->GetInput();

        outputMv->RemoveUser(instStore);
        inputMv->AddUser(instStore);
        if (userInput1 == outputMv) {
            instStore->SetStorePtr(inputMv);
        }
        if (userInput2 == outputMv) {
            instStore->SetInput(inputMv);
        }
    }
    else if (user->IsBranch()) {
        InstructionBranch* instBranch = static_cast<InstructionBranch*>(user);
        Value* userInput1 = instBranch->GetInput1();
        Value* userInput2 = instBranch->GetInput2();

        outputMv->RemoveUser(instBranch);
        inputMv->AddUser(instBranch);
        if (userInput1 == outputMv) {
            instBranch->SetInput1(inputMv);
        }
        if (userInput2 == outputMv) {
            instBranch->SetInput2(inputMv);
        }
    }
    else if (user->GetType() == InstructionType::Call) {
        InstructionCall* instCall = static_cast<InstructionCall*>(user);
        auto args = instCall->GetArguments();

        outputMv->RemoveUser(instCall);
        inputMv->AddUser(instCall);
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == outputMv) {
                instCall->SetArgument(i, inputMv);
            }
        }
    }
    else if (user->GetType() == InstructionType::Ret) {
        InstructionRet* instRet = static_cast<InstructionRet*>(user);

        outputMv->RemoveUser(instRet);
        inputMv->AddUser(instRet);
        instRet->SetReturnValue(inputMv);
    }
    else if (user->GetType() == InstructionType::Mv) {
        InstructionMv* instMv = static_cast<InstructionMv*>(user);

        outputMv->RemoveUser(instMv);
        inputMv->AddUser(instMv);
        instMv->SetInput(inputMv);
    }
}


}   // namespace VMIR
