#ifndef CONSTANT_FOLDING_PASS_H
#define CONSTANT_FOLDING_PASS_H

#include <type_traits>
#include <bit>

#include <Pass.h>

namespace VMIR {

class ConstantFoldingPass : public Pass {
public:
    ConstantFoldingPass();

    virtual void Run(Function* func) override;

private:
    // Combine optimization and get next instruction because current instruction can be erased due to optimizations
    Instruction* OptimizeInstructionAndGetNext(Instruction* inst) const;

    // Optimize arithmetic instruction if both inputs are constants and convert it to Mv with calculated resulting constant
    Instruction* OptimizeInstructionArithmetic(InstructionArithmetic* inst) const;

    // Helper function to get or create Value which holds calculated resulting constant
    Value* GetOrCreateConstantForMove(Value* input1, Value* input2, InstructionType op) const;
    
    void ReplaceMoveUserInputWithConstant(Instruction* user, InstructionMv* instMv) const;

    template <typename T>
    T PerformValueOperation(T a, T b, InstructionType op) const {
        switch (op) {
            default:                    return T();
            case InstructionType::Add:  return a + b;
            case InstructionType::Sub:  return a - b;
            case InstructionType::Mul:  return a * b;
            case InstructionType::Div:  return a / b;
            case InstructionType::Rem:  {
                if constexpr (std::is_integral_v<T>) {
                    return a % b;
                }
                return T();
            }
            case InstructionType::And: {
                if constexpr (std::is_integral_v<T>) {
                    return a & b;
                }
                return T();
            }
            case InstructionType::Or: {
                if constexpr (std::is_integral_v<T>) {
                    return a | b;
                }
                return T();
            }
            case InstructionType::Xor: {
                if constexpr (std::is_integral_v<T>) {
                    return a ^ b;
                }
                return T();
            }
            case InstructionType::Shl: {
                if constexpr (std::is_integral_v<T>) {
                    return a << b;
                }
                return T();
            }
            case InstructionType::Shr: {
                if constexpr (std::is_integral_v<T>) {
                    using UnsignedT = std::make_unsigned_t<T>;

                    UnsignedT ua = std::bit_cast<UnsignedT>(a);
                    UnsignedT ub = std::bit_cast<UnsignedT>(b);
                    UnsignedT res = ua >> ub;

                    return std::bit_cast<T>(res);
                }
                return T();
            }
            case InstructionType::Ashr: {
                if constexpr (std::is_integral_v<T>) {
                    using SignedT = std::make_signed_t<T>;

                    SignedT sa = std::bit_cast<SignedT>(a);
                    SignedT sb = std::bit_cast<SignedT>(b);
                    SignedT res = sa >> sb;

                    return std::bit_cast<T>(res);
                }
                return T();
            }
        }
    }
};

}   // namespace VMIR

#endif  // CONSTANT_FOLDING_PASS_H
