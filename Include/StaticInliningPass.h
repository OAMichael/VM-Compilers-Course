#ifndef STATIC_INLINING_PASS_H
#define STATIC_INLINING_PASS_H

#include <unordered_set>

#include <Pass.h>

namespace VMIR {

class StaticInliningPass : public Pass {
public:
    StaticInliningPass();

    virtual void Run(Function* func) override;

    size_t GetInlineInstructionCountThreshold() const;
    void SetInlineInstructionCountThreshold(size_t threshold);

private:
    void InlineCallees(Function* func);
    void InlineCall(InstructionCall* inst) const;

    // This set is used to prevent recursive calls inlining
    std::unordered_set<Function*> mFunctionsToProcess{};

    size_t mInlineInstructionCountThreshold;
};

}   // namespace VMIR

#endif  // STATIC_INLINING_PASS_H
