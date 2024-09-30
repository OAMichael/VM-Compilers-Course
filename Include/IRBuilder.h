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
    BasicBlock* CreateBasicBlock(std::vector<Instruction*>& instructions);
    BasicBlock* CreateBasicBlock(const std::vector<Instruction*>& instructions, Function* parentFunction);
    BasicBlock* CreateBasicBlock(const std::vector<Instruction*>& instructions, const std::string& name);
    BasicBlock* CreateBasicBlock(const std::vector<Instruction*>& instructions, Function* parentFunction, const std::string& name);


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
    inline Value* CreateValue(Function* parentFunction, const ValueType vt, const T value) {
        if (parentFunction == nullptr) {
            return nullptr;
        }

        if (auto it = mValuesWithData.find(parentFunction); it == mValuesWithData.end()) {
            mValuesWithData.insert({parentFunction, {}});
        }

        auto it = mValuesWithData.find(parentFunction);
        auto& funcValues = it->second;

        Value* v = new Value(vt, funcValues.size(), value);
        funcValues.push_back(v);
        return v;
    }


    InstructionAdd* CreateAdd();
    InstructionAdd* CreateAdd(const Value* input1, const Value* input2, const Value* output);
    InstructionAdd* CreateAdd(BasicBlock* parentBasicBlock);
    InstructionAdd* CreateAdd(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionSub* CreateSub();
    InstructionSub* CreateSub(const Value* input1, const Value* input2, const Value* output);
    InstructionSub* CreateSub(BasicBlock* parentBasicBlock);
    InstructionSub* CreateSub(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionMul* CreateMul();
    InstructionMul* CreateMul(const Value* input1, const Value* input2, const Value* output);
    InstructionMul* CreateMul(BasicBlock* parentBasicBlock);
    InstructionMul* CreateMul(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionDiv* CreateDiv();
    InstructionDiv* CreateDiv(const Value* input1, const Value* input2, const Value* output);
    InstructionDiv* CreateDiv(BasicBlock* parentBasicBlock);
    InstructionDiv* CreateDiv(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionRem* CreateRem();
    InstructionRem* CreateRem(const Value* input1, const Value* input2, const Value* output);
    InstructionRem* CreateRem(BasicBlock* parentBasicBlock);
    InstructionRem* CreateRem(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionAnd* CreateAnd();
    InstructionAnd* CreateAnd(const Value* input1, const Value* input2, const Value* output);
    InstructionAnd* CreateAnd(BasicBlock* parentBasicBlock);
    InstructionAnd* CreateAnd(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionOr* CreateOr();
    InstructionOr* CreateOr(const Value* input1, const Value* input2, const Value* output);
    InstructionOr* CreateOr(BasicBlock* parentBasicBlock);
    InstructionOr* CreateOr(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionXor* CreateXor();
    InstructionXor* CreateXor(const Value* input1, const Value* input2, const Value* output);
    InstructionXor* CreateXor(BasicBlock* parentBasicBlock);
    InstructionXor* CreateXor(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionShl* CreateShl();
    InstructionShl* CreateShl(const Value* input1, const Value* input2, const Value* output);
    InstructionShl* CreateShl(BasicBlock* parentBasicBlock);
    InstructionShl* CreateShl(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionShr* CreateShr();
    InstructionShr* CreateShr(const Value* input1, const Value* input2, const Value* output);
    InstructionShr* CreateShr(BasicBlock* parentBasicBlock);
    InstructionShr* CreateShr(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);

    InstructionLoad* CreateLoad();
    InstructionLoad* CreateLoad(const Value* loadPtr, const Value* output);
    InstructionLoad* CreateLoad(BasicBlock* parentBasicBlock);
    InstructionLoad* CreateLoad(BasicBlock* parentBasicBlock, const Value* loadPtr, const Value* output);

    InstructionStore* CreateStore();
    InstructionStore* CreateStore(const Value* storePtr, const Value* input);
    InstructionStore* CreateStore(BasicBlock* parentBasicBlock);
    InstructionStore* CreateStore(BasicBlock* parentBasicBlock, const Value* storePtr, const Value* input);

    InstructionJump* CreateJump();
    InstructionJump* CreateJump(BasicBlock* parentBasicBlock);
    InstructionJump* CreateJump(BasicBlock* parentBasicBlock, BasicBlock *jumpBB);

    InstructionBeq* CreateBeq();
    InstructionBeq* CreateBeq(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBeq* CreateBeq(BasicBlock* parentBasicBlock);
    InstructionBeq* CreateBeq(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBne* CreateBne();
    InstructionBne* CreateBne(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBne* CreateBne(BasicBlock* parentBasicBlock);
    InstructionBne* CreateBne(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBgt* CreateBgt();
    InstructionBgt* CreateBgt(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBgt* CreateBgt(BasicBlock* parentBasicBlock);
    InstructionBgt* CreateBgt(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBlt* CreateBlt();
    InstructionBlt* CreateBlt(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBlt* CreateBlt(BasicBlock* parentBasicBlock);
    InstructionBlt* CreateBlt(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBge* CreateBge();
    InstructionBge* CreateBge(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBge* CreateBge(BasicBlock* parentBasicBlock);
    InstructionBge* CreateBge(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionBle* CreateBle();
    InstructionBle* CreateBle(const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);
    InstructionBle* CreateBle(BasicBlock* parentBasicBlock);
    InstructionBle* CreateBle(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, BasicBlock *trueBB, BasicBlock *falseBB);

    InstructionCall* CreateCall();
    InstructionCall* CreateCall(const Function* function);
    InstructionCall* CreateCall(const Function* function, const Value* ret);
    InstructionCall* CreateCall(const Function* function, const std::vector<const Value*>& args);
    InstructionCall* CreateCall(const Function* function, const Value* ret, const std::vector<const Value*>& args);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock, const Function* function);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock, const Function* function, const Value* ret);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock, const Function* function, const std::vector<const Value*>& args);
    InstructionCall* CreateCall(BasicBlock* parentBasicBlock, const Function* function, const Value* ret, const std::vector<const Value*>& args);

    InstructionRet* CreateRet();
    InstructionRet* CreateRet(const Value* output);
    InstructionRet* CreateRet(BasicBlock* parentBasicBlock);
    InstructionRet* CreateRet(BasicBlock* parentBasicBlock, const Value* output);

    InstructionAlloc* CreateAlloc();
    InstructionAlloc* CreateAlloc(const Value* output);
    InstructionAlloc* CreateAlloc(const Value* output, const size_t count);
    InstructionAlloc* CreateAlloc(BasicBlock* parentBasicBlock);
    InstructionAlloc* CreateAlloc(BasicBlock* parentBasicBlock, const Value* output);
    InstructionAlloc* CreateAlloc(BasicBlock* parentBasicBlock, const Value* output, const size_t count);

    InstructionPhi* CreatePhi();
    InstructionPhi* CreatePhi(const Value* input1, const Value* input2, const Value* output);
    InstructionPhi* CreatePhi(BasicBlock* parentBasicBlock);
    InstructionPhi* CreatePhi(BasicBlock* parentBasicBlock, const Value* input1, const Value* input2, const Value* output);


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
        for (const auto* b : mBasicBlocks) {
            delete b;
        }
        for (const auto* i : mInstructions) {
            delete i;
        }
        for (const auto* f : mFunctions) {
            delete f;
        }
        for (const auto& cfg : mGraphs) {
            delete cfg.second;
        }
    } 

private:
    std::vector<Instruction*> mInstructions{};
    std::vector<BasicBlock*> mBasicBlocks{};
    std::vector<Function*> mFunctions{};

    // Deliberately differentiate between Values with no data and Values with data
    std::unordered_map<Function*, std::vector<Value*>> mValues{};
    std::unordered_map<Function*, std::vector<Value*>> mValuesWithData{};
    std::unordered_map<Function*, ControlFlowGraph*> mGraphs{};
};

}

#endif  // IR_BUILDER_H
