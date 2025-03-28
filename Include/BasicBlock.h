#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <vector>
#include <ostream>

#include <Instruction.h>

namespace VMIR {

class Function;
class Loop;


class BasicBlock {
public:
    enum Marker : uint32_t {
        None  = 0,
        Black = 1 << 0,
        Grey  = 1 << 1,
        Green = 1 << 2,

        All = Black | Grey | Green
    };

    using MarkerFlags = uint32_t;

    BasicBlock(const BasicBlockId id = -1, Function* parentFunction = nullptr, const std::string& name = "")
    : mId{id}, mParentFunction{parentFunction}, mName{name} {};

    inline Function *GetParentFunction() const { return mParentFunction; }
    inline void SetParentFunction(Function* parentFunction) { mParentFunction = parentFunction; }

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
    inline std::vector<BasicBlock*> GetSuccessors() const {
        std::vector<BasicBlock*> succs;
        if (mTrueSuccessor) {
            succs.push_back(mTrueSuccessor);
        }
        if (mFalseSuccessor) {
            succs.push_back(mFalseSuccessor);
        }
        return succs;
    }

    inline void SetSuccessor(BasicBlock* succ)      { mTrueSuccessor = succ; }
    inline void SetTrueSuccessor(BasicBlock* succ)  { mTrueSuccessor = succ; }
    inline void SetFalseSuccessor(BasicBlock* succ) { mFalseSuccessor = succ; }

    inline BasicBlock* GetImmediateDominator() const { return mImmediateDominator; }
    inline std::set<BasicBlock*>& GetDominatedBasicBlocks() { return mDominatedBlocks; }

    inline bool IsDominatedBy(BasicBlock* other) {
        if (other == nullptr) {
            return false;
        }
        return other->GetDominatedBasicBlocks().contains(this);
    }

    inline bool IsDominatorOf(BasicBlock* other) {
        if (other == nullptr) {
            return false;
        }
        return mDominatedBlocks.contains(other);
    }

    inline void SetImmediateDominator(BasicBlock* dom) { mImmediateDominator = dom; }

    inline bool IsMarked(const MarkerFlags marker = Marker::All) const { return mMarkedFlags & marker; }
    inline void SetMarked(const MarkerFlags marker = Marker::All)   { mMarkedFlags |= marker; }
    inline void SetUnmarked(const MarkerFlags marker = Marker::All) { mMarkedFlags &= ~marker; }

    inline Loop* GetLoop() const { return mLoop; }
    inline void SetLoop(Loop* loop) { mLoop = loop; }

    inline LiveRange& GetLiveRange() { return mLiveRange; }
    inline const LiveRange& GetLiveRange() const { return mLiveRange; }

    inline bool IsProducerOf(Value* value) const {
        if (value == nullptr || value->GetProducer() == nullptr || value->GetProducer()->GetParentBasicBlock() != this) {
            return false;
        }
        return true;
    }

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
        bool seenNotPhi = false;
        while (inst != nullptr) {
            if (!inst->IsValid()) {
                return false;
            }
            if (seenNotPhi && inst->IsPhi()) {
                return false;
            }
            if (!inst->IsPhi()) {
                seenNotPhi = true;
            }
            inst = inst->GetNext();
        }
        return true;
    }

private:
    BasicBlockId mId{-1};
    Function* mParentFunction{nullptr};
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
    BasicBlock* mImmediateDominator{nullptr};
    std::set<BasicBlock*> mDominatedBlocks{};

    MarkerFlags mMarkedFlags{Marker::None};

    Loop* mLoop{nullptr};

    LiveRange mLiveRange{};
};

}   // namespace VMIR

#endif  // BASIC_BLOCK_H
