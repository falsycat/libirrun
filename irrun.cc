#include "./irrun.h"

#include <atomic>
#include <iostream>
#include <memory>
#include <string>

#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

struct irrun_t {
  irrun_resolver_t resolver;
  void*            udata;

  std::string error;

  llvm::LLVMContext                           Context;
  std::unique_ptr<llvm::ExecutionEngine>      Engine;

  class Resolver : public llvm::LegacyJITSymbolResolver {
    irrun_t* super;

    Resolver() = delete;
    Resolver(const Resolver&) = delete;
    Resolver(Resolver&&) = delete;
    Resolver& operator=(const Resolver&) = delete;
    Resolver& operator=(Resolver&&) = delete;

    llvm::JITSymbol findSymbol(const std::string& name) final {
      const void* ptr = super->resolver?
        super->resolver(name.c_str(), super->udata):
        nullptr;
      return ptr?
        llvm::JITSymbol((llvm::JITTargetAddress) ptr, llvm::JITSymbolFlags::Exported):
        llvm::JITSymbol(nullptr);
    }
    llvm::JITSymbol findSymbolInLogicalDylib(const std::string& name) final {
      return 0;
    }

    public: explicit Resolver(irrun_t* s) : super(s) {}
  };

  irrun_t() = delete;
  irrun_t(const irrun_t&) = delete;
  irrun_t(irrun_t&&) = delete;
  irrun_t& operator=(const irrun_t&) = delete;
  irrun_t& operator=(irrun_t&&) = delete;

  explicit irrun_t(irrun_resolver_t r, void* u, llvm::CodeGenOpt::Level opt) :
      resolver(r), udata(u) {
    Engine.reset(
      llvm::EngineBuilder(std::make_unique<llvm::Module>("irrun", Context)).
        setEngineKind(llvm::EngineKind::Kind::JIT).
        setOptLevel(opt).
        setErrorStr(&error).
        setSymbolResolver(std::make_unique<Resolver>(this)).
        setVerifyModules(true).
        create());
  }

  void addModule(std::unique_ptr<llvm::Module> M) {
    Engine->addModule(std::move(M));
  }

  void* getSymbol(const std::string& str) {
    Engine->finalizeObject();
    return Engine->getPointerToNamedFunction(str, false  /* not to abort on fail */);
  }
};

extern "C" {

irrun_t* irrun_new(irrun_resolver_t resolver, void* udata, irrun_optimize_level_t opt) {
  static std::atomic_flag once_ = ATOMIC_FLAG_INIT;
  if (!once_.test_and_set()) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
  }

  const auto o =
    opt == IRRUN_OPTIMIZE_LEVEL_AGGRESSIVE? llvm::CodeGenOpt::Level::Aggressive:
    llvm::CodeGenOpt::Level::None;
  return new irrun_t(resolver, udata, o);
}
void irrun_delete(irrun_t* ctx) {
  delete ctx;
}
const char* irrun_get_error(const irrun_t* ctx) {
  return ctx->error == ""? nullptr: ctx->error.c_str();
}

int irrun_add_module_from_file(irrun_t* ctx, const char* path) {
  llvm::SMDiagnostic d;
  std::unique_ptr<llvm::Module> m(llvm::parseIRFile(path, d, ctx->Context));
  if (!m) {
    llvm::raw_string_ostream o(ctx->error);
    d.print("irrun", o);
    return 0;
  }
  ctx->addModule(std::move(m));
  return 1;
}

void* irrun_sym(irrun_t* ctx, const char* name) {
  return ctx->getSymbol(name);
}

}
