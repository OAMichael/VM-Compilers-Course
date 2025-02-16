#include <Instruction.h>
#include <BasicBlock.h>
#include <Function.h>

namespace VMIR {

bool Instruction::IsDominatedBy(Instruction* inst) {
    BasicBlock* instBB = inst->GetParentBasicBlock();
    if (mParentBasicBlock != instBB) {
        return mParentBasicBlock->IsDominatedBy(instBB);
    }
    return mParentBasicBlock->IndexOfInstruction(inst) > mParentBasicBlock->IndexOfInstruction(this);
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
    return mId != -1 && mParentBasicBlock != nullptr
        && mInput1 != nullptr && mInput2 != nullptr && mOutput != nullptr
        && mInput1->IsValid() && mInput2->IsValid() && mOutput->IsValid()
        && mInput1->GetValueType() == mInput2->GetValueType()
        && mInput1->GetValueType() == mOutput->GetValueType()
        && mInput1->GetValueType() != ValueType::Unknown;
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

    if (mFunction->GetReturnType().has_value() != (mOutput != nullptr)) {
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


std::string InstructionAlloc::GetAsString() const {
    std::string outStr = "";

    const std::string outName = mOutput->GetValueStr();
    const std::string outTypeStr = ValueTypeToIdStr(mOutput->GetValueType());
    outStr += outName + " = Alloc " + outTypeStr;
    
    if (mCount > 1) {
        outStr += ", " + std::to_string(mCount);
    }

    return outStr;
}

bool InstructionAlloc::IsValid() const {
    return mId != -1 && mParentBasicBlock != nullptr
        && mOutput != nullptr && mOutput->IsValid()
        && mOutput->GetValueType() == ValueType::Pointer && mCount > 0;
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

}   // namespace VMIR
