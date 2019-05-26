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

class LoopPrinter : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    //errs() << "RUN\n";
    if (const FunctionDecl *FS = Result.Nodes.getNodeAs<clang::FunctionDecl>("func")) {
      if (FS->hasBody()) {
        // errs() << "found function\n";
	// clang::LangOptions LangOpts;
	// LangOpts.CPlusPlus = true;
	// clang::PrintingPolicy Policy(LangOpts);
	// std::string TypeS;
	// llvm::raw_string_ostream s(TypeS);
	FS->printPretty(errs(), 0, Policy);
	//FS->dump();
	//errs() << "\n";
	numForLoops++;
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

  LoopPrinter printer;
  clang::ast_matchers::MatchFinder Finder;
  Finder.addMatcher(LoopMatcher, &printer);

  int result = iterateFunctions.run(newFrontendActionFactory(&Finder).get());
  cout << "Number of for loops: " << numForLoops << endl;
  // return result;
}
