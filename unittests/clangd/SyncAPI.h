//===--- SyncAPI.h - Sync version of ClangdServer's API ----------*- C++-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//
// This file contains synchronous versions of ClangdServer's async API. We
// deliberately don't expose the sync API outside tests to encourage using the
// async versions in clangd code.
#ifndef LLVM_CLANG_TOOLS_EXTRA_UNITTESTS_CLANGD_SYNCAPI_H
#define LLVM_CLANG_TOOLS_EXTRA_UNITTESTS_CLANGD_SYNCAPI_H

#include "ClangdServer.h"
#include <future>

namespace clang {
namespace clangd {

// Calls addDocument and then blockUntilIdleForTest.
void runAddDocument(ClangdServer &Server, PathRef File, StringRef Contents);

Tagged<CompletionList> runCodeComplete(ClangdServer &Server, PathRef File,
                                       Position Pos,
                                       clangd::CodeCompleteOptions Opts);

llvm::Expected<Tagged<SignatureHelp>>
runSignatureHelp(ClangdServer &Server, PathRef File, Position Pos);

llvm::Expected<Tagged<std::vector<Location>>>
runFindDefinitions(ClangdServer &Server, PathRef File, Position Pos);

llvm::Expected<Tagged<std::vector<DocumentHighlight>>>
runFindDocumentHighlights(ClangdServer &Server, PathRef File, Position Pos);

llvm::Expected<std::vector<tooling::Replacement>>
runRename(ClangdServer &Server, PathRef File, Position Pos, StringRef NewName);

std::string runDumpAST(ClangdServer &Server, PathRef File);

} // namespace clangd
} // namespace clang

#endif