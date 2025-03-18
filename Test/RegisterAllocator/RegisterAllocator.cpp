#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IRBuilder.h>
#include <LivenessAnalyzer.h>
#include <RegisterAllocator.h>


TEST(register_allocator, fact_recursive_gp) {
    /*
        Linear order:
            BB_1:
                                                                live: 0
                Beq i32 v0, 0 ? #BB_3 : #BB_2                   live: 2,      lin: 0
                                                                live: 4
            BB_2:
                                                                live: 4
                Beq i32 v0, 1 ? #BB_3 : #BB_4                   live: 6,      lin: 1
                                                                live: 8
            BB_4:
                                                                live: 8
                v1 = Sub i32 v0, 1                              live: 10,     lin: 2
                v2 = Call i32 #Func(i32 v1)                     live: 12,     lin: 3
                v3 = Mul i32 v0, v2                             live: 14,     lin: 4
                Ret i32 v3                                      live: 16,     lin: 5
                                                                live: 18
            BB_3:
                                                                live: 18
                Ret i32 1                                       live: 20,     lin: 6
                                                                live: 22
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Int32, {VMIR::ValueType::Int32}, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Func, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Func, "BB_3");
    VMIR::BasicBlock* BB_4 = IrBuilder->CreateBasicBlock(Func, "BB_4");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* zero = IrBuilder->CreateValue(0);
    VMIR::Value* one  = IrBuilder->CreateValue(1);

    VMIR::Value* v0 = Func->GetArg(0);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Int32);

    IrBuilder->CreateBeq(BB_1, v0, zero, BB_3, BB_2);

    IrBuilder->CreateBeq(BB_2, v0, one, BB_3, BB_4);

    IrBuilder->CreateRet(BB_3, one);

    IrBuilder->CreateSub(BB_4, v0, one, v1);
    IrBuilder->CreateCall(BB_4, Func, v2, {v1});
    IrBuilder->CreateMul(BB_4, v0, v2, v3);
    IrBuilder->CreateRet(BB_4, v3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    VMIR::RegisterAllocator* registerAllocator = IrBuilder->CreateRegisterAllocator(cfg, 2, 1);

    livenessAnalyzer->PerformLivenessAnalysis();
    registerAllocator->PerformRegisterAllocation();

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v1->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v1->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v2->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v2->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v3->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v3->GetLocation()), VMIR::GPRegisterLocation(0));

    IrBuilder->Cleanup();
}


TEST(register_allocator, fact_recursive_fp) {
    /*
        Linear order:
            BB_1:
                                                                live: 0
                Beq f32 v0, 0.000000 ? #BB_3 : #BB_2            live: 2,      lin: 0
                                                                live: 4
            BB_2:
                                                                live: 4
                Beq f32 v0, 1.000000 ? #BB_3 : #BB_4            live: 6,      lin: 1
                                                                live: 8
            BB_4:
                                                                live: 8
                v1 = Sub f32 v0, 1.000000                       live: 10,     lin: 2
                v2 = Call f32 #Func(f32 v1)                     live: 12,     lin: 3
                v3 = Mul f32 v0, v2                             live: 14,     lin: 4
                Ret f32 v3                                      live: 16,     lin: 5
                                                                live: 18
            BB_3:
                                                                live: 18
                Ret f32 1.000000                                live: 20,     lin: 6
                                                                live: 22
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Float32, {VMIR::ValueType::Float32}, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Func, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Func, "BB_3");
    VMIR::BasicBlock* BB_4 = IrBuilder->CreateBasicBlock(Func, "BB_4");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* zero = IrBuilder->CreateValue(0.0f);
    VMIR::Value* one  = IrBuilder->CreateValue(1.0f);

    VMIR::Value* v0 = Func->GetArg(0);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Float32);

    IrBuilder->CreateBeq(BB_1, v0, zero, BB_3, BB_2);

    IrBuilder->CreateBeq(BB_2, v0, one, BB_3, BB_4);

    IrBuilder->CreateRet(BB_3, one);

    IrBuilder->CreateSub(BB_4, v0, one, v1);
    IrBuilder->CreateCall(BB_4, Func, v2, {v1});
    IrBuilder->CreateMul(BB_4, v0, v2, v3);
    IrBuilder->CreateRet(BB_4, v3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    VMIR::RegisterAllocator* registerAllocator = IrBuilder->CreateRegisterAllocator(cfg, 1, 2);

    livenessAnalyzer->PerformLivenessAnalysis();
    registerAllocator->PerformRegisterAllocation();

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v1->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v1->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v2->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v2->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v3->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v3->GetLocation()), VMIR::FPRegisterLocation(0));

    IrBuilder->Cleanup();
}


TEST(register_allocator, fact_loop_gp) {
    /*
        Linear order:
            Entry:
                                                                live: 0
                v1 = Add ui64 0, 1                              live: 2,      lin: 0
                Jump #LoopPreheader                             live: 4,      lin: 1
                                                                live: 6
            LoopPreheader:
                                                                live: 6
                v2 = Add ui64 0, 2                              live: 8,      lin: 2
                Jump #LoopHeader                                live: 10,     lin: 3
                                                                live: 12
            LoopHeader:
                                                                live: 12
                v3 = Phi ui64 v2, v6                            live: 12,     lin: 4
                Bgt ui64 v3, v0 ? #LoopExit : #LoopBody         live: 14,     lin: 5
                                                                live: 16
            LoopBody:
                                                                live: 16
                v4 = Phi ui64 v1, v5                            live: 16,     lin: 6
                v5 = Mul ui64 v4, v3                            live: 18,     lin: 7
                v6 = Add ui64 v3, 1                             live: 20,     lin: 8
                Jump #LoopHeader                                live: 22,     lin: 9
                                                                live: 24
            LoopExit:
                                                                live: 24
                v7 = Phi ui64 v1, v5                            live: 24,     lin: 10
                Ret ui64 v7                                     live: 26,     lin: 11
                                                                live: 28
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, {VMIR::ValueType::Uint64}, "Func");

    VMIR::BasicBlock* EntryBB           = IrBuilder->CreateBasicBlock(Func, "Entry");
    VMIR::BasicBlock* LoopPreheaderBB   = IrBuilder->CreateBasicBlock(Func, "LoopPreheader");
    VMIR::BasicBlock* LoopHeaderBB      = IrBuilder->CreateBasicBlock(Func, "LoopHeader");
    VMIR::BasicBlock* LoopBodyBB        = IrBuilder->CreateBasicBlock(Func, "LoopBody");
    VMIR::BasicBlock* LoopExitBB        = IrBuilder->CreateBasicBlock(Func, "LoopExit");

    Func->SetEntryBasicBlock(EntryBB);

    VMIR::Value* zero = IrBuilder->CreateValue(0UL);
    VMIR::Value* one  = IrBuilder->CreateValue(1UL);
    VMIR::Value* two  = IrBuilder->CreateValue(2UL);

    VMIR::Value* v0 = Func->GetArg(0);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v6 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v7 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    IrBuilder->CreateAdd(EntryBB, zero, one, v1);
    IrBuilder->CreateJump(EntryBB, LoopPreheaderBB);

    IrBuilder->CreateAdd(LoopPreheaderBB, zero, two, v2);
    IrBuilder->CreateJump(LoopPreheaderBB, LoopHeaderBB);

    IrBuilder->CreatePhi(LoopHeaderBB, {v2, v6}, v3);
    IrBuilder->CreateBgt(LoopHeaderBB, v3, v0, LoopExitBB, LoopBodyBB);

    IrBuilder->CreatePhi(LoopBodyBB, {v1, v5}, v4);
    IrBuilder->CreateMul(LoopBodyBB, v4, v3, v5);
    IrBuilder->CreateAdd(LoopBodyBB, v3, one, v6);
    IrBuilder->CreateJump(LoopBodyBB, LoopHeaderBB);

    IrBuilder->CreatePhi(LoopExitBB, {v1, v5}, v7);
    IrBuilder->CreateRet(LoopExitBB, v7);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    VMIR::RegisterAllocator* registerAllocator = IrBuilder->CreateRegisterAllocator(cfg, 2, 2);

    livenessAnalyzer->PerformLivenessAnalysis();
    registerAllocator->PerformRegisterAllocation();

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v1->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v1->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v2->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v2->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v3->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v3->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v4->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v4->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v5->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v5->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v6->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v6->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v7->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v7->GetLocation()), VMIR::GPRegisterLocation(0));

    IrBuilder->Cleanup();
}


TEST(register_allocator, fact_loop_fp) {
    /*
        Linear order:
            Entry:
                                                                live: 0
                v1 = Add f64 0.000000, 1.000000                 live: 2,      lin: 0
                Jump #LoopPreheader                             live: 4,      lin: 1
                                                                live: 6
            LoopPreheader:
                                                                live: 6
                v2 = Add f64 0.000000, 2.000000                 live: 8,      lin: 2
                Jump #LoopHeader                                live: 10,     lin: 3
                                                                live: 12
            LoopHeader:
                                                                live: 12
                v3 = Phi f64 v6, v2                             live: 12,     lin: 4
                Bgt f64 v3, v0 ? #LoopExit : #LoopBody          live: 14,     lin: 5
                                                                live: 16
            LoopBody:
                                                                live: 16
                v4 = Phi f64 v5, v1                             live: 16,     lin: 6
                v5 = Mul f64 v4, v3                             live: 18,     lin: 7
                v6 = Add f64 v3, 1.000000                       live: 20,     lin: 8
                Jump #LoopHeader                                live: 22,     lin: 9
                                                                live: 24
            LoopExit:
                                                                live: 24
                v7 = Phi f64 v5, v1                             live: 24,     lin: 10
                Ret f64 v7                                      live: 26,     lin: 11
                                                                live: 28
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Float64, {VMIR::ValueType::Float64}, "Func");

    VMIR::BasicBlock* EntryBB           = IrBuilder->CreateBasicBlock(Func, "Entry");
    VMIR::BasicBlock* LoopPreheaderBB   = IrBuilder->CreateBasicBlock(Func, "LoopPreheader");
    VMIR::BasicBlock* LoopHeaderBB      = IrBuilder->CreateBasicBlock(Func, "LoopHeader");
    VMIR::BasicBlock* LoopBodyBB        = IrBuilder->CreateBasicBlock(Func, "LoopBody");
    VMIR::BasicBlock* LoopExitBB        = IrBuilder->CreateBasicBlock(Func, "LoopExit");

    Func->SetEntryBasicBlock(EntryBB);

    VMIR::Value* zero = IrBuilder->CreateValue(0.0);
    VMIR::Value* one  = IrBuilder->CreateValue(1.0);
    VMIR::Value* two  = IrBuilder->CreateValue(2.0);

    VMIR::Value* v0 = Func->GetArg(0);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v6 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v7 = IrBuilder->CreateValue(VMIR::ValueType::Float64);

    IrBuilder->CreateAdd(EntryBB, zero, one, v1);
    IrBuilder->CreateJump(EntryBB, LoopPreheaderBB);

    IrBuilder->CreateAdd(LoopPreheaderBB, zero, two, v2);
    IrBuilder->CreateJump(LoopPreheaderBB, LoopHeaderBB);

    IrBuilder->CreatePhi(LoopHeaderBB, {v2, v6}, v3);
    IrBuilder->CreateBgt(LoopHeaderBB, v3, v0, LoopExitBB, LoopBodyBB);

    IrBuilder->CreatePhi(LoopBodyBB, {v1, v5}, v4);
    IrBuilder->CreateMul(LoopBodyBB, v4, v3, v5);
    IrBuilder->CreateAdd(LoopBodyBB, v3, one, v6);
    IrBuilder->CreateJump(LoopBodyBB, LoopHeaderBB);

    IrBuilder->CreatePhi(LoopExitBB, {v1, v5}, v7);
    IrBuilder->CreateRet(LoopExitBB, v7);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    VMIR::RegisterAllocator* registerAllocator = IrBuilder->CreateRegisterAllocator(cfg, 2, 2);

    livenessAnalyzer->PerformLivenessAnalysis();
    registerAllocator->PerformRegisterAllocation();

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v1->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v1->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v2->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v2->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v3->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v3->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v4->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v4->GetLocation()), VMIR::FPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v5->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v5->GetLocation()), VMIR::FPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v6->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v6->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v7->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v7->GetLocation()), VMIR::FPRegisterLocation(0));

    IrBuilder->Cleanup();
}


TEST(register_allocator, from_lecture_1_gp) {
    /*
        Linear order:
            0:
                                                                live: 0
                v2 = Add ui64 v0, v1                            live: 2,      lin: 0
                v3 = Add ui64 v0, v1                            live: 4,      lin: 1
                Beq ui64 v0, v1 ? #2 : #1                       live: 6,      lin: 2
                                                                live: 8
            1:
                                                                live: 8
                v4 = Add ui64 v0, v1                            live: 10,     lin: 3
                v5 = Add ui64 v0, v1                            live: 12,     lin: 4
                Jump #3                                         live: 14,     lin: 5
                                                                live: 16
            2:
                                                                live: 16
                v6 = Add ui64 v0, v1                            live: 18,     lin: 6
                v7 = Add ui64 v0, v1                            live: 20,     lin: 7
                Jump #3                                         live: 22,     lin: 8
                                                                live: 24
            3:
                                                                live: 24
                v8 = Phi ui64 v4, v6                            live: 24,     lin: 9
                v9 = Phi ui64 v5, v7                            live: 24,     lin: 10
                v10 = Add ui64 v8, v2                           live: 26,     lin: 11
                v11 = Add ui64 v9, v3                           live: 28,     lin: 12
                v12 = Add ui64 v8, v10                          live: 30,     lin: 13
                Ret                                             live: 32,     lin: 14
                                                                live: 34
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction({VMIR::ValueType::Uint64, VMIR::ValueType::Uint64}, "Func");

    VMIR::BasicBlock* BB_0  = IrBuilder->CreateBasicBlock(Func, "0");
    VMIR::BasicBlock* BB_1  = IrBuilder->CreateBasicBlock(Func, "1");
    VMIR::BasicBlock* BB_2  = IrBuilder->CreateBasicBlock(Func, "2");
    VMIR::BasicBlock* BB_3  = IrBuilder->CreateBasicBlock(Func, "3");

    Func->SetEntryBasicBlock(BB_0);

    VMIR::Value* v0 = Func->GetArg(0);
    VMIR::Value* v1 = Func->GetArg(1);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v6 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v7 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v8 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v9 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v10 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v11 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v12 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    IrBuilder->CreateAdd(BB_0, v0, v1, v2);
    IrBuilder->CreateAdd(BB_0, v0, v1, v3);
    IrBuilder->CreateBeq(BB_0, v0, v1, BB_2, BB_1);

    IrBuilder->CreateAdd(BB_1, v0, v1, v4);
    IrBuilder->CreateAdd(BB_1, v0, v1, v5);
    IrBuilder->CreateJump(BB_1, BB_3);

    IrBuilder->CreateAdd(BB_2, v0, v1, v6);
    IrBuilder->CreateAdd(BB_2, v0, v1, v7);
    IrBuilder->CreateJump(BB_2, BB_3);

    IrBuilder->CreatePhi(BB_3, {v4, v6}, v8);
    IrBuilder->CreatePhi(BB_3, {v5, v7}, v9);
    IrBuilder->CreateAdd(BB_3, v8, v2, v10);
    IrBuilder->CreateAdd(BB_3, v9, v3, v11);
    IrBuilder->CreateAdd(BB_3, v8, v10, v12);
    IrBuilder->CreateRet(BB_3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    VMIR::RegisterAllocator* registerAllocator = IrBuilder->CreateRegisterAllocator(cfg, 4, 4);

    livenessAnalyzer->PerformLivenessAnalysis();
    registerAllocator->PerformRegisterAllocation();

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v2->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v2->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v3->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v3->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v4->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v4->GetLocation()), VMIR::GPRegisterLocation(2));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v5->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v5->GetLocation()), VMIR::GPRegisterLocation(3));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v6->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v6->GetLocation()), VMIR::GPRegisterLocation(2));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v7->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v7->GetLocation()), VMIR::GPRegisterLocation(3));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v8->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v8->GetLocation()), VMIR::GPRegisterLocation(2));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v9->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v9->GetLocation()), VMIR::GPRegisterLocation(3));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v10->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v10->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v11->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v11->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v12->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v12->GetLocation()), VMIR::GPRegisterLocation(0));

    IrBuilder->Cleanup();
}


TEST(register_allocator, from_lecture_1_fp) {
    /*
        Linear order:
            0:
                                                                live: 0
                v2 = Add f64 v0, v1                             live: 2,      lin: 0
                v3 = Add f64 v0, v1                             live: 4,      lin: 1
                Beq f64 v0, v1 ? #2 : #1                        live: 6,      lin: 2
                                                                live: 8
            1:
                                                                live: 8
                v4 = Add f64 v0, v1                             live: 10,     lin: 3
                v5 = Add f64 v0, v1                             live: 12,     lin: 4
                Jump #3                                         live: 14,     lin: 5
                                                                live: 16
            2:
                                                                live: 16
                v6 = Add f64 v0, v1                             live: 18,     lin: 6
                v7 = Add f64 v0, v1                             live: 20,     lin: 7
                Jump #3                                         live: 22,     lin: 8
                                                                live: 24
            3:
                                                                live: 24
                v8 = Phi f64 v6, v4                             live: 24,     lin: 9
                v9 = Phi f64 v5, v7                             live: 24,     lin: 10
                v10 = Add f64 v8, v2                            live: 26,     lin: 11
                v11 = Add f64 v9, v3                            live: 28,     lin: 12
                v12 = Add f64 v8, v10                           live: 30,     lin: 13
                Ret                                             live: 32,     lin: 14
                                                                live: 34
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction({VMIR::ValueType::Float64, VMIR::ValueType::Float64}, "Func");

    VMIR::BasicBlock* BB_0  = IrBuilder->CreateBasicBlock(Func, "0");
    VMIR::BasicBlock* BB_1  = IrBuilder->CreateBasicBlock(Func, "1");
    VMIR::BasicBlock* BB_2  = IrBuilder->CreateBasicBlock(Func, "2");
    VMIR::BasicBlock* BB_3  = IrBuilder->CreateBasicBlock(Func, "3");

    Func->SetEntryBasicBlock(BB_0);

    VMIR::Value* v0 = Func->GetArg(0);
    VMIR::Value* v1 = Func->GetArg(1);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v6 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v7 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v8 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v9 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v10 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v11 = IrBuilder->CreateValue(VMIR::ValueType::Float64);
    VMIR::Value* v12 = IrBuilder->CreateValue(VMIR::ValueType::Float64);

    IrBuilder->CreateAdd(BB_0, v0, v1, v2);
    IrBuilder->CreateAdd(BB_0, v0, v1, v3);
    IrBuilder->CreateBeq(BB_0, v0, v1, BB_2, BB_1);

    IrBuilder->CreateAdd(BB_1, v0, v1, v4);
    IrBuilder->CreateAdd(BB_1, v0, v1, v5);
    IrBuilder->CreateJump(BB_1, BB_3);

    IrBuilder->CreateAdd(BB_2, v0, v1, v6);
    IrBuilder->CreateAdd(BB_2, v0, v1, v7);
    IrBuilder->CreateJump(BB_2, BB_3);

    IrBuilder->CreatePhi(BB_3, {v4, v6}, v8);
    IrBuilder->CreatePhi(BB_3, {v5, v7}, v9);
    IrBuilder->CreateAdd(BB_3, v8, v2, v10);
    IrBuilder->CreateAdd(BB_3, v9, v3, v11);
    IrBuilder->CreateAdd(BB_3, v8, v10, v12);
    IrBuilder->CreateRet(BB_3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    VMIR::RegisterAllocator* registerAllocator = IrBuilder->CreateRegisterAllocator(cfg, 4, 4);

    livenessAnalyzer->PerformLivenessAnalysis();
    registerAllocator->PerformRegisterAllocation();

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v2->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v2->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v3->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v3->GetLocation()), VMIR::FPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v4->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v4->GetLocation()), VMIR::FPRegisterLocation(2));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v5->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v5->GetLocation()), VMIR::FPRegisterLocation(3));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v6->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v6->GetLocation()), VMIR::FPRegisterLocation(2));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v7->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v7->GetLocation()), VMIR::FPRegisterLocation(3));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v8->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v8->GetLocation()), VMIR::FPRegisterLocation(2));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v9->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v9->GetLocation()), VMIR::FPRegisterLocation(3));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v10->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v10->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v11->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v11->GetLocation()), VMIR::FPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v12->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v12->GetLocation()), VMIR::FPRegisterLocation(0));

    IrBuilder->Cleanup();
}


TEST(register_allocator, from_lecture_2) {
    /*
        Linear order:
            0:
                                                                live: 0
                v0 = Add ui64 0, 1                              live: 2,      lin: 0
                v1 = Add ui64 0, 10                             live: 4,      lin: 1
                v2 = Add ui64 0, 20                             live: 6,      lin: 2
                Jump #1                                         live: 8,      lin: 3
                                                                live: 10
            1:
                                                                live: 10
                v3 = Phi ui64 v0, v5                            live: 10,     lin: 4
                v4 = Phi ui64 v1, v6                            live: 10,     lin: 5
                Beq ui64 v4, v0 ? #2 : #3                       live: 12,     lin: 6
                                                                live: 14
            2:
                                                                live: 14
                v5 = Mul ui64 v3, v4                            live: 16,     lin: 7
                v6 = Sub ui64 v5, v0                            live: 18,     lin: 8
                Jump #1                                         live: 20,     lin: 9
                                                                live: 22
            3:
                                                                live: 22
                v7 = Add ui64 v2, v3                            live: 24,     lin: 10
                Ret                                             live: 26,     lin: 11
                                                                live: 28
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Func");

    VMIR::BasicBlock* BB_0  = IrBuilder->CreateBasicBlock(Func, "0");
    VMIR::BasicBlock* BB_1  = IrBuilder->CreateBasicBlock(Func, "1");
    VMIR::BasicBlock* BB_2  = IrBuilder->CreateBasicBlock(Func, "2");
    VMIR::BasicBlock* BB_3  = IrBuilder->CreateBasicBlock(Func, "3");

    Func->SetEntryBasicBlock(BB_0);

    VMIR::Value* zero   = IrBuilder->CreateValue(0UL);
    VMIR::Value* one    = IrBuilder->CreateValue(1UL);
    VMIR::Value* ten    = IrBuilder->CreateValue(10UL);
    VMIR::Value* twenty = IrBuilder->CreateValue(20UL);

    VMIR::Value* v0 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v6 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v7 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    IrBuilder->CreateAdd(BB_0, zero, one, v0);
    IrBuilder->CreateAdd(BB_0, zero, ten, v1);
    IrBuilder->CreateAdd(BB_0, zero, twenty, v2);
    IrBuilder->CreateJump(BB_0, BB_1);

    IrBuilder->CreatePhi(BB_1, {v5, v0}, v3);
    IrBuilder->CreatePhi(BB_1, {v6, v1}, v4);
    IrBuilder->CreateBeq(BB_1, v4, v0, BB_2, BB_3);

    IrBuilder->CreateMul(BB_2, v3, v4, v5);
    IrBuilder->CreateSub(BB_2, v5, v0, v6);
    IrBuilder->CreateJump(BB_2, BB_1);

    IrBuilder->CreateAdd(BB_3, v2, v3, v7);
    IrBuilder->CreateRet(BB_3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    VMIR::RegisterAllocator* registerAllocator = IrBuilder->CreateRegisterAllocator(cfg, 2, 2);

    livenessAnalyzer->PerformLivenessAnalysis();
    registerAllocator->PerformRegisterAllocation();

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v0->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v0->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v1->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v1->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::StackLocation>(v2->GetLocation()));
    EXPECT_EQ(std::get<VMIR::StackLocation>(v2->GetLocation()), VMIR::StackLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::StackLocation>(v3->GetLocation()));
    EXPECT_EQ(std::get<VMIR::StackLocation>(v3->GetLocation()), VMIR::StackLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v4->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v4->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v5->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v5->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::StackLocation>(v6->GetLocation()));
    EXPECT_EQ(std::get<VMIR::StackLocation>(v6->GetLocation()), VMIR::StackLocation(2));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v7->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v7->GetLocation()), VMIR::GPRegisterLocation(0));

    IrBuilder->Cleanup();
}


TEST(register_allocator, mixed_value_types) {
    /*
        Linear order:
            0:
                                                                live: 0
                v2 = Add ui64 v0, 3                             live: 2,      lin: 0
                v3 = Mul ui64 v2, 3                             live: 4,      lin: 1
                v4 = Add f32 v1, 3.140000                       live: 6,      lin: 2
                v5 = Div f32 v4, 2.718000                       live: 8,      lin: 3
                Jump #1                                         live: 10,     lin: 4
                                                                live: 12
            1:
                                                                live: 12
                v6 = Phi f32 v4, v12                            live: 12,     lin: 5
                v7 = Phi f32 v5, v13                            live: 12,     lin: 6
                v8 = Phi ui64 v3, v14                           live: 12,     lin: 7
                v9 = Add ui64 v8, v2                            live: 14,     lin: 8
                v10 = Mul ui64 v8, v9                           live: 16,     lin: 9
                v11 = Sub f32 v6, v7                            live: 18,     lin: 10
                Blt ui64 v10, 64 ? #2 : #3                      live: 20,     lin: 11
                                                                live: 22
            2:
                                                                live: 22
                v12 = Mul f32 v6, 2.718000                      live: 24,     lin: 12
                v13 = Div f32 v7, 2.718000                      live: 26,     lin: 13
                v14 = Add ui64 v8, 4                            live: 28,     lin: 14
                Jump #1                                         live: 30,     lin: 15
                                                                live: 32
            3:
                                                                live: 32
                Ret f32 v11                                     live: 34,     lin: 16
                                                                live: 36
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Float32, {VMIR::ValueType::Uint64, VMIR::ValueType::Float32}, "Func");

    VMIR::BasicBlock* BB_0  = IrBuilder->CreateBasicBlock(Func, "0");
    VMIR::BasicBlock* BB_1  = IrBuilder->CreateBasicBlock(Func, "1");
    VMIR::BasicBlock* BB_2  = IrBuilder->CreateBasicBlock(Func, "2");
    VMIR::BasicBlock* BB_3  = IrBuilder->CreateBasicBlock(Func, "3");

    Func->SetEntryBasicBlock(BB_0);

    VMIR::Value* three      = IrBuilder->CreateValue(3UL);
    VMIR::Value* four       = IrBuilder->CreateValue(4UL);
    VMIR::Value* sixtyFour  = IrBuilder->CreateValue(64UL);
    VMIR::Value* pi         = IrBuilder->CreateValue(3.14f);
    VMIR::Value* e          = IrBuilder->CreateValue(2.718f);

    VMIR::Value* v0  = Func->GetArg(0);
    VMIR::Value* v1  = Func->GetArg(1);
    VMIR::Value* v2  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v3  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v4  = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v5  = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v6  = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v7  = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v8  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v9  = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v10 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);
    VMIR::Value* v11 = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v12 = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v13 = IrBuilder->CreateValue(VMIR::ValueType::Float32);
    VMIR::Value* v14 = IrBuilder->CreateValue(VMIR::ValueType::Uint64);

    IrBuilder->CreateAdd(BB_0, v0, three, v2);
    IrBuilder->CreateMul(BB_0, v2, three, v3);
    IrBuilder->CreateAdd(BB_0, v1, pi, v4);
    IrBuilder->CreateDiv(BB_0, v4, e, v5);
    IrBuilder->CreateJump(BB_0, BB_1);

    IrBuilder->CreatePhi(BB_1, {v4, v12}, v6);
    IrBuilder->CreatePhi(BB_1, {v5, v13}, v7);
    IrBuilder->CreatePhi(BB_1, {v3, v14}, v8);
    IrBuilder->CreateAdd(BB_1, v8, v2, v9);
    IrBuilder->CreateMul(BB_1, v8, v9, v10);
    IrBuilder->CreateSub(BB_1, v6, v7, v11);
    IrBuilder->CreateBlt(BB_1, v10, sixtyFour, BB_2, BB_3);

    IrBuilder->CreateMul(BB_2, v6, e, v12);
    IrBuilder->CreateDiv(BB_2, v7, e, v13);
    IrBuilder->CreateAdd(BB_2, v8, four, v14);
    IrBuilder->CreateJump(BB_2, BB_1);

    IrBuilder->CreateRet(BB_3, v11);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    VMIR::RegisterAllocator* registerAllocator = IrBuilder->CreateRegisterAllocator(cfg, 2, 2);

    livenessAnalyzer->PerformLivenessAnalysis();
    registerAllocator->PerformRegisterAllocation();

    ASSERT_TRUE(std::holds_alternative<VMIR::StackLocation>(v2->GetLocation()));
    EXPECT_EQ(std::get<VMIR::StackLocation>(v2->GetLocation()), VMIR::StackLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v3->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v3->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v4->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v4->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v5->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v5->GetLocation()), VMIR::FPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v6->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v6->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v7->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v7->GetLocation()), VMIR::FPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v8->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v8->GetLocation()), VMIR::GPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v9->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v9->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v10->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v10->GetLocation()), VMIR::GPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::StackLocation>(v11->GetLocation()));
    EXPECT_EQ(std::get<VMIR::StackLocation>(v11->GetLocation()), VMIR::StackLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v12->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v12->GetLocation()), VMIR::FPRegisterLocation(0));

    ASSERT_TRUE(std::holds_alternative<VMIR::FPRegisterLocation>(v13->GetLocation()));
    EXPECT_EQ(std::get<VMIR::FPRegisterLocation>(v13->GetLocation()), VMIR::FPRegisterLocation(1));

    ASSERT_TRUE(std::holds_alternative<VMIR::GPRegisterLocation>(v14->GetLocation()));
    EXPECT_EQ(std::get<VMIR::GPRegisterLocation>(v14->GetLocation()), VMIR::GPRegisterLocation(0));

    IrBuilder->Cleanup();
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}