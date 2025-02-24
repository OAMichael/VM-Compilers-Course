#include <PeepholesPass.h>

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

    switch (rShiftInput2->GetValueType()) {
        default: break;
        case ValueType::Int8: {
            if (AreValuesHoldSameConstants<int8_t>(rShiftInput2, lShiftInput2)) {
                ReplaceShlWithMvOrZero<int8_t>(inst, lShift);
            }
            break;
        }
        case ValueType::Int16: {
            if (AreValuesHoldSameConstants<int16_t>(rShiftInput2, lShiftInput2)) {
                ReplaceShlWithMvOrZero<int16_t>(inst, lShift);
            }
            break;
        }
        case ValueType::Int32: {
            if (AreValuesHoldSameConstants<int32_t>(rShiftInput2, lShiftInput2)) {
                ReplaceShlWithMvOrZero<int32_t>(inst, lShift);
            }
            break;
        }
        case ValueType::Int64: {
            if (AreValuesHoldSameConstants<int64_t>(rShiftInput2, lShiftInput2)) {
                ReplaceShlWithMvOrZero<int64_t>(inst, lShift);
            }
            break;
        }
        case ValueType::Uint8: {
            if (AreValuesHoldSameConstants<uint8_t>(rShiftInput2, lShiftInput2)) {
                ReplaceShlWithMvOrZero<uint8_t>(inst, lShift);
            }
            break;
        }
        case ValueType::Uint16: {
            if (AreValuesHoldSameConstants<uint16_t>(rShiftInput2, lShiftInput2)) {
                ReplaceShlWithMvOrZero<uint16_t>(inst, lShift);
            }
            break;
        }
        case ValueType::Uint32: {
            if (AreValuesHoldSameConstants<uint32_t>(rShiftInput2, lShiftInput2)) {
                ReplaceShlWithMvOrZero<uint32_t>(inst, lShift);
            }
            break;
        }
        case ValueType::Uint64: {
            if (AreValuesHoldSameConstants<uint64_t>(rShiftInput2, lShiftInput2)) {
                ReplaceShlWithMvOrZero<uint64_t>(inst, lShift);
            }
            break;
        }
    }

    // Check if Ashr output has users after these operations and if not, erase the instruction
    if (rShiftOutput->GetUsers().empty()) {
        rShiftInput1->RemoveUser(inst);
        rShiftInput2->RemoveUser(inst);
        rShiftOutput->SetProducer(nullptr);

        bb->RemoveInstruction(inst);

        IrBuilder->RemoveInstruction(inst);
    }

    return true;
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
