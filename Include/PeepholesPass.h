#ifndef PEEPHOLES_PASS_H
#define PEEPHOLES_PASS_H

#include <Pass.h>

namespace VMIR {

class PeepholesPass : public Pass {
public:
    PeepholesPass();

    virtual void Run(Function* func) const override;

private:
    bool PerformSingleAddPeephole(InstructionAdd* inst) const;

    bool PerformSingleAshrPeephole(InstructionAshr* inst) const;
    bool PerformComplexAshrPeephole(InstructionAshr* inst) const;

    bool PerformSingleAndPeephole(InstructionAnd* inst) const;
};

}   // namespace VMIR

#endif  // PEEPHOLES_PASS_H
