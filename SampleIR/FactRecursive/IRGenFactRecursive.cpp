#include <iostream>

#include <IRBuilder.h>


int main() {
    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Fact = IrBuilder->CreateFunction(VMIR::ValueType::Int32, {VMIR::ValueType::Int32}, "Fact");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Fact, "BB_1");
    VMIR::BasicBlock* BB_2 = IrBuilder->CreateBasicBlock(Fact, "BB_2");
    VMIR::BasicBlock* BB_3 = IrBuilder->CreateBasicBlock(Fact, "BB_3");
    VMIR::BasicBlock* BB_4 = IrBuilder->CreateBasicBlock(Fact, "BB_4");

    Fact->SetEntryBasicBlock(BB_1);

    VMIR::Value* zero = IrBuilder->CreateValue(0);
    VMIR::Value* one  = IrBuilder->CreateValue(1);

    VMIR::Value* v0 = Fact->GetArg(0);
    VMIR::Value* v1 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v2 = IrBuilder->CreateValue(VMIR::ValueType::Int32);
    VMIR::Value* v3 = IrBuilder->CreateValue(VMIR::ValueType::Int32);

    // BB_1
    IrBuilder->CreateBeq(BB_1, v0, zero, BB_3, BB_2);

    // BB_2
    IrBuilder->CreateBeq(BB_2, v0, one, BB_3, BB_4);

    // BB_3
    IrBuilder->CreateRet(BB_3, one);

    // BB_4
    IrBuilder->CreateSub(BB_4, v0, one, v1);
    IrBuilder->CreateCall(BB_4, Fact, v2, {v1});
    IrBuilder->CreateMul(BB_4, v0, v2, v3);
    IrBuilder->CreateRet(BB_4, v3);

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
    VMIR::LoopAnalyzer* loopAnalyzer = IrBuilder->CreateLoopAnalyzer(cfg);

    cfg->BuildDominatorTree();
    loopAnalyzer->BuildLoopTree();

    cfg->GenerateDotFileCFG("FactRecursiveCFG");
    cfg->GenerateDotFileDomTree("FactRecursiveDomTree");
    loopAnalyzer->GenerateDotFileLoopTree("FactRecursiveLoopTree");

    return 0;
}