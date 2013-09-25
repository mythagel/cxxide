/*
 * Indexer.cpp
 *
 *  Created on: 25/09/2012
 *      Author: nicholas
 */

#include <iostream>
#include <cstdio>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Casting.h"

#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/FileSystemOptions.h"

#include "clang/Basic/SourceManager.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Basic/FileManager.h"

#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Frontend/Utils.h"
#include "clang/Frontend/CompilerInvocation.h"

#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"

#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Frontend/FrontendOptions.h"

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/Builtins.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Sema/Sema.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/Type.h"
#include "clang/AST/Decl.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/Ownership.h"
#include "clang/Sema/CodeCompleteConsumer.h"
#include "clang/AST/DeclGroup.h"

#include "clang/Parse/Parser.h"

#include "clang/Parse/ParseAST.h"
#include "clang/Frontend/CompilerInstance.h"

class MyASTConsumer: public clang::ASTConsumer
{
public:
	MyASTConsumer()
	 : clang::ASTConsumer()
	{
	}
	virtual ~MyASTConsumer()
	{
	}

	virtual bool HandleTopLevelDecl(clang::DeclGroupRef d)
	{
		static int count = 0;
		for (auto it = d.begin(); it != d.end(); ++it)
		{
//			std::cout << (*it)->getDeclKindName() << '\n';
			count++;
			clang::VarDecl *vd = llvm::dyn_cast<clang::VarDecl>(*it);
			if (!vd)
			{
				continue;
			}
			if (vd->isFileVarDecl() && !vd->hasExternalStorage())
			{
				std::cerr << "Read top-level variable decl: '";
				std::cerr << vd->getDeclName().getAsString();
				std::cerr << "'" << std::endl;
			}
		}
		return true;
	}
};

int main()
{
	using namespace clang;

	DiagnosticOptions* diagnosticOptions = new DiagnosticOptions;
	TextDiagnosticPrinter *pTextDiagnosticPrinter = new TextDiagnosticPrinter(llvm::outs(), diagnosticOptions, false);
	llvm::IntrusiveRefCntPtr<DiagnosticIDs> pDiagIDs;
	DiagnosticsEngine *pDiagnosticsEngine = new DiagnosticsEngine(pDiagIDs, diagnosticOptions, pTextDiagnosticPrinter);

	LangOptions languageOptions;
	CompilerInvocation::setLangDefaults(languageOptions, IK_CXX);
	languageOptions.CPlusPlus11 = true;

	FileSystemOptions fileSystemOptions;
	FileManager fileManager(fileSystemOptions);

	SourceManager sourceManager(*pDiagnosticsEngine, fileManager);

	llvm::IntrusiveRefCntPtr<HeaderSearchOptions> headerSearchOptions(new HeaderSearchOptions);
	headerSearchOptions->UseBuiltinIncludes = true;
	headerSearchOptions->UseStandardSystemIncludes = true;
	headerSearchOptions->UseStandardCXXIncludes = true;
//	headerSearchOptions->AddPath("/usr/include/linux", frontend::Angled, false, false);
//	headerSearchOptions->AddPath("/usr/include", frontend::Angled, false, false);
//	headerSearchOptions->AddPath("/home/nicholas/dev/build/cxxide/deps/llvm/lib/clang/3.4/include/", frontend::Angled, false, false);
//	headerSearchOptions->AddPath("/usr/include/c++/4.7", frontend::Angled, false, false);
//	headerSearchOptions->AddPath("/usr/include/x86_64-linux-gnu", frontend::Angled, false, false);
//	headerSearchOptions->AddPath("/usr/include/x86_64-linux-gnu/c++/4.7", frontend::Angled, false, false);

	TargetOptions targetOptions;
	targetOptions.Triple = llvm::sys::getDefaultTargetTriple();

	TargetInfo *pTargetInfo = TargetInfo::CreateTargetInfo(*pDiagnosticsEngine, &targetOptions);

	HeaderSearch headerSearch(headerSearchOptions, fileManager, *pDiagnosticsEngine, languageOptions, pTargetInfo);
	CompilerInstance compInst;

    llvm::IntrusiveRefCntPtr<clang::PreprocessorOptions> preprocessorOptions( new clang::PreprocessorOptions());
	Preprocessor preprocessor(preprocessorOptions, *pDiagnosticsEngine, languageOptions, pTargetInfo, sourceManager, headerSearch, compInst);

	FrontendOptions frontendOptions;
	InitializePreprocessor(preprocessor, *preprocessorOptions, *headerSearchOptions, frontendOptions);

	preprocessor.getBuiltinInfo().InitializeBuiltins(preprocessor.getIdentifierTable(), languageOptions);

	const FileEntry *pFile = fileManager.getFile("/home/nicholas/dev/4527961/v8_wrap_class.cpp");
	sourceManager.createMainFileID(pFile);

	IdentifierTable identifierTable(languageOptions);
	SelectorTable selectorTable;

	Builtin::Context builtinContext;
	builtinContext.InitializeTarget(*pTargetInfo);
	ASTContext astContext(languageOptions, sourceManager, pTargetInfo, identifierTable, selectorTable, builtinContext,
		0 /* size_reserve*/);
	MyASTConsumer astConsumer;

	Sema sema(preprocessor, astContext, astConsumer);

	CodeCompleteOptions codeCompleteOptions;
	PrintingCodeCompleteConsumer CompletionConsumer(codeCompleteOptions, llvm::outs());

	preprocessor.SetCodeCompletionPoint(pFile, 218, 6);

	pTextDiagnosticPrinter->BeginSourceFile(languageOptions, &preprocessor);
	ParseAST(preprocessor, &astConsumer, astContext, /*PrintStats*/false, TU_Complete, &CompletionConsumer);

	pTextDiagnosticPrinter->EndSourceFile();

    delete pDiagnosticsEngine;
	return 0;
}
