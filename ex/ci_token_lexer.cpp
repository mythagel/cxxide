#include <iostream>

#include "llvm/Support/Host.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/Diagnostic.h"

/*

Based on clang tutorials.
Fallback simple token lexer if source cannot be parsed.
This will be used to provide basic syntax hightlighting in a fallback case.

Semantic highlighting preceeding the error should be maintained, simple syntax hightlighting for the error.

Investigation required if the error can be bounded to the current function and maintain semantic highlighting
for code after the error point.

This would improve the case where errors are introduced while typing, having the following code flash between 
syntax/semantic highlighting.

This would probably be acceptable within the function body itself. Perhaps simple errors can be bounded to the current statement.

*/
int main()
{
    using namespace clang;

    CompilerInstance ci;
    DiagnosticOptions diagnosticOptions;
    ci.createDiagnostics();

    llvm::IntrusiveRefCntPtr<TargetOptions> pto( new TargetOptions());
    pto->Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *pti = TargetInfo::CreateTargetInfo(ci.getDiagnostics(), pto.getPtr());
    ci.setTarget(pti);

    ci.createFileManager();
    ci.createSourceManager(ci.getFileManager());
    ci.createPreprocessor();

	const FileEntry* pFile = ci.getFileManager().getFile("/home/nicholas/dev/cxxide/ex/simple.cpp");
    ci.getSourceManager().createMainFileID(pFile);
    ci.getPreprocessor().EnterMainSourceFile();
    
    ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(), &ci.getPreprocessor());
    Token tok;
    do {
        ci.getPreprocessor().Lex(tok);
        if(ci.getDiagnostics().hasErrorOccurred())
            break;
        SourceLocation sl = tok.getLocation();
        ci.getPreprocessor().DumpToken(tok);
        std::cout << " : " << sl.printToString(ci.getSourceManager()) << "\n";
        
        std::cerr << std::endl;
    } while (tok.isNot(tok::eof));
    ci.getDiagnosticClient().EndSourceFile();

    return 0;
}
