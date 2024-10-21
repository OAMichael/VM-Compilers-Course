#ifndef LOOP_H
#define LOOP_H

#include <set>

#include <BasicBlock.h>

namespace VMIR {

class Loop {
public:
    Loop(BasicBlock* header = nullptr) : mHeader{header} {};

    inline BasicBlock* GetHeader() const { return mHeader; }
    inline void SetHeader(BasicBlock* header) { mHeader = header; }

    inline const std::vector<BasicBlock*>& GetBasicBlocks() const { return mBasicBlocks; }
    inline std::vector<BasicBlock*>& GetBasicBlocks() { return mBasicBlocks; }

    inline const std::vector<BasicBlock*>& GetLatches() const { return mLatches; }
    inline std::vector<BasicBlock*>& GetLatches() { return mLatches; }

    inline bool IsReducible() const { return mIsReducible; }
    inline void SetReducible(bool reduce = true) { mIsReducible = reduce; }

    inline Loop* GetOuterLoop() const { return mOuterLoop; }
    inline void SetOuterLoop(Loop* outer) { mOuterLoop = outer; }

    inline const std::set<Loop*>& GetInnerLoops() const { return mInnerLoops; }
    inline std::set<Loop*>& GetInnerLoops() { return mInnerLoops; }
    inline void InsertInnerLoop(Loop* inner) { mInnerLoops.insert(inner); }
    inline void RemoveInnerLoop(Loop* inner) { mInnerLoops.erase(inner);  }

private:
    BasicBlock* mHeader{nullptr};               // Header of the loop
    std::vector<BasicBlock*> mBasicBlocks{};    // All basic blocks of the loop (including the header and all latches, but excluding inner loops' blocks)
    std::vector<BasicBlock*> mLatches{};        // Only latches of the loop
    bool mIsReducible{false};

    Loop* mOuterLoop{nullptr};
    std::set<Loop*> mInnerLoops{};
};

}   // namespace Loop

#endif  // LOOP_H
