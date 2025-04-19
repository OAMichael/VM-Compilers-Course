#include <Instruction.h>
#include <BasicBlock.h>
#include <Function.h>

namespace VMIR {

bool Instruction::IsDominatedBy(Instruction* inst) {
    BasicBlock* instBB = inst->GetParentBasicBlock();
    if (mParentBasicBlock != instBB) {
        return mParentBasicBlock->IsDominatedBy(instBB);
    }
    return mParentBasicBlock->IndexOfInstruction(inst) < mParentBasicBlock->IndexOfInstruction(this);
}


std::string InstructionArithmetic::GetAsString() const {
    const std::string in1Name = mInput1->GetValueStr();
    const std::string in2Name = mInput2->GetValueStr();
    const std::string outName = mOutput->GetValueStr();
    const std::string in1IdStr = ValueTypeToIdStr(mInput1->GetValueType());
    const std::string opTypeStr = InstructionTypeToStr(mType);
    return outName + " = " + opTypeStr + " " + in1IdStr + " " + in1Name + ", " + in2Name;
}

bool InstructionArithmetic::IsValid() const {
    auto IsValidSimple = [](Value* input1, Value* input2, Value* output) {
        return input1->GetValueType() == input2->GetValueType()
            && input1->GetValueType() == output->GetValueType();
    };

    auto IsValidPointer = [](Value* input1, Value* input2, Value* output, InstructionType type) {
        if (type != InstructionType::Add && type != InstructionType::Sub) {
            return false;
        }
        if (!output->IsPointer()) {
            return false;
        }
        return (input1->IsPointer() && input2->IsIntegralValueType())
            || (input1->IsIntegralValueType() && input2->IsPointer());
    };

    return mId != -1 && mParentBasicBlock != nullptr
        && mInput1 != nullptr && mInput2 != nullptr && mOutput != nullptr
        && mInput1->IsValid() && mInput2->IsValid() && mOutput->IsValid()
        && mInput1->GetValueType() != ValueType::Unknown
        && mInput2->GetValueType() != ValueType::Unknown
        && mOutput->GetValueType() != ValueType::Unknown
        && (IsValidSimple(mInput1, mInput2, mOutput) || IsValidPointer(mInput1, mInput2, mOutput, mType));
}

void InstructionArithmetic::PopulateInputs(std::vector<Value*>& inputs) const {
    inputs.push_back(mInput1);
    inputs.push_back(mInput2);
}


bool InstructionBitwise::IsValid() const {
    return InstructionArithmetic::IsValid()
        && mOutput->GetValueType() != ValueType::Float32
        && mOutput->GetValueType() != ValueType::Float64
        && mOutput->GetValueType() != ValueType::Pointer;
}


std::string InstructionLoad::GetAsString() const {
    const std::string outName = mOutput->GetValueStr();
    const std::string outTypeStr = ValueTypeToIdStr(mOutput->GetValueType());
    const std::string loadPtrName = mLoadPtr->GetValueStr();
    return outName + " = Load " + outTypeStr + ", ptr " + loadPtrName;
}

bool InstructionLoad::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr
        && mLoadPtr != nullptr && mOutput != nullptr
        && mLoadPtr->IsValid() && mOutput->IsValid()
        && mLoadPtr->GetValueType() == ValueType::Pointer;
}

void InstructionLoad::PopulateInputs(std::vector<Value*>& inputs) const {
    inputs.push_back(mLoadPtr);
}


std::string InstructionStore::GetAsString() const {
    const std::string inName = mInput->GetValueStr();
    const std::string inTypeStr = ValueTypeToIdStr(mInput->GetValueType());
    const std::string storePtrName = mStorePtr->GetValueStr();
    return "Store " + inTypeStr + " " + inName + ", ptr " + storePtrName;
}

bool InstructionStore::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr
        && mStorePtr != nullptr && mInput != nullptr
        && mStorePtr->IsValid() && mInput->IsValid()
        && mStorePtr->GetValueType() == ValueType::Pointer;
}

void InstructionStore::PopulateInputs(std::vector<Value*>& inputs) const {
    inputs.push_back(mStorePtr);
    inputs.push_back(mInput);
}


std::string InstructionJump::GetAsString() const {
    return "Jump #" + mJumpBB->GetName();
}

bool InstructionJump::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr && mJumpBB != nullptr;
}


std::string InstructionBranch::GetAsString() const {
    const std::string in1Name = mInput1->GetValueStr();
    const std::string in2Name = mInput2->GetValueStr();
    const std::string typeIdStr = ValueTypeToIdStr(mInput1->GetValueType());
    const std::string trueBBName = mTrueBB->GetName();
    const std::string falseBBName = mFalseBB->GetName();
    const std::string opTypeStr = InstructionTypeToStr(mType);
    return opTypeStr + " " + typeIdStr + " " + in1Name + ", " + in2Name + " ? #" + trueBBName + " : #" + falseBBName;
}

bool InstructionBranch::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr
        && mInput1 != nullptr && mInput2 != nullptr
        && mInput1->IsValid() && mInput2->IsValid()
        && mInput1->GetValueType() == mInput2->GetValueType()
        && mTrueBB != nullptr && mFalseBB != nullptr;
}

void InstructionBranch::PopulateInputs(std::vector<Value*>& inputs) const {
    inputs.push_back(mInput1);
    inputs.push_back(mInput2);
}


std::string InstructionCall::GetAsString() const {
    std::string outStr = "";
    if (mOutput != nullptr) {
        const std::string outName = mOutput->GetValueStr();
        outStr += outName + " = ";
    }

    outStr += "Call ";
    if (mOutput != nullptr) {
        const std::string retTypeStr = ValueTypeToIdStr(mOutput->GetValueType());
        outStr += retTypeStr + " ";
    }

    outStr += "#" + mFunction->GetName() + "(";
    for (size_t i = 0; i < mInputs.size(); ++i) {
        if (i > 0) {
            outStr += ", ";
        }
        const std::string argTypeStr = ValueTypeToIdStr(mInputs[i]->GetValueType());
        const std::string argName = mInputs[i]->GetValueStr();
        outStr += argTypeStr + " " + argName;
    }
    outStr += ")";

    return outStr;
}


bool InstructionCall::IsValid() const {
    if (mId == -1) {
        return false;
    }

    if (mFunction == nullptr) {
        return false;
    }

    if ((mFunction->GetReturnType() != ValueType::Void) != (mOutput != nullptr)) {
        return false;
    }

    if ((mOutput != nullptr) && (!mOutput->IsValid() || (mFunction->GetReturnType() != mOutput->GetValueType()))) {
        return false;
    }

    if (mFunction->GetArgs().size() != mInputs.size()) {
        return false;
    }

    for (size_t i = 0; i < mInputs.size(); ++i) {
        if (!mFunction->GetArg(i)->IsValid() || !mInputs[i]->IsValid()) {
            return false;
        }
        if (mFunction->GetArg(i)->GetValueType() != mInputs[i]->GetValueType()) {
            return false;
        }
    }

    return true;
}

void InstructionCall::PopulateInputs(std::vector<Value*>& inputs) const {
    inputs = mInputs;
}


std::string InstructionRet::GetAsString() const {
    std::string outStr = "Ret";
    if (mReturnValue != nullptr) {
        const std::string retTypeStr = ValueTypeToIdStr(mReturnValue->GetValueType());
        const std::string retName = mReturnValue->GetValueStr();
        outStr += " " + retTypeStr + " " + retName;
    }
    return outStr;
}

bool InstructionRet::IsValid() const {
    if (mId == -1) {
        return false;
    }

    if (mParentBasicBlock == nullptr) {
        return false;
    }

    if (mParentBasicBlock->GetParentFunction() == nullptr) {
        return false;
    }

    if ((mParentBasicBlock->GetParentFunction()->GetReturnType() != ValueType::Void) != (mReturnValue != nullptr)) {
        return false;
    }

    if ((mReturnValue != nullptr) && (!mReturnValue->IsValid() || mReturnValue->GetValueType() != mParentBasicBlock->GetParentFunction()->GetReturnType())) {
        return false;
    }

    return true;
}

void InstructionRet::PopulateInputs(std::vector<Value*>& inputs) const {
    if (mReturnValue) {
        inputs.push_back(mReturnValue);
    }
}


std::string InstructionAlloc::GetAsString() const {
    std::string outStr = "";

    const std::string outName = mOutput->GetValueStr();
    const std::string valueTypeStr = ValueTypeToIdStr(mValueType);
    outStr += outName + " = Alloc " + valueTypeStr;
    
    if (mCount > 1) {
        outStr += ", " + std::to_string(mCount);
    }

    return outStr;
}

bool InstructionAlloc::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr
        && mOutput != nullptr && mOutput->IsValid()
        && mOutput->GetValueType() == ValueType::Pointer
        && mValueType != ValueType::Unknown && mCount > 0;
}


std::string InstructionPhi::GetAsString() const {
    std::string outStr = "";
    const std::string outName = mOutput->GetValueStr();
    const std::string idStr = ValueTypeToIdStr(mOutput->GetValueType());

    outStr = outName + " = Phi " + idStr + " ";
    for (auto it = mInputs.begin(), end = mInputs.end(); it != end; ++it) {
        if (it != mInputs.begin()) {
            outStr += ", ";
        }
        outStr += (*it)->GetValueStr();
    }

    return outStr;
}

bool InstructionPhi::IsValid() const {
    if (mId == -1) {
        return false;
    }

    if (mParentBasicBlock == nullptr) {
        return false;
    }

    if (mOutput == nullptr || mInputs.size() < 2) {
        return false;
    }

    if (!mOutput->IsValid()) {
        return false;
    }

    for (auto* i : mInputs) {
        if (!i->IsValid()) {
            return false;
        }
        if (i->GetValueType() != mOutput->GetValueType()) {
            return false;
        }
    }

    return true;
}

void InstructionPhi::PopulateInputs(std::vector<Value*>& inputs) const {
    for (auto* i : mInputs) {
        inputs.push_back(i);
    }
}


std::string InstructionMv::GetAsString() const {
    const std::string inName = mInput->GetValueStr();
    const std::string outName = mOutput->GetValueStr();
    const std::string inIdStr = ValueTypeToIdStr(mInput->GetValueType());
    const std::string opTypeStr = InstructionTypeToStr(mType);
    return outName + " = " + opTypeStr + " " + inIdStr + " " + inName;
}

bool InstructionMv::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr
        && mInput != nullptr && mOutput != nullptr
        && mInput->IsValid() && mOutput->IsValid()
        && mInput->GetValueType() == mOutput->GetValueType()
        && mInput->GetValueType() != ValueType::Unknown;
}

void InstructionMv::PopulateInputs(std::vector<Value*>& inputs) const {
    inputs.push_back(mInput);
}


std::string InstructionNullCheck::GetAsString() const {
    const std::string inName = mInput->GetValueStr();
    const std::string inIdStr = ValueTypeToIdStr(mInput->GetValueType());
    return "NullCheck " + inIdStr + " " + inName;
}

bool InstructionNullCheck::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr
        && mInput != nullptr && mInput->IsValid()
        && mInput->GetValueType() == ValueType::Pointer;
}

void InstructionNullCheck::PopulateInputs(std::vector<Value*>& inputs) const {
    inputs.push_back(mInput);
}


std::string InstructionBoundsCheck::GetAsString() const {
    const std::string ptrName = mInputPtr->GetValueStr();
    const std::string arrayName = mInputArray->GetValueStr();
    const std::string ptrIdStr = ValueTypeToIdStr(mInputPtr->GetValueType());
    size_t arraySize = static_cast<InstructionAlloc*>(mInputArray->GetProducer())->GetCount();
    return "BoundsCheck " + ptrIdStr + " " + ptrName + ", [ptr " + arrayName + ", " + std::to_string(arraySize) + "]";
}

bool InstructionBoundsCheck::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr
        && mInputPtr != nullptr && mInputArray != nullptr
        && mInputPtr->IsValid() && mInputArray->IsValid()
        && mInputPtr->GetValueType() == ValueType::Pointer
        && mInputArray->GetValueType() == ValueType::Pointer
        && mInputArray->GetProducer() != nullptr
        && mInputArray->GetProducer()->GetType() == InstructionType::Alloc
        && mInputArray->GetProducer()->IsValid();
}

void InstructionBoundsCheck::PopulateInputs(std::vector<Value*>& inputs) const {
    inputs.push_back(mInputPtr);
    inputs.push_back(mInputArray);
}

}   // namespace VMIR
