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
 * clang++.cpp
 *
 *  Created on: 2013-10-31
 *      Author: nicholas
 */

#include "clang++.h"

namespace clang
{

namespace
{
std::vector<char*> convert_args(const std::vector<std::string>& args)
{
    std::vector<char*> argv;
    
    for(auto& arg : args)
        argv.push_back(const_cast<char*>(arg.c_str()));
    
    return argv;
}
}

struct string
{
    CXString _str;
    
    string(const string&) = delete;
    string& operator=(const string&) = delete;
    explicit string(CXString str)
     : _str(str)
    {
    }

    const char* c_str() const
    {
        return clang_getCString(_str);
    }

    std::string str() const
    {
        return clang_getCString(_str);
    }

    ~string()
    {
        clang_disposeString(_str);
    }
};

error::error(const std::string& what)
 : std::runtime_error(what)
{
}


index::index()
 : idx(clang_createIndex(0, 1))
{
    if(!idx)
        throw error("Unable to create index");
}
index::index(index&& o)
 : idx(nullptr)
{
    std::swap(idx, o.idx);
}
index::~index()
{
    /* The index must not be destroyed until all of the translation units created
     * within that index have been destroyed.*/
    clang_disposeIndex(idx);
}

translation_unit::translation_unit(CXTranslationUnit tu)
 : tu(tu)
{
}
translation_unit::translation_unit(translation_unit&& o)
 : tu(nullptr)
{
    std::swap(tu, o.tu);
}
std::string translation_unit::spelling()
{
    return string(clang_getTranslationUnitSpelling(tu)).str();
}

void translation_unit::reparse()
{
    int err = clang_reparseTranslationUnit(tu, 0, 0, clang_defaultReparseOptions(tu));
    if(err)
    {
        clang_disposeTranslationUnit(tu);
        tu = nullptr;
        throw error("unable to reparse tu");
    }
}

int translation_unit::code_complete(const std::string& filename, unsigned line, unsigned column)
{
    auto options = clang_defaultCodeCompleteOptions();
    auto results = clang_codeCompleteAt(tu, filename.c_str(), line, column, nullptr, 0, options);
    if (!results)
        throw error("Unable to perform code completion!\n");

    clang_sortCodeCompletionResults(results->Results, results->NumResults);

    for(unsigned i = 0; i != results->NumResults; ++i)
    {
        auto completion_result = results->Results + i;
        auto ks = string(clang_getCursorKindSpelling(completion_result->CursorKind)).str();
        fprintf(stderr, "%s: \n", ks.c_str());
    }
    
    clang_disposeCodeCompleteResults(results);
    return 0;
}

unsigned translation_unit::num_diagnostics()
{
    return clang_getNumDiagnostics(tu);
}

diagnostic translation_unit::get_diagnostic(unsigned idx)
{
    return { clang_getDiagnostic(tu, idx) };
}

diagnostic_set translation_unit::get_diagnostics()
{
    return { clang_getDiagnosticSetFromTU(tu) };
}

cursor translation_unit::get_cursor()
{
    return { clang_getTranslationUnitCursor(tu) };
}

cursor translation_unit::get_cursor(const source_location& sl)
{
    return { clang_getCursor(tu, sl.loc) };
}

unsigned translation_unit::resource_usage::size()
{
    return usage.numEntries;
}

CXTUResourceUsageEntry translation_unit::resource_usage::operator[](int idx)
{
    return usage.entries[idx];
}

translation_unit::resource_usage::~resource_usage()
{
    clang_disposeCXTUResourceUsage(usage);
}

translation_unit::resource_usage translation_unit::get_usage()
{
    return { clang_getCXTUResourceUsage(tu) };
}

translation_unit::~translation_unit()
{
    if(tu) clang_disposeTranslationUnit(tu);
}

translation_unit create_translation_unit(index& idx, const std::string& filename, const std::vector<std::string>& args)
{
    auto argv = convert_args(args);
    auto tu = clang_createTranslationUnitFromSourceFile(idx.idx, filename.c_str(), argv.size(), argv.data(), 0, nullptr);
    if(!tu)
        throw error("unable to parse tu");
    return { tu };
}

translation_unit parse_translation_unit(index& idx, const std::vector<std::string>& args)
{
    auto argv = convert_args(args);
    unsigned options = CXTranslationUnit_DetailedPreprocessingRecord | 
                        CXTranslationUnit_CacheCompletionResults;
    
    auto tu = clang_parseTranslationUnit(idx.idx, nullptr, argv.data(), argv.size(), nullptr, 0, options);
    if(!tu)
        throw error("unable to parse tu");

    // Have to reparse to cache completion results.
    int err = clang_reparseTranslationUnit(tu, 0, 0, clang_defaultReparseOptions(tu));
    if(err)
    {
        clang_disposeTranslationUnit(tu);
        throw error("unable to reparse tu");
    }

    return { tu };
}

cursor cursor::null()
{
    return { clang_getNullCursor() };
}

bool cursor::operator==(const cursor& o) const
{
    return clang_equalCursors(cur, o.cur);
}
bool cursor::isnull() const
{
    return clang_Cursor_isNull(cur);
}

unsigned cursor::hash()
{
    return clang_hashCursor(cur);
}

CXCursorKind cursor::kind() const
{
    return clang_getCursorKind(cur);
}

bool cursor::isDeclaration() const
{
    return clang_isDeclaration(kind());
}
bool cursor::isReference() const
{
    return clang_isReference(kind());
}
bool cursor::isExpression() const
{
    return clang_isExpression(kind());
}
bool cursor::isStatement() const
{
    return clang_isStatement(kind());
}
bool cursor::isAttribute() const
{
    return clang_isAttribute(kind());
}
bool cursor::isInvalid() const
{
    return clang_isInvalid(kind());
}
bool cursor::isTranslationUnit() const
{
    return clang_isTranslationUnit(kind());
}
bool cursor::isPreprocessing() const
{
    return clang_isPreprocessing(kind());
}
bool cursor::isUnexposed() const
{
    return clang_isUnexposed(kind());
}

CXLinkageKind cursor::linkage() const
{
    return clang_getCursorLinkage(cur);
}
CXAvailabilityKind cursor::availability() const
{
    return clang_getCursorAvailability(cur);
}
CXLanguageKind cursor::language() const
{
    return clang_getCursorLanguage(cur);
}

cursor cursor::semanticParent() const
{
    return { clang_getCursorSemanticParent(cur) };
}
cursor cursor::lexicalParent()
{
    return { clang_getCursorLexicalParent(cur) };
}

CXFile cursor::includedFile()
{
    return clang_getIncludedFile(cur);
}

source_location cursor::location()
{
    return { clang_getCursorLocation(cur) };
}
source_range cursor::extent()
{
    return { clang_getCursorExtent(cur) };
}

std::string cursor::type::spelling()
{
    return string(clang_getTypeSpelling(ctype)).str();
}
bool cursor::type::operator==(const type& o)
{
    return clang_equalTypes(ctype, o.ctype);
}
cursor::type cursor::type::canonical()
{
    return { clang_getCanonicalType(ctype) };
}
bool cursor::type::const_qualified()
{
    return clang_isConstQualifiedType(ctype);
}
bool cursor::type::volatile_qualified()
{
    return clang_isVolatileQualifiedType(ctype);
}
bool cursor::type::restrict_qualified()
{
    return clang_isRestrictQualifiedType(ctype);
}
cursor::type cursor::type::pointee_type()
{
    return { clang_getPointeeType(ctype) };
}
cursor cursor::type::type_declaration()
{
    return { clang_getTypeDeclaration(ctype) };
}

std::string cursor::type::kind_spelling()
{
    return string(clang_getTypeKindSpelling(ctype.kind)).str();
}
CXCallingConv cursor::type::function_calling_convention()
{
    return clang_getFunctionTypeCallingConv(ctype);
}
cursor::type cursor::type::result_type()
{
    return { clang_getResultType(ctype) };
}

int cursor::type::getNumArgTypes()
{
    return clang_getNumArgTypes(ctype);
}
cursor::type cursor::type::getArgType(unsigned idx)
{
    return { clang_getArgType(ctype, idx) };
}
bool cursor::type::isFunctionTypeVariadic()
{
    return clang_isFunctionTypeVariadic(ctype);
}
bool cursor::type::isPODType()
{
    return clang_isPODType(ctype);
}
cursor::type cursor::type::element_type()
{
    return { clang_getElementType(ctype) };
}
long long cursor::type::num_elements()
{
    return clang_getNumElements(ctype);
}
cursor::type cursor::type::array_element_type()
{
    return { clang_getArrayElementType(ctype) };
}
long long cursor::type::array_size()
{
    return clang_getArraySize(ctype);
}
long long cursor::type::align_of()
{
    return clang_Type_getAlignOf(ctype);
}
//cursor::type cursor::type::class_type()
//{
//    return { clang_Type_getClassType(ctype) };
//}
long long cursor::type::size_of()
{
    return clang_Type_getSizeOf(ctype);
}
long long cursor::type::offset_of(const char* s)
{
    return clang_Type_getOffsetOf(ctype, s);
}

cursor::type cursor::get_type()
{
    return { clang_getCursorType(cur) };
}
cursor::type cursor::typedefDeclUnderlyingType()
{
    return { clang_getTypedefDeclUnderlyingType(cur) };
}
cursor::type cursor::enumDeclIntegerType()
{
    return { clang_getEnumDeclIntegerType(cur) };
}
long long cursor::enumConstantDeclValue()
{
    return clang_getEnumConstantDeclValue(cur);
}
unsigned long long cursor::enumConstantDeclUnsignedValue()
{
    return clang_getEnumConstantDeclUnsignedValue(cur);
}
int cursor::fieldDeclBitWidth()
{
    return clang_getFieldDeclBitWidth(cur);
}
int cursor::numArguments()
{
    return clang_Cursor_getNumArguments(cur);
}
cursor cursor::getArgument(unsigned idx)
{
    return { clang_Cursor_getArgument(cur, idx) };
}
cursor::type cursor::getResultType()
{
    return { clang_getCursorResultType(cur) };
}
bool cursor::isBitField()
{
    return clang_Cursor_isBitField(cur);
}
bool cursor::isVirtualBase()
{
    return clang_isVirtualBase(cur);
}
CX_CXXAccessSpecifier cursor::getCXXAccessSpecifier()
{
    return clang_getCXXAccessSpecifier(cur);
}
unsigned cursor::numOverloadedDecls()
{
    return clang_getNumOverloadedDecls(cur);
}
cursor cursor::getOverloadedDecl(unsigned idx)
{
    return { clang_getOverloadedDecl(cur, idx) };
}

bool cursor::visitChildren(std::function<CXChildVisitResult(cursor cur, cursor parent)> visitor)
{
    return clang_visitChildren(cur, [](CXCursor cur, CXCursor parent, CXClientData client_data) -> CXChildVisitResult
    {
        auto visitor = static_cast<std::function<CXChildVisitResult(cursor cur, cursor parent)>*>(client_data);
        return (*visitor)({ cur }, { parent });
    }, &visitor);
}

std::string cursor::unified_symbol()
{
    return string(clang_getCursorUSR(cur)).str();
}
std::string cursor::spelling()
{
    return string(clang_getCursorSpelling(cur)).str();
}
std::string cursor::display_name()
{
    return string(clang_getCursorDisplayName(cur)).str();
}

cursor cursor::getCursorReferenced()
{
    return { clang_getCursorReferenced(cur) };
}
cursor cursor::getCursorDefinition()
{
    return { clang_getCursorDefinition(cur) };
}
bool cursor::isDefinition()
{
    return clang_isCursorDefinition(cur);
}
cursor cursor::getCanonical()
{
    return { clang_getCanonicalCursor(cur) };
}
bool cursor::isDynamicCall()
{
    return clang_Cursor_isDynamicCall(cur);
}
bool cursor::isVariadic()
{
    return clang_Cursor_isVariadic(cur);
}
source_range cursor::getCommentRange()
{
    return { clang_Cursor_getCommentRange(cur) };
}
std::string cursor::rawCommentText()
{
    return string(clang_Cursor_getRawCommentText(cur)).str();
}
std::string cursor::briefCommentText()
{
    return string(clang_Cursor_getBriefCommentText(cur)).str();
}
//CXComment cursor::getParsedComment()
//{
//    return clang_Cursor_getParsedComment(cur);
//}
bool cursor::isPureVirtual()
{
    return clang_CXXMethod_isPureVirtual(cur);
}
bool cursor::isStatic()
{
    return clang_CXXMethod_isStatic(cur);
}
bool cursor::isVirtual()
{
    return clang_CXXMethod_isVirtual(cur);
}

CXCursorKind cursor::getTemplateKind()
{
    return clang_getTemplateCursorKind(cur);
}
cursor cursor::getSpecializedTemplate()
{
    return { clang_getSpecializedCursorTemplate(cur) };
}
source_range cursor::getReferenceNameRange(unsigned NameFlags, unsigned PieceIndex)
{
    return { clang_getCursorReferenceNameRange(cur, NameFlags, PieceIndex) };
}

cursor_set::cursor_set()
 : set(clang_createCXCursorSet())
{
}

bool cursor_set::contains(const cursor& cur)
{
    return clang_CXCursorSet_contains(set, cur.cur);
}

bool cursor_set::insert(const cursor& cur)
{
    return clang_CXCursorSet_insert(set, cur.cur);
}

cursor_set::~cursor_set()
{
    clang_disposeCXCursorSet(set);
}

source_location source_location::null()
{
    return { clang_getNullLocation() };
}

bool source_location::operator==(const source_location& o) const
{
    return clang_equalLocations(loc, o.loc);
}

void source_location::expansion_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset)
{
    clang_getExpansionLocation(loc, file, line, column, offset);
}
void source_location::presumed_location(std::string *filename, unsigned *line, unsigned *column)
{
    if(filename)
    {
        CXString fn;
        clang_getPresumedLocation(loc, &fn, line, column);
        *filename = string(fn).str();
    }
    else
    {
        clang_getPresumedLocation(loc, nullptr, line, column);
    }
}
void source_location::instantiation_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset)
{
    clang_getInstantiationLocation(loc, file, line, column, offset);
}
void source_location::spelling_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset)
{
    clang_getSpellingLocation(loc, file, line, column, offset);
}
void source_location::file_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset)
{
    clang_getFileLocation(loc, file, line, column, offset);
}

source_range source_range::null()
{
    return { clang_getNullRange() };
}

bool source_range::operator==(const source_range& o) const
{
    return clang_equalRanges(range, o.range);
}

bool source_range::isnull() const
{
    return clang_Range_isNull(range);
}

source_location source_range::start()
{
    return { clang_getRangeStart(range) };
}
source_location source_range::end()
{
    return { clang_getRangeEnd(range) };
}

std::string file::filename() const
{
    return string(clang_getFileName(file)).str();
}
time_t file::filetime()
{
    return clang_getFileTime(file);
}

diagnostic::diagnostic(CXDiagnostic diag)
 : diag(diag)
{
}
diagnostic::diagnostic(diagnostic&& o)
 : diag(nullptr)
{
    std::swap(diag, o.diag);
}

diagnostic_set diagnostic::children()
{
    return { clang_getChildDiagnostics(diag) };
}

std::string diagnostic::format(unsigned options)
{
    return string(clang_formatDiagnostic(diag, options)).str();
}
std::string diagnostic::format()
{
    return string(clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions())).str();
}

CXDiagnosticSeverity diagnostic::severity()
{
    return clang_getDiagnosticSeverity(diag);
}

source_location diagnostic::location()
{
    return { clang_getDiagnosticLocation(diag) };
}

std::string diagnostic::spelling()
{
    return string(clang_getDiagnosticSpelling(diag)).str();
}

std::string diagnostic::option()
{
    return string(clang_getDiagnosticOption(diag, nullptr)).str();
}
std::string diagnostic::no_option()
{
    CXString disable;
    string enable(clang_getDiagnosticOption(diag, &disable));
    return string(disable).str();
}

unsigned diagnostic::category()
{
    return clang_getDiagnosticCategory(diag);
}

std::string diagnostic::category_name()
{
    return string(clang_getDiagnosticCategoryText(diag)).str();
}

unsigned diagnostic::num_ranges()
{
    return clang_getDiagnosticNumRanges(diag);
}
source_range diagnostic::get_range(unsigned idx)
{
    return { clang_getDiagnosticRange(diag, idx) };
}

unsigned diagnostic::num_fixits()
{
    return clang_getDiagnosticNumFixIts(diag);
}
diagnostic::fix_it diagnostic::get_fixit(int idx)
{
    CXSourceRange replacement_range;
    string str(clang_getDiagnosticFixIt(diag, idx, &replacement_range));
    
    return { str.str(), {replacement_range} };
}

diagnostic::~diagnostic()
{
    if(diag) clang_disposeDiagnostic(diag);
}

diagnostic_set::diagnostic_set(CXDiagnosticSet set)
 : set(set)
{
}
diagnostic_set::diagnostic_set(diagnostic_set&& o)
 : set(nullptr)
{
    std::swap(set, o.set);
}

unsigned diagnostic_set::size() const
{
    return clang_getNumDiagnosticsInSet(set);
}

diagnostic diagnostic_set::operator[](unsigned idx)
{
    return { clang_getDiagnosticInSet(set, idx) };
}

diagnostic_set::~diagnostic_set()
{
    if(set) clang_disposeDiagnosticSet(set);
}

diagnostic_set load_diagnositics(const std::string& filename)
{
    CXString errstr;
    CXLoadDiag_Error err;
    auto set = clang_loadDiagnostics(filename.c_str(), &err, &errstr);
    
    if(!set)
        throw error(string(errstr).str());
    
    return { set };
}

compilation_db::commands::commands(compilation_db& db)
{
    cmds = clang_CompilationDatabase_getAllCompileCommands(db.db);
}
compilation_db::commands::~commands()
{
    clang_CompileCommands_dispose(cmds);
}

compilation_db::compilation_db(const std::string& build_path)
 : db()
{
    CXCompilationDatabase_Error ec;
    db = clang_CompilationDatabase_fromDirectory(build_path.c_str(), &ec);
    if(!db) throw error("Unable to create compilation db");
    if(ec)
    {
        clang_CompilationDatabase_dispose(db);
        throw error("Unable to create compilation db");
    }
}

void compilation_db::index_db(index& idx)
{
    commands cmds(*this);
    unsigned num_cmds = clang_CompileCommands_getSize(cmds.cmds);
    
    for (unsigned i = 0; i < num_cmds; ++i)
    {
        auto cmd = clang_CompileCommands_getCommand(cmds.cmds, i);
        unsigned nargs = clang_CompileCommand_getNumArgs(cmd);
        
        std::vector<std::string> args;
        for (unsigned i = 0; i < nargs; ++i)
        {
            string arg(clang_CompileCommand_getArg(cmd, i));
            args.push_back(arg.c_str());
        }
        
        auto tu = parse_translation_unit(idx, args);
        
        // null action?!
    }
}

compilation_db::~compilation_db()
{
    clang_CompilationDatabase_dispose(db);
}

}

