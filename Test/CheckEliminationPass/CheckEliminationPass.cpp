#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IRBuilder.h>
#include <CheckEliminationPass.h>


TEST(check_elimination_pass, null_check_simple) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            NullCheck ptr v0
            v1 = Load ui64, ptr v0
            NullCheck ptr v0
            Store ui64 10, ptr v0
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            NullCheck ptr v0
            v1 = Load ui64, ptr v0
            Store ui64 10, ptr v0
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* ten = IrBuilder->CreateValue(uint64_t(10));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Uint64);
    IrBuilder->CreateNullCheck(BB_1, v0);
    IrBuilder->CreateLoad(BB_1, v0, v1);
    IrBuilder->CreateNullCheck(BB_1, v0);
    IrBuilder->CreateStore(BB_1, v0, ten);
    IrBuilder->CreateRet(BB_1);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 5);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::NullCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, null_check_multiple_bb_1) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            NullCheck ptr v0
            v1 = Load ui64, ptr v0
            Jump #BB_2

        BB_2: (preds: BB_1)
            NullCheck ptr v0
            Store ui64 5, ptr v0
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            NullCheck ptr v0
            v1 = Load ui64, ptr v0
            Jump #BB_2

        BB_2: (preds: BB_1)
            Store ui64 5, ptr v0
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* five = IrBuilder->CreateValue(uint64_t(5));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Foo, "BB_2");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Uint64);
    IrBuilder->CreateNullCheck(BB_1, v0);
    IrBuilder->CreateLoad(BB_1, v0, v1);
    IrBuilder->CreateJump(BB_1, BB_2);

    IrBuilder->CreateNullCheck(BB_2, v0);
    IrBuilder->CreateStore(BB_2, v0, five);
    IrBuilder->CreateRet(BB_2);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 4);
    ASSERT_EQ(BB_2->Size(), 2);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::NullCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Jump);
    inst = BB_2->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, null_check_multiple_bb_2) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            NullCheck ptr v0
            v1 = Load ui64, ptr v0
            Beq ui64 v1, 2 ? #BB_2 : #BB_3

        BB_2: (preds: BB_1)
            NullCheck ptr v0
            Store ui64 1, ptr v0
            Ret

        BB_3: (preds: BB_1)
            NullCheck ptr v0
            Store ui64 2, ptr v0
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            NullCheck ptr v0
            v1 = Load ui64, ptr v0
            Beq ui64 v1, 2 ? #BB_2 : #BB_3

        BB_2: (preds: BB_1)
            Store ui64 1, ptr v0
            Ret

        BB_3: (preds: BB_1)
            Store ui64 2, ptr v0
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* two = IrBuilder->CreateValue(uint64_t(2));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Foo, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Foo, "BB_3");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Uint64);
    IrBuilder->CreateNullCheck(BB_1, v0);
    IrBuilder->CreateLoad(BB_1, v0, v1);
    IrBuilder->CreateBeq(BB_1, v1, two, BB_2, BB_3);

    IrBuilder->CreateNullCheck(BB_2, v0);
    IrBuilder->CreateStore(BB_2, v0, one);
    IrBuilder->CreateRet(BB_2);

    IrBuilder->CreateNullCheck(BB_3, v0);
    IrBuilder->CreateStore(BB_3, v0, two);
    IrBuilder->CreateRet(BB_3);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 4);
    ASSERT_EQ(BB_2->Size(), 2);
    ASSERT_EQ(BB_3->Size(), 2);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::NullCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Beq);
    inst = BB_2->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);
    inst = BB_3->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, null_check_multiple_bb_3) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            Beq ui64 1, 2 ? #BB_2 : #BB_3

        BB_2: (preds: BB_1)
            NullCheck ptr v0
            Store ui64 1, ptr v0
            Ret

        BB_3: (preds: BB_1)
            NullCheck ptr v0
            Store ui64 2, ptr v0
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            Beq ui64 1, 2 ? #BB_2 : #BB_3

        BB_2: (preds: BB_1)
            NullCheck ptr v0
            Store ui64 1, ptr v0
            Ret

        BB_3: (preds: BB_1)
            NullCheck ptr v0
            Store ui64 2, ptr v0
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* two = IrBuilder->CreateValue(uint64_t(2));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Foo, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Foo, "BB_3");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Uint64);
    IrBuilder->CreateBeq(BB_1, one, two, BB_2, BB_3);

    IrBuilder->CreateNullCheck(BB_2, v0);
    IrBuilder->CreateStore(BB_2, v0, one);
    IrBuilder->CreateRet(BB_2);

    IrBuilder->CreateNullCheck(BB_3, v0);
    IrBuilder->CreateStore(BB_3, v0, two);
    IrBuilder->CreateRet(BB_3);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 2);
    ASSERT_EQ(BB_2->Size(), 3);
    ASSERT_EQ(BB_3->Size(), 3);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Beq);
    inst = BB_2->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::NullCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);
    inst = BB_3->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::NullCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, null_check_different_pointers) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            v1 = Alloc ui32
            NullCheck ptr v0
            v2 = Load ui64, ptr v0
            NullCheck ptr v1
            v3 = Load ui32, ptr v1
            NullCheck ptr v0
            Store ui64 4, ptr v0
            NullCheck ptr v1
            Store ui32 5, ptr v1
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64
            v1 = Alloc ui32
            NullCheck ptr v0
            v2 = Load ui64, ptr v0
            NullCheck ptr v1
            v3 = Load ui32, ptr v1
            Store ui64 4, ptr v0
            Store ui32 5, ptr v1
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Uint32);

    VMIR::Value* four = IrBuilder->CreateValue(uint64_t(4));
    VMIR::Value* five = IrBuilder->CreateValue(uint32_t(5));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Uint64);
    IrBuilder->CreateAlloc(BB_1, v1, VMIR::ValueType::Uint32);
    IrBuilder->CreateNullCheck(BB_1, v0);
    IrBuilder->CreateLoad(BB_1, v0, v2);
    IrBuilder->CreateNullCheck(BB_1, v1);
    IrBuilder->CreateLoad(BB_1, v1, v3);
    IrBuilder->CreateNullCheck(BB_1, v0);
    IrBuilder->CreateStore(BB_1, v0, four);
    IrBuilder->CreateNullCheck(BB_1, v1);
    IrBuilder->CreateStore(BB_1, v1, five);
    IrBuilder->CreateRet(BB_1);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 9);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::NullCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::NullCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, bounds_check_simple) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64, 4
            v1 = Add ptr v0, 1
            BoundsCheck ptr v1, [ptr v0, 4]
            v2 = Load ui64, ptr v1
            BoundsCheck ptr v1, [ptr v0, 4]
            Store ui64 2, ptr v1
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui64, 4
            v1 = Add ptr v0, 1
            BoundsCheck ptr v1, [ptr v0, 4]
            v2 = Load ui64, ptr v1
            Store ui64 2, ptr v1
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    // v0 is an array and v1 is a pointer, one is an index
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* two = IrBuilder->CreateValue(uint64_t(2));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Uint64, 4);
    IrBuilder->CreateAdd(BB_1, v0, one, v1);
    IrBuilder->CreateBoundsCheck(BB_1, v1, v0);
    IrBuilder->CreateLoad(BB_1, v1, v2);
    IrBuilder->CreateBoundsCheck(BB_1, v1, v0);
    IrBuilder->CreateStore(BB_1, v1, two);
    IrBuilder->CreateRet(BB_1);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 6);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Add);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, bounds_check_multiple_bb_1) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui32, 31
            v1 = Add ptr v0, 12
            BoundsCheck ptr v1, [ptr v0, 31]
            v2 = Load ui32, ptr v1
            Jump #BB_2

        BB_2: (preds: BB_1)
            BoundsCheck ptr v1, [ptr v0, 31]
            Store ui32 3, ptr v1
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc ui32, 31
            v1 = Add ptr v0, 12
            BoundsCheck ptr v1, [ptr v0, 31]
            v2 = Load ui32, ptr v1
            Jump #BB_2

        BB_2: (preds: BB_1)
            Store ui32 3, ptr v1
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    // v0 is an array and v1 is a pointer, twelve is an index
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint32);

    VMIR::Value* three = IrBuilder->CreateValue(uint32_t(3));
    VMIR::Value* twelve = IrBuilder->CreateValue(uint32_t(12));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Foo, "BB_2");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Uint32, 31);
    IrBuilder->CreateAdd(BB_1, v0, twelve, v1);
    IrBuilder->CreateBoundsCheck(BB_1, v1, v0);
    IrBuilder->CreateLoad(BB_1, v1, v2);
    IrBuilder->CreateJump(BB_1, BB_2);

    IrBuilder->CreateBoundsCheck(BB_2, v1, v0);
    IrBuilder->CreateStore(BB_2, v1, three);
    IrBuilder->CreateRet(BB_2);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 5);
    ASSERT_EQ(BB_2->Size(), 2);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Add);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Jump);
    inst = BB_2->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, bounds_check_multiple_bb_2) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc i16, 7
            v1 = Add ptr v0, 5
            BoundsCheck ptr v1, [ptr v0, 7]
            v2 = Load i16, ptr v1
            Beq i16 v2, 0 ? #BB_2 : #BB_3

        BB_2: (preds: BB_1)
            BoundsCheck ptr v1, [ptr v0, 7]
            Store i16 0, ptr v1
            Ret

        BB_3: (preds: BB_1)
            BoundsCheck ptr v1, [ptr v0, 7]
            Store i16 -3, ptr v1
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc i16, 7
            v1 = Add ptr v0, 5
            BoundsCheck ptr v1, [ptr v0, 7]
            v2 = Load i16, ptr v1
            Beq i16 v2, 0 ? #BB_2 : #BB_3

        BB_2: (preds: BB_1)
            Store i16 0, ptr v1
            Ret

        BB_3: (preds: BB_1)
            Store i16 -3, ptr v1
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    // v0 is an array and v1 is a pointer, five is an index
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Int16);

    VMIR::Value* zero = IrBuilder->CreateValue(int16_t(0));
    VMIR::Value* negThree = IrBuilder->CreateValue(int16_t(-3));
    VMIR::Value* five = IrBuilder->CreateValue(uint64_t(5));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Foo, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Foo, "BB_3");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Int16, 7);
    IrBuilder->CreateAdd(BB_1, v0, five, v1);
    IrBuilder->CreateBoundsCheck(BB_1, v1, v0);
    IrBuilder->CreateLoad(BB_1, v1, v2);
    IrBuilder->CreateBeq(BB_1, v2, zero, BB_2, BB_3);

    IrBuilder->CreateBoundsCheck(BB_2, v1, v0);
    IrBuilder->CreateStore(BB_2, v1, zero);
    IrBuilder->CreateRet(BB_2);

    IrBuilder->CreateBoundsCheck(BB_3, v1, v0);
    IrBuilder->CreateStore(BB_3, v1, negThree);
    IrBuilder->CreateRet(BB_3);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 5);
    ASSERT_EQ(BB_2->Size(), 2);
    ASSERT_EQ(BB_3->Size(), 2);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Add);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Beq);
    inst = BB_2->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);
    inst = BB_3->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, bounds_check_multiple_bb_3) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc i16, 7
            v1 = Add ptr v0, 5
            Beq i16 0, 0 ? #BB_2 : #BB_3

        BB_2: (preds: BB_1)
            BoundsCheck ptr v1, [ptr v0, 7]
            Store i16 0, ptr v1
            Ret

        BB_3: (preds: BB_1)
            BoundsCheck ptr v1, [ptr v0, 7]
            Store i16 -3, ptr v1
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc i16, 7
            v1 = Add ptr v0, 5
            Beq i16 0, 0 ? #BB_2 : #BB_3

        BB_2: (preds: BB_1)
            BoundsCheck ptr v1, [ptr v0, 7]
            Store i16 0, ptr v1
            Ret

        BB_3: (preds: BB_1)
            BoundsCheck ptr v1, [ptr v0, 7]
            Store i16 -3, ptr v1
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    // v0 is an array and v1 is a pointer, five is an index
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);

    VMIR::Value* zero = IrBuilder->CreateValue(int16_t(0));
    VMIR::Value* negThree = IrBuilder->CreateValue(int16_t(-3));
    VMIR::Value* five = IrBuilder->CreateValue(uint64_t(5));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Foo, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Foo, "BB_3");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Int16, 7);
    IrBuilder->CreateAdd(BB_1, v0, five, v1);
    IrBuilder->CreateBeq(BB_1, zero, zero, BB_2, BB_3);

    IrBuilder->CreateBoundsCheck(BB_2, v1, v0);
    IrBuilder->CreateStore(BB_2, v1, zero);
    IrBuilder->CreateRet(BB_2);

    IrBuilder->CreateBoundsCheck(BB_3, v1, v0);
    IrBuilder->CreateStore(BB_3, v1, negThree);
    IrBuilder->CreateRet(BB_3);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 3);
    ASSERT_EQ(BB_2->Size(), 3);
    ASSERT_EQ(BB_3->Size(), 3);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Add);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Beq);
    inst = BB_2->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);
    inst = BB_3->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}


TEST(check_elimination_pass, bounds_check_different_indices) {
    /*
        =============================== BEFORE CHECK ELIMINATION ==============================

        function void #Foo() {
        BB_1:
            v0 = Alloc f64, 4
            v1 = Add ptr v0, 1
            v2 = Add ptr v1, 1
            v3 = Add ptr v2, 1
            BoundsCheck ptr v0, [ptr v0, 4]
            v4 = Load f64, ptr v0
            BoundsCheck ptr v1, [ptr v0, 4]
            v5 = Load f64, ptr v1
            BoundsCheck ptr v2, [ptr v0, 4]
            v6 = Load f64, ptr v2
            BoundsCheck ptr v3, [ptr v0, 4]
            v7 = Load f64, ptr v3
            Jump #BB_2

        BB_2: (preds: BB_1)
            BoundsCheck ptr v0, [ptr v0, 4]
            Store f64 3.140000, ptr v0
            BoundsCheck ptr v1, [ptr v0, 4]
            Store f64 3.140000, ptr v1
            BoundsCheck ptr v2, [ptr v0, 4]
            Store f64 3.140000, ptr v2
            BoundsCheck ptr v3, [ptr v0, 4]
            Store f64 3.140000, ptr v3
            Ret
        }

        =======================================================================================
        =============================== AFTER CHECK ELIMINATION ===============================

        function void #Foo() {
        BB_1:
            v0 = Alloc f64, 4
            v1 = Add ptr v0, 1
            v2 = Add ptr v1, 1
            v3 = Add ptr v2, 1
            BoundsCheck ptr v0, [ptr v0, 4]
            v4 = Load f64, ptr v0
            BoundsCheck ptr v1, [ptr v0, 4]
            v5 = Load f64, ptr v1
            BoundsCheck ptr v2, [ptr v0, 4]
            v6 = Load f64, ptr v2
            BoundsCheck ptr v3, [ptr v0, 4]
            v7 = Load f64, ptr v3
            Jump #BB_2

        BB_2: (preds: BB_1)
            Store f64 3.140000, ptr v0
            Store f64 3.140000, ptr v1
            Store f64 3.140000, ptr v2
            Store f64 3.140000, ptr v3
            Ret
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    // v0 is an array and v1, v2, v3 are pointers
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Pointer);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v6 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v7 = IrBuilder->CreateValue(VMIR::ValueType::Float64);

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* pi = IrBuilder->CreateValue(double(3.14));

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Void, "Foo");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Foo, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Foo, "BB_2");
    Foo->SetEntryBasicBlock(BB_1);

    IrBuilder->CreateAlloc(BB_1, v0, VMIR::ValueType::Float64, 4);
    IrBuilder->CreateAdd(BB_1, v0, one, v1);
    IrBuilder->CreateAdd(BB_1, v1, one, v2);
    IrBuilder->CreateAdd(BB_1, v2, one, v3);
    IrBuilder->CreateBoundsCheck(BB_1, v0, v0);
    IrBuilder->CreateLoad(BB_1, v0, v4);
    IrBuilder->CreateBoundsCheck(BB_1, v1, v0);
    IrBuilder->CreateLoad(BB_1, v1, v5);
    IrBuilder->CreateBoundsCheck(BB_1, v2, v0);
    IrBuilder->CreateLoad(BB_1, v2, v6);
    IrBuilder->CreateBoundsCheck(BB_1, v3, v0);
    IrBuilder->CreateLoad(BB_1, v3, v7);
    IrBuilder->CreateJump(BB_1, BB_2);

    IrBuilder->CreateBoundsCheck(BB_2, v0, v0);
    IrBuilder->CreateStore(BB_2, v0, pi);
    IrBuilder->CreateBoundsCheck(BB_2, v1, v0);
    IrBuilder->CreateStore(BB_2, v1, pi);
    IrBuilder->CreateBoundsCheck(BB_2, v2, v0);
    IrBuilder->CreateStore(BB_2, v2, pi);
    IrBuilder->CreateBoundsCheck(BB_2, v3, v0);
    IrBuilder->CreateStore(BB_2, v3, pi);
    IrBuilder->CreateRet(BB_2);

    VMIR::CheckEliminationPass checkEliminationPass{};
    checkEliminationPass.Run(Foo);

    ASSERT_EQ(BB_1->Size(), 13);
    ASSERT_EQ(BB_2->Size(), 5);

    VMIR::Instruction* inst = BB_1->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Alloc);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Add);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Add);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Add);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::BoundsCheck);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Load);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Jump);
    inst = BB_2->Front();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Store);
    inst = inst->GetNext();
    EXPECT_EQ(inst->GetType(), VMIR::InstructionType::Ret);

    IrBuilder->Cleanup();
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}