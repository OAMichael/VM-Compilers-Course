#include <PeepholesPass.h>
#include <IRBuilder.h>

namespace VMIR {

static const char PEEPHOLES_PASS_NAME[] = "Peepholes Pass";

PeepholesPass::PeepholesPass() : Pass(PEEPHOLES_PASS_NAME) {}

// Main peepholes:
/*
    Add:
        [ v2 = Add ui64 v1, 0 ]     -->     [ v2 = Mv ui64 v1 ]

        [ v2 = Add ui64 v1, v1 ]    -->     [ v2 = Shl ui64 v1, 1 ]

    Ashr:
        [ v2 = Ashr i64 v1, 0 ]     -->     [ v2 = Mv i64 v1 ]

        [ v2 = Ashr i64 v1, n ]             [ v2 = Ashr i64 v1, n ] (if v2 has users other than Shl)
        [ v3 = Shl i64 v2, n ]     -->     / [ v3 = And i64 v1, ~(2^n - 1) ], if n < type bit count  \
                                           \ [ v3 = Mv i64 0 ],               if n >= type bit count /

    And:
        [ v2 = And ui64 v1, 0 ]     -->     [ v2 = Mv ui64 0 ]
        
        [ v2 = And ui64 v1, v1 ]    -->     [ v2 = Mv ui64 v1 ]
*/
void PeepholesPass::Run(Function* func) const {
    for (auto* bb : func->GetBasicBlocks()) {
        Instruction* inst = bb->Front();
        while (inst) {
            switch (inst->GetType()) {
                default: {
                    break;
                }
                case InstructionType::Add: {
                    PerformSingleAddPeephole(static_cast<InstructionAdd*>(inst));
                    break;
                }
                case InstructionType::Ashr: {
                    if (!PerformComplexAshrPeephole(static_cast<InstructionAshr*>(inst))) {
                        PerformSingleAshrPeephole(static_cast<InstructionAshr*>(inst));
                    }
                    break;
                }
                case InstructionType::And: {
                    PerformSingleAndPeephole(static_cast<InstructionAnd*>(inst));
                    break;
                }
            }

            inst = inst->GetNext();
        }
    }
}


/*
    [ v2 = Add ui64 v1, 0 ]     -->     [ v2 = Mv ui64 v1 ]

    [ v2 = Add ui64 v1, v1 ]    -->     [ v2 = Shl ui64 v1, 1 ]
*/
bool PeepholesPass::PerformSingleAddPeephole(InstructionAdd* inst) const {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    BasicBlock* bb = inst->GetParentBasicBlock();
    Value* input1 = inst->GetInput1();
    Value* input2 = inst->GetInput2();
    Value* output = inst->GetOutput();

    // Check if second value is zero and replace Add with Mv ...
    if (IsValueZero(input2)) {
        InstructionMv* instMv = IrBuilder->CreateMv();
        instMv->SetInput(input1);
        instMv->SetOutput(output);

        input1->RemoveUser(inst);
        input2->RemoveUser(inst);

        input1->AddUser(instMv);
        output->SetProducer(instMv);

        bb->InsertInstructionBefore(instMv, inst);
        bb->RemoveInstruction(inst);

        IrBuilder->RemoveInstruction(inst);

        return true;
    }

    // Replace Add with Shl ...
    if (input1 == input2 && input1->IsIntegralValueType()) {
        Value* shift1 = nullptr;
        switch (input1->GetValueType()) {
            default: break;
            case ValueType::Int8: {
                shift1 = IrBuilder->GetOrCreateValueWithData<int8_t>(bb->GetParentFunction(), 1);
                break;
            }
            case ValueType::Int16: {
                shift1 = IrBuilder->GetOrCreateValueWithData<int16_t>(bb->GetParentFunction(), 1);
                break;
            }
            case ValueType::Int32: {
                shift1 = IrBuilder->GetOrCreateValueWithData<int32_t>(bb->GetParentFunction(), 1);
                break;
            }
            case ValueType::Int64: {
                shift1 = IrBuilder->GetOrCreateValueWithData<int64_t>(bb->GetParentFunction(), 1);
                break;
            }
            case ValueType::Uint8: {
                shift1 = IrBuilder->GetOrCreateValueWithData<uint8_t>(bb->GetParentFunction(), 1);
                break;
            }
            case ValueType::Uint16: {
                shift1 = IrBuilder->GetOrCreateValueWithData<uint16_t>(bb->GetParentFunction(), 1);
                break;
            }
            case ValueType::Uint32: {
                shift1 = IrBuilder->GetOrCreateValueWithData<uint32_t>(bb->GetParentFunction(), 1);
                break;
            }
            case ValueType::Uint64: {
                shift1 = IrBuilder->GetOrCreateValueWithData<uint64_t>(bb->GetParentFunction(), 1);
                break;
            }
        }

        InstructionShl* instShl = IrBuilder->CreateShl();
        instShl->SetInput1(input1);
        instShl->SetInput2(shift1);
        instShl->SetOutput(output);

        // input1 == input2
        input1->RemoveUser(inst);

        input1->AddUser(instShl);
        shift1->AddUser(instShl);
        output->SetProducer(instShl);

        bb->InsertInstructionBefore(instShl, inst);
        bb->RemoveInstruction(inst);

        IrBuilder->RemoveInstruction(inst);

        return true;
    }

    return false;
}


/*
    [ v2 = Ashr i64 v1, 0 ]     -->     [ v2 = Mv i64 v1 ]
*/
bool PeepholesPass::PerformSingleAshrPeephole(InstructionAshr* inst) const {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    BasicBlock* bb = inst->GetParentBasicBlock();
    Value* input1 = inst->GetInput1();
    Value* input2 = inst->GetInput2();
    Value* output = inst->GetOutput();

    // Check if second value is zero and replace Ashr with Mv ...
    if (IsValueZero(input2)) {
        InstructionMv* instMv = IrBuilder->CreateMv();
        instMv->SetInput(input1);
        instMv->SetOutput(output);

        input1->RemoveUser(inst);
        input2->RemoveUser(inst);

        input1->AddUser(instMv);
        output->SetProducer(instMv);

        bb->InsertInstructionBefore(instMv, inst);
        bb->RemoveInstruction(inst);

        IrBuilder->RemoveInstruction(inst);

        return true;
    }

    return false;
}


// It was quite hard to invent simple peepholes with Ashr, so, we need to match a specific pattern
/*
    [ v2 = Ashr i64 v1, n ]             [ v2 = Ashr i64 v1, n ] (if v2 has users other than Shl)
    [ v3 = Shl i64 v2, n ]     -->     / [ v3 = And i64 v1, ~(2^n - 1) ], if n < type bit count  \
                                       \ [ v3 = Mv i64 0 ],               if n >= type bit count /
*/
bool PeepholesPass::PerformComplexAshrPeephole(InstructionAshr* inst) const {
    Instruction* next = inst->GetNext();
    if (!next || next->GetType() != InstructionType::Shl) {
        return false;
    }

    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    BasicBlock* bb = inst->GetParentBasicBlock();
    InstructionShl* lShift = static_cast<InstructionShl*>(next);

    Value* rShiftInput1 = inst->GetInput1();
    Value* rShiftInput2 = inst->GetInput2();
    Value* rShiftOutput = inst->GetOutput();

    Value* lShiftInput1 = lShift->GetInput1();
    Value* lShiftInput2 = lShift->GetInput2();
    Value* lShiftOutput = lShift->GetOutput();

    if (rShiftOutput->GetValueType() != lShiftOutput->GetValueType()) {
        return false;
    }

    if (!rShiftInput2->HasValue() || !lShiftInput2->HasValue() || lShiftInput1 != rShiftOutput) {
        return false;
    }


// Sorry for this mess... But...
#define SHIFT_ELIMINATE_CASE_MACRO(ValueT, CType, Bitsize)                                                                  \
    case ValueType::ValueT: {                                                                                               \
        CType rShiftAmount = rShiftInput2->GetValue<CType>().value();                                                       \
        CType lShiftAmount = lShiftInput2->GetValue<CType>().value();                                                       \
        if (rShiftAmount != lShiftAmount) {                                                                                 \
            return false;                                                                                                   \
        }                                                                                                                   \
                                                                                                                            \
        if (rShiftAmount < Bitsize) {                                                                                       \
            CType one = static_cast<CType>(1);                                                                              \
            CType andConstant = static_cast<CType>(~((one << lShiftAmount) - one));                                         \
            Value* andConstantValue = IrBuilder->GetOrCreateValueWithData<CType>(bb->GetParentFunction(), andConstant);     \
                                                                                                                            \
            InstructionAnd* instAnd = IrBuilder->CreateAnd();                                                               \
            instAnd->SetInput1(rShiftInput1);                                                                               \
            instAnd->SetInput2(andConstantValue);                                                                           \
            instAnd->SetOutput(lShiftOutput);                                                                               \
                                                                                                                            \
            lShiftInput1->RemoveUser(lShift);                                                                               \
            lShiftInput2->RemoveUser(lShift);                                                                               \
                                                                                                                            \
            rShiftInput1->AddUser(instAnd);                                                                                 \
            andConstantValue->AddUser(instAnd);                                                                             \
            lShiftOutput->SetProducer(instAnd);                                                                             \
                                                                                                                            \
            bb->InsertInstructionBefore(instAnd, lShift);                                                                   \
            bb->RemoveInstruction(lShift);                                                                                  \
                                                                                                                            \
            IrBuilder->RemoveInstruction(lShift);                                                                           \
        }                                                                                                                   \
        else {                                                                                                              \
            Value* zero = IrBuilder->GetOrCreateValueWithData<CType>(bb->GetParentFunction(), 0);                           \
                                                                                                                            \
            InstructionMv* instMv = IrBuilder->CreateMv();                                                                  \
            instMv->SetInput(zero);                                                                                         \
            instMv->SetOutput(lShiftOutput);                                                                                \
                                                                                                                            \
            lShiftInput1->RemoveUser(lShift);                                                                               \
            lShiftInput2->RemoveUser(lShift);                                                                               \
                                                                                                                            \
            zero->AddUser(instMv);                                                                                          \
            lShiftOutput->SetProducer(instMv);                                                                              \
                                                                                                                            \
            bb->InsertInstructionBefore(instMv, lShift);                                                                    \
            bb->RemoveInstruction(lShift);                                                                                  \
                                                                                                                            \
            IrBuilder->RemoveInstruction(lShift);                                                                           \
        }                                                                                                                   \
                                                                                                                            \
        /* Check if Ashr output has users after these operations */                                                         \
        if (rShiftOutput->GetUsers().empty()) {                                                                             \
            rShiftInput1->RemoveUser(inst);                                                                                 \
            rShiftInput2->RemoveUser(inst);                                                                                 \
            rShiftOutput->SetProducer(nullptr);                                                                             \
                                                                                                                            \
            bb->RemoveInstruction(inst);                                                                                    \
                                                                                                                            \
            IrBuilder->RemoveInstruction(inst);                                                                             \
        }                                                                                                                   \
        return true;                                                                                                        \
    }


    switch (rShiftInput2->GetValueType()) {
        default: return false;
        SHIFT_ELIMINATE_CASE_MACRO(Int8,   int8_t,    8);
        SHIFT_ELIMINATE_CASE_MACRO(Int16,  int16_t,  16);
        SHIFT_ELIMINATE_CASE_MACRO(Int32,  int32_t,  32);
        SHIFT_ELIMINATE_CASE_MACRO(Int64,  int64_t,  64);
        SHIFT_ELIMINATE_CASE_MACRO(Uint8,  uint8_t,   8);
        SHIFT_ELIMINATE_CASE_MACRO(Uint16, uint16_t, 16);
        SHIFT_ELIMINATE_CASE_MACRO(Uint32, uint32_t, 32);
        SHIFT_ELIMINATE_CASE_MACRO(Uint64, uint64_t, 64);
    }


#undef SHIFT_ELIMINATE_CASE_MACRO
}


/*
    [ v2 = And ui64 v1, 0 ]     -->     [ v2 = Mv ui64 0 ]

    [ v2 = And ui64 v1, v1 ]    -->     [ v2 = Mv ui64 v1 ]
*/
bool PeepholesPass::PerformSingleAndPeephole(InstructionAnd* inst) const {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    BasicBlock* bb = inst->GetParentBasicBlock();
    Value* input1 = inst->GetInput1();
    Value* input2 = inst->GetInput2();
    Value* output = inst->GetOutput();

    // Check if second value is zero and replace And with Mv ...
    if (IsValueZero(input2)) {
        InstructionMv* instMv = IrBuilder->CreateMv();
        instMv->SetInput(input2);
        instMv->SetOutput(output);

        input1->RemoveUser(inst);
        input2->RemoveUser(inst);

        input2->AddUser(instMv);
        output->SetProducer(instMv);

        bb->InsertInstructionBefore(instMv, inst);
        bb->RemoveInstruction(inst);

        IrBuilder->RemoveInstruction(inst);

        return true;
    }

    // Replace And with Mv ...
    if (input1 == input2) {
        InstructionMv* instMv = IrBuilder->CreateMv();
        instMv->SetInput(input1);
        instMv->SetOutput(output);

        // input1 == input2
        input1->RemoveUser(inst);

        input1->AddUser(instMv);
        output->SetProducer(instMv);

        bb->InsertInstructionBefore(instMv, inst);
        bb->RemoveInstruction(inst);

        IrBuilder->RemoveInstruction(inst);

        return true;
    }

    return false;
}

}   // namespace VMIR
