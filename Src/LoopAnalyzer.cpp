#include <functional>
#include <fstream>

#include <LoopAnalyzer.h>

namespace VMIR {

bool LoopAnalyzer::GenerateDotFileLoopTree(const std::string& filename) {
    std::ofstream out(filename + ".dot", std::ofstream::out);
    if (!out.is_open()) {
        return false;
    }

    out << "digraph G {\n";
    out << "    graph[color=\"#242038\"]\n";
    out << "    node[color=\"#242038\", shape=square]\n";
    out << "    edge[color=\"#242038\"]\n";

    out << "    Root [label=<<font point-size=\"24.0\"> Root </font>>]\n";
    for (const auto& l : mLoops) {
        auto* loop = l.second;
        std::string loopName = loop->GetHeader()->GetName();
        out << "    " << loopName << " [label=<<font point-size=\"24.0\"> " << loopName << " </font><br/> [";
        for (size_t i = 0; i < loop->GetBasicBlocks().size(); ++i) {
            if (i > 0) {
                out << ", ";
            }
            out << loop->GetBasicBlocks()[i]->GetName();
        }
        out << "] <br/> Reducible: " << std::boolalpha << loop->IsReducible() << ">]\n";
    }

    for (auto& pair : mLoops) {
        auto* loop = pair.second;
        if (loop->GetOuterLoop() == mRootLoop) {
            out << "    Root->" << loop->GetHeader()->GetName() << "\n";
        }
        else {
            out << "    " << loop->GetOuterLoop()->GetHeader()->GetName() << "->" << loop->GetHeader()->GetName() << "\n";
        }
    }

    out << "}\n";

    out.close();
    return true;
}

void LoopAnalyzer::BuildLoopTree() {
    if (mGraph == nullptr) {
        return;
    }

    if (!mGraph->IsDominatorTreeBuilt()) {
        mGraph->BuildDominatorTree();
    }

    // Collect back edges
    BasicBlock* entry = mGraph->GetEntryBasicBlock();
    auto& allBasicBlocks = mGraph->GetBasicBlocks();
    DFSBlackAndGrey(entry);

    for (auto* bb : allBasicBlocks) {
        bb->SetUnmarked();
    }

    // Populate loops
    size_t bbCount = allBasicBlocks.size();

    RPO rpo{};
    rpo.Run(entry, &bbCount);
    rpo.UnmarkAll();
    auto& rpoVec = rpo.GetBasicBlocks();

    for (auto it = rpoVec.rbegin(), end = rpoVec.rend(); it != end; ++it) {
        auto loopIt = mLoops.find(*it);
        if (loopIt == mLoops.end()) {
            continue;
        }

        Loop* loop = loopIt->second;
        BasicBlock* loopHeader = loop->GetHeader();
        auto& loopBlocks = loop->GetBasicBlocks();

        loopBlocks.push_back(loopHeader);
        loopHeader->SetLoop(loop);

        if (!loop->IsReducible()) {
            // For irreducible loops append all source of back edges in the loop
            auto& latches = loop->GetLatches();
            auto& loopBody = loop->GetBasicBlocks();
            for (auto* bb : latches) {
                loopBody.push_back(bb);
            }
        }
        else {
            // For reducible loops for all back edges run loop search
            auto& latches = loop->GetLatches();
            for (auto* latch : latches) {
                LoopSearch(latch, loop);
            }
        }
    }

    // Create Root loop (and thus, build loop tree)
    mRootLoop = new Loop();

    auto& rootLoopBB = mRootLoop->GetBasicBlocks();
    for (auto* bb : allBasicBlocks) {
        if (bb->GetLoop() == nullptr) {
            bb->SetLoop(mRootLoop);
            rootLoopBB.push_back(bb);
        }
    }
    for (auto& l : mLoops) {
        if (l.second == mRootLoop) {
            continue;
        }
        if (l.second->GetOuterLoop() == nullptr) {
            l.second->SetOuterLoop(mRootLoop);
            mRootLoop->InsertInnerLoop(l.second);
        }
    }

    mIsLoopTreeBuilt = true;
}

void LoopAnalyzer::DFSBlackAndGrey(BasicBlock* entryBB) {
    entryBB->SetMarked(BasicBlock::Marker::Grey | BasicBlock::Marker::Black);
    for (auto* succ : entryBB->GetSuccessors()) {
        if (succ->IsMarked(BasicBlock::Marker::Grey)) {
            // This is back edge:
            // - entryBB is loop latch
            // - succ is loop header
            Loop* loop = nullptr;
            auto it = mLoops.find(succ);
            if (it == mLoops.end()) {
                loop = CreateLoop(succ);
            }
            else {
                loop = it->second;
            }

            loop->SetHeader(succ);
            loop->GetLatches().push_back(entryBB);
            loop->SetReducible(loop->IsReducible() && succ->IsDominatorOf(entryBB));
            continue;
        }
        else if (!succ->IsMarked(BasicBlock::Marker::Black)) {
            DFSBlackAndGrey(succ);
        }
    }
    entryBB->SetUnmarked(BasicBlock::Marker::Grey);
}

void LoopAnalyzer::LoopSearch(BasicBlock* latch, Loop* loop) {
    // This time we can use almost default DFS (just different marker and we iterate over predecessors)
    DFS dfs{};
    dfs.SetMarker(BasicBlock::Marker::Green);
    dfs.SetReverse();

    BasicBlock* loopHeader = loop->GetHeader();
    auto& loopBlocks = loop->GetBasicBlocks();

    loopHeader->SetMarked(BasicBlock::Marker::Green);
    dfs.Run(latch);
    for (auto* bb : dfs.GetBasicBlocks()) {
        Loop* bbLoop = bb->GetLoop();
        if (bbLoop == nullptr) {
            // If block without loop: add to the loop
            bb->SetLoop(loop);
            loopBlocks.push_back(bb);
        }
        else if (bbLoop != loop) {
            // If block in other loop (inner): link outer and inner loops
            loop->InsertInnerLoop(bbLoop);

            Loop* outermostLoop = bbLoop->GetOuterLoop();
            if (outermostLoop != nullptr) {
                while (outermostLoop->GetOuterLoop() != nullptr) {
                    outermostLoop = outermostLoop->GetOuterLoop();
                }
                if (outermostLoop != loop) {
                    outermostLoop->SetOuterLoop(loop);
                }
            }
            else {
                bbLoop->SetOuterLoop(loop);
            }
        }
    }
    dfs.UnmarkAll();
    loopHeader->SetUnmarked(BasicBlock::Marker::Green);
}

}   // namespace VMIR
