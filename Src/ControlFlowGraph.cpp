#include <fstream>
#include <unordered_map>
#include <algorithm>

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
    // Run DFS to check all reachable blocks from entry basic block
    DFS dfs{};
    dfs.Run(mEntry);
    dfs.UnmarkAll();
    auto& allReachableBlocks = mEntry->GetDominatedBasicBlocks();
    auto& dfsVec = dfs.GetBasicBlocks();

    allReachableBlocks = std::move(std::set<BasicBlock*>(dfsVec.begin(), dfsVec.end()));
    dfsVec.clear();

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

        auto& allDominated = bb->GetDominatedBasicBlocks();

        std::set<BasicBlock*> reachable(dfsVec.begin(), dfsVec.end());
        std::set_difference(allReachableBlocks.begin(),
                            allReachableBlocks.end(),
                            reachable.begin(),
                            reachable.end(),
                            std::inserter(allDominated, allDominated.end()));
        dfsVec.clear();
    }

    // By this moment we have a set of dominated vertices for every vertex in the graph
    // But we need immediately dominated vertices to build a dominator tree
    // So, remove dominated vertices which are dominated by another dominated vertex
    for (auto* bb : mGraph) {
        const auto& allDominated = bb->GetDominatedBasicBlocks();
        std::set<BasicBlock*> immDominated = allDominated;
        immDominated.erase(bb);
        for (auto* dominatedBB : allDominated) {
            if (dominatedBB == bb) {
                continue;
            }

            const auto& dominatedBBAllDominated = dominatedBB->GetDominatedBasicBlocks();
            for (auto* dominatedBBDominatedBB : dominatedBBAllDominated) {
                if (dominatedBBDominatedBB == dominatedBB) {
                    continue;
                }
                immDominated.erase(dominatedBBDominatedBB);
            }
        }

        for (auto* dominatedBB : immDominated) {
            dominatedBB->SetImmediateDominator(bb);
        }
    }

    mDomTreeBuilt = true;
}

}   // namespace VMIR
