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
            v2 = Mv ui64 0
            v1 = Mv ui64 v2
            Ret ui64 v1
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    VMIR::Value* zero = IrBuilder->CreateValue(uint64_t(0));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateMv(Foo_BB_1, zero, v0);
    IrBuilder->CreateRet(Foo_BB_1, v0);


    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateCall(Bar_BB_1, Foo, v1);
    IrBuilder->CreateRet(Bar_BB_1, v1);

    VMIR::StaticInliningPass staticInliningPass{};

    std::cout << "=================================== BEFORE INLINING ===================================" << std::endl;
    IrBuilder->PrintIR(std::cout);
    staticInliningPass.Run(Bar);
    std::cout << "=================================== AFTER INLINING ====================================" << std::endl;
    IrBuilder->PrintIR(std::cout);

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

        Foo_BB_2: (preds: Foo_BB_1)
            v3 = Mv ui64 0
            Jump #PostCallTo_Foo

        Foo_BB_3: (preds: Foo_BB_1)
            v4 = Mv ui64 1
            Jump #PostCallTo_Foo

        PostCallTo_Foo: (preds: Foo_BB_2, Foo_BB_3)
            v2 = Phi ui64 v3, v4
            Ret ui64 v2
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    VMIR::BasicBlock* Foo_BB_2 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_2");
    VMIR::BasicBlock* Foo_BB_3 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_3");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    VMIR::Value* zero = IrBuilder->CreateValue(uint64_t(0));
    VMIR::Value* one  = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    IrBuilder->CreateBeq(Foo_BB_1, zero, zero, Foo_BB_2, Foo_BB_3);

    IrBuilder->CreateMv(Foo_BB_2, zero, v0);
    IrBuilder->CreateRet(Foo_BB_2, v0);

    IrBuilder->CreateMv(Foo_BB_3, one, v1);
    IrBuilder->CreateRet(Foo_BB_3, v1);


    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateCall(Bar_BB_1, Foo, v2);
    IrBuilder->CreateRet(Bar_BB_1, v2);

    VMIR::StaticInliningPass staticInliningPass{};

    std::cout << "=================================== BEFORE INLINING ===================================" << std::endl;
    IrBuilder->PrintIR(std::cout);
    staticInliningPass.Run(Bar);
    std::cout << "=================================== AFTER INLINING ====================================" << std::endl;
    IrBuilder->PrintIR(std::cout);

    IrBuilder->Cleanup();
}


TEST(static_inlining_pass, simple_args) {
    /*
        =================================== BEFORE INLINING ===================================

        function ui64 #Foo(ui64 v0) {
        Foo_BB_1:
            v1 = Add ui64 v0, 1
            Ret ui64 v1
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v2 = Call ui64 #Foo(ui64 2)
            Ret ui64 v2
        }

        =======================================================================================
        =================================== AFTER INLINING ====================================

        function ui64 #Foo(ui64 v0) {
        Foo_BB_1:
            v1 = Add ui64 v0, 1
            Ret ui64 v1
        }

        function ui64 #Bar() {
        Bar_BB_1:
            v4 = Add ui64 2, 1
            v2 = Mv ui64 v4
            Ret ui64 v2
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, {VMIR::ValueType::Uint64}, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateAdd(Foo_BB_1, Foo->GetArg(0), one, v0);
    IrBuilder->CreateRet(Foo_BB_1, v0);


    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    VMIR::Value* two = IrBuilder->CreateValue(uint64_t(2));
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateCall(Bar_BB_1, Foo, v1, {two});
    IrBuilder->CreateRet(Bar_BB_1, v1);

    VMIR::StaticInliningPass staticInliningPass{};

    std::cout << "=================================== BEFORE INLINING ===================================" << std::endl;
    IrBuilder->PrintIR(std::cout);
    staticInliningPass.Run(Bar);
    std::cout << "=================================== AFTER INLINING ====================================" << std::endl;
    IrBuilder->PrintIR(std::cout);

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
            v3 = Mv ui64 1
            v1 = Mv ui64 v3
            Ret ui64 v1
        }

        function ui64 #Baz() {
        Baz_BB_1:
            v4 = Mv ui64 1
            v5 = Mv ui64 v4
            v2 = Mv ui64 v5
            Ret ui64 v2
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateMv(Foo_BB_1, one, v0);
    IrBuilder->CreateRet(Foo_BB_1, v0);


    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");
    Bar->SetEntryBasicBlock(Bar_BB_1);

    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateCall(Bar_BB_1, Foo, v1);
    IrBuilder->CreateRet(Bar_BB_1, v1);


    VMIR::Function* Baz = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Baz");
    VMIR::BasicBlock* Baz_BB_1 = IrBuilder->CreateBasicBlock(Baz, "Baz_BB_1");
    Baz->SetEntryBasicBlock(Baz_BB_1);

    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateCall(Baz_BB_1, Bar, v2);
    IrBuilder->CreateRet(Baz_BB_1, v2);


    VMIR::StaticInliningPass staticInliningPass{};

    std::cout << "=================================== BEFORE INLINING ===================================" << std::endl;
    IrBuilder->PrintIR(std::cout);
    staticInliningPass.Run(Baz);
    std::cout << "=================================== AFTER INLINING ====================================" << std::endl;
    IrBuilder->PrintIR(std::cout);

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

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    Foo->SetEntryBasicBlock(Foo_BB_1);

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateMv(Foo_BB_1, one, v0);
    IrBuilder->CreateCall(Foo_BB_1, Foo, v1);
    IrBuilder->CreateMv(Foo_BB_1, v1, v2);
    IrBuilder->CreateRet(Foo_BB_1, v2);


    VMIR::StaticInliningPass staticInliningPass{};

    std::cout << "=================================== BEFORE INLINING ===================================" << std::endl;
    IrBuilder->PrintIR(std::cout);
    staticInliningPass.Run(Foo);
    std::cout << "=================================== AFTER INLINING ====================================" << std::endl;
    IrBuilder->PrintIR(std::cout);

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
            v6 = Mv ui64 1
            v7 = Call ui64 #Bar()
            v8 = Mv ui64 v7
            v4 = Mv ui64 v8
            v5 = Mv ui64 v4
            Ret ui64 v5
        }

        =======================================================================================
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Foo = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Foo");
    VMIR::Function* Bar = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Bar");

    VMIR::BasicBlock* Foo_BB_1 = IrBuilder->CreateBasicBlock(Foo, "Foo_BB_1");
    VMIR::BasicBlock* Bar_BB_1 = IrBuilder->CreateBasicBlock(Bar, "Bar_BB_1");

    Foo->SetEntryBasicBlock(Foo_BB_1);
    Bar->SetEntryBasicBlock(Bar_BB_1);

    VMIR::Value* one = IrBuilder->CreateValue(uint64_t(1));
    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateMv(Foo_BB_1, one, v0);
    IrBuilder->CreateCall(Foo_BB_1, Bar, v1);
    IrBuilder->CreateMv(Foo_BB_1, v1, v2);
    IrBuilder->CreateRet(Foo_BB_1, v2);


    VMIR::Value* two = IrBuilder->CreateValue(uint64_t(2));
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    IrBuilder->CreateMv(Bar_BB_1, two, v3);
    IrBuilder->CreateCall(Bar_BB_1, Foo, v4);
    IrBuilder->CreateMv(Bar_BB_1, v4, v5);
    IrBuilder->CreateRet(Bar_BB_1, v5);


    VMIR::StaticInliningPass staticInliningPass{};

    std::cout << "=================================== BEFORE INLINING ===================================" << std::endl;
    IrBuilder->PrintIR(std::cout);
    staticInliningPass.Run(Bar);
    std::cout << "=================================== AFTER INLINING ====================================" << std::endl;
    IrBuilder->PrintIR(std::cout);

    IrBuilder->Cleanup();
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}