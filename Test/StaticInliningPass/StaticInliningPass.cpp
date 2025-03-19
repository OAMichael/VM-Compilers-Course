#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IRBuilder.h>
#include <StaticInliningPass.h>


TEST(static_inlining_pass, simple) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Foo() {
        Foo_BB_1:
            v0 = Mv ui64 0
            Ret ui64 v0
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v1 = Call ui64 #Foo()
            Ret ui64 v1
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Foo() {
        Foo_BB_1:
            v0 = Mv ui64 0
            Ret ui64 v0
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v4 = Mv ui64 0
            v1 = Mv ui64 v4
            Ret ui64 v1
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* zero = IrBuilder->CreateValue(uint64_t(0));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* BarAfterInline_v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v4 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);


    // Callee
    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    IrBuilder->CreateMv(Foo_BB_1, zero, v0);
    IrBuilder->CreateRet(Foo_BB_1, v0);


    // Caller
    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    IrBuilder->CreateCall(Bar_BB_1, Foo, v1);
    IrBuilder->CreateRet(Bar_BB_1, v1);


    // Expected Caller view after inlining pass
    VMIR::Function* BarAfterInline = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "BarAfterInline");
    VMIR::BasicBlock* BarAfterInline_BB_1 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_1");
    BarAfterInline->SetEntryBasicBlock(BarAfterInline_BB_1);

    IrBuilder->CreateMv(BarAfterInline_BB_1, zero, BarAfterInline_v4);
    IrBuilder->CreateMv(BarAfterInline_BB_1, BarAfterInline_v4, BarAfterInline_v1);
    IrBuilder->CreateRet(BarAfterInline_BB_1, BarAfterInline_v1);


    // Check that built functions are valid
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());
    ASSERT_TRUE(BarAfterInline->IsValid());

    VMIR::StaticInliningPass staticInliningPass{};
    staticInliningPass.Run(Bar);

    // Check that functions are valid after optimization
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());

    // Now check that optimized function meets expected results
    ASSERT_TRUE(IrBuilder->CompareFunctions(Bar, BarAfterInline));

    IrBuilder->Cleanup();
}



TEST(static_inlining_pass, multiple_ret) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Foo() {
        Foo_BB_1:
            Beq ui64 0, 0 ? #Foo_BB_2 : #Foo_BB_3

        Foo_BB_2: (preds: Foo_BB_1)
            v0 = Mv ui64 0
            Ret ui64 v0

        Foo_BB_3: (preds: Foo_BB_1)
            v1 = Mv ui64 1
            Ret ui64 v1
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v2 = Call ui64 #Foo()
            Ret ui64 v2
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Foo() {
        Foo_BB_1:
            Beq ui64 0, 0 ? #Foo_BB_2 : #Foo_BB_3

        Foo_BB_2: (preds: Foo_BB_1)
            v0 = Mv ui64 0
            Ret ui64 v0

        Foo_BB_3: (preds: Foo_BB_1)
            v1 = Mv ui64 1
            Ret ui64 v1
        }

        function ui64 #Bar() {
        Bar_BB_1:
            Beq ui64 0, 0 ? #Foo_BB_2 : #Foo_BB_3

        Foo_BB_2: (preds: Bar_BB_1)
            v6 = Mv ui64 0
            Jump #PostCallTo_Foo

        Foo_BB_3: (preds: Bar_BB_1)
            v7 = Mv ui64 1
            Jump #PostCallTo_Foo

        PostCallTo_Foo: (preds: Foo_BB_2, Foo_BB_3)
            v2 = Phi ui64 v6, v7
            Ret ui64 v2
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* zero = IrBuilder->CreateValue(uint64_t(0));
    VMIR::Value* one  = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* BarAfterInline_v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v6 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v7 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);


    // Callee
    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    VMIR::BasicBlock* Foo_BB_2 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_2");
    VMIR::BasicBlock* Foo_BB_3 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_3");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    IrBuilder->CreateBeq(Foo_BB_1, zero, zero, Foo_BB_2, Foo_BB_3);

    IrBuilder->CreateMv(Foo_BB_2, zero, v0);
    IrBuilder->CreateRet(Foo_BB_2, v0);

    IrBuilder->CreateMv(Foo_BB_3, one, v1);
    IrBuilder->CreateRet(Foo_BB_3, v1);


    // Caller
    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    IrBuilder->CreateCall(Bar_BB_1, Foo, v2);
    IrBuilder->CreateRet(Bar_BB_1, v2);


    // Expected Caller view after inlining pass
    VMIR::Function* BarAfterInline = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "BarAfterInline");
    VMIR::BasicBlock* BarAfterInline_BB_1 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_1");
    VMIR::BasicBlock* BarAfterInline_BB_2 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_2");
    VMIR::BasicBlock* BarAfterInline_BB_3 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_3");
    VMIR::BasicBlock* BarAfterInline_BB_4 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_4");
    BarAfterInline->SetEntryBasicBlock(BarAfterInline_BB_1);

    IrBuilder->CreateBeq(BarAfterInline_BB_1, zero, zero, BarAfterInline_BB_2, BarAfterInline_BB_3);

    IrBuilder->CreateMv(BarAfterInline_BB_2, zero, BarAfterInline_v6);
    IrBuilder->CreateJump(BarAfterInline_BB_2, BarAfterInline_BB_4);

    IrBuilder->CreateMv(BarAfterInline_BB_3, one, BarAfterInline_v7);
    IrBuilder->CreateJump(BarAfterInline_BB_3, BarAfterInline_BB_4);

    IrBuilder->CreatePhi(BarAfterInline_BB_4, {BarAfterInline_v6, BarAfterInline_v7}, BarAfterInline_v2);
    IrBuilder->CreateRet(BarAfterInline_BB_4, BarAfterInline_v2);


    // Check that built functions are valid
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());
    ASSERT_TRUE(BarAfterInline->IsValid());

    VMIR::StaticInliningPass staticInliningPass{};
    staticInliningPass.Run(Bar);

    // Check that functions are valid after optimization
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());

    // Now check that optimized function meets expected results
    ASSERT_TRUE(IrBuilder->CompareFunctions(Bar, BarAfterInline));

    IrBuilder->Cleanup();
}


TEST(static_inlining_pass, simple_args) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Foo(ui64 v4) {
        Foo_BB_1:
            v0 = Add ui64 v4, 1
            Ret ui64 v0
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v1 = Call ui64 #Foo(ui64 2)
            Ret ui64 v1
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Foo(ui64 v4) {
        Foo_BB_1:
            v0 = Add ui64 v4, 1
            Ret ui64 v0
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v6 = Add ui64 2, 1
            v1 = Mv ui64 v6
            Ret ui64 v1
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* two = IrBuilder->CreateValue(uint64_t(2));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* BarAfterInline_v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v6 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);


    // Callee
    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, {VMIR::ValueType::Uint64}, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    IrBuilder->CreateAdd(Foo_BB_1, Foo->GetArg(0), one, v0);
    IrBuilder->CreateRet(Foo_BB_1, v0);


    // Caller
    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    IrBuilder->CreateCall(Bar_BB_1, Foo, v1, {two});
    IrBuilder->CreateRet(Bar_BB_1, v1);


    // Expected Caller view after inlining pass
    VMIR::Function* BarAfterInline = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "BarAfterInline");
    VMIR::BasicBlock* BarAfterInline_BB_1 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_1");
    BarAfterInline->SetEntryBasicBlock(BarAfterInline_BB_1);

    IrBuilder->CreateAdd(BarAfterInline_BB_1, two, one, BarAfterInline_v6);
    IrBuilder->CreateMv(BarAfterInline_BB_1, BarAfterInline_v6, BarAfterInline_v1);
    IrBuilder->CreateRet(BarAfterInline_BB_1, BarAfterInline_v1);


    // Check that built functions are valid
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());
    ASSERT_TRUE(BarAfterInline->IsValid());

    VMIR::StaticInliningPass staticInliningPass{};
    staticInliningPass.Run(Bar);

    // Check that functions are valid after optimization
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());

    // Now check that optimized function meets expected results
    ASSERT_TRUE(IrBuilder->CompareFunctions(Bar, BarAfterInline));

    IrBuilder->Cleanup();
}


TEST(static_inlining_pass, callee_entry_preds) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Foo() {
        Foo_BB_1: (preds: Foo_BB_3)
            v0 = Mv ui64 1
            Beq ui64 1, 1 ? #Foo_BB_2 : #Foo_BB_3

        Foo_BB_2: (preds: Foo_BB_1)
            Ret ui64 v0

        Foo_BB_3: (preds: Foo_BB_1)
            v1 = Mv ui64 0
            Jump #Foo_BB_1
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v2 = Call ui64 #Foo()
            Ret ui64 v2
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Foo() {
        Foo_BB_1: (preds: Foo_BB_3)
            v0 = Mv ui64 1
            Beq ui64 1, 1 ? #Foo_BB_2 : #Foo_BB_3

        Foo_BB_2: (preds: Foo_BB_1)
            Ret ui64 v0

        Foo_BB_3: (preds: Foo_BB_1)
            v1 = Mv ui64 0
            Jump #Foo_BB_1
        }

        function ui64 #Bar() {
        Bar_BB_1:
            Jump #Foo_BB_1

        Foo_BB_1: (preds: Foo_BB_3, Bar_BB_1)
            v6 = Mv ui64 1
            Beq ui64 1, 1 ? #Foo_BB_2 : #Foo_BB_3

        Foo_BB_2: (preds: Foo_BB_1)
            v2 = Mv ui64 v6
            Ret ui64 v2

        Foo_BB_3: (preds: Foo_BB_1)
            v7 = Mv ui64 0
            Jump #Foo_BB_1
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* zero = IrBuilder->CreateValue(uint64_t(0));
    VMIR::Value* one  = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* BarAfterInline_v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v6 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v7 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);


    // Callee
    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    VMIR::BasicBlock* Foo_BB_2 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_2");
    VMIR::BasicBlock* Foo_BB_3 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_3");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    IrBuilder->CreateMv(Foo_BB_1, one, v0);
    IrBuilder->CreateBeq(Foo_BB_1, one, one, Foo_BB_2, Foo_BB_3);

    IrBuilder->CreateRet(Foo_BB_2, v0);

    IrBuilder->CreateMv(Foo_BB_3, zero, v1);
    IrBuilder->CreateJump(Foo_BB_3, Foo_BB_1);


    // Caller
    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    IrBuilder->CreateCall(Bar_BB_1, Foo, v2);
    IrBuilder->CreateRet(Bar_BB_1, v2);


    // Expected Caller view after inlining pass
    VMIR::Function* BarAfterInline = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "BarAfterInline");
    VMIR::BasicBlock* BarAfterInline_BB_1 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_1");
    VMIR::BasicBlock* BarAfterInline_BB_2 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_2");
    VMIR::BasicBlock* BarAfterInline_BB_3 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_3");
    VMIR::BasicBlock* BarAfterInline_BB_4 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_4");
    BarAfterInline->SetEntryBasicBlock(BarAfterInline_BB_1);

    IrBuilder->CreateJump(BarAfterInline_BB_1, BarAfterInline_BB_2);

    IrBuilder->CreateMv(BarAfterInline_BB_2, one, BarAfterInline_v6);
    IrBuilder->CreateBeq(BarAfterInline_BB_2, one, one, BarAfterInline_BB_3, BarAfterInline_BB_4);

    IrBuilder->CreateMv(BarAfterInline_BB_3, BarAfterInline_v6, BarAfterInline_v2);
    IrBuilder->CreateRet(BarAfterInline_BB_3, BarAfterInline_v2);

    IrBuilder->CreateMv(BarAfterInline_BB_4, zero, BarAfterInline_v7);
    IrBuilder->CreateJump(BarAfterInline_BB_4, BarAfterInline_BB_2);


    // Check that built functions are valid
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());
    ASSERT_TRUE(BarAfterInline->IsValid());

    VMIR::StaticInliningPass staticInliningPass{};
    staticInliningPass.Run(Bar);

    // Check that functions are valid after optimization
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());

    // Now check that optimized function meets expected results
    ASSERT_TRUE(IrBuilder->CompareFunctions(Bar, BarAfterInline));

    IrBuilder->Cleanup();
}


TEST(static_inlining_pass, recursive) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Foo() {
        Foo_BB_1:
            v0 = Mv ui64 1
            Ret ui64 v0
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v1 = Call ui64 #Foo()
            Ret ui64 v1
        }

        function ui64 #Baz() {
        Baz_BB_1:
            v2 = Call ui64 #Bar()
            Ret ui64 v2
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Foo() {
        Foo_BB_1:
            v0 = Mv ui64 1
            Ret ui64 v0
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v6 = Mv ui64 1
            v1 = Mv ui64 v6
            Ret ui64 v1
        }

        function ui64 #Baz() {
        Baz_BB_1:
            v7 = Mv ui64 1
            v8 = Mv ui64 v7
            v2 = Mv ui64 v8
            Ret ui64 v2
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* BazAfterInline_v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BazAfterInline_v7 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BazAfterInline_v8 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);


    // Inner Callee
    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    IrBuilder->CreateMv(Foo_BB_1, one, v0);
    IrBuilder->CreateRet(Foo_BB_1, v0);


    // Outer Callee
    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    IrBuilder->CreateCall(Bar_BB_1, Foo, v1);
    IrBuilder->CreateRet(Bar_BB_1, v1);


    // Caller
    VMIR::Function* Baz = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Baz");
    VMIR::BasicBlock* Baz_BB_1 = IrBuilder->CreateBasicBlock(Baz, "Baz_BB_1");
    Baz->SetEntryBasicBlock(Baz_BB_1);

    IrBuilder->CreateCall(Baz_BB_1, Bar, v2);
    IrBuilder->CreateRet(Baz_BB_1, v2);


    // Expected Caller view after inlining pass
    VMIR::Function* BazAfterInline = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "BazAfterInline");
    VMIR::BasicBlock* BazAfterInline_BB_1 = IrBuilder->CreateBasicBlock(BazAfterInline, "BazAfterInline_BB_1");
    BazAfterInline->SetEntryBasicBlock(BazAfterInline_BB_1);

    IrBuilder->CreateMv(BazAfterInline_BB_1, one, BazAfterInline_v7);
    IrBuilder->CreateMv(BazAfterInline_BB_1, BazAfterInline_v7, BazAfterInline_v8);
    IrBuilder->CreateMv(BazAfterInline_BB_1, BazAfterInline_v8, BazAfterInline_v2);
    IrBuilder->CreateRet(BazAfterInline_BB_1, BazAfterInline_v2);


    // Check that built functions are valid
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());
    ASSERT_TRUE(Baz->IsValid());
    ASSERT_TRUE(BazAfterInline->IsValid());

    VMIR::StaticInliningPass staticInliningPass{};
    staticInliningPass.Run(Baz);

    // Check that functions are valid after optimization
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());
    ASSERT_TRUE(Baz->IsValid());

    // Now check that optimized function meets expected results
    ASSERT_TRUE(IrBuilder->CompareFunctions(Baz, BazAfterInline));

    IrBuilder->Cleanup();
}


TEST(static_inlining_pass, self_recursive) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Foo() {
        Foo_BB_1:
            v0 = Mv ui64 1
            v1 = Call ui64 #Foo()
            v2 = Mv ui64 v1
            Ret ui64 v2
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Foo() {
        Foo_BB_1:
            v0 = Mv ui64 1
            v1 = Call ui64 #Foo()
            v2 = Mv ui64 v1
            Ret ui64 v2
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* FooAfterInline_v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* FooAfterInline_v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* FooAfterInline_v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);


    // Callee and Caller simultaneously
    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    IrBuilder->CreateMv(Foo_BB_1, one, v0);
    IrBuilder->CreateCall(Foo_BB_1, Foo, v1);
    IrBuilder->CreateMv(Foo_BB_1, v1, v2);
    IrBuilder->CreateRet(Foo_BB_1, v2);


    // Expected Caller view after inlining pass
    VMIR::Function* FooAfterInline = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "FooAfterInline");
    VMIR::BasicBlock* FooAfterInline_BB_1 = IrBuilder->CreateBasicBlock(FooAfterInline, "FooAfterInline_BB_1");
    FooAfterInline->SetEntryBasicBlock(FooAfterInline_BB_1);

    IrBuilder->CreateMv(FooAfterInline_BB_1, one, FooAfterInline_v0);
    IrBuilder->CreateCall(FooAfterInline_BB_1, Foo, FooAfterInline_v1);
    IrBuilder->CreateMv(FooAfterInline_BB_1, FooAfterInline_v1, FooAfterInline_v2);
    IrBuilder->CreateRet(FooAfterInline_BB_1, FooAfterInline_v2);


    // Check that built functions are valid
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(FooAfterInline->IsValid());

    VMIR::StaticInliningPass staticInliningPass{};
    staticInliningPass.Run(Foo);

    // Check that functions are valid after optimization
    ASSERT_TRUE(Foo->IsValid());

    // Now check that optimized function meets expected results
    ASSERT_TRUE(IrBuilder->CompareFunctions(Foo, FooAfterInline));

    IrBuilder->Cleanup();
}


TEST(static_inlining_pass, recursion_loop) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Foo() {
        Foo_BB_1:
            v0 = Mv ui64 1
            v1 = Call ui64 #Bar()
            v2 = Mv ui64 v1
            Ret ui64 v2
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v3 = Mv ui64 2
            v4 = Call ui64 #Foo()
            v5 = Mv ui64 v4
            Ret ui64 v5
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Foo() {
        Foo_BB_1:
            v0 = Mv ui64 1
            v1 = Call ui64 #Bar()
            v2 = Mv ui64 v1
            Ret ui64 v2
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v3 = Mv ui64 2
            v12 = Mv ui64 1
            v13 = Call ui64 #Bar()
            v14 = Mv ui64 v13
            v4 = Mv ui64 v14
            v5 = Mv ui64 v4
            Ret ui64 v5
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* two = IrBuilder->CreateValue(uint64_t(2));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* BarAfterInline_v3  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v4  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v5  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v12 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v13 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* BarAfterInline_v14 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);


    // Callee, Caller and expected Caller view after inlining pass accordingly
    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::Function* BarAfterInline = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "BarAfterInline");

    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    VMIR::BasicBlock* BarAfterInline_BB_1 = IrBuilder->CreateBasicBlock(BarAfterInline, "BarAfterInline_BB_1");

    Foo->SetEntryBasicBlock(Foo_BB_1);
    Bar->SetEntryBasicBlock(Bar_BB_1);
    BarAfterInline->SetEntryBasicBlock(BarAfterInline_BB_1);

    IrBuilder->CreateMv(Foo_BB_1, one, v0);
    IrBuilder->CreateCall(Foo_BB_1, Bar, v1);
    IrBuilder->CreateMv(Foo_BB_1, v1, v2);
    IrBuilder->CreateRet(Foo_BB_1, v2);

    IrBuilder->CreateMv(Bar_BB_1, two, v3);
    IrBuilder->CreateCall(Bar_BB_1, Foo, v4);
    IrBuilder->CreateMv(Bar_BB_1, v4, v5);
    IrBuilder->CreateRet(Bar_BB_1, v5);

    IrBuilder->CreateMv(BarAfterInline_BB_1, two, BarAfterInline_v3);
    IrBuilder->CreateMv(BarAfterInline_BB_1, one, BarAfterInline_v12);
    IrBuilder->CreateCall(BarAfterInline_BB_1, Bar, BarAfterInline_v13);
    IrBuilder->CreateMv(BarAfterInline_BB_1, BarAfterInline_v13, BarAfterInline_v14);
    IrBuilder->CreateMv(BarAfterInline_BB_1, BarAfterInline_v14, BarAfterInline_v4);
    IrBuilder->CreateMv(BarAfterInline_BB_1, BarAfterInline_v4, BarAfterInline_v5);
    IrBuilder->CreateRet(BarAfterInline_BB_1, BarAfterInline_v5);


    // Check that built functions are valid
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());
    ASSERT_TRUE(BarAfterInline->IsValid());

    VMIR::StaticInliningPass staticInliningPass{};
    staticInliningPass.Run(Bar);

    // Check that functions are valid after optimization
    ASSERT_TRUE(Foo->IsValid());
    ASSERT_TRUE(Bar->IsValid());

    // Now check that optimized function meets expected results
    ASSERT_TRUE(IrBuilder->CompareFunctions(Bar, BarAfterInline));

    IrBuilder->Cleanup();
}


TEST(static_inlining_pass, lecture_example) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Callee(ui64 v30, ui64 v31) {
        BB_2:
            v7 = Mv ui64 v30
            v8 = Mv ui64 v31
            v13 = Mv ui64 1
            v14 = Mv ui64 10
            Jump #BB_3

        BB_3: (preds: BB_2)
            v9 = Add ui64 v7, v13
            v10 = Mul ui64 v8, v14
            Beq ui64 v9, v10 ? #BB_4 : #BB_5

        BB_4: (preds: BB_3)
            v11 = Mv ui64 10
            Ret ui64 v11

        BB_5: (preds: BB_3)
            v12 = Mv ui64 5
            Ret ui64 v12
        }

        function ui64 #Caller() {
        BB_0:
            v0 = Mv ui64 1
            v1 = Mv ui64 5
            Jump #BB_1

        BB_1: (preds: BB_0)
            v2 = Mv ui64 10
            v3 = Mv ui64 5
            v4 = Call ui64 #Callee(ui64 v2, ui64 v3)
            v5 = Sub ui64 v4, v0
            v6 = Div ui64 v5, v1
            Ret ui64 v6
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Callee(ui64 v30, ui64 v31) {
        BB_2:
            v7 = Mv ui64 v30
            v8 = Mv ui64 v31
            v13 = Mv ui64 1
            v14 = Mv ui64 10
            Jump #BB_3

        BB_3: (preds: BB_2)
            v9 = Add ui64 v7, v13
            v10 = Mul ui64 v8, v14
            Beq ui64 v9, v10 ? #BB_4 : #BB_5

        BB_4: (preds: BB_3)
            v11 = Mv ui64 10
            Ret ui64 v11

        BB_5: (preds: BB_3)
            v12 = Mv ui64 5
            Ret ui64 v12
        }

        function ui64 #Caller() {
        BB_0:
            v0 = Mv ui64 1
            v1 = Mv ui64 5
            Jump #BB_1

        BB_1: (preds: BB_0)
            v2 = Mv ui64 10
            v3 = Mv ui64 5
            v34 = Mv ui64 v2
            v35 = Mv ui64 v3
            v36 = Mv ui64 1
            v37 = Mv ui64 10
            Jump #BB_3

        BB_3: (preds: BB_1)
            v38 = Add ui64 v34, v36
            v39 = Mul ui64 v35, v37
            Beq ui64 v38, v39 ? #BB_4 : #BB_5

        BB_4: (preds: BB_3)
            v40 = Mv ui64 10
            Jump #PostCallTo_Callee

        BB_5: (preds: BB_3)
            v41 = Mv ui64 5
            Jump #PostCallTo_Callee

        PostCallTo_Callee: (preds: BB_4, BB_5)
            v4 = Phi ui64 v40, v41
            v5 = Sub ui64 v4, v0
            v6 = Div ui64 v5, v1
            Ret ui64 v6
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Value* one  = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* five = IrBuilder->CreateValue(uint64_t(5));
    VMIR::Value* ten  = IrBuilder->CreateValue(uint64_t(10));

    // Numbering according to lecture
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v6 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v7 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v11 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v12 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v13 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v14 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v15 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v17 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v19 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v20 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    VMIR::Value* CallerAfterInline_v0  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v1  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v2  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v3  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v4  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v5  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v6  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v34 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v35 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v36 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v37 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v38 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v39 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v40 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* CallerAfterInline_v41 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);


    // Callee
    VMIR::Function* Callee = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, {VMIR::ValueType::Uint64, VMIR::ValueType::Uint64}, "Callee");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Callee, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Callee, "BB_3");
    VMIR::BasicBlock* BB_4 = IrBuilder->CreateBasicBlock(Callee, "BB_4");
    VMIR::BasicBlock* BB_5 = IrBuilder->CreateBasicBlock(Callee, "BB_5");
    Callee->SetEntryBasicBlock(BB_2);

    IrBuilder->CreateMv(BB_2, Callee->GetArg(0), v11);
    IrBuilder->CreateMv(BB_2, Callee->GetArg(1), v12);
    IrBuilder->CreateMv(BB_2, one, v19);
    IrBuilder->CreateMv(BB_2, ten, v20);
    IrBuilder->CreateJump(BB_2, BB_3);

    IrBuilder->CreateAdd(BB_3, v11, v19, v13);
    IrBuilder->CreateMul(BB_3, v12, v20, v14);
    IrBuilder->CreateBeq(BB_3, v13, v14, BB_4, BB_5);

    IrBuilder->CreateMv(BB_4, ten, v15);
    IrBuilder->CreateRet(BB_4, v15);

    IrBuilder->CreateMv(BB_5, five, v17);
    IrBuilder->CreateRet(BB_5, v17);


    // Caller
    VMIR::Function* Caller = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Caller");
    VMIR::BasicBlock* BB_0 = IrBuilder->CreateBasicBlock(Caller, "BB_0");
    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Caller, "BB_1");
    Caller->SetEntryBasicBlock(BB_0);

    IrBuilder->CreateMv(BB_0, one, v1);
    IrBuilder->CreateMv(BB_0, five, v2);
    IrBuilder->CreateJump(BB_0, BB_1);

    IrBuilder->CreateMv(BB_1, ten, v3);
    IrBuilder->CreateMv(BB_1, five, v4);
    IrBuilder->CreateCall(BB_1, Callee, v5, {v3, v4});
    IrBuilder->CreateSub(BB_1, v5, v1, v6);
    IrBuilder->CreateDiv(BB_1, v6, v2, v7);
    IrBuilder->CreateRet(BB_1, v7);


    // Expected Caller view after inlining pass
    VMIR::Function* CallerAfterInline = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "CallerAfterInline");
    VMIR::BasicBlock* CallerAfterInline_BB_1 = IrBuilder->CreateBasicBlock(CallerAfterInline, "CallerAfterInline_BB_1");
    VMIR::BasicBlock* CallerAfterInline_BB_2 = IrBuilder->CreateBasicBlock(CallerAfterInline, "CallerAfterInline_BB_2");
    VMIR::BasicBlock* CallerAfterInline_BB_3 = IrBuilder->CreateBasicBlock(CallerAfterInline, "CallerAfterInline_BB_3");
    VMIR::BasicBlock* CallerAfterInline_BB_4 = IrBuilder->CreateBasicBlock(CallerAfterInline, "CallerAfterInline_BB_4");
    VMIR::BasicBlock* CallerAfterInline_BB_5 = IrBuilder->CreateBasicBlock(CallerAfterInline, "CallerAfterInline_BB_5");
    VMIR::BasicBlock* CallerAfterInline_BB_6 = IrBuilder->CreateBasicBlock(CallerAfterInline, "CallerAfterInline_BB_6");
    CallerAfterInline->SetEntryBasicBlock(CallerAfterInline_BB_1);

    IrBuilder->CreateMv(CallerAfterInline_BB_1, one, CallerAfterInline_v0);
    IrBuilder->CreateMv(CallerAfterInline_BB_1, five, CallerAfterInline_v1);
    IrBuilder->CreateJump(CallerAfterInline_BB_1, CallerAfterInline_BB_2);

    IrBuilder->CreateMv(CallerAfterInline_BB_2, ten, CallerAfterInline_v2);
    IrBuilder->CreateMv(CallerAfterInline_BB_2, five, CallerAfterInline_v3);
    IrBuilder->CreateMv(CallerAfterInline_BB_2, CallerAfterInline_v2, CallerAfterInline_v34);
    IrBuilder->CreateMv(CallerAfterInline_BB_2, CallerAfterInline_v3, CallerAfterInline_v35);
    IrBuilder->CreateMv(CallerAfterInline_BB_2, one, CallerAfterInline_v36);
    IrBuilder->CreateMv(CallerAfterInline_BB_2, ten, CallerAfterInline_v37);
    IrBuilder->CreateJump(CallerAfterInline_BB_2, CallerAfterInline_BB_3);

    IrBuilder->CreateAdd(CallerAfterInline_BB_3, CallerAfterInline_v34, CallerAfterInline_v36, CallerAfterInline_v38);
    IrBuilder->CreateMul(CallerAfterInline_BB_3, CallerAfterInline_v35, CallerAfterInline_v37, CallerAfterInline_v39);
    IrBuilder->CreateBeq(CallerAfterInline_BB_3, CallerAfterInline_v38, CallerAfterInline_v39, CallerAfterInline_BB_4, CallerAfterInline_BB_5);

    IrBuilder->CreateMv(CallerAfterInline_BB_4, ten, CallerAfterInline_v40);
    IrBuilder->CreateJump(CallerAfterInline_BB_4, CallerAfterInline_BB_6);

    IrBuilder->CreateMv(CallerAfterInline_BB_5, five, CallerAfterInline_v41);
    IrBuilder->CreateJump(CallerAfterInline_BB_5, CallerAfterInline_BB_6);

    IrBuilder->CreatePhi(CallerAfterInline_BB_6, {CallerAfterInline_v40, CallerAfterInline_v41}, CallerAfterInline_v4);
    IrBuilder->CreateSub(CallerAfterInline_BB_6, CallerAfterInline_v4, CallerAfterInline_v0, CallerAfterInline_v5);
    IrBuilder->CreateDiv(CallerAfterInline_BB_6, CallerAfterInline_v5, CallerAfterInline_v1, CallerAfterInline_v6);
    IrBuilder->CreateRet(CallerAfterInline_BB_6, CallerAfterInline_v6);


    // Check that built functions are valid
    ASSERT_TRUE(Callee->IsValid());
    ASSERT_TRUE(Caller->IsValid());
    ASSERT_TRUE(CallerAfterInline->IsValid());

    VMIR::StaticInliningPass staticInliningPass{};
    staticInliningPass.SetInlineInstructionCountThreshold(15);
    staticInliningPass.Run(Caller);

    // Check that functions are valid after optimization
    ASSERT_TRUE(Callee->IsValid());
    ASSERT_TRUE(Caller->IsValid());

    // Now check that optimized function meets expected results
    ASSERT_TRUE(IrBuilder->CompareFunctions(Caller, CallerAfterInline));

    IrBuilder->Cleanup();
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}