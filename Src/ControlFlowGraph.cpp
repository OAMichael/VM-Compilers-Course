#include <fstream>
#include <unordered_map>

#include <ControlFlowGraph.h>

namespace VMIR {

void DFS::Run(BasicBlock* entryBB, BasicBlock* ignoredBB) {
    DFSInternal(entryBB, ignoredBB);
}

void DFS::UnmarkAll() {
    for (auto* bb : mDFSVector) {
        bb->SetMarked(false);
    }
}

void DFS::DFSInternal(BasicBlock* block, BasicBlock* ignoredBB) {
    if (block == ignoredBB) {
        return;
    }

    block->SetMarked();
    mDFSVector.push_back(block);

    auto* trueSucc = block->GetTrueSuccessor();
    if (trueSucc && !trueSucc->IsMarked()) {
        DFSInternal(trueSucc, ignoredBB);
    }

    auto* falseSucc = block->GetFalseSuccessor();
    if (falseSucc && !falseSucc->IsMarked()) {
        DFSInternal(falseSucc, ignoredBB);
    }
}


void RPO::Run(BasicBlock* entryBB, size_t* pCount, BasicBlock* ignoredBB) {
    mRPOVector.resize(*pCount);
    RPOInternal(entryBB, pCount, ignoredBB);
}

void RPO::UnmarkAll() {
    for (auto* bb : mRPOVector) {
        bb->SetMarked(false);
    }
}

void RPO::RPOInternal(BasicBlock* block, size_t* pCount, BasicBlock* ignoredBB) {
    if (block == ignoredBB) {
        return;
    }

    block->SetMarked();

    auto* trueSucc = block->GetTrueSuccessor();
    if (trueSucc && !trueSucc->IsMarked()) {
        RPOInternal(trueSucc, pCount, ignoredBB);
    }

    auto* falseSucc = block->GetFalseSuccessor();
    if (falseSucc && !falseSucc->IsMarked()) {
        RPOInternal(falseSucc, pCount, ignoredBB);
    }

    mRPOVector[--(*pCount)] = block;
}


ControlFlowGraph::ControlFlowGraph(const Function* function) {
    if (!function) {
        return;
    }

    // Copy
    mGraph = function->GetBasicBlocks();
    mEntry = function->GetEntryBasicBlock();
}

ControlFlowGraph::ControlFlowGraph(const std::vector<BasicBlock*> basicBlocks) {
    // Copy
    mGraph = basicBlocks;
    if (basicBlocks.size() > 0) {
        mEntry = basicBlocks[0];
    }
}

bool ControlFlowGraph::GenerateDotFileCFG(const std::string& filename) {
    std::ofstream out(filename + ".dot", std::ofstream::out);
    if (!out.is_open()) {
        return false;
    }

    out << "digraph G {\n";
    out << "    graph[color=\"#242038\"]\n";
    out << "    node[color=\"#242038\", shape=square]\n";
    out << "    edge[color=\"#242038\"]\n";

    for (const auto* bb : mGraph) {
        const auto& preds = bb->GetPredecessors();
        for (const auto* pred : preds) {
            out << "    " << pred->GetName() << "->" << bb->GetName() << "\n";
        }
    }

    out << "}\n";

    out.close();
    return true;
}

bool ControlFlowGraph::GenerateDotFileDomTree(const std::string& filename) {
    std::ofstream out(filename + ".dot", std::ofstream::out);
    if (!out.is_open()) {
        return false;
    }

    out << "digraph G {\n";
    out << "    graph[color=\"#242038\"]\n";
    out << "    node[color=\"#242038\", shape=square]\n";
    out << "    edge[color=\"#242038\"]\n";

    for (const auto* bb : mGraph) {
        const auto* idom = bb->GetImmediateDominator();
        if (idom != nullptr) {
            out << "    " << idom->GetName() << "->" << bb->GetName() << "\n";
        }
    }

    out << "}\n";

    out.close();
    return true;
}

void ControlFlowGraph::BuildDominatorTree() {
    // Key:     basic block
    // Value:   list of all blocks dominated by key block (i.e. key = dom(value))
    std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> dominatedBlocks;

    // Run DFS to check all reachable blocks from entry basic block
    DFS dfs{};
    dfs.Run(mEntry);
    dfs.UnmarkAll();
    dominatedBlocks.insert({mEntry, std::move(dfs.GetBasicBlocks())});
    std::vector<BasicBlock*>& allReachableBlocks = dominatedBlocks[mEntry];

    // Now, for every vertex != entry:
    // - Remove this vertex from the graph (dfs.Run(mEntry, bb) means do DFS but ignore bb)
    // - Obtain all reachable vertices from entry vertex without removed vertex
    // - The difference between all reachable vertices and reachable vertices without removed one
    // is a set of vertices dominated by removed vertex
    for (auto* bb : mGraph) {
        if (bb == mEntry) {
            continue;
        }
        dfs.Run(mEntry, bb);
        dfs.UnmarkAll();

        std::vector<BasicBlock*> reachable = std::move(dfs.GetBasicBlocks());
        std::vector<BasicBlock*> dominated{};
        for (auto* rb : allReachableBlocks) {
            auto it = std::find(reachable.begin(), reachable.end(), rb);
            if (it == reachable.end()) {
                dominated.push_back(rb);
            }
        }
        dominatedBlocks.insert({bb, std::move(dominated)});
    }

    // By this moment we have list of dominated vertices for every vertex in the graph
    // But we need immediately dominated vertices to build a dominator tree
    // So, remove dominated vertices which are dominated by another dominated vertex
    for (auto& bbDoms : dominatedBlocks) {
        auto* bb = bbDoms.first;
        const auto& allDominated = bbDoms.second;
        std::vector<BasicBlock*> immDominated = allDominated;

        for (size_t i = 0; i < immDominated.size(); ++i) {
            if (immDominated[i] == nullptr) {
                continue;
            }

            if (immDominated[i] == bb) {
                immDominated[i] = nullptr;
                continue;
            }

            auto* dominatedBB = immDominated[i];
            const auto& dominatedBBAllDominated = dominatedBlocks[dominatedBB];
            for (size_t j = 0; j < dominatedBBAllDominated.size(); ++j) {
                auto* dominatedBBDominatedBB = dominatedBBAllDominated[j];
                if (immDominated[i] == dominatedBBDominatedBB) {
                    continue;
                }

                auto domIt = std::find(immDominated.begin(), immDominated.end(), dominatedBBDominatedBB);
                if (domIt != immDominated.end()) {
                    *domIt = nullptr;
                }
            }
        }

        std::erase_if(immDominated, [](BasicBlock* bb) { return bb == nullptr; });
        for (auto* immDominatedBB : immDominated) {
            if (immDominatedBB != nullptr) {
                immDominatedBB->SetImmediateDominator(bb);
            }
        }
    }

    mDomTreeBuilt = true;
}

}   // namespace VMIR
