#include <iostream>

#include <IRBuilder.h>


int main() {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Fact = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, {VMIR::ValueType::Uint64}, "Fact");

    VMIR::BasicBlock* EntryBB           = IrBuilder->CreateBasicBlock(Fact, "Entry");
    VMIR::BasicBlock* LoopPreheaderBB   = IrBuilder->CreateBasicBlock(Fact, "LoopPreheader");
    VMIR::BasicBlock* LoopHeaderBB      = IrBuilder->CreateBasicBlock(Fact, "LoopHeader");
    VMIR::BasicBlock* LoopBodyBB        = IrBuilder->CreateBasicBlock(Fact, "LoopBody");
    VMIR::BasicBlock* LoopExitBB        = IrBuilder->CreateBasicBlock(Fact, "LoopExit");

    Fact->SetEntryBasicBlock(EntryBB);

    VMIR::Value* zero = IrBuilder->CreateValue(Fact, 0UL);
    VMIR::Value* one  = IrBuilder->CreateValue(Fact, 1UL);
    VMIR::Value* two  = IrBuilder->CreateValue(Fact, 2UL);

    VMIR::Value* v0 = Fact->GetArg(0);
    VMIR::Value* v1 = IrBuilder->CreateValue(Fact, VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(Fact, VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(Fact, VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(Fact, VMIR::ValueType::Uint64);
    VMIR::Value* v5 = IrBuilder->CreateValue(Fact, VMIR::ValueType::Uint64);
    VMIR::Value* v6 = IrBuilder->CreateValue(Fact, VMIR::ValueType::Uint64);
    VMIR::Value* v7 = IrBuilder->CreateValue(Fact, VMIR::ValueType::Uint64);

    // EntryBB
    IrBuilder->CreateAdd(EntryBB, zero, one, v1);
    IrBuilder->CreateJump(EntryBB, LoopPreheaderBB);

    // LoopPreheaderBB
    IrBuilder->CreateAdd(LoopPreheaderBB, zero, two, v2);
    IrBuilder->CreateJump(LoopPreheaderBB, LoopHeaderBB);

    // LoopHeaderBB
    IrBuilder->CreatePhi(LoopHeaderBB, {v2, v6}, v3);
    IrBuilder->CreateBgt(LoopHeaderBB, v3, v0, LoopExitBB, LoopBodyBB);

    // LoopBodyBB
    IrBuilder->CreatePhi(LoopBodyBB, {v1, v5}, v4);
    IrBuilder->CreateMul(LoopBodyBB, v4, v3, v5);
    IrBuilder->CreateAdd(LoopBodyBB, v3, one, v6);
    IrBuilder->CreateJump(LoopBodyBB, LoopHeaderBB);

    // LoopExitBB
    IrBuilder->CreatePhi(LoopExitBB, {v1, v5}, v7);
    IrBuilder->CreateRet(LoopExitBB, v7);

    // Check validity
    if (!Fact->IsValid()) {
        std::cerr << "Function \"" << Fact->GetName() << "\" is invalid" << std::endl;
        return -1;
    }

    // Print IR
    Fact->Print(std::cout);
    std::cout << "\n";

    // Create Control Flow Graph
    VMIR::ControlFlowGraph* cfg = IrBuilder->CreateControlFlowGraph(Fact);
    cfg->GenerateDotFileCFG("FactLoopCFG");
    cfg->BuildDominatorTree();
    cfg->GenerateDotFileDomTree("FactLoopDomTree");

    return 0;
}