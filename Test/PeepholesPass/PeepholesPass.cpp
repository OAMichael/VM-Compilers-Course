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

    VMIR::Value* fourtyTwo  = IrBuilder->CreateValue(Func, 42UL);
    VMIR::Value* zero       = IrBuilder->CreateValue(Func, 0UL);
    VMIR::Value* one        = IrBuilder->CreateValue(Func, 1UL);

    VMIR::Value* pi         = IrBuilder->CreateValue(Func, 3.14);
    VMIR::Value* zeroF      = IrBuilder->CreateValue(Func, 0.0);
    VMIR::Value* negOne     = IrBuilder->CreateValue(Func, -1.0);


    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Float64);
    VMIR::Value* v4 = IrBuilder->CreateValue(Func, VMIR::ValueType::Float64);
    VMIR::Value* v5 = IrBuilder->CreateValue(Func, VMIR::ValueType::Float64);

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

    VMIR::Value* num    = IrBuilder->CreateValue(Func, 2718UL);

    VMIR::Value* e      = IrBuilder->CreateValue(Func, 2.718);
    VMIR::Value* negOne = IrBuilder->CreateValue(Func, -1.0);


    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Float64);
    VMIR::Value* v4 = IrBuilder->CreateValue(Func, VMIR::ValueType::Float64);
    VMIR::Value* v5 = IrBuilder->CreateValue(Func, VMIR::ValueType::Float64);

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



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}