//===- CCov.cpp - Simple Code Coverage Pass -------------------------------===//
//
// SPDX-License-Identifier: MIT
//
//===----------------------------------------------------------------------===//

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <array>

using namespace llvm;

#define DEBUG_TYPE "ccov"

namespace {
  struct CCov : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    CCov() : ModulePass(ID) {}
    bool runOnModule(Module &M) override;

    private:
      enum TraceAttribute {
        CCov_Entry = 0x01,
        CCov_Ret = 0x02,
      };
      const static std::string LogFunctionName;
      Constant *FileName;
      Constant *FunctionName;
      const Instruction *getFirstDbg(BasicBlock &BB) const;
      const Instruction *getLastDbg(BasicBlock &BB) const;
      void addCoverageTraceCall(Instruction *Inst,
          FunctionCallee CoverageFunction);
  };
} // namespace

const Instruction *CCov::getFirstDbg(BasicBlock &BB) const {
  for (auto IB = BB.begin(), IE = BB.end(); IB != IE; ++IB) {
    if (IB->getDebugLoc())
      return &(*IB);
  }
  return nullptr;
}

const Instruction *CCov::getLastDbg(BasicBlock &BB) const {
  for (auto IB = BB.rbegin(), IE = BB.rend(); IB != IE; ++IB) {
    if (IB->getDebugLoc())
      return &(*IB);
  }
  return nullptr;
}

void CCov::addCoverageTraceCall(Instruction *Inst,
    FunctionCallee CoverageFunction) {
  IRBuilder<> Builder(Inst);
  const DebugLoc &Loc = Inst->getDebugLoc();
  std::array<Value *, 4> Args;
  uint32_t Attribute = 0;
  if (!FileName) {
    std::string FileNameStr = Inst->getModule()->getSourceFileName();
    FileName = Builder.CreateGlobalStringPtr(FileNameStr);
  }
  if (!FunctionName) {
    std::string FunctionNameStr = Inst->getFunction()->getName().str();
    FunctionName = Builder.CreateGlobalStringPtr(FunctionNameStr);
    Attribute |= CCov_Entry;
  }
  if (Inst->getOpcode() == Instruction::Ret)
    Attribute |= CCov_Ret;
  Args[0] = FileName;
  Args[1] = FunctionName;
  Args[2] = Builder.getInt32((uint32_t)Loc.getLine());
  Args[3] = Builder.getInt32(Attribute);
  Builder.CreateCall(CoverageFunction, ArrayRef<Value *>(Args));
}

bool CCov::runOnModule(Module &M) {
  FileName = nullptr;

  FunctionCallee CoverageFunction = M.getOrInsertFunction(LogFunctionName,
      FunctionType::getVoidTy(M.getContext()),
      Type::getInt8PtrTy(M.getContext()),
      Type::getInt8PtrTy(M.getContext()),
      Type::getInt32Ty(M.getContext()),
      Type::getInt32Ty(M.getContext()));

  // Function Level
  for (auto FI = M.getFunctionList().begin(),
            FE = M.getFunctionList().end();
            FI != FE; ++FI) {
    // Skip coverage log function
    if (FI->getName() == LogFunctionName)
      continue;
    FunctionName = nullptr;
    // Basic Block Level
    for (auto BI = FI->getBasicBlockList().begin(),
              BE = FI->getBasicBlockList().end();
              BI != BE; ++BI) {
      Instruction *FrontInst = const_cast<Instruction *>(getFirstDbg(*BI));
      Instruction *BackInst = const_cast<Instruction *>(getLastDbg(*BI));

      addCoverageTraceCall(FrontInst, CoverageFunction);
      if (FrontInst != BackInst && BackInst->getOpcode() == Instruction::Ret)
        addCoverageTraceCall(BackInst, CoverageFunction);
    }
    FunctionName = nullptr;
  }
  return false;
}

const std::string CCov::LogFunctionName = "__log_coverage";
char CCov::ID = 0;
static RegisterPass<CCov> X("ccov", "CCov Pass");
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder &Builder,
      legacy::PassManagerBase &PM) { PM.add(new CCov()); });
