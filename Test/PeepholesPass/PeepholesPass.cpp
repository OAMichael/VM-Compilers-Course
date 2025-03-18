#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IRBuilder.h>
#include <PeepholesPass.h>


TEST(peepholes_pass, add_first) {
    /*
        BB_1:
            v0 = Mv ui64 42
            v1 = Add ui64 v0, 0
            v2 = Add ui64 v0, 1
            v3 = Mv f64 3.14
            v4 = Add f64 v3, 0.0
            v5 = Add f64 v3, -1.0
            Ret ui64 v1
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* fourtyTwo  = IrBuilder->CreateValue(42UL);
    VMIR::Value* zero       = IrBuilder->CreateValue(0UL);
    VMIR::Value* one        = IrBuilder->CreateValue(1UL);

    VMIR::Value* pi         = IrBuilder->CreateValue(3.14);
    VMIR::Value* zeroF      = IrBuilder->CreateValue(0.0);
    VMIR::Value* negOne     = IrBuilder->CreateValue(-1.0);

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Float64);

    IrBuilder->CreateMv(BB_1, fourtyTwo, v0);
    IrBuilder->CreateAdd(BB_1, v0, zero, v1);
    IrBuilder->CreateAdd(BB_1, v0, one, v2);
    IrBuilder->CreateMv(BB_1, pi, v3);
    IrBuilder->CreateAdd(BB_1, v3, zeroF, v4);
    IrBuilder->CreateAdd(BB_1, v3, negOne, v5);
    IrBuilder->CreateRet(BB_1, v1);

    VMIR::PeepholesPass peepholesPass{};
    peepholesPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 7);

    VMIR::Instruction* inst1 = BB_1->Front();
    VMIR::Instruction* inst2 = inst1->GetNext();
    VMIR::Instruction* inst3 = inst2->GetNext();
    VMIR::Instruction* inst4 = inst3->GetNext();
    VMIR::Instruction* inst5 = inst4->GetNext();
    VMIR::Instruction* inst6 = inst5->GetNext();
    VMIR::Instruction* inst7 = inst6->GetNext();

    EXPECT_EQ(inst1->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst2->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst3->GetType(), VMIR::InstructionType::Add);
    EXPECT_EQ(inst4->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst5->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst6->GetType(), VMIR::InstructionType::Add);
    EXPECT_EQ(inst7->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(peepholes_pass, add_second) {
    /*
        BB_1:
            v0 = Mv ui64 2718
            v1 = Add ui64 v0, v0
            v2 = Add ui64 v0, v1
            v3 = Mv f64 2.718
            v4 = Add f64 v3, v3
            v5 = Add f64 v3, -1.0
            Ret ui64 v1
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* num    = IrBuilder->CreateValue(2718UL);

    VMIR::Value* e      = IrBuilder->CreateValue(2.718);
    VMIR::Value* negOne = IrBuilder->CreateValue(-1.0);

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Float64);

    IrBuilder->CreateMv(BB_1, num, v0);
    IrBuilder->CreateAdd(BB_1, v0, v0, v1);
    IrBuilder->CreateAdd(BB_1, v0, v1, v2);
    IrBuilder->CreateMv(BB_1, e, v3);
    IrBuilder->CreateAdd(BB_1, v3, v3, v4);
    IrBuilder->CreateAdd(BB_1, v3, negOne, v5);
    IrBuilder->CreateRet(BB_1, v1);

    VMIR::PeepholesPass peepholesPass{};
    peepholesPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 7);

    VMIR::Instruction* inst1 = BB_1->Front();
    VMIR::Instruction* inst2 = inst1->GetNext();
    VMIR::Instruction* inst3 = inst2->GetNext();
    VMIR::Instruction* inst4 = inst3->GetNext();
    VMIR::Instruction* inst5 = inst4->GetNext();
    VMIR::Instruction* inst6 = inst5->GetNext();
    VMIR::Instruction* inst7 = inst6->GetNext();

    EXPECT_EQ(inst1->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst2->GetType(), VMIR::InstructionType::Shl);
    EXPECT_EQ(inst3->GetType(), VMIR::InstructionType::Add);
    EXPECT_EQ(inst4->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst5->GetType(), VMIR::InstructionType::Add);
    EXPECT_EQ(inst6->GetType(), VMIR::InstructionType::Add);
    EXPECT_EQ(inst7->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(peepholes_pass, ashr_first) {
    /*
        BB_1:
            v0 = Mv ui64 7
            v1 = Ashr ui64 v0, 0
            v2 = Ashr ui64 v0, v1
            v3 = Ashr ui64 3, v2
            Ret ui64 v2
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* zero  = IrBuilder->CreateValue(0UL);
    VMIR::Value* three = IrBuilder->CreateValue(3UL);
    VMIR::Value* seven = IrBuilder->CreateValue(7UL);

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    IrBuilder->CreateMv(BB_1, seven, v0);
    IrBuilder->CreateAshr(BB_1, v0, zero, v1);
    IrBuilder->CreateAshr(BB_1, v0, v1, v2);
    IrBuilder->CreateAshr(BB_1, three, v2, v3);
    IrBuilder->CreateRet(BB_1, v2);

    VMIR::PeepholesPass peepholesPass{};
    peepholesPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 5);

    VMIR::Instruction* inst1 = BB_1->Front();
    VMIR::Instruction* inst2 = inst1->GetNext();
    VMIR::Instruction* inst3 = inst2->GetNext();
    VMIR::Instruction* inst4 = inst3->GetNext();
    VMIR::Instruction* inst5 = inst4->GetNext();

    EXPECT_EQ(inst1->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst2->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst3->GetType(), VMIR::InstructionType::Ashr);
    EXPECT_EQ(inst4->GetType(), VMIR::InstructionType::Ashr);
    EXPECT_EQ(inst5->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(peepholes_pass, ashr_second) {
    /*
        BB_1:
            v0 = Mv ui64 7
            v1 = Ashr ui64 v0, 4        <-- Check this disappears
            v2 = Shl ui64 v1, 4         <-- Check this converts to And v0 with ~(2^4 - 1)
            Jump #BB_2

        BB_2:
            v3 = Mv i32 3
            v4 = Ashr i32 v3, 6         <-- Check this stays the same because v6 uses it
            v5 = Shl i32 v4, 6          <-- Check this converts to And v3 with ~(2^6 - 1)
            v6 = Mv i32 v4
            Jump #BB_3

        BB_3:
            v7 = Mv i8, 2
            v8 = Ashr i8 v7, 9          <-- Check this disappears
            v9 = Shl i8 v8, 9           <-- Check this converts to Mv i8 0
            Ret i8 v9
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Int8, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Func, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Func, "BB_3");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* seven = IrBuilder->CreateValue(uint64_t(7));
    VMIR::Value* four  = IrBuilder->CreateValue(uint64_t(4));

    VMIR::Value* three = IrBuilder->CreateValue(int32_t(3));
    VMIR::Value* six   = IrBuilder->CreateValue(int32_t(6));

    VMIR::Value* two   = IrBuilder->CreateValue(int8_t(2));
    VMIR::Value* nine  = IrBuilder->CreateValue(int8_t(9));

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v6 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v7 = IrBuilder->CreateValue(VMIR::ValueType::Int8);
    VMIR::Value* v8 = IrBuilder->CreateValue(VMIR::ValueType::Int8);
    VMIR::Value* v9 = IrBuilder->CreateValue(VMIR::ValueType::Int8);

    IrBuilder->CreateMv(BB_1, seven, v0);
    IrBuilder->CreateAshr(BB_1, v0, four, v1);
    IrBuilder->CreateShl(BB_1, v1, four, v2);
    IrBuilder->CreateJump(BB_1, BB_2);

    IrBuilder->CreateMv(BB_2, three, v3);
    IrBuilder->CreateAshr(BB_2, v3, six, v4);
    IrBuilder->CreateShl(BB_2, v4, six, v5);
    IrBuilder->CreateMv(BB_2, v4, v6);
    IrBuilder->CreateJump(BB_2, BB_3);

    IrBuilder->CreateMv(BB_3, two, v7);
    IrBuilder->CreateAshr(BB_3, v7, nine, v8);
    IrBuilder->CreateShl(BB_3, v8, nine, v9);
    IrBuilder->CreateRet(BB_3, v9);

    VMIR::PeepholesPass peepholesPass{};
    peepholesPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 3);
    ASSERT_EQ(BB_2->Size(), 5);
    ASSERT_EQ(BB_3->Size(), 3);

    VMIR::Instruction* inst1  = BB_1->Front();
    VMIR::Instruction* inst2  = inst1->GetNext();
    VMIR::Instruction* inst3  = inst2->GetNext();
    VMIR::Instruction* inst4  = BB_2->Front();
    VMIR::Instruction* inst5  = inst4->GetNext();
    VMIR::Instruction* inst6  = inst5->GetNext();
    VMIR::Instruction* inst7  = inst6->GetNext();
    VMIR::Instruction* inst8  = inst7->GetNext();
    VMIR::Instruction* inst9  = BB_3->Front();
    VMIR::Instruction* inst10 = inst9->GetNext();
    VMIR::Instruction* inst11 = inst10->GetNext();

    EXPECT_EQ(inst1->GetType(),  VMIR::InstructionType::Mv);
    EXPECT_EQ(inst2->GetType(),  VMIR::InstructionType::And);
    EXPECT_EQ(inst3->GetType(),  VMIR::InstructionType::Jump);
    EXPECT_EQ(inst4->GetType(),  VMIR::InstructionType::Mv);
    EXPECT_EQ(inst5->GetType(),  VMIR::InstructionType::Ashr);
    EXPECT_EQ(inst6->GetType(),  VMIR::InstructionType::And);
    EXPECT_EQ(inst7->GetType(),  VMIR::InstructionType::Mv);
    EXPECT_EQ(inst8->GetType(),  VMIR::InstructionType::Jump);
    EXPECT_EQ(inst9->GetType(),  VMIR::InstructionType::Mv);
    EXPECT_EQ(inst10->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst11->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(peepholes_pass, and_first) {
    /*
        BB_1:
            v0 = Mv i64 37
            v1 = And i64 v0, 0
            v2 = And i64 v0, 1
            v3 = And i64 v0, v2
            Ret i64 v1
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Int64, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* thirtySeven = IrBuilder->CreateValue(37L);
    VMIR::Value* zero        = IrBuilder->CreateValue(0L);
    VMIR::Value* one         = IrBuilder->CreateValue(1L);

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Int64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Int64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Int64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Int64);

    IrBuilder->CreateMv(BB_1, thirtySeven, v0);
    IrBuilder->CreateAnd(BB_1, v0, zero, v1);
    IrBuilder->CreateAnd(BB_1, v0, one, v2);
    IrBuilder->CreateAnd(BB_1, v0, v2, v3);
    IrBuilder->CreateRet(BB_1, v1);

    VMIR::PeepholesPass peepholesPass{};
    peepholesPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 5);

    VMIR::Instruction* inst1 = BB_1->Front();
    VMIR::Instruction* inst2 = inst1->GetNext();
    VMIR::Instruction* inst3 = inst2->GetNext();
    VMIR::Instruction* inst4 = inst3->GetNext();
    VMIR::Instruction* inst5 = inst4->GetNext();

    EXPECT_EQ(inst1->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst2->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst3->GetType(), VMIR::InstructionType::And);
    EXPECT_EQ(inst4->GetType(), VMIR::InstructionType::And);
    EXPECT_EQ(inst5->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(peepholes_pass, and_second) {
    /*
        BB_1:
            v0 = Mv i32 37
            v1 = And i32 v0, v0
            v2 = And i32 v0, v1
            v3 = And i32 v0, v2
            v4 = And i32 v3, v3
            Ret i32 v1
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Int32, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* thirtySeven = IrBuilder->CreateValue(37);

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Int32);

    IrBuilder->CreateMv(BB_1, thirtySeven, v0);
    IrBuilder->CreateAnd(BB_1, v0, v0, v1);
    IrBuilder->CreateAnd(BB_1, v0, v1, v2);
    IrBuilder->CreateAnd(BB_1, v0, v2, v3);
    IrBuilder->CreateAnd(BB_1, v3, v3, v4);
    IrBuilder->CreateRet(BB_1, v1);

    VMIR::PeepholesPass peepholesPass{};
    peepholesPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 6);

    VMIR::Instruction* inst1 = BB_1->Front();
    VMIR::Instruction* inst2 = inst1->GetNext();
    VMIR::Instruction* inst3 = inst2->GetNext();
    VMIR::Instruction* inst4 = inst3->GetNext();
    VMIR::Instruction* inst5 = inst4->GetNext();
    VMIR::Instruction* inst6 = inst5->GetNext();

    EXPECT_EQ(inst1->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst2->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst3->GetType(), VMIR::InstructionType::And);
    EXPECT_EQ(inst4->GetType(), VMIR::InstructionType::And);
    EXPECT_EQ(inst5->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst6->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}