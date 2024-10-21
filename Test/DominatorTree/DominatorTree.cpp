#include <gtest/gtest.h>

#include <IRBuilder.h>
#include <ControlFlowGraph.h>


TEST(dominator_tree, example1) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example1");

    VMIR::BasicBlock* A = IrBuilder->CreateBasicBlock(Func, "A");
    VMIR::BasicBlock* B = IrBuilder->CreateBasicBlock(Func, "B");
    VMIR::BasicBlock* C = IrBuilder->CreateBasicBlock(Func, "C");
    VMIR::BasicBlock* D = IrBuilder->CreateBasicBlock(Func, "D");
    VMIR::BasicBlock* E = IrBuilder->CreateBasicBlock(Func, "E");
    VMIR::BasicBlock* F = IrBuilder->CreateBasicBlock(Func, "F");
    VMIR::BasicBlock* G = IrBuilder->CreateBasicBlock(Func, "G");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    // 1 edge:  A -> B
    IrBuilder->CreateJump(A, B);

    // 2 edges: B -> C and B -> F
    IrBuilder->CreateBeq(B, nullptr, nullptr, C, F);

    // 1 edge:  C -> D
    IrBuilder->CreateJump(C, D);

    // 2 edges: F -> E and F -> G
    IrBuilder->CreateBeq(F, nullptr, nullptr, E, G);

    // 1 edge:  E -> D
    IrBuilder->CreateJump(E, D);

    // 1 edge:  G -> D
    IrBuilder->CreateJump(G, D);

    // Total: 8 edges (according to example 1)

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    cfg->BuildDominatorTree();

    // Now check all dominance dependencies
    EXPECT_EQ(A->GetImmediateDominator(), nullptr);
    EXPECT_EQ(B->GetImmediateDominator(), A);
    EXPECT_EQ(C->GetImmediateDominator(), B);
    EXPECT_EQ(D->GetImmediateDominator(), B);
    EXPECT_EQ(E->GetImmediateDominator(), F);
    EXPECT_EQ(F->GetImmediateDominator(), B);
    EXPECT_EQ(G->GetImmediateDominator(), F);

    EXPECT_TRUE(B->IsDominatedBy(A));
    EXPECT_TRUE(C->IsDominatedBy(B));
    EXPECT_TRUE(C->IsDominatedBy(A));
    EXPECT_TRUE(D->IsDominatedBy(B));
    EXPECT_TRUE(D->IsDominatedBy(A));
    EXPECT_TRUE(F->IsDominatedBy(B));
    EXPECT_TRUE(F->IsDominatedBy(A));
    EXPECT_TRUE(E->IsDominatedBy(F));
    EXPECT_TRUE(E->IsDominatedBy(B));
    EXPECT_TRUE(E->IsDominatedBy(A));
    EXPECT_TRUE(G->IsDominatedBy(F));
    EXPECT_TRUE(G->IsDominatedBy(B));
    EXPECT_TRUE(G->IsDominatedBy(A));

    IrBuilder->Cleanup();
}


TEST(dominator_tree, example2) {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction("Example2");

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

    // 1 edge:  A -> B
    IrBuilder->CreateJump(A, B);

    // 2 edges: B -> C and B -> J
    IrBuilder->CreateBeq(B, nullptr, nullptr, C, J);

    // 1 edge:  J -> C
    IrBuilder->CreateJump(J, C);

    // 1 edge:  C -> D
    IrBuilder->CreateJump(C, D);

    // 2 edges: D -> C and D -> E
    IrBuilder->CreateBeq(D, nullptr, nullptr, C, E);

    // 1 edge:  E -> F
    IrBuilder->CreateJump(E, F);

    // 2 edges: F -> E and F -> G
    IrBuilder->CreateBeq(F, nullptr, nullptr, E, G);

    // 2 edges: G -> H and G -> I
    IrBuilder->CreateBeq(G, nullptr, nullptr, H, I);

    // 1 edge:  H -> B
    IrBuilder->CreateJump(H, B);

    // 1 edge:  I -> K
    IrBuilder->CreateJump(I, K);

    // Total: 14 edges (according to example 2)

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    cfg->BuildDominatorTree();

    // Now check all dominance dependencies
    EXPECT_EQ(A->GetImmediateDominator(), nullptr);
    EXPECT_EQ(B->GetImmediateDominator(), A);
    EXPECT_EQ(C->GetImmediateDominator(), B);
    EXPECT_EQ(D->GetImmediateDominator(), C);
    EXPECT_EQ(E->GetImmediateDominator(), D);
    EXPECT_EQ(F->GetImmediateDominator(), E);
    EXPECT_EQ(G->GetImmediateDominator(), F);
    EXPECT_EQ(H->GetImmediateDominator(), G);
    EXPECT_EQ(I->GetImmediateDominator(), G);
    EXPECT_EQ(J->GetImmediateDominator(), B);
    EXPECT_EQ(K->GetImmediateDominator(), I);

    EXPECT_TRUE(B->IsDominatedBy(A));
    EXPECT_TRUE(C->IsDominatedBy(B));
    EXPECT_TRUE(C->IsDominatedBy(A));
    EXPECT_TRUE(D->IsDominatedBy(C));
    EXPECT_TRUE(D->IsDominatedBy(B));
    EXPECT_TRUE(D->IsDominatedBy(A));
    EXPECT_TRUE(E->IsDominatedBy(D));
    EXPECT_TRUE(E->IsDominatedBy(C));
    EXPECT_TRUE(E->IsDominatedBy(B));
    EXPECT_TRUE(E->IsDominatedBy(A));
    EXPECT_TRUE(F->IsDominatedBy(E));
    EXPECT_TRUE(F->IsDominatedBy(D));
    EXPECT_TRUE(F->IsDominatedBy(C));
    EXPECT_TRUE(F->IsDominatedBy(B));
    EXPECT_TRUE(F->IsDominatedBy(A));
    EXPECT_TRUE(G->IsDominatedBy(F));
    EXPECT_TRUE(G->IsDominatedBy(E));
    EXPECT_TRUE(G->IsDominatedBy(D));
    EXPECT_TRUE(G->IsDominatedBy(C));
    EXPECT_TRUE(G->IsDominatedBy(B));
    EXPECT_TRUE(G->IsDominatedBy(A));
    EXPECT_TRUE(H->IsDominatedBy(G));
    EXPECT_TRUE(H->IsDominatedBy(F));
    EXPECT_TRUE(H->IsDominatedBy(E));
    EXPECT_TRUE(H->IsDominatedBy(D));
    EXPECT_TRUE(H->IsDominatedBy(C));
    EXPECT_TRUE(H->IsDominatedBy(B));
    EXPECT_TRUE(H->IsDominatedBy(A));
    EXPECT_TRUE(I->IsDominatedBy(G));
    EXPECT_TRUE(I->IsDominatedBy(F));
    EXPECT_TRUE(I->IsDominatedBy(E));
    EXPECT_TRUE(I->IsDominatedBy(D));
    EXPECT_TRUE(I->IsDominatedBy(C));
    EXPECT_TRUE(I->IsDominatedBy(B));
    EXPECT_TRUE(I->IsDominatedBy(A));
    EXPECT_TRUE(J->IsDominatedBy(B));
    EXPECT_TRUE(J->IsDominatedBy(A));
    EXPECT_TRUE(I->IsDominatedBy(G));
    EXPECT_TRUE(K->IsDominatedBy(G));
    EXPECT_TRUE(K->IsDominatedBy(F));
    EXPECT_TRUE(K->IsDominatedBy(E));
    EXPECT_TRUE(K->IsDominatedBy(D));
    EXPECT_TRUE(K->IsDominatedBy(C));
    EXPECT_TRUE(K->IsDominatedBy(B));
    EXPECT_TRUE(K->IsDominatedBy(A));

    IrBuilder->Cleanup();
}


TEST(dominator_tree, example3) {
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
    VMIR::BasicBlock* I = IrBuilder->CreateBasicBlock(Func, "I");

    // Just create dummy graph. The fucntion will be invalid but we need only graph
    Func->SetEntryBasicBlock(A);

    // 1 edge:  A -> B
    IrBuilder->CreateJump(A, B);

    // 2 edges: B -> C and B -> E
    IrBuilder->CreateBeq(B, nullptr, nullptr, C, E);

    // 1 edge:  C -> D
    IrBuilder->CreateJump(C, D);

    // 2 edges: E -> D and E -> F
    IrBuilder->CreateBeq(E, nullptr, nullptr, D, F);

    // 1 edge:  D -> G
    IrBuilder->CreateJump(D, G);

    // 2 edges: F -> B and F -> H
    IrBuilder->CreateBeq(F, nullptr, nullptr, B, H);

    // 2 edges: G -> C and G -> I
    IrBuilder->CreateBeq(G, nullptr, nullptr, C, I);

    // 2 edges: H -> G and H -> I
    IrBuilder->CreateBeq(H, nullptr, nullptr, G, I);

    // Total: 13 edges (according to example 3)

    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Func);
    cfg->BuildDominatorTree();

    // Now check all dominance dependencies
    EXPECT_EQ(A->GetImmediateDominator(), nullptr);
    EXPECT_EQ(B->GetImmediateDominator(), A);
    EXPECT_EQ(C->GetImmediateDominator(), B);
    EXPECT_EQ(D->GetImmediateDominator(), B);
    EXPECT_EQ(E->GetImmediateDominator(), B);
    EXPECT_EQ(F->GetImmediateDominator(), E);
    EXPECT_EQ(G->GetImmediateDominator(), B);
    EXPECT_EQ(H->GetImmediateDominator(), F);
    EXPECT_EQ(I->GetImmediateDominator(), B);

    EXPECT_TRUE(B->IsDominatedBy(A));
    EXPECT_TRUE(C->IsDominatedBy(B));
    EXPECT_TRUE(C->IsDominatedBy(A));
    EXPECT_TRUE(D->IsDominatedBy(B));
    EXPECT_TRUE(D->IsDominatedBy(A));
    EXPECT_TRUE(E->IsDominatedBy(B));
    EXPECT_TRUE(E->IsDominatedBy(A));
    EXPECT_TRUE(F->IsDominatedBy(E));
    EXPECT_TRUE(F->IsDominatedBy(B));
    EXPECT_TRUE(F->IsDominatedBy(A));
    EXPECT_TRUE(G->IsDominatedBy(B));
    EXPECT_TRUE(G->IsDominatedBy(A));
    EXPECT_TRUE(H->IsDominatedBy(F));
    EXPECT_TRUE(H->IsDominatedBy(E));
    EXPECT_TRUE(H->IsDominatedBy(B));
    EXPECT_TRUE(H->IsDominatedBy(A));
    EXPECT_TRUE(I->IsDominatedBy(B));
    EXPECT_TRUE(I->IsDominatedBy(A));

    IrBuilder->Cleanup();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}