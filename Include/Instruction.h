#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>

#include <Value.h>

namespace VMIR {

class BasicBlock;
class Function;

enum class InstructionType {
    Add = 0,
    Sub,
    Mul,
    Div,
    Rem,
    And,
    Or,
    Xor,
    Shl,
    Shr,
    Ashr,
    Load,
    Store,
    Jump,
    Beq,
    Bne,
    Bgt,
    Blt,
    Bge,
    Ble,
    Call,
    Ret,
    Alloc,
    Phi,
    Mv,

    Unknown
};


static inline const char *InstructionTypeToStr(const InstructionType it) {
    switch (it) {
        default:
        case InstructionType::Unknown:  return "Unknown";
        case InstructionType::Add:      return "Add";
        case InstructionType::Sub:      return "Sub";
        case InstructionType::Mul:      return "Mul";
        case InstructionType::Div:      return "Div";
        case InstructionType::Rem:      return "Rem";
        case InstructionType::And:      return "And";
        case InstructionType::Or:       return "Or";
        case InstructionType::Xor:      return "Xor";
        case InstructionType::Shl:      return "Shl";
        case InstructionType::Shr:      return "Shr";
        case InstructionType::Ashr:     return "Ashr";
        case InstructionType::Load:     return "Load";
        case InstructionType::Store:    return "Store";
        case InstructionType::Jump:     return "Jump";
        case InstructionType::Beq:      return "Beq";
        case InstructionType::Bne:      return "Bne";
        case InstructionType::Bgt:      return "Bgt";
        case InstructionType::Blt:      return "Blt";
        case InstructionType::Bge:      return "Bge";
        case InstructionType::Ble:      return "Ble";
        case InstructionType::Call:     return "Call";
        case InstructionType::Ret:      return "Ret";
        case InstructionType::Alloc:    return "Alloc";
        case InstructionType::Phi:      return "Phi";
        case InstructionType::Mv:       return "Mv";
    }
}


// Base interface for all instructions
class Instruction {
public:
    Instruction(const InstructionType type, const InstructionId id) : mType{type}, mId{id} {};

    virtual ~Instruction() {};

    virtual std::string GetAsString() const = 0;
    virtual bool IsValid() const = 0;
    virtual bool IsTerminator() const { return false; }
    virtual Value* GetOutput() const { return nullptr; }

    bool IsArithmetic() const { return mType == InstructionType::Add
                                    || mType == InstructionType::Sub
                                    || mType == InstructionType::Mul
                                    || mType == InstructionType::Div
                                    || mType == InstructionType::Rem
                                    || mType == InstructionType::And
                                    || mType == InstructionType::Or
                                    || mType == InstructionType::Xor
                                    || mType == InstructionType::Shl
                                    || mType == InstructionType::Shr; }

    bool IsBranch() const { return mType == InstructionType::Beq
                                || mType == InstructionType::Bne
                                || mType == InstructionType::Bgt
                                || mType == InstructionType::Blt
                                || mType == InstructionType::Bge
                                || mType == InstructionType::Ble; }

    bool IsPhi() const { return mType == InstructionType::Phi; }

    inline InstructionType GetType() const { return mType; }

    inline InstructionId GetId() const { return mId; }
    inline void SetId(const InstructionId id) { mId = id; }

    inline BasicBlock* GetParentBasicBlock() const { return mParentBasicBlock; }
    inline void SetParentBasicBlock(BasicBlock* parentBasicBlock) { mParentBasicBlock = parentBasicBlock; }

    inline Instruction* GetPrev() const { return mPrev; }
    inline void SetPrev(Instruction* prev) { mPrev = prev; }

    inline Instruction* GetNext() const { return mNext; }
    inline void SetNext(Instruction* next) { mNext = next; }

    bool IsDominatedBy(Instruction* inst);

    inline uint64_t GetLinearNumber() const { return mLinearNumber; }
    inline void SetLinearNumber(uint64_t lin) { mLinearNumber = lin; }

    inline uint64_t GetLiveNumber() const { return mLiveNumber; }
    inline void SetLiveNumber(uint64_t live) { mLiveNumber = live; }

    inline LiveInterval& GetLiveInterval() { return GetOutput()->GetLiveInterval(); }
    inline const LiveInterval& GetLiveInterval() const { return GetOutput()->GetLiveInterval(); }

protected:
    InstructionType mType{};
    InstructionId mId{-1};
    BasicBlock* mParentBasicBlock{nullptr};
    uint64_t mLinearNumber{0};
    uint64_t mLiveNumber{0};

    Instruction* mPrev{};
    Instruction* mNext{};
};


// Interface for arithmetic instructions
class InstructionArithmetic : public Instruction {
public:
    // Constructors
    InstructionArithmetic(const InstructionType type, const InstructionId id) : Instruction(type, id) {};

    InstructionArithmetic(const InstructionType type, const InstructionId id, Value* input1, Value* input2, Value* output)
    : Instruction(type, id), mInput1{input1}, mInput2{input2}, mOutput{output} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual Value* GetOutput() const override { return mOutput; }

    // Getters
    inline Value* GetInput1() { return mInput1; }
    inline Value* GetInput2() { return mInput2; }

    // Setters
    inline void SetInput1(Value* value) { mInput1 = value; }
    inline void SetInput2(Value* value) { mInput2 = value; }
    inline void SetOutput(Value* value) { mOutput = value; }

protected:
    Value* mInput1;
    Value* mInput2;
    Value* mOutput;
};



// Simple arithmetic instructions

class InstructionAdd final : public InstructionArithmetic {
public:
    // Constructors
    InstructionAdd(const InstructionId id) : InstructionArithmetic(InstructionType::Add, id) {};

    InstructionAdd(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionArithmetic(InstructionType::Add, id, input1, input2, output) {};
};

class InstructionSub final : public InstructionArithmetic {
public:
    // Constructors
    InstructionSub(const InstructionId id) : InstructionArithmetic(InstructionType::Sub, id) {};

    InstructionSub(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionArithmetic(InstructionType::Sub, id, input1, input2, output) {};
};

class InstructionMul final : public InstructionArithmetic {
public:
    // Constructors
    InstructionMul(const InstructionId id) : InstructionArithmetic(InstructionType::Mul, id) {};

    InstructionMul(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionArithmetic(InstructionType::Mul, id, input1, input2, output) {};
};

class InstructionDiv final : public InstructionArithmetic {
public:
    // Constructors
    InstructionDiv(const InstructionId id) : InstructionArithmetic(InstructionType::Div, id) {};

    InstructionDiv(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionArithmetic(InstructionType::Div, id, input1, input2, output) {};
};

class InstructionRem final : public InstructionArithmetic {
public:
    // Constructors
    InstructionRem(const InstructionId id) : InstructionArithmetic(InstructionType::Rem, id) {};

    InstructionRem(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionArithmetic(InstructionType::Rem, id, input1, input2, output) {};
};



// Bitwise instructions

class InstructionBitwise : public InstructionArithmetic {
public:
    // Constructors
    InstructionBitwise(const InstructionType type, const InstructionId id) : InstructionArithmetic(type, id) {};

    InstructionBitwise(const InstructionType type, const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionArithmetic(type, id, input1, input2, output) {};

    // Validity rules for bitwise instructions are stricter
    virtual bool IsValid() const override;
};


class InstructionAnd final : public InstructionBitwise {
public:
    // Constructors
    InstructionAnd(const InstructionId id) : InstructionBitwise(InstructionType::And, id) {};

    InstructionAnd(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionBitwise(InstructionType::And, id, input1, input2, output) {};
};

class InstructionOr final : public InstructionBitwise {
public:
    // Constructors
    InstructionOr(const InstructionId id) : InstructionBitwise(InstructionType::And, id) {};

    InstructionOr(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionBitwise(InstructionType::And, id, input1, input2, output) {};
};

class InstructionXor final : public InstructionBitwise {
public:
    // Constructors
    InstructionXor(const InstructionId id) : InstructionBitwise(InstructionType::And, id) {};

    InstructionXor(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionBitwise(InstructionType::And, id, input1, input2, output) {};
};

class InstructionShl final : public InstructionBitwise {
public:
    // Constructors
    InstructionShl(const InstructionId id) : InstructionBitwise(InstructionType::Shl, id) {};

    InstructionShl(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionBitwise(InstructionType::Shl, id, input1, input2, output) {};
};

class InstructionShr final : public InstructionBitwise {
public:
    // Constructors
    InstructionShr(const InstructionId id) : InstructionBitwise(InstructionType::Shr, id) {};

    InstructionShr(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionBitwise(InstructionType::Shr, id, input1, input2, output) {};
};

class InstructionAshr final : public InstructionBitwise {
public:
    // Constructors
    InstructionAshr(const InstructionId id) : InstructionBitwise(InstructionType::Ashr, id) {};

    InstructionAshr(const InstructionId id, Value* input1, Value* input2, Value* output)
    : InstructionBitwise(InstructionType::Ashr, id, input1, input2, output) {};
};



// Memory instrucions

class InstructionLoad final : public Instruction {
public:
    // Constructors
    InstructionLoad(const InstructionId id) : Instruction(InstructionType::Load, id) {};

    InstructionLoad(const InstructionId id, Value* loadPtr, Value* output)
    : Instruction(InstructionType::Load, id), mLoadPtr{loadPtr}, mOutput{output} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual Value* GetOutput() const override { return mOutput; }

    // Getters
    inline Value* GetLoadPtr() { return mLoadPtr; }

    // Setters
    inline void SetLoadPtr(Value* value) { mLoadPtr = value; }
    inline void SetOutput(Value* value)  { mOutput = value; }

private:
    Value* mLoadPtr;
    Value* mOutput;
};


class InstructionStore final : public Instruction {
public:
    // Constructors
    InstructionStore(const InstructionId id) : Instruction(InstructionType::Store, id) {};

    InstructionStore(const InstructionId id, Value* storePtr, Value* input)
    : Instruction(InstructionType::Store, id), mStorePtr{storePtr}, mInput{input} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline Value* GetStorePtr() { return mStorePtr; }
    inline Value* GetInput()    { return mInput; }

    // Setters
    inline void SetStorePtr(Value* value) { mStorePtr = value; }
    inline void SetInput(Value* value)    { mInput = value; }

private:
    Value* mStorePtr;
    Value* mInput;
};



// Jump instruction

class InstructionJump final : public Instruction {
public:
    // Constructors
    InstructionJump(const InstructionId id) : Instruction(InstructionType::Jump, id) {};

    InstructionJump(const InstructionId id, BasicBlock *pJumpBB) : Instruction(InstructionType::Jump, id), mJumpBB{pJumpBB} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual bool IsTerminator() const override { return true; }

    // Getters
    inline const BasicBlock *GetJumpBasicBlock() const { return mJumpBB; }
    inline BasicBlock *GetJumpBasicBlock() { return mJumpBB; }

    // Setters
    inline void SetJumpBasicBlock(BasicBlock *pJumpBB) { mJumpBB = pJumpBB; }

private:
    BasicBlock *mJumpBB{nullptr};
};



// Branch instructions

class InstructionBranch : public Instruction {
public:
    // Constructors
    InstructionBranch(const InstructionType type, const InstructionId id) : Instruction(type, id) {};

    InstructionBranch(const InstructionType type, const InstructionId id, Value* input1, Value* input2, BasicBlock *pTrueBB, BasicBlock *pFalseBB)
    : Instruction(type, id), mInput1{input1}, mInput2{input2}, mTrueBB{pTrueBB}, mFalseBB{pFalseBB} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual bool IsTerminator() const override { return true; }

    // Getters
    inline Value* GetInput1() { return mInput1; }
    inline Value* GetInput2() { return mInput2; }

    inline const BasicBlock *GetTrueBasicBlock()  const { return mTrueBB; }
    inline const BasicBlock *GetFalseBasicBlock() const { return mFalseBB; }

    inline BasicBlock *GetTrueBasicBlock()  { return mTrueBB; }
    inline BasicBlock *GetFalseBasicBlock() { return mFalseBB; }

    // Setters
    inline void SetInput1(Value* value) { mInput1 = value; }
    inline void SetInput2(Value* value) { mInput2 = value; }

    inline void SetTrueBasicBlock(BasicBlock *pTrueBB) { mTrueBB = pTrueBB; }
    inline void SetFalseBasicBlock(BasicBlock *pFalseBB) { mFalseBB = pFalseBB; }

private:
    Value* mInput1;
    Value* mInput2;

    BasicBlock *mTrueBB{nullptr};
    BasicBlock *mFalseBB{nullptr};
};


class InstructionBeq final : public InstructionBranch {
public:
    InstructionBeq(const InstructionId id) : InstructionBranch(InstructionType::Beq, id) {};

    InstructionBeq(const InstructionId id, Value* input1, Value* input2, BasicBlock *pTrueBB = nullptr, BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Beq, id, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBne final : public InstructionBranch {
public:
    InstructionBne(const InstructionId id) : InstructionBranch(InstructionType::Bne, id) {};

    InstructionBne(const InstructionId id, Value* input1, Value* input2, BasicBlock *pTrueBB = nullptr, BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Bne, id, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBgt final : public InstructionBranch {
public:
    InstructionBgt(const InstructionId id) : InstructionBranch(InstructionType::Bgt, id) {};

    InstructionBgt(const InstructionId id, Value* input1, Value* input2, BasicBlock *pTrueBB = nullptr, BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Bgt, id, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBlt final : public InstructionBranch {
public:
    InstructionBlt(const InstructionId id) : InstructionBranch(InstructionType::Blt, id) {};

    InstructionBlt(const InstructionId id, Value* input1, Value* input2, BasicBlock *pTrueBB = nullptr, BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Blt, id, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBge final : public InstructionBranch {
public:
    InstructionBge(const InstructionId id) : InstructionBranch(InstructionType::Bge, id) {};

    InstructionBge(const InstructionId id, Value* input1, Value* input2, BasicBlock *pTrueBB = nullptr, BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Bge, id, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBle final : public InstructionBranch {
public:
    InstructionBle(const InstructionId id) : InstructionBranch(InstructionType::Ble, id) {};

    InstructionBle(const InstructionId id, Value* input1, Value* input2, BasicBlock *pTrueBB = nullptr, BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Ble, id, input1, input2, pTrueBB, pFalseBB) {};
};



// Call instruction

class InstructionCall final : public Instruction {
public:
    // Constructors
    InstructionCall(const InstructionId id) : Instruction(InstructionType::Call, id) {};
    InstructionCall(const InstructionId id, Function* function) : Instruction(InstructionType::Call, id), mFunction{function} {};
    
    InstructionCall(const InstructionId id, Function* function, Value* ret)
    : Instruction(InstructionType::Call, id), mFunction{function}, mOutput{ret} {};
    
    InstructionCall(const InstructionId id, Function* function, const std::vector<Value*>& args)
    : Instruction(InstructionType::Call, id), mFunction{function}, mInputs{args} {};
    
    InstructionCall(const InstructionId id, Function* function, Value* ret, const std::vector<Value*>& args)
    : Instruction(InstructionType::Call, id), mFunction{function}, mOutput{ret}, mInputs{args} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual Value* GetOutput() const override { return mOutput; }

    // Getters
    inline Function* GetFunction() { return mFunction; }
    inline Value* GetReturnValue() { return mOutput; }
    inline const std::vector<Value*>& GetArguments() const { return mInputs; }

    // Setters
    inline void SetFunction(Function* function) { mFunction = function; }
    inline void SetReturnValue(Value* value) { mOutput = value; }
    inline void SetArguments(const std::vector<Value*>& args) { mInputs = args; }

private:
    Function* mFunction{nullptr};

    Value* mOutput{nullptr};
    std::vector<Value*> mInputs{};
};



// Ret instruction

class InstructionRet final : public Instruction {
public:
    // Constructors
    InstructionRet(const InstructionId id) : Instruction(InstructionType::Ret, id) {};
    InstructionRet(const InstructionId id, Value* returnValue) : Instruction(InstructionType::Ret, id), mReturnValue{returnValue} {};
    
    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual bool IsTerminator() const override { return true; }

    // Getters
    inline Value* GetReturnValue() { return mReturnValue; }

    // Setters
    inline void SetReturnValue(Value* returnValue) { mReturnValue = returnValue; }

private:
    Value* mReturnValue{nullptr};
};



// Alloc instruction

class InstructionAlloc final : public Instruction {
public:
    // Constructors
    InstructionAlloc(const InstructionId id) : Instruction(InstructionType::Alloc, id) {};
    InstructionAlloc(const InstructionId id, Value* output) : Instruction(InstructionType::Alloc, id), mOutput{output} {};
    InstructionAlloc(const InstructionId id, Value* output, const size_t count)
    : Instruction(InstructionType::Alloc, id), mOutput{output}, mCount{count} {};
    
    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual Value* GetOutput() const override { return mOutput; }

    // Getters
    inline size_t GetCount() const { return mCount; }

    // Setters
    inline void SetOutput(Value* output) { mOutput = output; }
    inline void SetCount(const size_t count) { mCount = count; }

private:
    Value* mOutput;
    size_t mCount{1};
};



// Phi instruction

class InstructionPhi final : public Instruction {
public:
    // Constructors
    InstructionPhi(const InstructionId id) : Instruction(InstructionType::Phi, id) {};
    InstructionPhi(const InstructionId id, const std::set<Value*>& inputs, Value* output)
    : Instruction(InstructionType::Phi, id), mInputs{inputs}, mOutput{output} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual Value* GetOutput() const override { return mOutput; }

    // Getters
    inline const std::set<Value*>& GetInputs() const { return mInputs; }

    // Setters
    inline void SetInputs(const std::set<Value*>& inputs) { mInputs = inputs; }
    inline void AddInput(Value* input)       { mInputs.insert(input); }
    inline bool HasInput(Value* input) const { return mInputs.contains(input); }
    inline void RemoveInput(Value* input)    { mInputs.erase(input); }

    inline void SetOutput(Value* value) { mOutput = value; }

private:
    std::set<Value*> mInputs{};
    Value* mOutput;
};



// Mv instruction

class InstructionMv final : public Instruction {
public:
    // Constructors
    InstructionMv(const InstructionId id) : Instruction(InstructionType::Mv, id) {};
    InstructionMv(const InstructionId id, Value* input, Value* output)
    : Instruction(InstructionType::Mv, id), mInput{input}, mOutput{output} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;
    virtual Value* GetOutput() const override { return mOutput; }

    // Getters
    inline Value* GetInput() const { return mInput; }

    // Setters
    inline void SetInput(Value* input) { mInput = input; }
    inline void SetOutput(Value* value) { mOutput = value; }

private:
    Value* mInput;
    Value* mOutput;
};

}   // namespace VMIR

#endif  // INSTRUCTION_H
