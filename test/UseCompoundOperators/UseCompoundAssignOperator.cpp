

#include "gtest/gtest.h"
#include "../../UseCompoundAssignOperator/UseCompoundAssignOperator.h"
#include "../../UseCompoundAssignOperator/UseCompoundAssignOperatorActions.h"
#include "../../UseCompoundAssignOperator/UseCompoundAssignOperatorMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Process.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "Core/Transform.h"

using namespace clang;
using namespace clang::tooling;
using clang::ast_matchers::MatchFinder;

// TODO generalize this code

class UseCompoundAssignOperatorTest : public UseCompoundAssignOperatorTransform {

    public:
    UseCompoundAssignOperatorTest( const TransformOptions& Options ) :
	UseCompoundAssignOperatorTransform(Options)
    {
    }
   
    auto transform_and_compare() {
	TransformOptions Options;

	SmallString<128> CurrentDir;
	std::error_code EC = llvm::sys::fs::current_path(CurrentDir);
	assert(!EC);
	(void)EC;

	SmallString<128> SourceFile = CurrentDir;
	llvm::sys::path::append(SourceFile, "a.cc");

	SmallString<128> HeaderFile = CurrentDir;
	llvm::sys::path::append(HeaderFile, "a.h");

	SmallString<128> HeaderBFile = CurrentDir;
	llvm::sys::path::append(HeaderBFile, "temp");
	llvm::sys::path::append(HeaderBFile, "b.h");

	StringRef ExcludeDir = llvm::sys::path::parent_path(HeaderBFile);

	IncludeExcludeInfo IncInfo;
	Options.ModifiableFiles.readListFromString(CurrentDir, ExcludeDir);

	tooling::FixedCompilationDatabase Compilations(CurrentDir.str(),
						     std::vector<std::string>());
	std::vector<std::string> Sources;
	Sources.push_back(SourceFile.str());
    //
    //    tooling::ClangTool Tool(Compilations, Sources);
    //

    //
    //    DummyTransform T("dummy", Options);
    //    MatchFinder Finder;
    //    Finder.addMatcher(varDecl().bind("decl"), new ModifiableCallback(T));
    //    Tool.run(tooling::newFrontendActionFactory(&Finder).get());

	ClangTool UseCompoundAssignOperatorTool(Compilations, Sources );

	// TODO need this as text to compare with the needed outcome
	
	// Fill in some code that can be compiled in memory
        UseCompoundAssignOperatorTool.mapVirtualFile(SourceFile,
    		      "#include \"a.h\"\n"
    		      "#include \"temp/b.h\"\n"
    		      "int a;\n"
		      "void fun() {\n"
		      "  a = a + 1;\n"
		      "}\n"
		      );
        UseCompoundAssignOperatorTool.mapVirtualFile(HeaderFile, "int b;");
        UseCompoundAssignOperatorTool.mapVirtualFile(HeaderBFile, "int c;");

	unsigned AcceptedChanges = 0;

	MatchFinder Finder;
	UseCompoundAssignOperatorFixer Fixer(AcceptedChanges, *this );

	Finder.addMatcher(makeUseCompoundAssignOperatorMatcher(), &Fixer);

	// syntactic checks are done inside
	if (int result = UseCompoundAssignOperatorTool.run(createActionFactory(Finder))) {
	    llvm::errs() << "Error encountered during translation.\n";
	    return result;
	}

	return 0;
    }
};


TEST( UseCompoundOperatorTest, testText ) {
    TransformOptions Options;
    UseCompoundAssignOperatorTest Test(Options);
    EXPECT_EQ(Test.transform_and_compare(), 0);
}
