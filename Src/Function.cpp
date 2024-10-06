#include <Function.h>
#include <IRBuilder.h>

namespace VMIR {

Function::Function(const std::vector<ValueType>& argsTypes, const std::string& name) : mName{name}, mRetType{ValueType::Void} {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();
    for (size_t i = 0; i < argsTypes.size(); ++i) {
        mArgs.push_back(IrBuilder->CreateValue(this, argsTypes[i]));
    }
};

Function::Function(const ValueType retType, const std::vector<ValueType>& argsTypes, const std::string& name) : mName{name}, mRetType{retType} {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();
    for (size_t i = 0; i < argsTypes.size(); ++i) {
        mArgs.push_back(IrBuilder->CreateValue(this, argsTypes[i]));
    }
};

}   // namespace VMIR
