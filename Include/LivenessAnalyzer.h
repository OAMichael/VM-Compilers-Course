#ifndef LIVENESS_ANALYZER_H
#define LIVENESS_ANALYZER_H

#include <ControlFlowGraph.h>
#include <BasicBlock.h>
#include <LoopAnalyzer.h>

namespace VMIR {

class LivenessAnalyzer {
public:
    LivenessAnalyzer() = delete;
    LivenessAnalyzer(ControlFlowGraph* graph) : mGraph{graph} {}

    bool PerformLivenessAnalysis();

    inline ControlFlowGraph* GetGraph() const { return mGraph; }
    inline const std::vector<BasicBlock*>& GetBasicBlocksLinearOrder() const { return mBBLinearOrder; }
    inline LoopAnalyzer* GetLoopAnalyzer() const { return mLoopAnalyzer; }

private:
    void CreateBasicBlocksLinearOrder(BasicBlock* entry);
    void AssignLinearAndLiveNumbers();
    void CalculateLiveRanges();

    bool CheckIfBlockCanBeVisited(BasicBlock* bb);

    void VisitLoopBlock(BasicBlock* bb, std::set<BasicBlock*>& exitBlocks);
    void VisitLoop(Loop* loop, std::set<BasicBlock*>& exitBlocks);

    ControlFlowGraph* mGraph{nullptr};
    LoopAnalyzer* mLoopAnalyzer{nullptr};
    std::vector<BasicBlock*> mBBLinearOrder;
};

}   // namespace VMIR

#endif  // LIVENESS_ANALYZER_H
