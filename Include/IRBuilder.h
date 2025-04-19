#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#include <unordered_map>
#include <limits>
#include <list>

#include <Instruction.h>
#include <BasicBlock.h>
#include <Function.h>
#include <ControlFlowGraph.h>
#include <LoopAnalyzer.h>
#include <LivenessAnalyzer.h>
#include <RegisterAllocator.h>

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


    inline Value* CreateValue(const ValueType vt) {
        ValueId id = GenerateNewValueId();
        Value* v = new Value(id, vt);
        mValues.emplace(id, v);
        return v;
    }

    template <typename T>
    requires NumericType<T>
    inline Value* CreateValue(const T value) {
        ValueId id = GenerateNewValueWithDataId();
        Value* v = new Value(id, value);
        mValuesWithData.emplace(id, v);
        return v;
    }

    template <typename T>
    requires NumericType<T>
    inline Value* GetOrCreateValueWithData(const T data) {
        auto it = std::find_if(mValuesWithData.begin(), mValuesWithData.end(), [data](const auto& e) {
            Value* v = e.second;
            if (v->GetValueType() != TypeToValueType<T>()) {
                return false;
            }
            return v->GetValue<T>().value() == data;
        });

        if (it != mValuesWithData.end()) {
            return it->second;
        }
        return CreateValue<T>(data);
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

    InstructionAshr* CreateAshr();
    InstructionAshr* CreateAshr(Value* input1, Value* input2, Value* output);
    InstructionAshr* CreateAshr(BasicBlock* parentBasicBlock);
    InstructionAshr* CreateAshr(BasicBlock* parentBasicBlock, Value* input1, Value* input2, Value* output);

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
    InstructionRet* CreateRet(Value* returnValue);
    InstructionRet* CreateRet(BasicBlock* parentBasicBlock);
    InstructionRet* CreateRet(BasicBlock* parentBasicBlock, Value* returnValue);

    InstructionAlloc* CreateAlloc();
    InstructionAlloc* CreateAlloc(Value* output, ValueType type, size_t count = 1);
    InstructionAlloc* CreateAlloc(BasicBlock* parentBasicBlock);
    InstructionAlloc* CreateAlloc(BasicBlock* parentBasicBlock, Value* output, ValueType type, size_t count = 1);

    InstructionPhi* CreatePhi();
    InstructionPhi* CreatePhi(const std::list<Value*>& inputs, Value* output);
    InstructionPhi* CreatePhi(BasicBlock* parentBasicBlock);
    InstructionPhi* CreatePhi(BasicBlock* parentBasicBlock, const std::list<Value*>& inputs, Value* output);

    InstructionMv* CreateMv();
    InstructionMv* CreateMv(Value* input, Value* output);
    InstructionMv* CreateMv(BasicBlock* parentBasicBlock);
    InstructionMv* CreateMv(BasicBlock* parentBasicBlock, Value* input, Value* output);

    InstructionNullCheck* CreateNullCheck();
    InstructionNullCheck* CreateNullCheck(Value* input);
    InstructionNullCheck* CreateNullCheck(BasicBlock* parentBasicBlock);
    InstructionNullCheck* CreateNullCheck(BasicBlock* parentBasicBlock, Value* input);

    InstructionBoundsCheck* CreateBoundsCheck();
    InstructionBoundsCheck* CreateBoundsCheck(Value* inputPtr, Value* inputArray);
    InstructionBoundsCheck* CreateBoundsCheck(BasicBlock* parentBasicBlock);
    InstructionBoundsCheck* CreateBoundsCheck(BasicBlock* parentBasicBlock, Value* inputPtr, Value* inputArray);


    Value* CopyValue(Value* src);
    Instruction* CopyInstruction(Instruction* src);
    BasicBlock* CopyBasicBlock(BasicBlock* src);
    Function* CopyFunction(Function* src);

    // Check that return type, parameters, entry basic block, control flow, data flow and instructions are the same
    bool CompareFunctions(Function* lhs, Function* rhs) const;


    void RemoveValue(Value* value) {
        if (!value) {
            return;
        }

        ValueId id = value->GetId();
        if (id == -1) {
            return;
        }

        if (mValues.contains(id)) {
            delete mValues[id];
            mValues.erase(id);
        }
        if (mValuesWithData.contains(id)) {
            delete mValuesWithData[id];
            mValuesWithData.erase(id);
        }
    }

    void RemoveInstruction(Instruction* inst) {
        if (!inst) {
            return;
        }

        InstructionId id = inst->GetId();
        if (id == -1) {
            return;
        }

        if (mInstructions.contains(id)) {
            delete mInstructions[id];
            mInstructions.erase(id);
        }
    }

    void RemoveBasicBlock(BasicBlock* bb) {
        if (!bb) {
            return;
        }

        BasicBlockId id = bb->GetId();
        if (id == -1) {
            return;
        }

        if (mBasicBlocks.contains(id)) {
            delete mBasicBlocks[id];
            mBasicBlocks.erase(id);
        }
    }

    void RemoveFunction(Function* func) {
        if (!func) {
            return;
        }

        auto it = std::find(mFunctions.begin(), mFunctions.end(), func);
        if (it == mFunctions.end()) {
            return;
        }

        delete *it;
        mFunctions.erase(it);

        if (mGraphs.contains(func)) {
            ControlFlowGraph* cfg = mGraphs[func];

            delete cfg;
            mGraphs.erase(func);

            if (mLoopAnalyzers.contains(cfg)) {
                delete mLoopAnalyzers[cfg];
                mLoopAnalyzers.erase(cfg);
            }
            if (mLivenessAnalyzers.contains(cfg)) {
                delete mLivenessAnalyzers[cfg];
                mLivenessAnalyzers.erase(cfg);
            }
            if (mRegisterAllocators.contains(cfg)) {
                delete mRegisterAllocators[cfg];
                mRegisterAllocators.erase(cfg);
            }
        }
    }


    inline ControlFlowGraph* CreateControlFlowGraph(Function* function) {
        ControlFlowGraph* cfg = new ControlFlowGraph(function);
        mGraphs.insert({function, cfg});
        return cfg;
    }

    inline LoopAnalyzer* CreateLoopAnalyzer(ControlFlowGraph* graph) {
        LoopAnalyzer* loopAnalyzer = new LoopAnalyzer(graph);
        mLoopAnalyzers.insert({graph, loopAnalyzer});
        return loopAnalyzer;
    }

    inline LivenessAnalyzer* CreateLivenessAnalyzer(ControlFlowGraph* graph) {
        LivenessAnalyzer* livenessAnalyzer = new LivenessAnalyzer(graph);
        mLivenessAnalyzers.insert({graph, livenessAnalyzer});
        return livenessAnalyzer;
    }

    inline RegisterAllocator* CreateRegisterAllocator(ControlFlowGraph* graph, uint32_t GPRegCount, uint32_t FPRegCount) {
        RegisterAllocator* registerAllocator = new RegisterAllocator(graph, GPRegCount, FPRegCount);
        mRegisterAllocators.insert({graph, registerAllocator});
        return registerAllocator;
    }

    inline ControlFlowGraph* GetOrCreateControlFlowGraph(Function* function) {
        if (auto it = mGraphs.find(function); it != mGraphs.end()) {
            return it->second;
        }
        return CreateControlFlowGraph(function);
    }

    inline LoopAnalyzer* GetOrCreateLoopAnalyzer(ControlFlowGraph* graph) {
        if (auto it = mLoopAnalyzers.find(graph); it != mLoopAnalyzers.end()) {
            return it->second;
        }
        return CreateLoopAnalyzer(graph);
    }

    inline LivenessAnalyzer* GetOrCreateLivenessAnalyzer(ControlFlowGraph* graph) {
        if (auto it = mLivenessAnalyzers.find(graph); it != mLivenessAnalyzers.end()) {
            return it->second;
        }
        return CreateLivenessAnalyzer(graph);
    }

    inline RegisterAllocator* GetOrCreateRegisterAllocator(ControlFlowGraph* graph, uint32_t GPRegCount, uint32_t FPRegCount) {
        if (auto it = mRegisterAllocators.find(graph); it != mRegisterAllocators.end()) {
            return it->second;
        }
        return CreateRegisterAllocator(graph, GPRegCount, FPRegCount);
    }

    inline void Cleanup() {
        for (const auto& v : mValues) {
            delete v.second;
        }
        for (const auto& v : mValuesWithData) {
            delete v.second;
        }
        for (const auto& i : mInstructions) {
            delete i.second;
        }
        for (const auto& b : mBasicBlocks) {
            delete b.second;
        }
        for (const auto* f : mFunctions) {
            delete f;
        }
        for (const auto& cfg : mGraphs) {
            delete cfg.second;
        }
        for (const auto& la : mLoopAnalyzers) {
            delete la.second;
        }
        for (const auto& la : mLivenessAnalyzers) {
            delete la.second;
        }
        for (const auto& ra : mRegisterAllocators) {
            delete ra.second;
        }

        mBasicBlocks.clear();
        mFunctions.clear();
        mInstructions.clear();
        mValues.clear();
        mValuesWithData.clear();
        mGraphs.clear();
        mLoopAnalyzers.clear();
        mLivenessAnalyzers.clear();
        mRegisterAllocators.clear();

        mValuesIDs = -1;
        mValuesWithDataIDs = std::numeric_limits<ValueId>::max();
        mInstructionsIDs = -1;
        mBasicBlockIDs = -1;
    }

    void PrintIR(std::ostream& out);
    void PrintDebug(std::ostream& out);

private:
    ValueId GenerateNewValueId() {
        return ++mValuesIDs;
    }

    ValueId GenerateNewValueWithDataId() {
        return --mValuesWithDataIDs;
    }

    InstructionId GenerateNewInstructionId() {
        return ++mInstructionsIDs;
    }

    BasicBlockId GenerateNewBasicBlockId() {
        return ++mBasicBlockIDs;
    }

    ValueId mValuesIDs = -1;
    ValueId mValuesWithDataIDs = std::numeric_limits<ValueId>::max();
    InstructionId mInstructionsIDs = -1;
    BasicBlockId mBasicBlockIDs = -1;

    // Deliberately differentiate between Values with no data and Values with data
    std::unordered_map<ValueId, Value*> mValues{};
    std::unordered_map<ValueId, Value*> mValuesWithData{};
    std::unordered_map<InstructionId, Instruction*> mInstructions{};
    std::unordered_map<BasicBlockId, BasicBlock*> mBasicBlocks{};

    std::list<Function*> mFunctions{};

    std::unordered_map<Function*, ControlFlowGraph*> mGraphs{};
    std::unordered_map<ControlFlowGraph*, LoopAnalyzer*> mLoopAnalyzers{};
    std::unordered_map<ControlFlowGraph*, LivenessAnalyzer*> mLivenessAnalyzers{};
    std::unordered_map<ControlFlowGraph*, RegisterAllocator*> mRegisterAllocators{};
};

}

#endif  // IR_BUILDER_H
