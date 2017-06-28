//===----------------------------------------------------------------------===//
//
//                        JFS - The JIT Fuzzing Solver
//
// Copyright 2017 Daniel Liew
//
// This file is distributed under the MIT license.
// See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//

#include "jfs/CXXFuzzingBackend/CXXFuzzingSolver.h"
#include "jfs/Core/JFSContext.h"
#include "jfs/Core/SMTLIB2Parser.h"
#include "jfs/Core/ScopedJFSContextErrorHandler.h"
#include "jfs/FuzzingCommon/DummyFuzzingSolver.h"
#include "jfs/Support/ErrorMessages.h"
#include "jfs/Support/version.h"
#include "jfs/Transform/QueryPassManager.h"
#include "jfs/Transform/StandardPasses.h"
#include "jfs/Z3Backend/Z3Solver.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace jfs;
using namespace jfs::core;
using namespace jfs::transform;

namespace {
llvm::cl::opt<std::string> InputFilename(llvm::cl::Positional,
                                         llvm::cl::desc("<input file>"),
                                         llvm::cl::init("-"));
llvm::cl::opt<unsigned> Verbosity("v", llvm::cl::desc("Verbosity level"),
                                  llvm::cl::init(0));

llvm::cl::opt<unsigned>
    MaxTime("max-time", llvm::cl::desc("Max allowed solver time (seconds). "
                                       "Default is 0 which means no maximum"),
            llvm::cl::init(0));

enum BackendTy {
  DUMMY_FUZZING_SOLVER,
  Z3_SOLVER,
  CXX_FUZZING_SOLVER,
};

llvm::cl::opt<BackendTy>
    SolverBackend(llvm::cl::desc("Solver backend"),
                  llvm::cl::values(clEnumValN(DUMMY_FUZZING_SOLVER, "dummy",
                                              "dummy solver (default)"),
                                   clEnumValN(Z3_SOLVER, "z3", "Z3 backend"),
                                   clEnumValN(CXX_FUZZING_SOLVER, "cxx",
                                              "CXX fuzzing backend")),
                  llvm::cl::init(DUMMY_FUZZING_SOLVER));
}

void printVersion() {
  llvm::outs() << support::getVersionString() << "\n";
  llvm::outs() << "\n";
  llvm::cl::PrintVersionMessage();
  return;
}

class ToolErrorHandler : public JFSContextErrorHandler {
  JFSContextErrorHandler::ErrorAction handleZ3error(JFSContext &ctx,
                                                    Z3_error_code ec) {
    ctx.getErrorStream() << "(error \"" << Z3_get_error_msg(ctx.z3Ctx, ec)
                         << "\")\n";
    exit(1);
    return JFSContextErrorHandler::STOP; // Unreachable.
  }

};


int main(int argc, char** argv) {
  llvm::cl::SetVersionPrinter(printVersion);
  llvm::cl::ParseCommandLineOptions(argc, argv);

  JFSContextConfig ctxCfg;
  ctxCfg.verbosity = Verbosity;
  JFSContext ctx(ctxCfg);
  auto bufferOrError = llvm::MemoryBuffer::getFileOrSTDIN(InputFilename);
  if (auto error = bufferOrError.getError()) {
    ctx.getErrorStream() << jfs::support::getMessageForFailedOpenFileOrSTDIN(
        InputFilename, error);
    return 1;
  }
  auto buffer(std::move(bufferOrError.get()));

  ToolErrorHandler toolHandler;
  ScopedJFSContextErrorHandler errorHandler(ctx, &toolHandler);
  SMTLIB2Parser parser(ctx);
  auto query = parser.parseMemoryBuffer(std::move(buffer));
  if (Verbosity > 10)
    ctx.getDebugStream() << *query;

  // Run standard transformations
  QueryPassManager pm;
  AddStandardPasses(pm);
  pm.run(*query);
  if (Verbosity > 10)
    ctx.getDebugStream() << *query;

  // Create solver
  // TODO: Refactor this so it can be used elsewhere
  SolverOptions solverOptions;
  solverOptions.maxTime = MaxTime;
  std::unique_ptr<Solver> solver;
  switch (SolverBackend) {
  case DUMMY_FUZZING_SOLVER:
    solver.reset(new jfs::fuzzingCommon::DummyFuzzingSolver(solverOptions));
    break;
  case Z3_SOLVER:
    solver.reset(new jfs::z3Backend::Z3Solver(solverOptions));
    break;
  case CXX_FUZZING_SOLVER:
    solver.reset(new jfs::cxxfb::CXXFuzzingSolver(solverOptions));
    break;
  default:
    llvm_unreachable("unknown solver backend");
  }

  if (Verbosity > 0)
    ctx.getDebugStream() << "(using solver \"" << solver->getName() << "\")\n";

  auto response = solver->solve(*query, /*produceModel=*/false);
  llvm::outs() << SolverResponse::getSatString(response->sat) << "\n";
  return 0;
}
