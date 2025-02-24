#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IRBuilder.h>
#include <ConstantFoldingPass.h>


TEST(constant_folding_pass, simple) {
    /*
        BB_1:
            v0 = Mv f64 3.14
            Ret f64 v0

                ||
                ||
                ||
                \/

        BB_1:
            Ret f64 3.14
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Float64, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* val_pi  = IrBuilder->CreateValue(Func, 3.14);

    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Float64);

    IrBuilder->CreateMv(BB_1, val_pi, v0);
    IrBuilder->CreateRet(BB_1, v0);

    VMIR::ConstantFoldingPass constantFoldingPass{};
    constantFoldingPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 1);
    ASSERT_EQ(BB_1->Front()->GetType(), VMIR::InstructionType::Ret);

    VMIR::InstructionRet* retInst = static_cast<VMIR::InstructionRet*>(BB_1->Front());
    VMIR::Value* retVal = retInst->GetReturnValue();

    ASSERT_NE(retVal, nullptr);
    ASSERT_TRUE(retVal->HasValue());
    EXPECT_DOUBLE_EQ(retVal->GetValue<double>().value(), 3.14);

    IrBuilder->Cleanup();
}


TEST(constant_folding_pass, mv_add) {
    /*
        BB_1:
            v0 = Mv ui64 97
            v1 = Mv ui64 87
            v2 = Add ui64 v0, v1
            v3 = Add ui64 v0, 314
            v4 = Add ui64 v2, v3
            Ret ui64 v4

                ||
                ||
                ||
                \/

        BB_1:
            Ret ui64 595
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* val_97  = IrBuilder->CreateValue(Func, 97UL);
    VMIR::Value* val_87  = IrBuilder->CreateValue(Func, 87UL);
    VMIR::Value* val_314 = IrBuilder->CreateValue(Func, 314UL);

    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateMv(BB_1, val_97, v0);
    IrBuilder->CreateMv(BB_1, val_87, v1);
    IrBuilder->CreateAdd(BB_1, v0, v1, v2);
    IrBuilder->CreateAdd(BB_1, v0, val_314, v3);
    IrBuilder->CreateAdd(BB_1, v2, v3, v4);
    IrBuilder->CreateRet(BB_1, v4);

    VMIR::ConstantFoldingPass constantFoldingPass{};
    constantFoldingPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 1);
    ASSERT_EQ(BB_1->Front()->GetType(), VMIR::InstructionType::Ret);

    VMIR::InstructionRet* retInst = static_cast<VMIR::InstructionRet*>(BB_1->Front());
    VMIR::Value* retVal = retInst->GetReturnValue();

    ASSERT_NE(retVal, nullptr);
    ASSERT_TRUE(retVal->HasValue());
    EXPECT_EQ(retVal->GetValue<uint64_t>().value(), 595);

    IrBuilder->Cleanup();
}


TEST(constant_folding_pass, multiple) {
    /*
        BB_1:
            v0 = Mv ui32 45
            v1 = Mv ui32 78
            v2 = Add ui32 v0, v1
            v3 = Mul ui32 v2, v1
            v4 = Sub ui32 v3, v2
            v5 = Add ui32 v4, v4
            v6 = Ashr ui32 v5, 3
            v7 = Xor ui32 v6, v3
            v8 = Add ui32 v7, 555
            Ret ui32 v8

                ||
                ||
                ||
                \/

        BB_1:
            Ret ui32 11888
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint32, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* val_45  = IrBuilder->CreateValue(Func, 45u);
    VMIR::Value* val_78  = IrBuilder->CreateValue(Func, 78u);
    VMIR::Value* val_3   = IrBuilder->CreateValue(Func, 3u);
    VMIR::Value* val_555 = IrBuilder->CreateValue(Func, 555u);

    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v4 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v5 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v6 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v7 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v8 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);

    IrBuilder->CreateMv(BB_1, val_45, v0);
    IrBuilder->CreateMv(BB_1, val_78, v1);
    IrBuilder->CreateAdd(BB_1, v0, v1, v2);
    IrBuilder->CreateMul(BB_1, v2, v1, v3);
    IrBuilder->CreateSub(BB_1, v3, v2, v4);
    IrBuilder->CreateAdd(BB_1, v4, v4, v5);
    IrBuilder->CreateAshr(BB_1, v5, val_3, v6);
    IrBuilder->CreateXor(BB_1, v6, v3, v7);
    IrBuilder->CreateAdd(BB_1, v7, val_555, v8);
    IrBuilder->CreateRet(BB_1, v8);

    VMIR::ConstantFoldingPass constantFoldingPass{};
    constantFoldingPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 1);
    ASSERT_EQ(BB_1->Front()->GetType(), VMIR::InstructionType::Ret);

    VMIR::InstructionRet* retInst = static_cast<VMIR::InstructionRet*>(BB_1->Front());
    VMIR::Value* retVal = retInst->GetReturnValue();

    ASSERT_NE(retVal, nullptr);
    ASSERT_TRUE(retVal->HasValue());
    EXPECT_EQ(retVal->GetValue<uint32_t>().value(), 11888);

    IrBuilder->Cleanup();
}


TEST(constant_folding_pass, phi_behavior) {
    /*
        BB_1:
            v0 = Mv ui32 45
            Beq ui32 v0, 1 ? #BB_2 : #BB_3

        BB_2:
            v1 = Mv ui32 67
            Jump #BB_4

        BB_3:
            v2 = Mv ui32 55
            Jump #BB_4

        BB_4:
            v3 = Phi ui32 v1, v2
            Ret ui32 v3

                ||
                ||
                ||
                \/

        BB_1:
            Beq ui32 45, 1 ? #BB_2 : #BB_3

        BB_2:
            v1 = Mv ui32 67
            Jump #BB_4

        BB_3:
            v2 = Mv ui32 55
            Jump #BB_4

        BB_4:
            v3 = Phi ui32 v1, v2
            Ret ui32 v3
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint32, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Func, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Func, "BB_3");
    VMIR::BasicBlock* BB_4 = IrBuilder->CreateBasicBlock(Func, "BB_4");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* val_45 = IrBuilder->CreateValue(Func, 45u);
    VMIR::Value* val_1  = IrBuilder->CreateValue(Func, 1u);
    VMIR::Value* val_67 = IrBuilder->CreateValue(Func, 67u);
    VMIR::Value* val_55 = IrBuilder->CreateValue(Func, 55u);

    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);

    IrBuilder->CreateMv(BB_1, val_45, v0);
    IrBuilder->CreateBeq(BB_1, v0, val_1, BB_2, BB_3);

    IrBuilder->CreateMv(BB_2, val_67, v1);
    IrBuilder->CreateJump(BB_2, BB_4);

    IrBuilder->CreateMv(BB_3, val_55, v2);
    IrBuilder->CreateJump(BB_3, BB_4);

    IrBuilder->CreatePhi(BB_4, {v1, v2}, v3);
    IrBuilder->CreateRet(BB_4, v3);

    VMIR::ConstantFoldingPass constantFoldingPass{};
    constantFoldingPass.Run(Func);

    ASSERT_EQ(BB_1->Size(), 1);
    ASSERT_EQ(BB_2->Size(), 2);
    ASSERT_EQ(BB_3->Size(), 2);
    ASSERT_EQ(BB_4->Size(), 2);

    VMIR::Instruction* inst1 = BB_1->Front();
    VMIR::Instruction* inst2 = BB_2->Front();
    VMIR::Instruction* inst3 = inst2->GetNext();
    VMIR::Instruction* inst4 = BB_3->Front();
    VMIR::Instruction* inst5 = inst4->GetNext();
    VMIR::Instruction* inst6 = BB_4->Front();
    VMIR::Instruction* inst7 = inst6->GetNext();

    ASSERT_EQ(inst1->GetType(), VMIR::InstructionType::Beq);
    VMIR::InstructionBeq* inst1_beq = static_cast<VMIR::InstructionBeq*>(inst1);
    VMIR::Value* inst1_input1 = inst1_beq->GetInput1();
    VMIR::Value* inst1_input2 = inst1_beq->GetInput2();
    ASSERT_NE(inst1_input1, nullptr);
    ASSERT_NE(inst1_input2, nullptr);
    ASSERT_TRUE(inst1_input1->HasValue());
    ASSERT_TRUE(inst1_input2->HasValue());
    EXPECT_EQ(inst1_input1->GetValue<uint32_t>().value(), 45u);
    EXPECT_EQ(inst1_input2->GetValue<uint32_t>().value(), 1u);

    EXPECT_EQ(inst2->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst3->GetType(), VMIR::InstructionType::Jump);

    EXPECT_EQ(inst4->GetType(), VMIR::InstructionType::Mv);
    EXPECT_EQ(inst5->GetType(), VMIR::InstructionType::Jump);

    ASSERT_EQ(inst6->GetType(), VMIR::InstructionType::Phi);
    VMIR::InstructionPhi* inst6_phi = static_cast<VMIR::InstructionPhi*>(inst6);
    EXPECT_EQ(inst6_phi->GetInputs().size(), 2);
    EXPECT_THAT(inst6_phi->GetInputs(), ::testing::UnorderedElementsAre(v1, v2));

    EXPECT_EQ(inst7->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}