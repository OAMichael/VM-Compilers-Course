#ifndef STATIC_INLINING_PASS_H
#define STATIC_INLINING_PASS_H

#include <unordered_set>

#include <Pass.h>

namespace VMIR {

class StaticInliningPass : public Pass {
public:
    StaticInliningPass();

    virtual void Run(Function* func) override;

private:
    static constexpr size_t INLINE_INSTRUCTION_COUNT_THRESHOLD = 10;

    void InlineCallees(Function* func);
    void InlineCall(InstructionCall* inst) const;

    // This set is used to prevent recursive calls inlining
    std::unordered_set<Function*> mFunctionsToProcess{};
};

}   // namespace VMIR

#endif  // STATIC_INLINING_PASS_H
