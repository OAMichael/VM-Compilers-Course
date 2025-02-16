#ifndef PEEPHOLES_PASS_H
#define PEEPHOLES_PASS_H

#include <Pass.h>

namespace VMIR {

class PeepholesPass : public Pass {
public:
    PeepholesPass();

    virtual void Run(Function* func) const override;

private:
    void PerformSingleAddPeephole(InstructionAdd* inst) const;

    void PerformSingleAshrPeephole(InstructionAshr* inst) const;

    void PerformSingleAndPeephole(InstructionAnd* inst) const;
};

}   // namespace VMIR

#endif  // PEEPHOLES_PASS_H
