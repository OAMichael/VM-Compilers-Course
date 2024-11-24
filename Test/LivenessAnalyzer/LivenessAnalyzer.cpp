#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IRBuilder.h>
#include <LivenessAnalyzer.h>


static void CheckLoopCompactness(VMIR::Loop* loop, const std::vector<VMIR::BasicBlock*>& linearOrder) {
    auto* header = loop->GetHeader();
    const auto& innerLoops = loop->GetInnerLoops();
    size_t loopBBCount = loop->GetBasicBlocks().size();
    auto bbIt = std::find(linearOrder.cbegin(), linearOrder.cend(), header);
    while (bbIt != linearOrder.cend()) {
        auto* bb = *bbIt;
        auto* bbLoop = bb->GetLoop();
        if (bbLoop != loop) {
            if (innerLoops.find(bbLoop) != innerLoops.cend()) {
                // The block is associated with inner loop
                CheckLoopCompactness(bbLoop, linearOrder);
                ++bbIt;
                continue;
            }
            else {
                // The block indicates that the loop has ended
                break;
            }
        }
        --loopBBCount;
        ++bbIt;
    }

    EXPECT_EQ(loopBBCount, 0);
}

static void TestLinearOrderInvariants(VMIR::LivenessAnalyzer* livenessAnalyzer) {
    // Test linear order invariants:
    // - Dominators of a block appear before it
    // - Loop blocks come together

    // Dominators of a block appear before it
    // The condition for immediate dominators for every basic block implies the condition for all dominators
    const auto& linearOrder = livenessAnalyzer->GetBasicBlocksLinearOrder();
    for (auto* bb : linearOrder) {
        auto* bbImmDom = bb->GetImmediateDominator();

        if (bbImmDom != nullptr) {
            auto bbIt = std::find(linearOrder.cbegin(), linearOrder.cend(), bb);
            auto immDomIt = std::find(linearOrder.cbegin(), linearOrder.cend(), bbImmDom);

            EXPECT_TRUE(std::distance(immDomIt, bbIt) > 0);
        }
    }

    // Loop blocks come together
    auto* loopAnalyzer = livenessAnalyzer->GetLoopAnalyzer();
    const auto& loops = loopAnalyzer->GetLoops();
    for (const auto& bbLoop : loops) {
        auto* loop = bbLoop.second;
        if (loop == loopAnalyzer->GetRootLoop()) {
            continue;
        }

        CheckLoopCompactness(loop, linearOrder);
    }
}



TEST(liveness_analyzer, linear_order__1) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example1");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    VMIR::Value* v1  = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, v1, v1, C, D);

    IrBuilder->CreateJump(D, E);

    IrBuilder->CreateJump(E, B);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__2) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example2");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    VMIR::Value* v1  = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateJump(B, C);

    IrBuilder->CreateBeq(C, v1, v1, D, F);

    IrBuilder->CreateBeq(D, v1, v1, E, F);

    IrBuilder->CreateJump(E, B);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__3) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example3");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");
    VMIR::BasicBlock* G = IrBuilder->CreateBasicBlock(Func, "G");
    VMIR::BasicBlock* H = IrBuilder->CreateBasicBlock(Func, "H");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    VMIR::Value* v1  = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, v1, v1, C, D);

    IrBuilder->CreateBeq(C, v1, v1, E, F);

    IrBuilder->CreateJump(D, F);

    IrBuilder->CreateJump(F, G);

    IrBuilder->CreateBeq(G, v1, v1, H, B);

    IrBuilder->CreateJump(H, A);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__4) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example4");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");
    VMIR::BasicBlock* G = IrBuilder->CreateBasicBlock(Func, "G");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    VMIR::Value* v1  = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, v1, v1, C, F);

    IrBuilder->CreateJump(C, D);

    IrBuilder->CreateBeq(F, v1, v1, E, G);

    IrBuilder->CreateJump(E, D);

    IrBuilder->CreateJump(G, D);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__5) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example5");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");
    VMIR::BasicBlock* G = IrBuilder->CreateBasicBlock(Func, "G");
    VMIR::BasicBlock* H = IrBuilder->CreateBasicBlock(Func, "H");
    VMIR::BasicBlock* I = IrBuilder->CreateBasicBlock(Func, "I");
    VMIR::BasicBlock* J = IrBuilder->CreateBasicBlock(Func, "J");
    VMIR::BasicBlock* K = IrBuilder->CreateBasicBlock(Func, "K");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    VMIR::Value* v1  = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, v1, v1, C, J);

    IrBuilder->CreateJump(J, C);

    IrBuilder->CreateJump(C, D);

    IrBuilder->CreateBeq(D, v1, v1, C, E);

    IrBuilder->CreateJump(E, F);

    IrBuilder->CreateBeq(F, v1, v1, E, G);

    IrBuilder->CreateBeq(G, v1, v1, H, I);

    IrBuilder->CreateJump(H, B);

    IrBuilder->CreateJump(I, K);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__big) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("ExampleBig");

    VMIR::BasicBlock* Start = IrBuilder->CreateBasicBlock(Func, "Start");
    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");
    VMIR::BasicBlock* G = IrBuilder->CreateBasicBlock(Func, "G");
    VMIR::BasicBlock* H = IrBuilder->CreateBasicBlock(Func, "H");
    VMIR::BasicBlock* I = IrBuilder->CreateBasicBlock(Func, "I");
    VMIR::BasicBlock* K = IrBuilder->CreateBasicBlock(Func, "K");
    VMIR::BasicBlock* L = IrBuilder->CreateBasicBlock(Func, "L");
    VMIR::BasicBlock* O = IrBuilder->CreateBasicBlock(Func, "O");
    VMIR::BasicBlock* Q = IrBuilder->CreateBasicBlock(Func, "Q");
    VMIR::BasicBlock* R = IrBuilder->CreateBasicBlock(Func, "R");
    VMIR::BasicBlock* T = IrBuilder->CreateBasicBlock(Func, "T");
    VMIR::BasicBlock* V = IrBuilder->CreateBasicBlock(Func, "V");
    VMIR::BasicBlock* W = IrBuilder->CreateBasicBlock(Func, "W");
    VMIR::BasicBlock* X = IrBuilder->CreateBasicBlock(Func, "X");
    VMIR::BasicBlock* End = IrBuilder->CreateBasicBlock(Func, "End");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(Start);

    VMIR::Value* v1  = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(Start, A);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateJump(B, C);

    IrBuilder->CreateBeq(C, v1, v1, R, D);

    IrBuilder->CreateJump(R, B);

    IrBuilder->CreateJump(D, E);

    IrBuilder->CreateBeq(E, v1, v1, F, L);

    IrBuilder->CreateJump(L, I);

    IrBuilder->CreateJump(I, A);

    IrBuilder->CreateBeq(F, v1, v1, G, H);

    IrBuilder->CreateJump(G, Q);

    IrBuilder->CreateJump(Q, W);

    IrBuilder->CreateJump(W, A);

    IrBuilder->CreateBeq(H, v1, v1, T, O);

    IrBuilder->CreateJump(O, E);

    IrBuilder->CreateJump(T, V);

    IrBuilder->CreateBeq(V, v1, v1, X, K);

    IrBuilder->CreateJump(K, A);

    IrBuilder->CreateJump(X, End);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__from_lecture_1) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("ExampleLecture1");

    VMIR::BasicBlock* BB_0  = IrBuilder->CreateBasicBlock(Func, "0");
    VMIR::BasicBlock* BB_1  = IrBuilder->CreateBasicBlock(Func, "1");
    VMIR::BasicBlock* BB_2  = IrBuilder->CreateBasicBlock(Func, "2");
    VMIR::BasicBlock* BB_3  = IrBuilder->CreateBasicBlock(Func, "3");
    VMIR::BasicBlock* BB_4  = IrBuilder->CreateBasicBlock(Func, "4");
    VMIR::BasicBlock* BB_5  = IrBuilder->CreateBasicBlock(Func, "5");
    VMIR::BasicBlock* BB_6  = IrBuilder->CreateBasicBlock(Func, "6");
    VMIR::BasicBlock* BB_7  = IrBuilder->CreateBasicBlock(Func, "7");
    VMIR::BasicBlock* BB_8  = IrBuilder->CreateBasicBlock(Func, "8");
    VMIR::BasicBlock* BB_9  = IrBuilder->CreateBasicBlock(Func, "9");
    VMIR::BasicBlock* BB_10 = IrBuilder->CreateBasicBlock(Func, "10");
    VMIR::BasicBlock* BB_11 = IrBuilder->CreateBasicBlock(Func, "11");
    VMIR::BasicBlock* BB_12 = IrBuilder->CreateBasicBlock(Func, "12");
    VMIR::BasicBlock* BB_13 = IrBuilder->CreateBasicBlock(Func, "13");
    VMIR::BasicBlock* BB_14 = IrBuilder->CreateBasicBlock(Func, "14");
    VMIR::BasicBlock* BB_15 = IrBuilder->CreateBasicBlock(Func, "15");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(BB_0);

    VMIR::Value* v1  = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(BB_0, BB_2);

    IrBuilder->CreateBeq(BB_2, v1, v1, BB_3, BB_4);

    IrBuilder->CreateBeq(BB_4, v1, v1, BB_3, BB_5);

    IrBuilder->CreateJump(BB_5, BB_11);

    IrBuilder->CreateBeq(BB_11, v1, v1, BB_12, BB_13);

    IrBuilder->CreateJump(BB_12, BB_4);

    IrBuilder->CreateJump(BB_13, BB_1);

    IrBuilder->CreateJump(BB_3, BB_6);

    IrBuilder->CreateJump(BB_6, BB_7);

    IrBuilder->CreateJump(BB_7, BB_8);

    IrBuilder->CreateBeq(BB_8, v1, v1, BB_9, BB_14);

    IrBuilder->CreateJump(BB_9, BB_10);

    IrBuilder->CreateJump(BB_10, BB_6);

    IrBuilder->CreateJump(BB_14, BB_15);

    IrBuilder->CreateJump(BB_15, BB_3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__from_lecture_2) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("ExampleLecture2");

    VMIR::BasicBlock* BB_0  = IrBuilder->CreateBasicBlock(Func, "0");
    VMIR::BasicBlock* BB_1  = IrBuilder->CreateBasicBlock(Func, "1");
    VMIR::BasicBlock* BB_2  = IrBuilder->CreateBasicBlock(Func, "2");
    VMIR::BasicBlock* BB_3  = IrBuilder->CreateBasicBlock(Func, "3");
    VMIR::BasicBlock* BB_4  = IrBuilder->CreateBasicBlock(Func, "4");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(BB_0);

    VMIR::Value* v1  = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(BB_0, BB_1);

    IrBuilder->CreateBeq(BB_1, v1, v1, BB_3, BB_2);

    IrBuilder->CreateJump(BB_2, BB_1);

    IrBuilder->CreateJump(BB_3, BB_4);

    IrBuilder->CreateRet(BB_4);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__triple_nested_loop) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("TripleNestedLoop");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateJump(B, C);

    IrBuilder->CreateJump(C, D);

    IrBuilder->CreateBeq(D, v1, v1, C, E);

    IrBuilder->CreateBeq(E, v1, v1, B, F);

    IrBuilder->CreateJump(F, A);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__quadruple_nested_loop) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("QuadrupleNestedLoop");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");
    VMIR::BasicBlock* G = IrBuilder->CreateBasicBlock(Func, "G");
    VMIR::BasicBlock* H = IrBuilder->CreateBasicBlock(Func, "H");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateJump(B, C);

    IrBuilder->CreateJump(C, D);

    IrBuilder->CreateJump(D, E);

    IrBuilder->CreateBeq(E, v1, v1, D, F);

    IrBuilder->CreateBeq(F, v1, v1, C, G);

    IrBuilder->CreateBeq(G, v1, v1, B, H);

    IrBuilder->CreateJump(H, A);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, linear_order__quintuple_nested_loop) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("QuintupleNestedLoop");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");
    VMIR::BasicBlock* G = IrBuilder->CreateBasicBlock(Func, "G");
    VMIR::BasicBlock* H = IrBuilder->CreateBasicBlock(Func, "H");
    VMIR::BasicBlock* I = IrBuilder->CreateBasicBlock(Func, "I");
    VMIR::BasicBlock* J = IrBuilder->CreateBasicBlock(Func, "J");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateJump(B, C);

    IrBuilder->CreateJump(C, D);

    IrBuilder->CreateJump(D, E);

    IrBuilder->CreateJump(E, F);

    IrBuilder->CreateBeq(F, v1, v1, E, G);

    IrBuilder->CreateBeq(G, v1, v1, D, H);

    IrBuilder->CreateBeq(H, v1, v1, C, I);

    IrBuilder->CreateBeq(I, v1, v1, B, J);

    IrBuilder->CreateJump(J, A);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);

    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, liveness__fact) {
    /*
        Linear order:
            BB_1:
                live: 0
                Beq i32 v0, 0 ? #BB_3 : #BB_2  live: 2,  lin: 0
                live: 4

            BB_2:
                live: 4
                Beq i32 v0, 1 ? #BB_3 : #BB_4  live: 6,  lin: 1
                live: 8

            BB_4:
                live: 8
                v1 = Sub i32 v0, 1             live: 10,  lin: 2
                v2 = Call i32 #Func(i32 v1)    live: 12,  lin: 3
                v3 = Mul i32 v0, v2            live: 14,  lin: 4
                Ret i32 v3                     live: 16,  lin: 5
                live: 18

            BB_3:
                live: 18
                Ret i32 1                      live: 20,  lin: 6
                live: 22
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Int32, {VMIR::ValueType::Int32}, "livenessFact");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Func, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Func, "BB_3");
    VMIR::BasicBlock* BB_4 = IrBuilder->CreateBasicBlock(Func, "BB_4");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* zero = IrBuilder->CreateValue(Func, 0);
    VMIR::Value* one  = IrBuilder->CreateValue(Func, 1);

    VMIR::Value* v0 = Func->GetArg(0);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Int32);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Int32);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Int32);

    auto* instTermBB_1  = IrBuilder->CreateBeq(BB_1, v0, zero, BB_3, BB_2);

    auto* instTermBB_2  = IrBuilder->CreateBeq(BB_2, v0, one, BB_3, BB_4);

    auto* instTermBB_3  = IrBuilder->CreateRet(BB_3, one);

    auto* instV1        = IrBuilder->CreateSub(BB_4, v0, one, v1);
    auto* instV2        = IrBuilder->CreateCall(BB_4, Func, v2, {v1});
    auto* instV3        = IrBuilder->CreateMul(BB_4, v0, v2, v3);
    auto* instTermBB_4  = IrBuilder->CreateRet(BB_4, v3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);


    EXPECT_EQ(instTermBB_1->GetLinearNumber(),  0);
    EXPECT_EQ(instTermBB_2->GetLinearNumber(),  1);
    EXPECT_EQ(instV1->GetLinearNumber(),        2);
    EXPECT_EQ(instV2->GetLinearNumber(),        3);
    EXPECT_EQ(instV3->GetLinearNumber(),        4);
    EXPECT_EQ(instTermBB_3->GetLinearNumber(),  6);
    EXPECT_EQ(instTermBB_4->GetLinearNumber(),  5);


    auto& BB_1_LiveRange = BB_1->GetLiveRange();
    EXPECT_TRUE(BB_1_LiveRange.IsValid());
    EXPECT_EQ(BB_1_LiveRange, VMIR::LiveRange(0, 4));
    
    auto& BB_2_LiveRange = BB_2->GetLiveRange();
    EXPECT_TRUE(BB_2_LiveRange.IsValid());
    EXPECT_EQ(BB_2_LiveRange, VMIR::LiveRange(4, 8));

    auto& BB_3_LiveRange = BB_3->GetLiveRange();
    EXPECT_TRUE(BB_3_LiveRange.IsValid());
    EXPECT_EQ(BB_3_LiveRange, VMIR::LiveRange(18, 22));

    auto& BB_4_LiveRange = BB_4->GetLiveRange();
    EXPECT_TRUE(BB_4_LiveRange.IsValid());
    EXPECT_EQ(BB_4_LiveRange, VMIR::LiveRange(8, 18));

    EXPECT_EQ(instTermBB_1->GetLiveNumber(),    2);
    EXPECT_EQ(instTermBB_2->GetLiveNumber(),    6);
    EXPECT_EQ(instV1->GetLiveNumber(),          10);
    EXPECT_EQ(instV2->GetLiveNumber(),          12);
    EXPECT_EQ(instV3->GetLiveNumber(),          14);
    EXPECT_EQ(instTermBB_3->GetLiveNumber(),    20);
    EXPECT_EQ(instTermBB_4->GetLiveNumber(),    16);


    auto& instV1_LiveInterval = instV1->GetLiveInterval();
    EXPECT_TRUE(instV1_LiveInterval.IsValid());
    EXPECT_EQ(instV1_LiveInterval, VMIR::LiveInterval(10, 12));

    auto& instV2_LiveInterval = instV2->GetLiveInterval();
    EXPECT_TRUE(instV2_LiveInterval.IsValid());
    EXPECT_EQ(instV2_LiveInterval, VMIR::LiveInterval(12, 14));

    auto& instV3_LiveInterval = instV3->GetLiveInterval();
    EXPECT_TRUE(instV3_LiveInterval.IsValid());
    EXPECT_EQ(instV3_LiveInterval, VMIR::LiveInterval(14, 16));


    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, liveness__from_lecture_1) {
    /*
        Linear order:
            0:
                live: 0
                v2 = Add ui64 v0, v1           live: 2,  lin: 0
                v3 = Add ui64 v0, v1           live: 4,  lin: 1
                Beq ui64 v0, v1 ? #2 : #1      live: 6,  lin: 2
                live: 8

            1:
                live: 8
                v4 = Add ui64 v0, v1           live: 10,  lin: 3
                v5 = Add ui64 v0, v1           live: 12,  lin: 4
                Jump #3                        live: 14,  lin: 5
                live: 16

            2:
                live: 16
                v6 = Add ui64 v0, v1           live: 18,  lin: 6
                v7 = Add ui64 v0, v1           live: 20,  lin: 7
                Jump #3                        live: 22,  lin: 8
                live: 24

            3:
                live: 24
                v8 = Phi ui64 v4, v6           live: 24,  lin: 9
                v9 = Phi ui64 v5, v7           live: 24,  lin: 10
                v10 = Add ui64 v8, v2          live: 26,  lin: 11
                v11 = Add ui64 v9, v3          live: 28,  lin: 12
                v12 = Add ui64 v8, v10         live: 30,  lin: 13
                Ret                            live: 32,  lin: 14
                live: 34
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("liveness1");

    VMIR::BasicBlock* BB_0  = IrBuilder->CreateBasicBlock(Func, "0");
    VMIR::BasicBlock* BB_1  = IrBuilder->CreateBasicBlock(Func, "1");
    VMIR::BasicBlock* BB_2  = IrBuilder->CreateBasicBlock(Func, "2");
    VMIR::BasicBlock* BB_3  = IrBuilder->CreateBasicBlock(Func, "3");

    Func->SetEntryBasicBlock(BB_0);

    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v6 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v7 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v8 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v9 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v10 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v11 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v12 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    auto* instV2        = IrBuilder->CreateAdd(BB_0, v0, v1, v2);
    auto* instV3        = IrBuilder->CreateAdd(BB_0, v0, v1, v3);
    auto* instTermBB_0  = IrBuilder->CreateBeq(BB_0, v0, v1, BB_2, BB_1);

    auto* instV4        = IrBuilder->CreateAdd(BB_1, v0, v1, v4);
    auto* instV5        = IrBuilder->CreateAdd(BB_1, v0, v1, v5);
    auto* instTermBB_1  = IrBuilder->CreateJump(BB_1, BB_3);

    auto* instV6        = IrBuilder->CreateAdd(BB_2, v0, v1, v6);
    auto* instV7        = IrBuilder->CreateAdd(BB_2, v0, v1, v7);
    auto* instTermBB_2  = IrBuilder->CreateJump(BB_2, BB_3);

    auto* instV8        = IrBuilder->CreatePhi(BB_3, {v4, v6}, v8);
    auto* instV9        = IrBuilder->CreatePhi(BB_3, {v5, v7}, v9);
    auto* instV10       = IrBuilder->CreateAdd(BB_3, v8, v2, v10);
    auto* instV11       = IrBuilder->CreateAdd(BB_3, v9, v3, v11);
    auto* instV12       = IrBuilder->CreateAdd(BB_3, v8, v10, v12);
    auto* instTermBB_3  = IrBuilder->CreateRet(BB_3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);


    EXPECT_EQ(instV2->GetLinearNumber(),        0);
    EXPECT_EQ(instV3->GetLinearNumber(),        1);
    EXPECT_EQ(instTermBB_0->GetLinearNumber(),  2);
    EXPECT_EQ(instV4->GetLinearNumber(),        3);
    EXPECT_EQ(instV5->GetLinearNumber(),        4);
    EXPECT_EQ(instTermBB_1->GetLinearNumber(),  5);
    EXPECT_EQ(instV6->GetLinearNumber(),        6);
    EXPECT_EQ(instV7->GetLinearNumber(),        7);
    EXPECT_EQ(instTermBB_2->GetLinearNumber(),  8);
    EXPECT_EQ(instV8->GetLinearNumber(),        9);
    EXPECT_EQ(instV9->GetLinearNumber(),       10);
    EXPECT_EQ(instV10->GetLinearNumber(),      11);
    EXPECT_EQ(instV11->GetLinearNumber(),      12);
    EXPECT_EQ(instV12->GetLinearNumber(),      13);
    EXPECT_EQ(instTermBB_3->GetLinearNumber(), 14);


    auto& BB_0_LiveRange = BB_0->GetLiveRange();
    EXPECT_TRUE(BB_0_LiveRange.IsValid());
    EXPECT_EQ(BB_0_LiveRange, VMIR::LiveRange(0, 8));
    
    auto& BB_1_LiveRange = BB_1->GetLiveRange();
    EXPECT_TRUE(BB_1_LiveRange.IsValid());
    EXPECT_EQ(BB_1_LiveRange, VMIR::LiveRange(8, 16));

    auto& BB_2_LiveRange = BB_2->GetLiveRange();
    EXPECT_TRUE(BB_2_LiveRange.IsValid());
    EXPECT_EQ(BB_2_LiveRange, VMIR::LiveRange(16, 24));

    auto& BB_3_LiveRange = BB_3->GetLiveRange();
    EXPECT_TRUE(BB_3_LiveRange.IsValid());
    EXPECT_EQ(BB_3_LiveRange, VMIR::LiveRange(24, 34));


    EXPECT_EQ(instV2->GetLiveNumber(),          2);
    EXPECT_EQ(instV3->GetLiveNumber(),          4);
    EXPECT_EQ(instTermBB_0->GetLiveNumber(),    6);
    EXPECT_EQ(instV4->GetLiveNumber(),         10);
    EXPECT_EQ(instV5->GetLiveNumber(),         12);
    EXPECT_EQ(instTermBB_1->GetLiveNumber(),   14);
    EXPECT_EQ(instV6->GetLiveNumber(),         18);
    EXPECT_EQ(instV7->GetLiveNumber(),         20);
    EXPECT_EQ(instTermBB_2->GetLiveNumber(),   22);
    EXPECT_EQ(instV8->GetLiveNumber(),         24);     // Phi
    EXPECT_EQ(instV9->GetLiveNumber(),         24);     // Phi
    EXPECT_EQ(instV10->GetLiveNumber(),        26);
    EXPECT_EQ(instV11->GetLiveNumber(),        28);
    EXPECT_EQ(instV12->GetLiveNumber(),        30);
    EXPECT_EQ(instTermBB_3->GetLiveNumber(),   32);


    auto& instV2_LiveInterval = instV2->GetLiveInterval();
    EXPECT_TRUE(instV2_LiveInterval.IsValid());
    EXPECT_EQ(instV2_LiveInterval, VMIR::LiveInterval(2, 26));

    auto& instV3_LiveInterval = instV3->GetLiveInterval();
    EXPECT_TRUE(instV3_LiveInterval.IsValid());
    EXPECT_EQ(instV3_LiveInterval, VMIR::LiveInterval(4, 28));

    auto& instV4_LiveInterval = instV4->GetLiveInterval();
    EXPECT_TRUE(instV4_LiveInterval.IsValid());
    EXPECT_EQ(instV4_LiveInterval, VMIR::LiveInterval(10, 16));

    auto& instV5_LiveInterval = instV5->GetLiveInterval();
    EXPECT_TRUE(instV5_LiveInterval.IsValid());
    EXPECT_EQ(instV5_LiveInterval, VMIR::LiveInterval(12, 16));

    auto& instV6_LiveInterval = instV6->GetLiveInterval();
    EXPECT_TRUE(instV6_LiveInterval.IsValid());
    EXPECT_EQ(instV6_LiveInterval, VMIR::LiveInterval(18, 24));

    auto& instV7_LiveInterval = instV7->GetLiveInterval();
    EXPECT_TRUE(instV7_LiveInterval.IsValid());
    EXPECT_EQ(instV7_LiveInterval, VMIR::LiveInterval(20, 24));

    // Phi
    auto& instV8_LiveInterval = instV8->GetLiveInterval();
    EXPECT_TRUE(instV8_LiveInterval.IsValid());
    EXPECT_EQ(instV8_LiveInterval, VMIR::LiveInterval(24, 30));

    // Phi
    auto& instV9_LiveInterval = instV9->GetLiveInterval();
    EXPECT_TRUE(instV9_LiveInterval.IsValid());
    EXPECT_EQ(instV9_LiveInterval, VMIR::LiveInterval(24, 28));

    auto& instV10_LiveInterval = instV10->GetLiveInterval();
    EXPECT_TRUE(instV10_LiveInterval.IsValid());
    EXPECT_EQ(instV10_LiveInterval, VMIR::LiveInterval(26, 30));

    auto& instV11_LiveInterval = instV11->GetLiveInterval();
    EXPECT_TRUE(instV11_LiveInterval.IsValid());
    EXPECT_EQ(instV11_LiveInterval, VMIR::LiveInterval(28, 30));

    auto& instV12_LiveInterval = instV12->GetLiveInterval();
    EXPECT_TRUE(instV12_LiveInterval.IsValid());
    EXPECT_EQ(instV12_LiveInterval, VMIR::LiveInterval(30, 32));


    IrBuilder->Cleanup();
}


TEST(liveness_analyzer, liveness__from_lecture_2) {
    /*
        Linear order:
            0:
                live: 0
                v0 = Add ui64 0, 1             live: 2,  lin: 0
                v1 = Add ui64 0, 10            live: 4,  lin: 1
                v2 = Add ui64 0, 20            live: 6,  lin: 2
                Jump #1                        live: 8,  lin: 3
                live: 10

            1:
                live: 10
                v3 = Phi ui64 v0, v5           live: 10,  lin: 4
                v4 = Phi ui64 v6, v1           live: 10,  lin: 5
                Beq ui64 v4, v0 ? #2 : #3      live: 12,  lin: 6
                live: 14

            2:
                live: 14
                v5 = Mul ui64 v3, v4           live: 16,  lin: 7
                v6 = Sub ui64 v5, v0           live: 18,  lin: 8
                Jump #1                        live: 20,  lin: 9
                live: 22

            3:
                live: 22
                v7 = Add ui64 v2, v3           live: 24,  lin: 10
                Ret                            live: 26,  lin: 11
                live: 28
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("liveness2");

    VMIR::BasicBlock* BB_0  = IrBuilder->CreateBasicBlock(Func, "0");
    VMIR::BasicBlock* BB_1  = IrBuilder->CreateBasicBlock(Func, "1");
    VMIR::BasicBlock* BB_2  = IrBuilder->CreateBasicBlock(Func, "2");
    VMIR::BasicBlock* BB_3  = IrBuilder->CreateBasicBlock(Func, "3");

    Func->SetEntryBasicBlock(BB_0);

    VMIR::Value* zero   = IrBuilder->CreateValue(Func, 0UL);
    VMIR::Value* one    = IrBuilder->CreateValue(Func, 1UL);
    VMIR::Value* ten    = IrBuilder->CreateValue(Func, 10UL);
    VMIR::Value* twenty = IrBuilder->CreateValue(Func, 20UL);

    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v6 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v7 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    auto* instV0        = IrBuilder->CreateAdd(BB_0, zero, one, v0);
    auto* instV1        = IrBuilder->CreateAdd(BB_0, zero, ten, v1);
    auto* instV2        = IrBuilder->CreateAdd(BB_0, zero, twenty, v2);
    auto* instTermBB_0  = IrBuilder->CreateJump(BB_0, BB_1);

    auto* instV3        = IrBuilder->CreatePhi(BB_1, {v5, v0}, v3);
    auto* instV4        = IrBuilder->CreatePhi(BB_1, {v6, v1}, v4);
    auto* instTermBB_1  = IrBuilder->CreateBeq(BB_1, v4, v0, BB_2, BB_3);

    auto* instV5        = IrBuilder->CreateMul(BB_2, v3, v4, v5);
    auto* instV6        = IrBuilder->CreateSub(BB_2, v5, v0, v6);
    auto* instTermBB_2  = IrBuilder->CreateJump(BB_2, BB_1);

    auto* instV7        = IrBuilder->CreateAdd(BB_3, v2, v3, v7);
    auto* instTermBB_3  = IrBuilder->CreateRet(BB_3);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LivenessAnalyzer* livenessAnalyzer = IrBuilder->CreateLivenessAnalyzer(cfg);
    livenessAnalyzer->PerformLivenessAnalysis();

    TestLinearOrderInvariants(livenessAnalyzer);


    EXPECT_EQ(instV0->GetLinearNumber(),        0);
    EXPECT_EQ(instV1->GetLinearNumber(),        1);
    EXPECT_EQ(instV2->GetLinearNumber(),        2);
    EXPECT_EQ(instTermBB_0->GetLinearNumber(),  3);
    EXPECT_EQ(instV3->GetLinearNumber(),        4);     // Phi
    EXPECT_EQ(instV4->GetLinearNumber(),        5);     // Phi
    EXPECT_EQ(instTermBB_1->GetLinearNumber(),  6);
    EXPECT_EQ(instV5->GetLinearNumber(),        7);
    EXPECT_EQ(instV6->GetLinearNumber(),        8);
    EXPECT_EQ(instTermBB_2->GetLinearNumber(),  9);
    EXPECT_EQ(instV7->GetLinearNumber(),        10);
    EXPECT_EQ(instTermBB_3->GetLinearNumber(),  11);


    auto& BB_0_LiveRange = BB_0->GetLiveRange();
    EXPECT_TRUE(BB_0_LiveRange.IsValid());
    EXPECT_EQ(BB_0_LiveRange, VMIR::LiveRange(0, 10));
    
    auto& BB_1_LiveRange = BB_1->GetLiveRange();
    EXPECT_TRUE(BB_1_LiveRange.IsValid());
    EXPECT_EQ(BB_1_LiveRange, VMIR::LiveRange(10, 14));

    auto& BB_2_LiveRange = BB_2->GetLiveRange();
    EXPECT_TRUE(BB_2_LiveRange.IsValid());
    EXPECT_EQ(BB_2_LiveRange, VMIR::LiveRange(14, 22));

    auto& BB_3_LiveRange = BB_3->GetLiveRange();
    EXPECT_TRUE(BB_3_LiveRange.IsValid());
    EXPECT_EQ(BB_3_LiveRange, VMIR::LiveRange(22, 28));


    EXPECT_EQ(instV0->GetLiveNumber(),          2);
    EXPECT_EQ(instV1->GetLiveNumber(),          4);
    EXPECT_EQ(instV2->GetLiveNumber(),          6);
    EXPECT_EQ(instTermBB_0->GetLiveNumber(),    8);
    EXPECT_EQ(instV3->GetLiveNumber(),          10);     // Phi
    EXPECT_EQ(instV4->GetLiveNumber(),          10);     // Phi
    EXPECT_EQ(instTermBB_1->GetLiveNumber(),    12);
    EXPECT_EQ(instV5->GetLiveNumber(),          16);
    EXPECT_EQ(instV6->GetLiveNumber(),          18);
    EXPECT_EQ(instTermBB_2->GetLiveNumber(),    20);
    EXPECT_EQ(instV7->GetLiveNumber(),          24);
    EXPECT_EQ(instTermBB_3->GetLiveNumber(),    26);


    auto& instV0_LiveInterval = instV0->GetLiveInterval();
    EXPECT_TRUE(instV0_LiveInterval.IsValid());
    EXPECT_EQ(instV0_LiveInterval, VMIR::LiveInterval(2, 22));

    auto& instV1_LiveInterval = instV1->GetLiveInterval();
    EXPECT_TRUE(instV1_LiveInterval.IsValid());
    EXPECT_EQ(instV1_LiveInterval, VMIR::LiveInterval(4, 10));

    auto& instV2_LiveInterval = instV2->GetLiveInterval();
    EXPECT_TRUE(instV2_LiveInterval.IsValid());
    EXPECT_EQ(instV2_LiveInterval, VMIR::LiveInterval(6, 24));

    // Phi
    auto& instV3_LiveInterval = instV3->GetLiveInterval();
    EXPECT_TRUE(instV3_LiveInterval.IsValid());
    EXPECT_EQ(instV3_LiveInterval, VMIR::LiveInterval(10, 24));

    // Phi
    auto& instV4_LiveInterval = instV4->GetLiveInterval();
    EXPECT_TRUE(instV4_LiveInterval.IsValid());
    EXPECT_EQ(instV4_LiveInterval, VMIR::LiveInterval(10, 16));

    auto& instV5_LiveInterval = instV5->GetLiveInterval();
    EXPECT_TRUE(instV5_LiveInterval.IsValid());
    EXPECT_EQ(instV5_LiveInterval, VMIR::LiveInterval(16, 22));

    auto& instV6_LiveInterval = instV6->GetLiveInterval();
    EXPECT_TRUE(instV6_LiveInterval.IsValid());
    EXPECT_EQ(instV6_LiveInterval, VMIR::LiveInterval(18, 22));

    auto& instV7_LiveInterval = instV7->GetLiveInterval();
    EXPECT_TRUE(instV7_LiveInterval.IsValid());
    EXPECT_EQ(instV7_LiveInterval, VMIR::LiveInterval(24, 26));


    IrBuilder->Cleanup();
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}