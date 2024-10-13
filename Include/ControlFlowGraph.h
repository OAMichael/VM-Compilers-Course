#ifndef CONTROL_FLOW_GRAPH_H
#define CONTROL_FLOW_GRAPH_H

#include <BasicBlock.h>
#include <Function.h>

namespace VMIR {

class DFS {
public:
    void Run(BasicBlock* entryBB, BasicBlock* ignoredBB = nullptr);

    inline std::vector<BasicBlock*>& GetBasicBlocks() { return mDFSVector; }
    inline void Clear() { mDFSVector.clear(); }
    void UnmarkAll();

private:
    void DFSInternal(BasicBlock* block, BasicBlock* ignoredBB);

    std::vector<BasicBlock*> mDFSVector{};
};


class RPO {
public:
    void Run(BasicBlock* entryBB, size_t* pCount, BasicBlock* ignoredBB = nullptr);

    inline std::vector<BasicBlock*>& GetBasicBlocks() { return mRPOVector; }
    inline void Clear() { mRPOVector.clear(); }
    void UnmarkAll();

private:
    void RPOInternal(BasicBlock* entryBB, size_t* pCount, BasicBlock* ignoredBB);

    std::vector<BasicBlock*> mRPOVector{};
};


class ControlFlowGraph {
public:
    ControlFlowGraph() = delete;
    ControlFlowGraph(const Function* function);
    ControlFlowGraph(const std::vector<BasicBlock*> basicBlocks);
    
    inline const std::vector<BasicBlock*>& GetBasicBlocks() const { return mGraph; }

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
