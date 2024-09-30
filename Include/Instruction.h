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
    }
}


static inline bool IsInstructionTerminator(const InstructionType it) {
    switch (it) {
        default:
        case InstructionType::Unknown:
        case InstructionType::Add:
        case InstructionType::Sub:
        case InstructionType::Mul:
        case InstructionType::Div:
        case InstructionType::Rem:
        case InstructionType::And:
        case InstructionType::Or:
        case InstructionType::Xor:
        case InstructionType::Shl:
        case InstructionType::Shr:
        case InstructionType::Load:
        case InstructionType::Store:
        case InstructionType::Call:
        case InstructionType::Alloc:
        case InstructionType::Phi: {
            return false;
        }
        case InstructionType::Jump:
        case InstructionType::Beq:
        case InstructionType::Bne:
        case InstructionType::Bgt:
        case InstructionType::Blt:
        case InstructionType::Bge:
        case InstructionType::Ble:
        case InstructionType::Ret: {
            return true;
        }
    }
}


// Base interface for all instructions
class Instruction {
public:
    Instruction(const InstructionType type) : mType{type} {};

    virtual ~Instruction() {};

    virtual std::string GetAsString() const = 0;
    virtual bool IsValid() const = 0;

    inline InstructionType GetType() const { return mType; }
    inline void SetParentBasicBlock(const BasicBlock* parentBasicBlock) { mParentBasicBlock = parentBasicBlock; }

protected:
    InstructionType mType{};
    const BasicBlock* mParentBasicBlock{nullptr};
};


// Interface for arithmetic instructions
class InstructionArithmetic : public Instruction {
public:
    // Constructors
    InstructionArithmetic(const InstructionType type) : Instruction(type) {};

    InstructionArithmetic(const InstructionType type, const Value* input1, const Value* input2, const Value* output) 
    : Instruction(type), mInput1{input1}, mInput2{input2}, mOutput{output} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const Value* GetInput1() const { return mInput1; }
    inline const Value* GetInput2() const { return mInput2; }
    inline const Value* GetOutput() const { return mOutput; }

    // Setters
    inline void SetInput1(const Value* value) { mInput1 = value; }
    inline void SetInput2(const Value* value) { mInput2 = value; }
    inline void SetOutput(const Value* value) { mOutput = value; }

protected:
    const Value* mInput1;
    const Value* mInput2;
    const Value* mOutput;
};



// Simple arithmetic instructions

class InstructionAdd final : public InstructionArithmetic {
public:
    // Constructors
    InstructionAdd() : InstructionArithmetic(InstructionType::Add) {};

    InstructionAdd(const Value* input1, const Value* input2, const Value* output)
    : InstructionArithmetic(InstructionType::Add, input1, input2, output) {};
};

class InstructionSub final : public InstructionArithmetic {
public:
    // Constructors
    InstructionSub() : InstructionArithmetic(InstructionType::Sub) {};

    InstructionSub(const Value* input1, const Value* input2, const Value* output)
    : InstructionArithmetic(InstructionType::Sub, input1, input2, output) {};
};

class InstructionMul final : public InstructionArithmetic {
public:
    // Constructors
    InstructionMul() : InstructionArithmetic(InstructionType::Mul) {};

    InstructionMul(const Value* input1, const Value* input2, const Value* output)
    : InstructionArithmetic(InstructionType::Mul, input1, input2, output) {};
};

class InstructionDiv final : public InstructionArithmetic {
public:
    // Constructors
    InstructionDiv() : InstructionArithmetic(InstructionType::Div) {};

    InstructionDiv(const Value* input1, const Value* input2, const Value* output)
    : InstructionArithmetic(InstructionType::Div, input1, input2, output) {};
};

class InstructionRem final : public InstructionArithmetic {
public:
    // Constructors
    InstructionRem() : InstructionArithmetic(InstructionType::Rem) {};

    InstructionRem(const Value* input1, const Value* input2, const Value* output)
    : InstructionArithmetic(InstructionType::Rem, input1, input2, output) {};
};



// Bitwise instructions

class InstructionBitwise : public InstructionArithmetic {
public:
    // Constructors
    InstructionBitwise(const InstructionType type) : InstructionArithmetic(type) {};

    InstructionBitwise(const InstructionType type, const Value* input1, const Value* input2, const Value* output)
    : InstructionArithmetic(type, input1, input2, output) {};

    // Validity rules for bitwise instructions are stricter
    virtual bool IsValid() const override;
};


class InstructionAnd final : public InstructionBitwise {
public:
    // Constructors
    InstructionAnd() : InstructionBitwise(InstructionType::And) {};

    InstructionAnd(const Value* input1, const Value* input2, const Value* output)
    : InstructionBitwise(InstructionType::And, input1, input2, output) {};
};

class InstructionOr final : public InstructionBitwise {
public:
    // Constructors
    InstructionOr() : InstructionBitwise(InstructionType::And) {};

    InstructionOr(const Value* input1, const Value* input2, const Value* output)
    : InstructionBitwise(InstructionType::And, input1, input2, output) {};
};

class InstructionXor final : public InstructionBitwise {
public:
    // Constructors
    InstructionXor() : InstructionBitwise(InstructionType::And) {};

    InstructionXor(const Value* input1, const Value* input2, const Value* output)
    : InstructionBitwise(InstructionType::And, input1, input2, output) {};
};

class InstructionShl final : public InstructionBitwise {
public:
    // Constructors
    InstructionShl() : InstructionBitwise(InstructionType::Shl) {};

    InstructionShl(const Value* input1, const Value* input2, const Value* output)
    : InstructionBitwise(InstructionType::Shl, input1, input2, output) {};
};

class InstructionShr final : public InstructionBitwise {
public:
    // Constructors
    InstructionShr() : InstructionBitwise(InstructionType::Shr) {};

    InstructionShr(const Value* input1, const Value* input2, const Value* output)
    : InstructionBitwise(InstructionType::Shr, input1, input2, output) {};
};



// Memory instrucions

class InstructionLoad final : public Instruction {
public:
    // Constructors
    InstructionLoad() : Instruction(InstructionType::Load) {};

    InstructionLoad(const Value* loadPtr, const Value* output) 
    : Instruction(InstructionType::Load), mLoadPtr{loadPtr}, mOutput{output} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const Value* GetLoadPtr() const { return mLoadPtr; }
    inline const Value* GetOutput() const { return mOutput; }

    // Setters
    inline void SetLoadPtr(const Value* value) { mLoadPtr = value; }
    inline void SetOutput(const Value* value) { mOutput = value; }

private:
    const Value* mLoadPtr;
    const Value* mOutput;
};


class InstructionStore final : public Instruction {
public:
    // Constructors
    InstructionStore() : Instruction(InstructionType::Store) {};

    InstructionStore(const Value* storePtr, const Value* input) 
    : Instruction(InstructionType::Store), mStorePtr{storePtr}, mInput{input} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const Value* GetStorePtr() const { return mStorePtr; }
    inline const Value* GetInput() const { return mInput; }

    // Setters
    inline void SetStorePtr(const Value* value) { mStorePtr = value; }
    inline void SetInput(const Value* value) { mInput = value; }

private:
    const Value* mStorePtr;
    const Value* mInput;
};



// Jump instruction

class InstructionJump final : public Instruction {
public:
    // Constructors
    InstructionJump() : Instruction(InstructionType::Jump) {};

    InstructionJump(const BasicBlock *pJumpBB) : Instruction(InstructionType::Jump), mJumpBB{pJumpBB} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const BasicBlock *GetJumpBasicBlock() const { return mJumpBB; }

    // Setters
    inline void SetJumpBasicBlock(const BasicBlock *pJumpBB) { mJumpBB = pJumpBB; }

private:
    const BasicBlock *mJumpBB{nullptr};
};



// Branch instructions

class InstructionBranch : public Instruction {
public:
    // Constructors
    InstructionBranch(const InstructionType type) : Instruction(type) {};

    InstructionBranch(const InstructionType type, const Value* input1, const Value* input2, const BasicBlock *pTrueBB, const BasicBlock *pFalseBB)
    : Instruction(type), mInput1{input1}, mInput2{input2}, mTrueBB{pTrueBB}, mFalseBB{pFalseBB} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const Value* GetInput1() const { return mInput1; }
    inline const Value* GetInput2() const { return mInput2; }

    inline const BasicBlock *GetTrueBasicBlock()  const { return mTrueBB; }
    inline const BasicBlock *GetFalseBasicBlock() const { return mFalseBB; }

    // Setters
    inline void SetInput1(const Value* value) { mInput1 = value; }
    inline void SetInput2(const Value* value) { mInput2 = value; }

    inline void SetTrueBasicBlock(const BasicBlock *pTrueBB) { mTrueBB = pTrueBB; }
    inline void SetFalseBasicBlock(const BasicBlock *pFalseBB) { mFalseBB = pFalseBB; }

private:
    const Value* mInput1;
    const Value* mInput2;

    const BasicBlock *mTrueBB{nullptr};
    const BasicBlock *mFalseBB{nullptr};
};


class InstructionBeq final : public InstructionBranch {
public:
    InstructionBeq() : InstructionBranch(InstructionType::Beq) {};

    InstructionBeq(const Value* input1, const Value* input2, const BasicBlock *pTrueBB = nullptr, const BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Beq, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBne final : public InstructionBranch {
public:
    InstructionBne() : InstructionBranch(InstructionType::Bne) {};

    InstructionBne(const Value* input1, const Value* input2, const BasicBlock *pTrueBB = nullptr, const BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Bne, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBgt final : public InstructionBranch {
public:
    InstructionBgt() : InstructionBranch(InstructionType::Bgt) {};

    InstructionBgt(const Value* input1, const Value* input2, const BasicBlock *pTrueBB = nullptr, const BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Bgt, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBlt final : public InstructionBranch {
public:
    InstructionBlt() : InstructionBranch(InstructionType::Blt) {};

    InstructionBlt(const Value* input1, const Value* input2, const BasicBlock *pTrueBB = nullptr, const BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Blt, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBge final : public InstructionBranch {
public:
    InstructionBge() : InstructionBranch(InstructionType::Bge) {};

    InstructionBge(const Value* input1, const Value* input2, const BasicBlock *pTrueBB = nullptr, const BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Bge, input1, input2, pTrueBB, pFalseBB) {};
};

class InstructionBle final : public InstructionBranch {
public:
    InstructionBle() : InstructionBranch(InstructionType::Ble) {};

    InstructionBle(const Value* input1, const Value* input2, const BasicBlock *pTrueBB = nullptr, const BasicBlock *pFalseBB = nullptr)
    : InstructionBranch(InstructionType::Ble, input1, input2, pTrueBB, pFalseBB) {};
};



// Call instruction

class InstructionCall final : public Instruction {
public:
    // Constructors
    InstructionCall() : Instruction(InstructionType::Call) {};
    InstructionCall(const Function* function) : Instruction(InstructionType::Call), mFunction{function} {};
    
    InstructionCall(const Function* function, const Value* ret)
    : Instruction(InstructionType::Call), mFunction{function}, mOutput{ret} {};
    
    InstructionCall(const Function* function, const std::vector<const Value*>& args)
    : Instruction(InstructionType::Call), mFunction{function}, mInputs{args} {};
    
    InstructionCall(const Function* function, const Value* ret, const std::vector<const Value*>& args)
    : Instruction(InstructionType::Call), mFunction{function}, mOutput{ret}, mInputs{args} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const Function* GetFunction() const { return mFunction; }
    inline const Value* GetReturnValue() const { return mOutput; }
    inline const std::vector<const Value*>& GetArguments() const { return mInputs; }

    // Setters
    inline void SetFunction(const Function* function) { mFunction = function; }
    inline void SetReturnValue(const Value* value) { mOutput = value; }
    inline void SetArguments(const std::vector<const Value*>& args) { mInputs = args; }

private:
    const Function* mFunction{nullptr};

    const Value* mOutput{nullptr};
    std::vector<const Value*> mInputs{};
};



// Ret instruction

class InstructionRet final : public Instruction {
public:
    // Constructors
    InstructionRet() : Instruction(InstructionType::Ret) {};
    InstructionRet(const Value* output) : Instruction(InstructionType::Ret), mOutput{output} {};
    
    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const Value* GetOutput() const { return mOutput; }

    // Setters
    inline void SetOutput(const Value* output) { mOutput = output; }

private:
    const Value* mOutput{nullptr};
};



// Alloc instruction

class InstructionAlloc final : public Instruction {
public:
    // Constructors
    InstructionAlloc() : Instruction(InstructionType::Alloc) {};
    InstructionAlloc(const Value* output) : Instruction(InstructionType::Alloc), mOutput{output} {};
    InstructionAlloc(const Value* output, const size_t count) : Instruction(InstructionType::Alloc), mOutput{output}, mCount{count} {};
    
    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const Value* GetOutput() const { return mOutput; }
    inline size_t GetCount() const { return mCount; }

    // Setters
    inline void SetOutput(const Value* output) { mOutput = output; }
    inline void SetCount(const size_t count) { mCount = count; }

private:
    const Value* mOutput;
    size_t mCount{1};
};



// Phi instruction

class InstructionPhi final : public Instruction {
public:
    // Constructors
    InstructionPhi() : Instruction(InstructionType::Phi) {};
    InstructionPhi(const Value* input1, const Value* input2, const Value* output)
    : Instruction(InstructionType::Phi), mInput1{input1}, mInput2{input2}, mOutput{output} {};

    virtual std::string GetAsString() const override;
    virtual bool IsValid() const override;

    // Getters
    inline const Value* GetInput1() const { return mInput1; }
    inline const Value* GetInput2() const { return mInput2; }
    inline const Value* GetOutput() const { return mOutput; }

    // Setters
    inline void SetInput1(const Value* value) { mInput1 = value; }
    inline void SetInput2(const Value* value) { mInput2 = value; }
    inline void SetOutput(const Value* value) { mOutput = value; }

private:
    const Value* mInput1;
    const Value* mInput2;
    const Value* mOutput;
};

}   // namespace VMIR

#endif  // INSTRUCTION_H
