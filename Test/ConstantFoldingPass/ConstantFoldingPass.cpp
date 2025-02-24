#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <IRBuilder.h>
#include <ConstantFoldingPass.h>


TEST(constant_folding_pass, mv_add) {
    /*
        BB_1:
            v0 = Mv ui64 97
            v1 = Mv ui64 87
            v2 = Add ui64 v0, v1
            v3 = Add ui64 v0, 314
            v4 = Add ui64 v2, v3
            Ret ui64 v4
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint64, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* val_97  = IrBuilder->CreateValue(Func, 97UL);
    VMIR::Value* val_87  = IrBuilder->CreateValue(Func, 87UL);
    VMIR::Value* val_314 = IrBuilder->CreateValue(Func, 314UL);

    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);
    VMIR::Value* v4 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint64);

    IrBuilder->CreateMv(BB_1, val_97, v0);
    IrBuilder->CreateMv(BB_1, val_87, v1);
    IrBuilder->CreateAdd(BB_1, v0, v1, v2);
    IrBuilder->CreateAdd(BB_1, v0, val_314, v3);
    IrBuilder->CreateAdd(BB_1, v2, v3, v4);
    IrBuilder->CreateRet(BB_1, v4);

    VMIR::ConstantFoldingPass constantFoldingPass{};

    IrBuilder->PrintIR(std::cout);
    constantFoldingPass.Run(Func);
    IrBuilder->PrintIR(std::cout);

    IrBuilder->Cleanup();
}


TEST(constant_folding_pass, multiple) {
    /*
        BB_1:
            v0 = Mv ui32 45
            v1 = Mv ui32 78
            v2 = Add ui32 v0, v1
            v3 = Mul ui32 v2, v1
            v4 = Sub ui32 v3, v2
            v5 = Add ui32 v4, v4
            v6 = Ashr ui32 v5, 3
            v7 = Xor ui32 v6, v3
            v8 = Add ui32 v7, 555
            Ret ui32 v8
    */

    VMIR::IRBuilder* IrBuilder = VMIR::IRBuilder::GetInstance();

    VMIR::Function* Func = IrBuilder->CreateFunction(VMIR::ValueType::Uint32, "Func");

    VMIR::BasicBlock* BB_1 = IrBuilder->CreateBasicBlock(Func, "BB_1");

    Func->SetEntryBasicBlock(BB_1);

    VMIR::Value* val_45  = IrBuilder->CreateValue(Func, 45u);
    VMIR::Value* val_78  = IrBuilder->CreateValue(Func, 78u);
    VMIR::Value* val_3   = IrBuilder->CreateValue(Func, 3u);
    VMIR::Value* val_555 = IrBuilder->CreateValue(Func, 555u);

    VMIR::Value* v0 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v1 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v2 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v3 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v4 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v5 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v6 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v7 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);
    VMIR::Value* v8 = IrBuilder->CreateValue(Func, VMIR::ValueType::Uint32);

    IrBuilder->CreateMv(BB_1, val_45, v0);
    IrBuilder->CreateMv(BB_1, val_78, v1);
    IrBuilder->CreateAdd(BB_1, v0, v1, v2);
    IrBuilder->CreateMul(BB_1, v2, v1, v3);
    IrBuilder->CreateSub(BB_1, v3, v2, v4);
    IrBuilder->CreateAdd(BB_1, v4, v4, v5);
    IrBuilder->CreateAshr(BB_1, v5, val_3, v6);
    IrBuilder->CreateXor(BB_1, v6, v3, v7);
    IrBuilder->CreateAdd(BB_1, v7, val_555, v8);
    IrBuilder->CreateRet(BB_1, v8);

    VMIR::ConstantFoldingPass constantFoldingPass{};

    IrBuilder->PrintIR(std::cout);
    constantFoldingPass.Run(Func);
    IrBuilder->PrintIR(std::cout);

    IrBuilder->Cleanup();
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}