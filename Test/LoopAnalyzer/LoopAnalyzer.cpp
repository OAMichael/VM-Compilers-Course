#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IRBuilder.h>
#include <LoopAnalyzer.h>


TEST(loop_analyzer, example1) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example1");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, nullptr, nullptr, C, D);

    IrBuilder->CreateJump(D, E);

    IrBuilder->CreateJump(E, B);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LoopAnalyzer* loopAnalyzer = IrBuilder->CreateLoopAnalyzer(cfg);
    loopAnalyzer->BuildLoopTree();

    auto* rootLoop = loopAnalyzer->GetRootLoop();
    auto& loops = loopAnalyzer->GetLoops();

    // Now check all loop relationships
    EXPECT_EQ(loops.size(), 1);
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(B)));

    auto* loopB = loops.at(B);

    EXPECT_EQ(loopB->GetHeader(), B);
    EXPECT_EQ(loopB->GetOuterLoop(), rootLoop);
    EXPECT_THAT(loopB->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopB->GetBasicBlocks(), ::testing::UnorderedElementsAre(B, D, E));
    EXPECT_THAT(loopB->GetLatches(), ::testing::UnorderedElementsAre(E));
    EXPECT_TRUE(loopB->IsReducible());

    IrBuilder->Cleanup();
}


TEST(loop_analyzer, example2) {
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

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateJump(B, C);

    IrBuilder->CreateBeq(C, nullptr, nullptr, D, F);

    IrBuilder->CreateBeq(D, nullptr, nullptr, E, F);

    IrBuilder->CreateJump(E, B);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LoopAnalyzer* loopAnalyzer = IrBuilder->CreateLoopAnalyzer(cfg);
    loopAnalyzer->BuildLoopTree();

    auto* rootLoop = loopAnalyzer->GetRootLoop();
    auto& loops = loopAnalyzer->GetLoops();

    // Now check all loop relationships
    EXPECT_EQ(loops.size(), 1);
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(B)));

    auto* loopB = loops.at(B);

    EXPECT_EQ(loopB->GetHeader(), B);
    EXPECT_EQ(loopB->GetOuterLoop(), rootLoop);
    EXPECT_THAT(loopB->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopB->GetBasicBlocks(), ::testing::UnorderedElementsAre(B, C, D, E));
    EXPECT_THAT(loopB->GetLatches(), ::testing::UnorderedElementsAre(E));
    EXPECT_TRUE(loopB->IsReducible());

    IrBuilder->Cleanup();
}


TEST(loop_analyzer, example3) {
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

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, nullptr, nullptr, C, D);

    IrBuilder->CreateBeq(C, nullptr, nullptr, E, F);

    IrBuilder->CreateJump(D, F);

    IrBuilder->CreateJump(F, G);

    IrBuilder->CreateBeq(G, nullptr, nullptr, H, B);

    IrBuilder->CreateJump(H, A);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LoopAnalyzer* loopAnalyzer = IrBuilder->CreateLoopAnalyzer(cfg);
    loopAnalyzer->BuildLoopTree();

    auto* rootLoop = loopAnalyzer->GetRootLoop();
    auto& loops = loopAnalyzer->GetLoops();

    // Now check all loop relationships
    EXPECT_EQ(loops.size(), 2);
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(A)));
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(B)));

    auto* loopA = loops.at(A);
    auto* loopB = loops.at(B);

    EXPECT_EQ(loopA->GetHeader(), A);
    EXPECT_EQ(loopA->GetOuterLoop(), rootLoop);
    EXPECT_THAT(loopA->GetInnerLoops(), ::testing::Contains(loopB));
    EXPECT_THAT(loopA->GetBasicBlocks(), ::testing::UnorderedElementsAre(A, H));
    EXPECT_THAT(loopA->GetLatches(), ::testing::UnorderedElementsAre(H));
    EXPECT_TRUE(loopA->IsReducible());

    EXPECT_EQ(loopB->GetHeader(), B);
    EXPECT_EQ(loopB->GetOuterLoop(), loopA);
    EXPECT_THAT(loopB->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopB->GetBasicBlocks(), ::testing::UnorderedElementsAre(B, C, D, F, G));
    EXPECT_THAT(loopB->GetLatches(), ::testing::UnorderedElementsAre(G));
    EXPECT_TRUE(loopB->IsReducible());

    IrBuilder->Cleanup();
}


TEST(loop_analyzer, example4) {
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

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, nullptr, nullptr, C, F);

    IrBuilder->CreateJump(C, D);

    IrBuilder->CreateBeq(F, nullptr, nullptr, E, G);

    IrBuilder->CreateJump(E, D);

    IrBuilder->CreateJump(G, D);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LoopAnalyzer* loopAnalyzer = IrBuilder->CreateLoopAnalyzer(cfg);
    loopAnalyzer->BuildLoopTree();

    auto& loops = loopAnalyzer->GetLoops();

    // Now check all loop relationships
    EXPECT_EQ(loops.size(), 0);

    IrBuilder->Cleanup();
}


TEST(loop_analyzer, example5) {
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

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, nullptr, nullptr, C, J);

    IrBuilder->CreateJump(J, C);

    IrBuilder->CreateJump(C, D);

    IrBuilder->CreateBeq(D, nullptr, nullptr, C, E);

    IrBuilder->CreateJump(E, F);

    IrBuilder->CreateBeq(F, nullptr, nullptr, E, G);

    IrBuilder->CreateBeq(G, nullptr, nullptr, H, I);

    IrBuilder->CreateJump(H, B);

    IrBuilder->CreateJump(I, K);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LoopAnalyzer* loopAnalyzer = IrBuilder->CreateLoopAnalyzer(cfg);
    loopAnalyzer->BuildLoopTree();

    auto* rootLoop = loopAnalyzer->GetRootLoop();
    auto& loops = loopAnalyzer->GetLoops();

    // Now check all loop relationships
    EXPECT_EQ(loops.size(), 3);
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(B)));
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(C)));
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(E)));

    auto* loopB = loops.at(B);
    auto* loopC = loops.at(C);
    auto* loopE = loops.at(E);

    EXPECT_EQ(loopB->GetHeader(), B);
    EXPECT_EQ(loopB->GetOuterLoop(), rootLoop);
    EXPECT_THAT(loopB->GetInnerLoops(), ::testing::Contains(loopC));
    EXPECT_THAT(loopB->GetInnerLoops(), ::testing::Contains(loopE));
    EXPECT_THAT(loopB->GetBasicBlocks(), ::testing::UnorderedElementsAre(B, J, G, H));
    EXPECT_THAT(loopB->GetLatches(), ::testing::UnorderedElementsAre(H));
    EXPECT_TRUE(loopB->IsReducible());

    EXPECT_EQ(loopC->GetHeader(), C);
    EXPECT_EQ(loopC->GetOuterLoop(), loopB);
    EXPECT_THAT(loopC->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopC->GetBasicBlocks(), ::testing::UnorderedElementsAre(C, D));
    EXPECT_THAT(loopC->GetLatches(), ::testing::UnorderedElementsAre(D));
    EXPECT_TRUE(loopC->IsReducible());

    EXPECT_EQ(loopE->GetHeader(), E);
    EXPECT_EQ(loopE->GetOuterLoop(), loopB);
    EXPECT_THAT(loopE->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopE->GetBasicBlocks(), ::testing::UnorderedElementsAre(E, F));
    EXPECT_THAT(loopE->GetLatches(), ::testing::UnorderedElementsAre(F));
    EXPECT_TRUE(loopE->IsReducible());

    IrBuilder->Cleanup();
}


TEST(loop_analyzer, example6) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example6");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");
    VMIR::BasicBlock* G = IrBuilder->CreateBasicBlock(Func, "G");
    VMIR::BasicBlock* H = IrBuilder->CreateBasicBlock(Func, "H");
    VMIR::BasicBlock* I = IrBuilder->CreateBasicBlock(Func, "I");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateBeq(B, nullptr, nullptr, C, E);

    IrBuilder->CreateJump(C, D);

    IrBuilder->CreateBeq(E, nullptr, nullptr, D, F);

    IrBuilder->CreateJump(D, G);

    IrBuilder->CreateBeq(F, nullptr, nullptr, B, H);

    IrBuilder->CreateBeq(G, nullptr, nullptr, C, I);

    IrBuilder->CreateBeq(H, nullptr, nullptr, G, I);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LoopAnalyzer* loopAnalyzer = IrBuilder->CreateLoopAnalyzer(cfg);
    loopAnalyzer->BuildLoopTree();

    auto* rootLoop = loopAnalyzer->GetRootLoop();
    auto& loops = loopAnalyzer->GetLoops();

    // Now check all loop relationships
    EXPECT_EQ(loops.size(), 2);
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(B)));
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(C)));

    auto* loopB = loops.at(B);
    auto* loopC = loops.at(C);

    EXPECT_EQ(loopB->GetHeader(), B);
    EXPECT_EQ(loopB->GetOuterLoop(), rootLoop);
    EXPECT_THAT(loopB->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopB->GetBasicBlocks(), ::testing::UnorderedElementsAre(B, E, F));
    EXPECT_THAT(loopB->GetLatches(), ::testing::UnorderedElementsAre(F));
    EXPECT_TRUE(loopB->IsReducible());

    EXPECT_EQ(loopC->GetHeader(), C);
    EXPECT_EQ(loopC->GetOuterLoop(), rootLoop);
    EXPECT_THAT(loopC->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopC->GetBasicBlocks(), ::testing::UnorderedElementsAre(C, G));
    EXPECT_THAT(loopC->GetLatches(), ::testing::UnorderedElementsAre(G));
    EXPECT_FALSE(loopC->IsReducible());

    IrBuilder->Cleanup();
}


TEST(loop_analyzer, exampleBig) {
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

    IrBuilder->CreateJump(Start, A);

    IrBuilder->CreateJump(A, B);

    IrBuilder->CreateJump(B, C);

    IrBuilder->CreateBeq(C, nullptr, nullptr, R, D);

    IrBuilder->CreateJump(R, B);

    IrBuilder->CreateJump(D, E);

    IrBuilder->CreateBeq(E, nullptr, nullptr, F, L);

    IrBuilder->CreateJump(L, I);

    IrBuilder->CreateJump(I, A);

    IrBuilder->CreateBeq(F, nullptr, nullptr, G, H);

    IrBuilder->CreateJump(G, Q);

    IrBuilder->CreateJump(Q, W);

    IrBuilder->CreateJump(W, A);

    IrBuilder->CreateBeq(H, nullptr, nullptr, T, O);

    IrBuilder->CreateJump(O, E);

    IrBuilder->CreateJump(T, V);

    IrBuilder->CreateBeq(V, nullptr, nullptr, X, K);

    IrBuilder->CreateJump(K, A);

    IrBuilder->CreateJump(X, End);

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    VMIR::LoopAnalyzer* loopAnalyzer = IrBuilder->CreateLoopAnalyzer(cfg);

    loopAnalyzer->BuildLoopTree();

    auto* rootLoop = loopAnalyzer->GetRootLoop();
    auto& loops = loopAnalyzer->GetLoops();

    // Now check all loop relationships
    EXPECT_EQ(loops.size(), 3);
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(A)));
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(B)));
    EXPECT_THAT(loops, ::testing::Contains(::testing::Key(E)));

    auto* loopA = loops.at(A);
    auto* loopB = loops.at(B);
    auto* loopE = loops.at(E);

    EXPECT_EQ(loopA->GetHeader(), A);
    EXPECT_EQ(loopA->GetOuterLoop(), rootLoop);
    EXPECT_THAT(loopA->GetInnerLoops(), ::testing::Contains(loopB));
    EXPECT_THAT(loopA->GetInnerLoops(), ::testing::Contains(loopE));
    EXPECT_THAT(loopA->GetBasicBlocks(), ::testing::UnorderedElementsAre(A, D, L, I, G, Q, W, T, V, K));
    EXPECT_THAT(loopA->GetLatches(), ::testing::UnorderedElementsAre(W, I, K));
    EXPECT_TRUE(loopA->IsReducible());

    EXPECT_EQ(loopB->GetHeader(), B);
    EXPECT_EQ(loopB->GetOuterLoop(), loopA);
    EXPECT_THAT(loopB->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopB->GetBasicBlocks(), ::testing::UnorderedElementsAre(B, C, R));
    EXPECT_THAT(loopB->GetLatches(), ::testing::UnorderedElementsAre(R));
    EXPECT_TRUE(loopB->IsReducible());

    EXPECT_EQ(loopE->GetHeader(), E);
    EXPECT_EQ(loopE->GetOuterLoop(), loopA);
    EXPECT_THAT(loopE->GetInnerLoops(), ::testing::IsEmpty());
    EXPECT_THAT(loopE->GetBasicBlocks(), ::testing::UnorderedElementsAre(E, F, H, O));
    EXPECT_THAT(loopE->GetLatches(), ::testing::UnorderedElementsAre(O));
    EXPECT_TRUE(loopE->IsReducible());

    IrBuilder->Cleanup();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}