#ifndef LOOP_ANALYZER_H
#define LOOP_ANALYZER_H

#include <unordered_map>

#include <ControlFlowGraph.h>
#include <BasicBlock.h>
#include <Loop.h>

namespace VMIR {

class LoopAnalyzer {
public:
    LoopAnalyzer() = delete;
    LoopAnalyzer(ControlFlowGraph* graph) : mGraph{graph} {}

    ~LoopAnalyzer() { Cleanup(); }

    inline Loop* CreateLoop(BasicBlock* header) {
        Loop* loop = new Loop(header);
        mLoops.insert({header, loop});
        return loop;
    }

    inline ControlFlowGraph* GetGraph() const { return mGraph; }
    inline const std::unordered_map<BasicBlock*, Loop*>& GetLoops() const { return mLoops; }
    inline Loop* GetRootLoop() const { return mRootLoop; } 

    bool GenerateDotFileLoopTree(const std::string& filename);

    void BuildLoopTree();
    inline bool IsLoopTreeBuilt() const { return mIsLoopTreeBuilt; }

    inline bool HasIrreducibleLoops() const {
        for (const auto& ll : mLoops) {
            if (!ll.second->IsReducible()) {
                return true;
            }
        }
        return false;
    }

    inline void Cleanup() {
        for (const auto& l : mLoops) {
            delete l.second;
        }
        delete mRootLoop;

        mLoops.clear();
        mRootLoop = nullptr;
    }

private:
    // Sadly, this time we need slightly different DFS. So, define a new one
    void DFSBlackAndGrey(BasicBlock* entryBB);

    void LoopSearch(BasicBlock* latch, Loop* loop);

    ControlFlowGraph* mGraph{nullptr};
    std::unordered_map<BasicBlock*, Loop*> mLoops{};    // All loops except root loop
    Loop* mRootLoop{nullptr};

    bool mIsLoopTreeBuilt{false};
};

}   // namespace VMIR

#endif  // LOOP_ANALYZER_H
