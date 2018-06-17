//===----------------------------------------------------------------------===//
//
//                        JFS - The JIT Fuzzing Solver
//
// Copyright 2018 J. Ryan Stinnett
//
// This file is distributed under the MIT license.
// See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//

#include "Driver.h"

#include "API.h"
#include "Log.h"
#include "Signals.h"
#include "TestInput.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace prf {

typedef std::chrono::system_clock::time_point time_point;

struct Fuzzer {
  const time_point startTime = std::chrono::system_clock::now();
  Options opts;
  TestInput* testInput;
  uint runs;
  API* API;
};
static Fuzzer gFuzzer;

int Driver(int& argc, char**& argv) {
  gFuzzer.opts = BuildOptions(argc, argv);
  const Options& opts = gFuzzer.opts;

  const uint& dataLength = opts.dataLength;
  // Allow zero data length only in special case where we are doing a single
  // run. JFS will do this when constraints consist only of constant
  // expressions and constant folding is disabled.
  if (!dataLength && opts.maxRuns != 1) {
    Debug("Test data length currently required");
    exit(1);
  }

  gFuzzer.testInput = new TestInput(dataLength, opts.seed);
  uint maxRuns = std::numeric_limits<uint>::max();
  if (opts.maxRuns >= 0) {
    maxRuns = opts.maxRuns;
  }

  // Set all appropriate signal and timer handlers
  Signals signals(opts);

  // Load any optional functions
  gFuzzer.API = new API();

  // Start testing loop
  TestOneInputT* runTest = gFuzzer.API->TestOneInput;
  TestInput& testInput = *gFuzzer.testInput;
  uint& runs = gFuzzer.runs;
  for (runs = 0; runs < maxRuns; runs++) {
    testInput.generate();
    runTest(testInput.get(), dataLength);
  }

  PrintFinalStats();
  gFuzzer.API->AtExit();
  return 0;
}

void PrintFinalStats() {
  if (!gFuzzer.opts.printFinalStats) {
    return;
  }
  Print("Runs: ", gFuzzer.runs);
  std::chrono::duration<float> elapsed =
      std::chrono::system_clock::now() - gFuzzer.startTime;
  Print("Elapsed Time: ", elapsed.count(), "s");
}

Options BuildOptions(int& argc, char**& argv) {
  std::vector<std::string> args(argv + 1, argv + argc);
  assert(args.size() == argc - 1);

  Options opts;

  for (auto arg : args) {
    int equals = arg.find('=');
    if (arg[0] != '-' || equals < 2) {
      Debug("Ignored unknown option: ", arg);
      continue;
    }
    auto flagInput = arg.substr(1, equals - 1);
    auto valueInput = arg.substr(equals + 1);
#define PRF_OPTION_UINT(flag, name, _)                                         \
    if (flagInput == #flag) {                                                  \
      opts.name = stoul(valueInput);                                           \
      Debug("opts.", #name, " = ", opts.name);                                 \
      continue;                                                                \
    }
#define PRF_OPTION_INT(flag, name, _)                                          \
    if (flagInput == #flag) {                                                  \
      opts.name = stoi(valueInput);                                            \
      Debug("opts.", #name, " = ", opts.name);                                 \
      continue;                                                                \
    }
#define PRF_OPTION_BOOL(flag, name, _)                                         \
    if (flagInput == #flag) {                                                  \
      opts.name = !!stoul(valueInput);                                         \
      Debug("opts.", #name, " = ", opts.name);                                 \
      continue;                                                                \
    }
#define PRF_OPTION_STRING(flag, name, _)                                       \
    if (flagInput == #flag) {                                                  \
      opts.name = std::move(valueInput);                                       \
      Debug("opts.", #name, " = ", opts.name);                                 \
      continue;                                                                \
    }
#include "Options.def"
#undef PRF_OPTION_UINT
#undef PRF_OPTION_INT
#undef PRF_OPTION_BOOL
#undef PRF_OPTION_STRING
    Debug("Ignored unknown option: ", arg);
  }

  return opts;
}

void WriteArtifact(const char* artifactType) {
  static uint fileIndex = 0;
  const std::string& artifactDir = gFuzzer.opts.artifactPrefix;
  if (artifactDir.empty()) {
    return;
  }
  std::stringstream filePath;
  filePath << artifactDir << artifactType << "-" << (fileIndex++);
  Debug("Writing arifact to: ", filePath.str());
  std::fstream file(filePath.str(), std::ios::binary | std::ios::out);
  file << gFuzzer.testInput->str();
}

void AbortHandler(int sig) {
  Debug("Abort occurred!");
  Debug("Found artifact: ", gFuzzer.testInput->str());
  WriteArtifact("abort");
  PrintFinalStats();
  gFuzzer.API->AtExit();
  exit(gFuzzer.opts.errorExitCode);
}

void TimeoutHandler(int sig) {
  Debug("Timeout occurred!");
  PrintFinalStats();
  gFuzzer.API->AtExit();
  exit(gFuzzer.opts.timeoutExitCode);
}

} // namespace prf
