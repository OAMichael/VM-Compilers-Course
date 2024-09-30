#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <vector>
#include <ostream>

#include <Instruction.h>

namespace VMIR {

class Function;

using BasicBlockId = int64_t;

class BasicBlock {
public:
    BasicBlock(const Function* parentFunction = nullptr, const BasicBlockId id = -1, const std::string& name = "")
    : mParentFunction{parentFunction}, mId{id}, mName{name} {};
    
    BasicBlock(const std::vector<Instruction*>& instructions, const Function* parentFunction = nullptr, const BasicBlockId id = -1, const std::string& name = "")
    : mParentFunction{parentFunction}, mId(id), mName{name}, mInstructions{instructions} {};

    inline const Function *GetParentFunction() const { return mParentFunction; }
    inline void SetParentFunction(const Function* parentFunction) { mParentFunction = parentFunction; }

    inline BasicBlockId GetId() const { return mId; }
    inline void SetId(const BasicBlockId id) { mId = id; }

    inline std::string GetName() const { return mName.length() > 0 ? (mName) : ("<Unnamed BB#" + std::to_string(mId) + ">"); }
    inline void SetName(const std::string& name) { mName = name; }

    inline Instruction *Front() const { return mInstructions.front(); }
    inline Instruction *Back()  const { return mInstructions.back(); }
    inline size_t Size() const { return mInstructions.size(); }

    inline Instruction *GetInstruction(const size_t idx) const { return mInstructions[idx]; }
    inline void AppendInstruction(Instruction* const inst) { mInstructions.push_back(inst); }

    inline const std::vector<const BasicBlock*>& GetPredecessors() const { return mPredecessors; }
    inline const std::vector<const BasicBlock*>& GetSuccessors() const { return mSuccessors; }

    inline void AddPredecessor(const BasicBlock* pred) { mPredecessors.push_back(pred); }
    inline void AddSuccessor(const BasicBlock* succ)   { mSuccessors.push_back(succ); }

    inline void Print(std::ostream& out) const {
        out << GetName() << ":";
        if (mPredecessors.size() > 0) {
            out << " (preds: ";
            for (size_t i = 0; i < mPredecessors.size(); ++i) {
                if (i > 0) {
                    out << ", ";
                }
                out << mPredecessors[i]->GetName();
            }
            out << ")";
        }
        out << "\n";

        for (const auto* inst : mInstructions) {
            out << "    " << inst->GetAsString() << "\n";
        }
    }

    inline bool IsValid() const {
        if (mParentFunction == nullptr || mId == -1 || mInstructions.size() == 0) {
            return false;
        }
        if (!IsInstructionTerminator(mInstructions.back()->GetType())) {
            return false;
        }
        for (const auto* inst : mInstructions) {
            if (!inst->IsValid()) {
                return false;
            }
        }
        return true;
    }

private:
    const Function* mParentFunction{nullptr};
    BasicBlockId mId{-1};
    std::string mName{};
    std::vector<Instruction*> mInstructions{};

    std::vector<const BasicBlock*> mPredecessors{};
    std::vector<const BasicBlock*> mSuccessors{};
};

}   // namespace VMIR

#endif  // BASIC_BLOCK_H
