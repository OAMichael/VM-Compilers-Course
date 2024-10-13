#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <vector>
#include <ostream>

#include <Instruction.h>

namespace VMIR {

class Function;

class BasicBlock {
public:
    BasicBlock(const Function* parentFunction = nullptr, const BasicBlockId id = -1, const std::string& name = "")
    : mParentFunction{parentFunction}, mId{id}, mName{name} {};

    inline const Function *GetParentFunction() const { return mParentFunction; }
    inline void SetParentFunction(const Function* parentFunction) { mParentFunction = parentFunction; }

    inline BasicBlockId GetId() const { return mId; }
    inline void SetId(const BasicBlockId id) { mId = id; }

    inline std::string GetName() const { return mName.length() > 0 ? (mName) : ("<Unnamed BB#" + std::to_string(mId) + ">"); }
    inline void SetName(const std::string& name) { mName = name; }

    inline Instruction* Front() const { return mFirstInst; }
    inline Instruction* Back()  const { return mLastInst; }
    inline size_t Size() const { return mSize; }

    Instruction *GetInstruction(const size_t idx) const {
        if (idx >= mSize) {
            return nullptr;
        }

        Instruction* ret = mFirstInst;
        for (size_t i = idx; i > 0; --i) {
            ret = ret->GetNext();
        }
        return ret;
    }

    std::optional<size_t> IndexOfInstruction(Instruction* inst) const {
        if (inst == nullptr || inst->GetParentBasicBlock() != this || mSize == 0) {
            return std::nullopt;
        }

        size_t idx = 0;
        Instruction* bbInst = mFirstInst;
        while (bbInst != nullptr && bbInst != inst) {
            bbInst = bbInst->GetNext();
            ++idx;
        }

        if (idx >= mSize) {
            return std::nullopt;
        }
        return idx;
    }

    void PrependInstruction(Instruction* inst) {
        if (mFirstInst == nullptr) {
            mFirstInst = mLastInst = inst;
        }
        else {
            inst->SetNext(mFirstInst);
            mFirstInst->SetPrev(inst);
            mFirstInst = inst;
        }
        inst->SetParentBasicBlock(this);
        ++mSize;
    }

    void AppendInstruction(Instruction* inst)  {
        if (mLastInst == nullptr) {
            mFirstInst = mLastInst = inst;
        }
        else {
            mLastInst->SetNext(inst);
            inst->SetPrev(mLastInst);
            mLastInst = inst;
        }
        inst->SetParentBasicBlock(this);
        ++mSize;
    }

    void InsertInstructionBefore(Instruction* inst, Instruction* position) {
        if (position == nullptr || position->GetParentBasicBlock() != this) {
            return;
        }

        Instruction* instBefore = position->GetPrev();
        if (instBefore != nullptr) {
            instBefore->SetNext(inst);
            inst->SetPrev(instBefore);
        }
        else {
            mFirstInst = inst;
        }

        inst->SetNext(position);
        position->SetPrev(inst);

        inst->SetParentBasicBlock(this);
        ++mSize;
    }

    void InsertInstructionAfter(Instruction* inst, Instruction* position) {
        if (position == nullptr || position->GetParentBasicBlock() != this) {
            return;
        }

        Instruction* instAfter = position->GetNext();
        if (instAfter != nullptr) {
            inst->SetNext(instAfter);
            instAfter->SetPrev(inst);
        }
        else {
            mLastInst = inst;
        }

        position->SetNext(inst);
        inst->SetPrev(position);

        inst->SetParentBasicBlock(this);
        ++mSize;
    }

    void RemoveInstruction(Instruction* inst) {
        if (inst == nullptr || inst->GetParentBasicBlock() != this) {
            return;
        }

        Instruction* instBefore = inst->GetPrev();
        Instruction* instAfter = inst->GetNext();

        if (instBefore != nullptr) {
            instBefore->SetNext(instAfter);
        }
        else {
            mFirstInst = instAfter;
        }

        if (instAfter != nullptr) {
            instAfter->SetPrev(instBefore);
        }
        else {
            mLastInst = instBefore;
        }

        inst->SetParentBasicBlock(nullptr);
        --mSize;
    }

    inline const std::set<BasicBlock*>& GetPredecessors() const { return mPredecessors; }

    inline void SetPredecessors(const std::set<BasicBlock*>& preds) { mPredecessors = preds; }
    inline void AddPredecessor(BasicBlock* pred)       { mPredecessors.insert(pred); }
    inline bool HasPredecessor(BasicBlock* pred) const { return mPredecessors.contains(pred); }
    inline void RemovePredecessor(BasicBlock* pred)    { mPredecessors.erase(pred); }

    inline BasicBlock* GetSuccessor()      { return mTrueSuccessor; }
    inline BasicBlock* GetTrueSuccessor()  { return mTrueSuccessor; }
    inline BasicBlock* GetFalseSuccessor() { return mFalseSuccessor; }
    inline std::pair<BasicBlock*, BasicBlock*> GetSuccessors() const { return std::make_pair(mTrueSuccessor, mFalseSuccessor); }

    inline void SetSuccessor(BasicBlock* succ)      { mTrueSuccessor = succ; }
    inline void SetTrueSuccessor(BasicBlock* succ)  { mTrueSuccessor = succ; }
    inline void SetFalseSuccessor(BasicBlock* succ) { mFalseSuccessor = succ; }

    inline bool IsMarked() const { return mMarked; }
    inline BasicBlock* GetImmediateDominator() const { return mImmediateDominator; }

    inline bool IsDominatedBy(BasicBlock* dom) {
        if (mImmediateDominator == nullptr) {
            return false;
        }
        if (mImmediateDominator == dom) {
            return true;
        }
        return mImmediateDominator->IsDominatedBy(dom);
    }

    inline void SetMarked(bool marked = true) { mMarked = marked; }
    inline void SetImmediateDominator(BasicBlock* dom) { mImmediateDominator = dom; }

    inline void Print(std::ostream& out) const {
        out << GetName() << ":";
        if (mPredecessors.size() > 0) {
            out << " (preds: ";
            for (auto it = mPredecessors.begin(), end = mPredecessors.end(); it != end; ++it) {
                if (it != mPredecessors.begin()) {
                    out << ", ";
                }
                out << (*it)->GetName();
            }
            out << ")";
        }
        out << "\n";

        Instruction* inst = mFirstInst;
        while (inst != nullptr) {
            out << "    " << inst->GetAsString() << "\n";
            inst = inst->GetNext();
        }
    }

    inline bool IsValid() const {
        if (mParentFunction == nullptr || mId == -1 || mFirstInst == nullptr) {
            return false;
        }
        if (!mLastInst->IsTerminator()) {
            return false;
        }

        Instruction* inst = mFirstInst;
        while (inst != nullptr) {
            if (!inst->IsValid()) {
                return false;
            }
            inst = inst->GetNext();
        }
        return true;
    }

private:
    const Function* mParentFunction{nullptr};
    BasicBlockId mId{-1};
    std::string mName{};

    // A basic block contains first and last element of linked list of instruction
    Instruction* mFirstInst{};
    Instruction* mLastInst{};
    size_t mSize{0};

    std::set<BasicBlock*> mPredecessors{};

    // True successor is also unconditional successor for Jump instruction
    BasicBlock* mTrueSuccessor{nullptr};
    BasicBlock* mFalseSuccessor{nullptr};

    // Information related to dominator tree
    bool mMarked{false};
    BasicBlock* mImmediateDominator{nullptr};
};

}   // namespace VMIR

#endif  // BASIC_BLOCK_H
