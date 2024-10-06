#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#include <unordered_map>

#include <Instruction.h>
#include <BasicBlock.h>
#include <Function.h>
#include <ControlFlowGraph.h>

namespace VMIR {

// Make it singleton
class IRBuilder {
public:
	IRBuilder(const IRBuilder& other) = delete;

	static IRBuilder* GetInstance() {
		if (!mInstancePtr)
			mInstancePtr = new IRBuilder();

		return mInstancePtr;
	}

    ~IRBuilder();

private:
	static IRBuilder* mInstancePtr;
	IRBuilder();

public:
    Function* CreateFunction(const std::string& name = "");
    Function* CreateFunction(const ValueType retType, const std::string& name = "");
    Function* CreateFunction(const std::vector<ValueType>& argsTypes, const std::string& name = "");
    Function* CreateFunction(const ValueType retType, const std::vector<ValueType>& argsTypes, const std::string& name = "");

    BasicBlock* CreateBasicBlock();
    BasicBlock* CreateBasicBlock(Function* parentFunction);
    BasicBlock* CreateBasicBlock(const std::string& name);
    BasicBlock* CreateBasicBlock(Function* parentFunction, const std::string& name);


    inline Value* CreateValue(Function* parentFunction, const ValueType vt) {
        if (parentFunction == nullptr) {
            return nullptr;
        }

        if (auto it = mValues.find(parentFunction); it == mValues.end()) {
            mValues.insert({parentFunction, {}});
        }

        auto it = mValues.find(parentFunction);
        auto& funcValues = it->second;

        Value* v = new Value(vt, funcValues.size());
        funcValues.push_back(v);
        return v;
    }

    template <typename T>
    requires NumericType<T>
    inline Value* CreateValue(Function* parentFunction, const T value) {
        if (parentFunction == nullptr) {
            return nullptr;
        }

        if (auto it = mValuesWithData.find(parentFunction); it == mValuesWithData.end()) {
            mValuesWithData.insert({parentFunction, {}});
        }

        auto it = mValuesWithData.find(parentFunction);
        auto& funcValues = it->second;

        Value* v = new Value(funcValues.size(), value);
        funcValues.push_back(v);
        return v;
    }


    InstructionAdd* CreateAdd();
    InstructionAdd* CreateAdd(Value* input1, Value* input2, Value* output);
    InstructionAdd* CreateAdd(BasicBlock* parentBasicBlock);
    InstructionAdd* CreateAdd(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionSub* CreateSub();
    InstructionSub* CreateSub(Value* input1, Value* input2, Value* output);
    InstructionSub* CreateSub(BasicBlock* parentBasicBlock);
    InstructionSub* CreateSub(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionMul* CreateMul();
    InstructionMul* CreateMul(Value* input1, Value* input2, Value* output);
    InstructionMul* CreateMul(BasicBlock* parentBasicBlock);
    InstructionMul* CreateMul(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionDiv* CreateDiv();
    InstructionDiv* CreateDiv(Value* input1, Value* input2, Value* output);
    InstructionDiv* CreateDiv(BasicBlock* parentBasicBlock);
    InstructionDiv* CreateDiv(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionRem* CreateRem();
    InstructionRem* CreateRem(Value* input1, Value* input2, Value* output);
    InstructionRem* CreateRem(BasicBlock* parentBasicBlock);
    InstructionRem* CreateRem(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionAnd* CreateAnd();
    InstructionAnd* CreateAnd(Value* input1, Value* input2, Value* output);
    InstructionAnd* CreateAnd(BasicBlock* parentBasicBlock);
    InstructionAnd* CreateAnd(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionOr* CreateOr();
    InstructionOr* CreateOr(Value* input1, Value* input2, Value* output);
    InstructionOr* CreateOr(BasicBlock* parentBasicBlock);
    InstructionOr* CreateOr(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionXor* CreateXor();
    InstructionXor* CreateXor(Value* input1, Value* input2, Value* output);
    InstructionXor* CreateXor(BasicBlock* parentBasicBlock);
    InstructionXor* CreateXor(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionShl* CreateShl();
    InstructionShl* CreateShl(Value* input1, Value* input2, Value* output);
    InstructionShl* CreateShl(BasicBlock* parentBasicBlock);
    InstructionShl* CreateShl(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionShr* CreateShr();
    InstructionShr* CreateShr(Value* input1, Value* input2, Value* output);
    InstructionShr* CreateShr(BasicBlock* parentBasicBlock);
    InstructionShr* CreateShr(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

    InstructionLoad* CreateLoad();
    InstructionLoad* CreateLoad(Value* loadPtr, Value* output);
    InstructionLoad* CreateLoad(BasicBlock* parentBasicBlock);
    InstructionLoad* CreateLoad(BasicBlock* parentBasicBlock, Value* loadPtr, Value* output);

    InstructionStore* CreateStore();
    InstructionStore* CreateStore(Value* storePtr, Value* input);
    InstructionStore* CreateStore(BasicBlock* parentBasicBlock);
    InstructionStore* CreateStore(BasicBlock* parentBasicBlock, Value* storePtr, Value* input);

    InstructionJump* CreateJump();
    InstructionJump* CreateJump(BasicBlock* parentBasicBlock);
    InstructionJump* CreateJump(BasicBlock* parentBasicBlock, BasicBlock *jumpBB);

    InstructionBeq* CreateBeq();
    InstructionBeq* CreateBeq(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBeq* CreateBeq(BasicBlock* parentBasicBlock);
    InstructionBeq* CreateBeq(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBne* CreateBne();
    InstructionBne* CreateBne(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBne* CreateBne(BasicBlock* parentBasicBlock);
    InstructionBne* CreateBne(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBgt* CreateBgt();
    InstructionBgt* CreateBgt(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBgt* CreateBgt(BasicBlock* parentBasicBlock);
    InstructionBgt* CreateBgt(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBlt* CreateBlt();
    InstructionBlt* CreateBlt(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBlt* CreateBlt(BasicBlock* parentBasicBlock);
    InstructionBlt* CreateBlt(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBge* CreateBge();
    InstructionBge* CreateBge(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBge* CreateBge(BasicBlock* parentBasicBlock);
    InstructionBge* CreateBge(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBle* CreateBle();
    InstructionBle* CreateBle(Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBle* CreateBle(BasicBlock* parentBasicBlock);
    InstructionBle* CreateBle(BasicBlock* parentBasicBlock, Value* input1, Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionCall* CreateCall();
    InstructionCall* CreateCall(Function* function);
    InstructionCall* CreateCall(Function* function, Value* ret);
    InstructionCall* CreateCall(Function* function, const std::vector<Value*>& args);
    InstructionCall* CreateCall(Function* function, Value* ret, const std::vector<Value*>& args);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock, Function* function);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock, Function* function, Value* ret);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock, Function* function, const std::vector<Value*>& args);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock, Function* function, Value* ret, const std::vector<Value*>& args);

    InstructionRet* CreateRet();
    InstructionRet* CreateRet(Value* output);
    InstructionRet* CreateRet(BasicBlock* parentBasicBlock);
    InstructionRet* CreateRet(BasicBlock* parentBasicBlock, Value* output);

    InstructionAlloc* CreateAlloc();
    InstructionAlloc* CreateAlloc(Value* output);
    InstructionAlloc* CreateAlloc(Value* output, const size_t count);
    InstructionAlloc* CreateAlloc(BasicBlock* parentBasicBlock);
    InstructionAlloc* CreateAlloc(BasicBlock* parentBasicBlock, Value* output);
    InstructionAlloc* CreateAlloc(BasicBlock* parentBasicBlock, Value* output, const size_t count);

    InstructionPhi* CreatePhi();
    InstructionPhi* CreatePhi(const std::set<Value*>& inputs, Value* output);
    InstructionPhi* CreatePhi(BasicBlock* parentBasicBlock);
    InstructionPhi* CreatePhi(BasicBlock* parentBasicBlock, const std::set<Value*>& inputs, Value* output);


    inline ControlFlowGraph* CreateControlFlowGraph(Function* function) {
        ControlFlowGraph* cfg = new ControlFlowGraph(function);
        mGraphs.insert({function, cfg});
        return cfg;
    }


    inline void Cleanup() {
        for (const auto& v : mValues) {
            for (const auto* vv : v.second) {
                delete vv;
            }
        }
        for (const auto& v : mValuesWithData) {
            for (const auto* vv : v.second) {
                delete vv;
            }
        }
        for (const auto& i : mInstructions) {
            delete i.second;
        }
        for (const auto* b : mBasicBlocks) {
            delete b;
        }
        for (const auto* f : mFunctions) {
            delete f;
        }
        for (const auto& cfg : mGraphs) {
            delete cfg.second;
        }
    }

    void PrintDebug(std::ostream& out) const;

private:
    std::vector<BasicBlock*> mBasicBlocks{};
    std::vector<Function*> mFunctions{};

    std::unordered_map<InstructionId, Instruction*> mInstructions{};

    // Deliberately differentiate between Values with no data and Values with data
    std::unordered_map<Function*, std::vector<Value*>> mValues{};
    std::unordered_map<Function*, std::vector<Value*>> mValuesWithData{};
    std::unordered_map<Function*, ControlFlowGraph*> mGraphs{};
};

}

#endif  // IR_BUILDER_H
