#include <CheckEliminationPass.h>
#include <ControlFlowGraph.h>
#include <IRBuilder.h>

#include <unordered_set>

namespace VMIR {

static const char CHECK_ELIMINATION_PASS_NAME[] = "Check Elimination Pass";

CheckEliminationPass::CheckEliminationPass() : Pass(CHECK_ELIMINATION_PASS_NAME) {}

void CheckEliminationPass::Run(Function* func) {
    IRBuilder* IrBuilder = IRBuilder::GetInstance();

    ControlFlowGraph* cfg = IrBuilder->GetOrCreateControlFlowGraph(func);
    cfg->BuildDominatorTree();

    size_t bbCount = cfg->GetBasicBlocks().size();

    RPO rpo{};
    rpo.Run(cfg->GetEntryBasicBlock(), &bbCount);
    rpo.UnmarkAll();

    auto& rpoBasicBlocks = rpo.GetBasicBlocks();

    for (auto* bb : rpoBasicBlocks) {
        Instruction* inst = bb->Front();
        while (inst) {
            if (inst->GetType() == InstructionType::NullCheck) {
                InstructionNullCheck* instNullCheck = static_cast<InstructionNullCheck*>(inst);

                Value* input = instNullCheck->GetInput();
                std::vector<Instruction*> dominatedNullChecks;
                for (auto* user : input->GetUsers()) {
                    if (user == instNullCheck) {
                        continue;
                    }
                    if (user->GetType() != InstructionType::NullCheck) {
                        continue;
                    }
                    if (user->IsDominatedBy(instNullCheck)) {
                        dominatedNullChecks.push_back(user);
                    }
                }

                for (auto* dominatedNullCheck : dominatedNullChecks) {
                    auto* checkBB = dominatedNullCheck->GetParentBasicBlock();

                    input->RemoveUser(dominatedNullCheck);
                    checkBB->RemoveInstruction(dominatedNullCheck);
                    IrBuilder->RemoveInstruction(dominatedNullCheck);
                }
            }
            else if (inst->GetType() == InstructionType::BoundsCheck) {
                InstructionBoundsCheck* instBoundsCheck = static_cast<InstructionBoundsCheck*>(inst);

                Value* inputPtr = instBoundsCheck->GetInputPtr();
                Value* inputArray = instBoundsCheck->GetInputArray();
                std::vector<Instruction*> dominatedBoundsChecks;
                for (auto* user : inputArray->GetUsers()) {
                    if (user == instBoundsCheck) {
                        continue;
                    }
                    if (user->GetType() != InstructionType::BoundsCheck) {
                        continue;
                    }

                    InstructionBoundsCheck* userBoundsCheck = static_cast<InstructionBoundsCheck*>(user);
                    if (userBoundsCheck->GetInputPtr() != inputPtr) {
                        continue;
                    }
                    if (userBoundsCheck->IsDominatedBy(instBoundsCheck)) {
                        dominatedBoundsChecks.push_back(user);
                    }
                }

                for (auto* dominatedBoundsCheck : dominatedBoundsChecks) {
                    auto* checkBB = dominatedBoundsCheck->GetParentBasicBlock();

                    inputPtr->RemoveUser(dominatedBoundsCheck);
                    inputArray->RemoveUser(dominatedBoundsCheck);
                    checkBB->RemoveInstruction(dominatedBoundsCheck);
                    IrBuilder->RemoveInstruction(dominatedBoundsCheck);
                }
            }
            inst = inst->GetNext();
        }
    }
}

}   // namespace VMIR
