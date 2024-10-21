#ifndef CONTROL_FLOW_GRAPH_H
#define CONTROL_FLOW_GRAPH_H

#include <BasicBlock.h>
#include <Function.h>

namespace VMIR {

class DFS {
public:
    void Run(BasicBlock* entryBB, BasicBlock* ignoredBB = nullptr);

    inline std::vector<BasicBlock*>& GetBasicBlocks() { return mDFSVector; }
    inline BasicBlock::Marker GetMarker() const { return mMarker; }
    inline void SetMarker(const BasicBlock::Marker marker) { mMarker = marker; }
    inline void Clear() { mDFSVector.clear(); }
    inline void SetReverse(bool reverse = true) { mReverse = reverse; }
    void UnmarkAll();

private:
    void DFSInternal(BasicBlock* block, BasicBlock* ignoredBB);

    std::vector<BasicBlock*> mDFSVector{};
    BasicBlock::Marker mMarker = BasicBlock::Marker::All;
    bool mReverse{false};
};


class RPO {
public:
    void Run(BasicBlock* entryBB, size_t* pCount, BasicBlock* ignoredBB = nullptr);

    inline std::vector<BasicBlock*>& GetBasicBlocks() { return mRPOVector; }
    inline BasicBlock::Marker GetMarker() const { return mMarker; }
    inline void SetMarker(const BasicBlock::Marker marker) { mMarker = marker; }
    inline void Clear() { mRPOVector.clear(); }
    void UnmarkAll();

private:
    void RPOInternal(BasicBlock* entryBB, size_t* pCount, BasicBlock* ignoredBB);

    std::vector<BasicBlock*> mRPOVector{};
    BasicBlock::Marker mMarker = BasicBlock::Marker::All;
};


class ControlFlowGraph {
public:
    ControlFlowGraph() = delete;
    ControlFlowGraph(const Function* function);
    ControlFlowGraph(const std::vector<BasicBlock*> basicBlocks);
    
    inline const std::vector<BasicBlock*>& GetBasicBlocks() const { return mGraph; }
    inline BasicBlock* GetEntryBasicBlock() const { return mEntry; }

    bool GenerateDotFileCFG(const std::string& filename);
    bool GenerateDotFileDomTree(const std::string& filename);

    void BuildDominatorTree();
    inline bool IsDominatorTreeBuilt() const { return mDomTreeBuilt; }

private:
    // Each basic block contains its predecessors and successors
    // So, we store only basic blocks
    std::vector<BasicBlock*> mGraph{};
    BasicBlock* mEntry{};
    bool mDomTreeBuilt{false};
};

}   // namespace VMIR

#endif  // CONTROL_FLOW_GRAPH_H
