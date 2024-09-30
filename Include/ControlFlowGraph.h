#ifndef CONTROL_FLOW_GRAPH_H
#define CONTROL_FLOW_GRAPH_H

#include <BasicBlock.h>
#include <Function.h>

namespace VMIR {

class ControlFlowGraph {
public:
    ControlFlowGraph() = delete;
    ControlFlowGraph(const Function* function) : mGraph{function->GetBasicBlocks()} {};
    ControlFlowGraph(const std::vector<BasicBlock*> basicBlocks) : mGraph{basicBlocks} {};
    
    inline const std::vector<BasicBlock*>& GetBasicBlocks() const { return mGraph; }
    inline void AppendBasicBlock(BasicBlock* basicBlock) { mGraph.push_back(basicBlock); }

    bool GenerateDotFile(const std::string& filename);

private:
    // Each basic block contains its predecessors and successors
    // So, we store only basic blocks
    std::vector<BasicBlock*> mGraph{};
};

}   // namespace VMIR

#endif  // CONTROL_FLOW_GRAPH_H
