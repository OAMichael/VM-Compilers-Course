#include <fstream>

#include <ControlFlowGraph.h>

namespace VMIR {

bool ControlFlowGraph::GenerateDotFile(const std::string& filename) {
    std::ofstream out(filename + ".dot", std::ofstream::out);
    if (!out.is_open()) {
        return false;
    }

    out << "digraph G {\n";
    out << "    graph[color=\"#242038\"]\n";
    out << "    node[color=\"#242038\", shape=square]\n";
    out << "    edge[color=\"#242038\"]\n";

    for (const auto* bb : mGraph) {
        const auto& succs = bb->GetSuccessors();
        for (const auto* succ : succs) {
            out << "    " << bb->GetName() + "->" + succ->GetName() << "\n";
        }
    }

    out << "}\n";

    out.close();
    return true;
}

}   // namespace VMIR
