#include <Function.h>
#include <IRBuilder.h>

namespace VMIR {

Function::Function(const std::vector<ValueType>& argsTypes, const std::string& name) : mName{name}, mRetType{ValueType::Void} {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();
    for (size_t i = 0; i < argsTypes.size(); ++i) {
        mArgs.push_back(IrBuilder->CreateValue(argsTypes[i]));
    }
};

Function::Function(const ValueType retType, const std::vector<ValueType>& argsTypes, const std::string& name) : mName{name}, mRetType{retType} {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();
    for (size_t i = 0; i < argsTypes.size(); ++i) {
        mArgs.push_back(IrBuilder->CreateValue(argsTypes[i]));
    }
};

size_t Function::GetInstructionCount() const {
    size_t instCount = 0;
    for (const auto* bb : GetBasicBlocks()) {
        instCount += bb->Size();
    }
    return instCount;
}

size_t Function::GetReturnCount() const {
    size_t retCount = 0;
    for (const auto* bb : GetBasicBlocks()) {
        if (bb->Back()->GetType() == InstructionType::Ret) {
            ++retCount;
        }
    }
    return retCount;
}

}   // namespace VMIR
