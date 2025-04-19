#ifndef CHECK_ELIMINATION_PASS_H
#define CHECK_ELIMINATION_PASS_H

#include <Pass.h>

namespace VMIR {

class CheckEliminationPass : public Pass {
public:
    CheckEliminationPass();

    virtual void Run(Function* func) override;
};

}   // namespace VMIR

#endif  // CHECK_ELIMINATION_PASS_H
