#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

// c-index-test best example of using libclang

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

namespace clang
{

struct error : std::runtime_error
{
    error(const std::string& what)
     : std::runtime_error(what)
    {
    }
};

// internal helper!
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

// An "index" that consists of a set of translation units that would typically be linked together into an executable or library. 
struct index
{
    CXIndex idx;
    // std::vector<translation_unit> units;

    index(const index&) = delete;
    index& operator=(const index&) = delete;
    
    index()
     : idx(clang_createIndex(0, 1))
    {
        if(!idx)
            throw error("Unable to create index");
    }
    index(index&& o)
     : idx(nullptr)
    {
        std::swap(idx, o.idx);
    }
    ~index()
    {
        //units.clear();
        /* The index must not be destroyed until all of the translation units created
         * within that index have been destroyed.*/
        clang_disposeIndex(idx);
    }
};
struct translation_unit
{
    CXTranslationUnit tu;
    
    translation_unit(CXTranslationUnit tu)
     : tu(tu)
    {
    }
    translation_unit(translation_unit&& o)
     : tu(nullptr)
    {
        std::swap(tu, o.tu);
    }
    translation_unit(const translation_unit&) = delete;
    translation_unit& operator=(const translation_unit&) = delete;
    
    std::string spelling()
    {
        return string(clang_getTranslationUnitSpelling(tu)).str();
    }
    
    void reparse()
    {
        int err = clang_reparseTranslationUnit(tu, 0, 0, clang_defaultReparseOptions(tu));
        if(err)
        {
            clang_disposeTranslationUnit(tu);
            tu = nullptr;
            throw error("unable to reparse tu");
        }
    }
    
    int code_complete(const std::string& filename, unsigned line, unsigned column)
    {
        auto options = clang_defaultCodeCompleteOptions();
        auto results = clang_codeCompleteAt(tu, filename.c_str(), line, column, nullptr, 0, options);
        if (!results)
            throw error("Unable to perform code completion!\n");

        /* Sort the code-completion results based on the typed text. */
        clang_sortCodeCompletionResults(results->Results, results->NumResults);

        for (unsigned i = 0; i != results->NumResults; ++i)
        {
            auto completion_result = results->Results + i;
            auto ks = string(clang_getCursorKindSpelling(completion_result->CursorKind)).str();
            fprintf(stderr, "%s: \n", ks.c_str());
        }
        
        clang_disposeCodeCompleteResults(results);
    }
    
    unsigned num_diagnostics()
    {
        return clang_getNumDiagnostics(tu);
    }
    
//    diagnostic get_diagnostic(unsigned idx)
//    {
//        return { clang_getDiagnostic(tu, idx) };
//    }
    
//    diagnostic_set get_diagnostics()
//    {
//        return { clang_getDiagnosticSetFromTU(tu) };
//    }
    
//    cursor get_cursor()
//    {
//        return { clang_getTranslationUnitCursor(tu) };
//    }
    
    struct resource_usage
    {
        CXTUResourceUsage usage;
        
        unsigned size()
        {
            return usage.numEntries;
        }
        
        CXTUResourceUsageEntry operator[](int idx)
        {
            return usage.entries[idx];
        }
        
        ~resource_usage()
        {
            clang_disposeCXTUResourceUsage(usage);
        }
    };
    
    resource_usage get_usage()
    {
        return { clang_getCXTUResourceUsage(tu) };
    }
    
    ~translation_unit()
    {
        if(tu) clang_disposeTranslationUnit(tu);
    }
};

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

// big deal - this is the main interface!
struct cursor
{
    CXCursor cur;

    static cursor null()
    {
        return { clang_getNullCursor() };
    }

    bool operator==(const cursor& o) const
    {
        return clang_equalCursors(cur, o.cur);
    }
    bool isnull() const
    {
        return clang_Cursor_isNull(cur);
    }
    
    unsigned hash()
    {
        return clang_hashCursor(cur);
    }
    
    CXCursorKind kind() const
    {
        return clang_getCursorKind(cur);
    }
    
    bool isDeclaration() const
    {
        return clang_isDeclaration(kind());
    }
    bool isReference() const
    {
        return clang_isReference(kind());
    }
    bool isExpression() const
    {
        return clang_isExpression(kind());
    }
    bool isStatement() const
    {
        return clang_isStatement(kind());
    }
    bool isAttribute() const
    {
        return clang_isAttribute(kind());
    }
    bool isInvalid() const
    {
        return clang_isInvalid(kind());
    }
    bool isTranslationUnit() const
    {
        return clang_isTranslationUnit(kind());
    }
    bool isPreprocessing() const
    {
        return clang_isPreprocessing(kind());
    }
    bool isUnexposed() const
    {
        return clang_isUnexposed(kind());
    }
    
    CXLinkageKind linkage() const
    {
        return clang_getCursorLinkage(cur);
    }
    CXAvailabilityKind availability() const
    {
        return clang_getCursorAvailability(cur);
    }
    CXLanguageKind language() const
    {
        return clang_getCursorLanguage(cur);
    }
    
    cursor semanticParent() const
    {
        return { clang_getCursorSemanticParent(cur) };
    }
    cursor lexicalParent()
    {
        return { clang_getCursorLexicalParent(cur) };
    }
    
    CXFile includedFile()
    {
        return clang_getIncludedFile(cur);
    }
};

struct cursor_set
{
    CXCursorSet set;
    
    cursor_set()
     : set(clang_createCXCursorSet())
    {
    }
    
    bool contains(const cursor& cur)
    {
        return clang_CXCursorSet_contains(set, cur.cur);
    }
    
    bool insert(const cursor& cur)
    {
        return clang_CXCursorSet_insert(set, cur);
    }
    
    ~cursor_set()
    {
        clang_disposeCXCursorSet(set);
    }
};

struct source_location
{
    CXSourceLocation loc;
    
    static source_location null()
    {
        return { clang_getNullLocation() };
    }
    
    bool operator==(const source_location& o) const
    {
        return clang_equalLocations(loc, o.loc);
    }
    
    void expansion_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset)
    {
        clang_getExpansionLocation(loc, file, line, column, offset);
    }
    void presumed_location(std::string *filename, unsigned *line, unsigned *column)
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
    void instantiation_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset)
    {
        clang_getInstantiationLocation(loc, file, line, column, offset);
    }
    void spelling_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset)
    {
        clang_getSpellingLocation(loc, file, line, column, offset);
    }
    void file_location(CXFile *file, unsigned *line, unsigned *column, unsigned *offset)
    {
        clang_getFileLocation(loc, file, line, column, offset);
    }
};
struct source_range
{
    CXSourceRange range;
    
    static source_range null()
    {
        return { clang_getNullRange() };
    }
    
    bool operator==(const source_range& o) const
    {
        return clang_equalRanges(range, o.range);
    }
    
    bool isnull() const
    {
        return clang_Range_isNull(range);
    }
    
    source_location start()
    {
        return { clang_getRangeStart(range) };
    }
    source_location end()
    {
        return { clang_getRangeEnd(range) };
    }
};
struct file
{
    CXFile file;
    
    std::string filename() const
    {
        return string(clang_getFileName(file)).str();
    }
    time_t filetime()
    {
        return clang_getFileTime(file);
    }
};

//CXDiagnosticSet becomes diagnostic_set
struct diagnostic
{
    CXDiagnostic diag;
    
    diagnostic(CXDiagnostic diag)
     : diag(diag)
    {
    }
    diagnostic(diagnostic&& o)
     : diag(nullptr)
    {
        std::swap(diag, o.diag);
    }
    
    diagnostic(const diagnostic&) = delete;
    diagnostic& operator=(const diagnostic&) = delete;
    
//    diagnostic_set children()
//    {
//        return { clang_getChildDiagnostics(diag) };
//    }
    
    std::string format(unsigned options)
    {
        return string(clang_formatDiagnostic(diag, options)).str();
    }
    std::string format()
    {
        return string(clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions())).str();
    }
    
    CXDiagnosticSeverity severity()
    {
        return clang_getDiagnosticSeverity(diag);
    }
    
    source_location location()
    {
        return { clang_getDiagnosticLocation(diag) };
    }
    
    std::string spelling()
    {
        return string(clang_getDiagnosticSpelling(diag)).str();
    }
    
    std::string option()
    {
        return string(clang_getDiagnosticOption(diag, nullptr)).str();
    }
    std::string no_option()
    {
        CXString disable;
        string enable(clang_getDiagnosticOption(diag, &disable));
        return string(disable).str();
    }
    
    unsigned category()
    {
        return clang_getDiagnosticCategory(diag);
    }
    
    std::string category_name()
    {
        return string(clang_getDiagnosticCategoryText(diag)).str();
    }
    
    unsigned num_ranges()
    {
        return clang_getDiagnosticNumRanges(diag);
    }
    source_range get_range(unsigned idx)
    {
        return { clang_getDiagnosticRange(diag, idx) };
    }
    
    unsigned num_fixits()
    {
        return clang_getDiagnosticNumFixIts(diag);
    }
    struct fix_it
    {
        std::string replace;
        source_range range;
    };
    fix_it get_fixit(int idx)
    {
        CXSourceRange replacement_range;
        string str(clang_getDiagnosticFixIt(diag, idx, &replacement_range));
        
        return { str.str(), {replacement_range} };
    }
    
    ~diagnostic()
    {
        if(diag) clang_disposeDiagnostic(diag);
    }
};
struct diagnostic_set
{
    CXDiagnosticSet set;
    
    diagnostic_set(CXDiagnosticSet set)
     : set(set)
    {
    }
    diagnostic_set(diagnostic_set&& o)
     : set(nullptr)
    {
        std::swap(set, o.set);
    }
    
    diagnostic_set(const diagnostic_set&) = delete;
    diagnostic_set& operator=(const diagnostic_set&) = delete;
    
    unsigned size() const
    {
        return clang_getNumDiagnosticsInSet(set);
    }
    
    diagnostic operator[](unsigned idx)
    {
        return { clang_getDiagnosticInSet(set, idx) };
    }
    
    ~diagnostic_set()
    {
        if(set) clang_disposeDiagnosticSet(set);
    }
};

diagnostic_set load_diagnositics(const std::string& filename)
{
    CXString errstr;
    CXLoadDiag_Error err;
    auto set = clang_loadDiagnostics(filename.c_str(), &err, &errstr);
    
    if(!set)
        throw error(string(errstr).str());
    
    return { set };
}

struct compilation_db
{
    struct commands
    {
        CXCompileCommands cmds;
        
        commands(const commands&) = delete;
        commands& operator=(const commands&) = delete;
        commands(compilation_db& db)
        {
            cmds = clang_CompilationDatabase_getAllCompileCommands(db.db);
        }
        ~commands()
        {
            clang_CompileCommands_dispose(cmds);
        }
    };

    CXCompilationDatabase db;
    
    compilation_db(const compilation_db&) = delete;
    compilation_db& operator=(const compilation_db&) = delete;
    
    compilation_db(const std::string& build_path)
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
    
    void index_db(index& idx)
    {
        commands cmds(*this);
        auto num_cmds = clang_CompileCommands_getSize(cmds.cmds);
        
        for (auto i = 0; i < num_cmds; ++i)
        {
            auto cmd = clang_CompileCommands_getCommand(cmds.cmds, i);

            auto nargs = clang_CompileCommand_getNumArgs(cmd);
            
            std::vector<std::string> args;
            for (auto i = 0; i < nargs; ++i)
            {
                string arg(clang_CompileCommand_getArg(cmd, i));
                args.push_back(arg.c_str());
            }
            // TODO bullshit test crap - libclang not installed - use older headers...
            args.push_back("-I/usr/include/clang/3.0/include/");
            
            auto tu = parse_translation_unit(idx, args);
            
            std::string filename = tu.spelling();
            std::cerr << "parsed: " << filename << "\n";
            
            if(filename == "/home/nicholas/Development/cxxide/src/cmake/cmake.cpp")
            {
                tu.code_complete(filename, 198, 40);
            }
            
            // null action?!
        }
    }
    
    ~compilation_db()
    {
        clang_CompilationDatabase_dispose(db);
    }
};

}

int main()
{
    clang::index idx;
    clang::compilation_db db("/home/nicholas/dev/build/cxxide");
    
    db.index_db(idx);
    
    return 0;
}

