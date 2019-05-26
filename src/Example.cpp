#include <iostream>

#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>

using namespace std;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory MyToolCategory("My tool options");
static int numForLoops = 0;
DeclarationMatcher LoopMatcher = functionDecl().bind("func");
  // forStmt(hasLoopInit(binaryOperator(hasOperatorName("="))),
  // 	  hasIncrement(unaryOperator(hasOperatorName("--")))).bind("forLoop");

//whileStmt().bind("whileLoop"); //whileStmt().bind("whileLoop");//hasCondition(binaryOperator(hasOperatorName("<")))).bind("whileLoop"); //forStmt(hasLoopInit(declStmt(hasSingleDecl(varDecl(hasInitializer(integerLiteral(equals(0)))))))).bind("forLoop");

std::string makeAbsolute(const std::string& filename) {
  llvm::SmallString<256> absolutePath(filename);
  const auto failure = llvm::sys::path::remove_dots(absolutePath, true);
  assert(!failure && "Error cleaning path before making it absolute");
  (void)failure;
  const auto error = llvm::sys::fs::make_absolute(absolutePath);
  assert(!error && "Error generating absolute path");
  (void)error;
  return absolutePath.str();
}

bool hasPrefix(const std::string str, const std::string prefix) {
  auto res = std::mismatch(prefix.begin(), prefix.end(), str.begin());

  if (res.first == prefix.end()) {
    // foo is a prefix of foobar.
    return true;
  }

  return false;
}

class LoopPrinter : public MatchFinder::MatchCallback {
public:

  
  vector<string> targetFileNames;

  LoopPrinter(const vector<string>& targets) : targetFileNames(targets) {}
  
  virtual void run(const MatchFinder::MatchResult &Result) {
    //errs() << "RUN\n";
    if (const FunctionDecl *FS = Result.Nodes.getNodeAs<clang::FunctionDecl>("func")) {
      SourceRange r = FS->getSourceRange();

      SourceLocation loc = r.getBegin();

      string fileLoc = Result.SourceManager->getFilename(loc);
      if (hasPrefix(fileLoc, "/Users/dillon/CppWorkspace/clang-tools")) {
        errs() << "File loc = " << fileLoc << "\n";
        for (auto& srcFile : targetFileNames) {
          string fullPath = makeAbsolute(srcFile);
          errs() << "fullPath = " << fullPath << "\n";
          if (fileLoc == makeAbsolute(srcFile)) {
            errs() << "File = " << fileLoc << "\n";
            errs() << "Source start = " << (r.getBegin()).printToString(*(Result.SourceManager)) << "\n";

            if (FS->hasBody()) {
              numForLoops++;
            }
          }
        }
      }
    }
  }
};

int main(int argc, const char **argv) {

  using namespace clang::tooling;  // NOLINT(build/namespaces)

  CommonOptionsParser options(argc, argv, MyToolCategory);
  const auto& sources = options.getSourcePathList();
  auto& db = options.getCompilations();

  cout << "Sources" << endl;
  for (string src : sources) {
    cout << "  " << src << endl;
  }

  cout << "Compile commands " << endl;
  for (CompileCommand cmd : db.getAllCompileCommands()) {
    cout << "Dir = " << cmd.Directory << endl;
    cout << "Filename = " << cmd.Filename << endl;
    cout << "CommandLine = " << endl;
    for (auto cmd : cmd.CommandLine) {
      cout << "  " << cmd << endl;
    }
  }

  clang::tooling::ClangTool iterateFunctions(db, sources);
  // ClangExpand::Search search;
  // auto result = search.run(db, sources, {});
  // std::string errMsg = "err";
  // std::string dir = "/Users/dillon/CWorkspace/git/";
  // auto cdb = CompilationDatabase::loadFromDirectory(dir, errMsg);
  // auto files = cdb->getAllFiles();
  // ClangTool Tool(*(cdb.get()), cdb->getAllFiles());

  LoopPrinter printer(sources);
  clang::ast_matchers::MatchFinder Finder;
  Finder.addMatcher(LoopMatcher, &printer);

  int result = iterateFunctions.run(newFrontendActionFactory(&Finder).get());
  cout << "Number of for loops: " << numForLoops << endl;
  // return result;
}
