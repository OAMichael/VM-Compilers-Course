#ifndef PASS_H
#define PASS_H

#include <Function.h>

namespace VMIR {

class Pass {
public:
    Pass(const std::string& name) : mName{name} {}

    virtual void Run(Function* func) = 0;

    inline const std::string& GetName() const { return mName; }

protected:
    std::string mName{};
};

}   // namespace VMIR

#endif  // PASS_H
