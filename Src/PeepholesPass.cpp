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

        [ v2 = Ashr i64 v1, n ]     -->     [ v3 = And i64 v1, ~(2^n - 1) ]
        [ v3 = Shl i64 v2, n ]

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
                    PerformSingleAshrPeephole(static_cast<InstructionAshr*>(inst));
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

void PeepholesPass::PerformSingleAddPeephole(InstructionAdd* inst) const {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    Value* input1 = inst->GetInput1();
    Value* input2 = inst->GetInput2();
    Value* output = inst->GetOutput();

    // Check if second value is zero
    if (input2->HasValue()) {
        bool isZero = false;
        switch (input2->GetValueType()) {
            default: break;
            case ValueType::Int8: {
                isZero = input2->GetValue<int8_t>() == int8_t(0);
                break;
            }
            case ValueType::Int16: {
                isZero = input2->GetValue<int16_t>() == int16_t(0);
                break;
            }
            case ValueType::Int32: {
                isZero = input2->GetValue<int32_t>() == int32_t(0);
                break;
            }
            case ValueType::Int64: {
                isZero = input2->GetValue<int64_t>() == int64_t(0);
                break;
            }
            case ValueType::Uint8: {
                isZero = input2->GetValue<uint8_t>() == uint8_t(0);
                break;
            }
            case ValueType::Uint16: {
                isZero = input2->GetValue<uint16_t>() == uint16_t(0);
                break;
            }
            case ValueType::Uint32: {
                isZero = input2->GetValue<uint32_t>() == uint32_t(0);
                break;
            }
            case ValueType::Uint64: {
                isZero = input2->GetValue<uint64_t>() == uint64_t(0);
                break;
            }
            case ValueType::Float32: {
                isZero = input2->GetValue<float>() == float(0.0f);
                break;
            }
            case ValueType::Float64: {
                isZero = input2->GetValue<double>() == double(0.0);
                break;
            }
        }

        // Replace Add with Mv ...
        if (isZero) {
            BasicBlock* bb = inst->GetParentBasicBlock();

            InstructionMv* instMv = IrBuilder->CreateMv();
            instMv->SetInput(input1);
            instMv->SetOutput(output);

            input1->RemoveUser(inst);
            input2->RemoveUser(inst);
            output->RemoveUser(inst);

            input1->AddUser(instMv);
            output->AddUser(instMv);

            bb->InsertInstructionBefore(instMv, inst);
            bb->RemoveInstruction(inst);

            IrBuilder->RemoveInstruction(inst);
        }
    }

    // Replace Add with Shl ...
    if (input1 == input2 && input1->IsIntegralValueType()) {
        BasicBlock* bb = inst->GetParentBasicBlock();

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

        input1->RemoveUser(inst);
        input2->RemoveUser(inst);
        output->RemoveUser(inst);

        input1->AddUser(instShl);
        shift1->AddUser(instShl);
        output->AddUser(instShl);

        bb->InsertInstructionBefore(instShl, inst);
        bb->RemoveInstruction(inst);

        IrBuilder->RemoveInstruction(inst);
    }
}

void PeepholesPass::PerformSingleAshrPeephole(InstructionAshr* inst) const {
    (void)inst;
}

void PeepholesPass::PerformSingleAndPeephole(InstructionAnd* inst) const {
    (void)inst;
}

}   // namespace VMIR
