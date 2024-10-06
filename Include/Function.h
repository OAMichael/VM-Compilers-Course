#ifndef FUNCTION_H
#define FUNCTION_H

#include <BasicBlock.h>

namespace VMIR {

class Function {
public:
    // Constructors
    Function(const std::string& name = "") : mName{name}, mRetType{ValueType::Void} {};
    Function(const ValueType retType, const std::string& name = "") : mName{name}, mRetType{retType} {};

    Function(const std::vector<ValueType>& argsTypes, const std::string& name = "");
    Function(const ValueType retType, const std::vector<ValueType>& argsTypes, const std::string& name = "");

    // Getters
    inline std::string GetName() const { return mName; }

    inline std::optional<ValueType> GetReturnType() const { return mRetType; }
    inline const std::vector<Value*>& GetArgs() const { return mArgs; }
    inline const Value* GetArg(const size_t idx) const { return mArgs[idx]; }
    inline Value* GetArg(const size_t idx) { return mArgs[idx]; }

    inline const std::vector<BasicBlock*>& GetBasicBlocks() const { return mBasicBlocks; }
    inline BasicBlock* GetBasicBlock(const size_t idx) const { return mBasicBlocks[idx]; }

    inline size_t Size() const { return mBasicBlocks.size(); }

    // Setters
    inline void SetName(const std::string& name) { mName = name; }

    inline void AppendBasicBlock(BasicBlock* basicBlock) { mBasicBlocks.push_back(basicBlock); }

    inline void Print(std::ostream& out) const {
        out << "function " << ValueTypeToIdStr(mRetType) << " ";

        out << "#" << GetName() << "(";
        for (size_t i = 0; i < mArgs.size(); ++i) {
            if (i > 0) {
                out << ", ";
            }

            out << ValueTypeToIdStr(mArgs[i]->GetValueType()) << " " << mArgs[i]->GetValueStr();
        }
        out << ") {\n";

        for (size_t i = 0; i < mBasicBlocks.size(); ++i) {
            if (i > 0) {
                out << "\n";
            }
            mBasicBlocks[i]->Print(out);
        }

        out << "}";
    }

    inline bool IsValid() const {
        if (mName.length() == 0 || mBasicBlocks.size() == 0) {
            return false;
        }
        for (const auto* bb : mBasicBlocks) {
            if (!bb->IsValid()) {
                return false;
            }
        }
        return true;
    }

private:
    std::string mName{};
    ValueType mRetType{ValueType::Unknown};
    std::vector<Value*> mArgs{};
    std::vector<BasicBlock*> mBasicBlocks{};
};

}   // namespace VMIR

#endif  // FUNCTION_H
