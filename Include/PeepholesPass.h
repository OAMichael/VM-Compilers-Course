#ifndef PEEPHOLES_PASS_H
#define PEEPHOLES_PASS_H

#include <Pass.h>
#include <IRBuilder.h>

namespace VMIR {

class PeepholesPass : public Pass {
public:
    PeepholesPass();

    virtual void Run(Function* func) override;

private:
    bool PerformSingleAddPeephole(InstructionAdd* inst) const;

    bool PerformSingleAshrPeephole(InstructionAshr* inst) const;
    bool PerformComplexAshrPeephole(InstructionAshr* inst) const;

    bool PerformSingleAndPeephole(InstructionAnd* inst) const;

    template <typename T>
    bool AreValuesHoldSameConstants(Value* v1, Value* v2) const {
        T data1 = v1->GetValue<T>().value();
        T data2 = v2->GetValue<T>().value();
        return data1 == data2;
    }

    template <typename T>
    void ReplaceShlWithMvOrZero(InstructionAshr* rShift, InstructionShl* lShift) const {
        IRBuilder* IrBuilder = IRBuilder::GetInstance();

        BasicBlock* bb = rShift->GetParentBasicBlock();

        Value* rShiftInput1 = rShift->GetInput1();

        Value* lShiftInput1 = lShift->GetInput1();
        Value* lShiftInput2 = lShift->GetInput2();
        Value* lShiftOutput = lShift->GetOutput();

        T shiftAmount = lShiftInput2->GetValue<T>().value();

        constexpr T bitSize = 8 * sizeof(T);
        if (shiftAmount < bitSize) {
            T one = static_cast<T>(1);
            T andConstant = static_cast<T>(~((one << shiftAmount) - one));
            Value* andConstantValue = IrBuilder->GetOrCreateValueWithData<T>(andConstant);

            InstructionAnd* instAnd = IrBuilder->CreateAnd();
            instAnd->SetInput1(rShiftInput1);
            instAnd->SetInput2(andConstantValue);
            instAnd->SetOutput(lShiftOutput);

            lShiftInput1->RemoveUser(lShift);
            lShiftInput2->RemoveUser(lShift);

            rShiftInput1->AddUser(instAnd);
            andConstantValue->AddUser(instAnd);
            lShiftOutput->SetProducer(instAnd);

            bb->InsertInstructionBefore(instAnd, lShift);
            bb->RemoveInstruction(lShift);

            IrBuilder->RemoveInstruction(lShift);
        }
        else {
            Value* zero = IrBuilder->GetOrCreateValueWithData<T>(0);

            InstructionMv* instMv = IrBuilder->CreateMv();
            instMv->SetInput(zero);
            instMv->SetOutput(lShiftOutput);

            lShiftInput1->RemoveUser(lShift);
            lShiftInput2->RemoveUser(lShift);

            zero->AddUser(instMv);
            lShiftOutput->SetProducer(instMv);

            bb->InsertInstructionBefore(instMv, lShift);
            bb->RemoveInstruction(lShift);

            IrBuilder->RemoveInstruction(lShift);
        }
    }
};

}   // namespace VMIR

#endif  // PEEPHOLES_PASS_H
