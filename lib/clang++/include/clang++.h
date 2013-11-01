/* 
 * Copyright (C) 2013  Nicholas Gill
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * clang++.h
 *
 *  Created on: 2013-10-31
 *      Author: nicholas
 */

#ifndef CLANGPP_H_
#define CLANGPP_H_
#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>

namespace clang
{

struct error : std::runtime_error
{
    error(const std::string& what);
};

struct diagnostic;
struct diagnostic_set;
struct cursor;
struct translation_unit;
struct source_location;
struct source_range;
struct token_set;
struct code_completion_string;
struct code_complete_results;
struct file;

struct index
{
    CXIndex idx;

    index(const index&) = delete;
    index& operator=(const index&) = delete;
    
    index();
    index(index&& o);
    ~index();
};

struct translation_unit
{
    CXTranslationUnit tu;
    
    translation_unit(CXTranslationUnit tu);
    translation_unit(translation_unit&& o);
    translation_unit(const translation_unit&) = delete;
    translation_unit& operator=(const translation_unit&) = delete;
    
    std::string spelling();
    
    void reparse();
    
    code_complete_results codeCompleteAt(const std::string& filename, unsigned line, unsigned column);
    
    unsigned num_diagnostics();
    diagnostic get_diagnostic(unsigned idx);
    diagnostic_set get_diagnostics();
    
    cursor get_cursor();
    cursor get_cursor(const source_location& sl);
    
    struct resource_usage
    {
        CXTUResourceUsage usage;
        
        unsigned size();
        
        CXTUResourceUsageEntry operator[](int idx);
        
        ~resource_usage();
    };
    
    resource_usage get_usage();
    
    token_set tokenize(source_range range);
    
    void visitInclusions(std::function<void(file, CXSourceLocation*, unsigned)> visitor);
    
    ~translation_unit();
};

translation_unit create_translation_unit(index& idx, const std::string& filename, const std::vector<std::string>& args);
translation_unit parse_translation_unit(index& idx, const std::vector<std::string>& args);

struct cursor
{
    CXCursor cur;

    static cursor null();

    bool operator==(const cursor& o) const;
    bool isnull() const;
    
    unsigned hash();
    
    CXCursorKind kind() const;
    
    bool isDeclaration() const;
    bool isReference() const;
    bool isExpression() const;
    bool isStatement() const;
    bool isAttribute() const;
    bool isInvalid() const;
    bool isTranslationUnit() const;
    bool isPreprocessing() const;
    bool isUnexposed() const;
    
    CXLinkageKind linkage() const;
    CXAvailabilityKind availability() const;
    CXLanguageKind language() const;
    
    cursor semanticParent() const;
    cursor lexicalParent();
    
    CXFile includedFile();
    
    source_location location();
    source_range extent();
    
    struct type
    {
        CXType ctype;
        
        std::string spelling();
        bool operator==(const type& o);
        type canonical();
        bool const_qualified();
        bool volatile_qualified();
        bool restrict_qualified();
        type pointee_type();
        cursor type_declaration();
        
        std::string kind_spelling();
        CXCallingConv function_calling_convention();
        type result_type();
        
        int getNumArgTypes();
        type getArgType(unsigned idx);
        bool isFunctionTypeVariadic();
        bool isPODType();
        type element_type();
        long long num_elements();
        type array_element_type();
        long long array_size();
        long long align_of();
        //type class_type();
        long long size_of();
        long long offset_of(const char* s);
    };
    
    type get_type();
    type typedefDeclUnderlyingType();
    type enumDeclIntegerType();
    long long enumConstantDeclValue();
    unsigned long long enumConstantDeclUnsignedValue();
    int fieldDeclBitWidth();
    int numArguments();
    cursor getArgument(unsigned idx);
    type getResultType();
    bool isBitField();
    bool isVirtualBase();
    CX_CXXAccessSpecifier getCXXAccessSpecifier();
    unsigned numOverloadedDecls();
    cursor getOverloadedDecl(unsigned idx);
    bool visitChildren(std::function<CXChildVisitResult(cursor cur, cursor parent)> visitor);
    
    std::string unified_symbol();
    std::string spelling();
    std::string display_name();
    
    cursor getCursorReferenced();
    cursor getCursorDefinition();
    bool isDefinition();
    cursor getCanonical();
    bool isDynamicCall();
    bool isVariadic();
    source_range getCommentRange();
    std::string rawCommentText();
    std::string briefCommentText();
    //CXComment getParsedComment();
    bool isPureVirtual();
    bool isStatic();
    bool isVirtual();
    
    CXCursorKind getTemplateKind();
    cursor getSpecializedTemplate();
    source_range getReferenceNameRange(unsigned NameFlags, unsigned PieceIndex);
    code_completion_string completionString();
};

CXResult findReferencesInFile(cursor cur, file f, std::function<CXVisitorResult(cursor, source_range)> visitor);
CXResult findIncludesInFile(translation_unit tu, file f, std::function<CXVisitorResult(cursor, source_range)> visitor);

struct token
{
    CXToken tok;
    CXTranslationUnit tu;
    
    CXTokenKind kind();
    std::string spelling();
    source_location location();
    source_range extent();
};

struct token_set
{
    CXTranslationUnit tu;
    CXToken *tokens;
    unsigned size;
    
    token_set(CXTranslationUnit tu, CXToken *tokens, unsigned size);
    token_set(token_set&& o);
    token_set(const token_set&) = delete;
    token_set& operator=(const token_set&) = delete;
    
    // annotate?
    
    ~token_set();
};

struct code_completion_string
{
    CXCompletionString str;
    
    CXCompletionChunkKind chunkKind(unsigned idx);
    std::string chunkText(unsigned idx);
    code_completion_string chunkCompletionString(unsigned idx);
    unsigned chunks();
    unsigned priority();
    CXAvailabilityKind availability();
    unsigned numAnnotations();
    std::string annotation(unsigned idx);
    std::string parent();
    std::string briefComment();
};

struct code_complete_results
{
    CXCodeCompleteResults* results;
    
    code_complete_results(CXCodeCompleteResults* results);
    code_complete_results(code_complete_results&& o);
    code_complete_results(const code_complete_results&) = delete;
    code_complete_results& operator=(const code_complete_results&) = delete;
    
    void sort();
    unsigned numDiagnostics();
    diagnostic getDiagnostic(unsigned idx);
    unsigned long long contexts();
    CXCursorKind containerKind(bool* IsIncomplete);
    std::string containerUSR();
    
    ~code_complete_results();
};

struct cursor_set
{
    CXCursorSet set;
    
    cursor_set();
    
    bool contains(const cursor& cur);
    
    bool insert(const cursor& cur);
    
    ~cursor_set();
};

struct source_location
{
    CXSourceLocation loc;
    
    static source_location null();
    
    bool operator==(const source_location& o) const;
    
    void expansion_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset);
    void presumed_location(std::string *filename, unsigned *line, unsigned *column);
    void instantiation_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset);
    void spelling_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset);
    void file_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset);
};

struct source_range
{
    CXSourceRange range;
    
    static source_range null();
    
    bool operator==(const source_range& o) const;
    
    bool isnull() const;
    
    source_location start();
    source_location end();
};

struct file
{
    CXFile file;
    
    std::string filename() const;
    time_t filetime();
};

struct diagnostic
{
    CXDiagnostic diag;
    
    diagnostic(CXDiagnostic diag);
    diagnostic(diagnostic&& o);
    diagnostic(const diagnostic&) = delete;
    diagnostic& operator=(const diagnostic&) = delete;
    
    diagnostic_set children();
    
    std::string format(unsigned options);
    std::string format();
    
    CXDiagnosticSeverity severity();
    
    source_location location();
    
    std::string spelling();
    
    std::string option();
    std::string no_option();
    
    unsigned category();
    
    std::string category_name();
    
    unsigned num_ranges();
    source_range get_range(unsigned idx);
    
    unsigned num_fixits();
    struct fix_it
    {
        std::string replace;
        source_range range;
    };
    fix_it get_fixit(int idx);
    
    ~diagnostic();
};

struct diagnostic_set
{
    CXDiagnosticSet set;
    
    diagnostic_set(CXDiagnosticSet set);
    diagnostic_set(diagnostic_set&& o);
    diagnostic_set(const diagnostic_set&) = delete;
    diagnostic_set& operator=(const diagnostic_set&) = delete;
    
    unsigned size() const;
    diagnostic operator[](unsigned idx);
    
    ~diagnostic_set();
};

diagnostic_set load_diagnositics(const std::string& filename);

struct compilation_db
{
    CXCompilationDatabase db;
    
    struct commands
    {
        CXCompileCommands cmds;
        
        commands(const commands&) = delete;
        commands& operator=(const commands&) = delete;
        commands(compilation_db& db);
        
        ~commands();
    };

    compilation_db(const compilation_db&) = delete;
    compilation_db& operator=(const compilation_db&) = delete;
    compilation_db(const std::string& build_path);
    
    void index_db(index& idx);
    
    ~compilation_db();
};

}

#endif /* CLANGPP_H_ */
