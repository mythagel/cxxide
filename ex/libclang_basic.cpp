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
    error(const char* what)
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

    index(const index&) = delete;
    index& operator=(const index&) = delete;
    
    index()
     : idx(clang_createIndex(0, 1))
    {
        if(!idx)
            throw error("Unable to create index");
    }
    ~index()
    {
        clang_disposeIndex(idx);
    }
};
struct translation_unit
{
};
struct cursor
{
};
struct source_range
{
};
struct source_location
{
};
struct file
{
};

//CXDiagnosticSet becomes diagnostic_set
struct diagnostic
{

};
struct diagnostic_set
{
    
};

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
            
            auto argv = convert_args(args);
            
            unsigned options = CXTranslationUnit_DetailedPreprocessingRecord | 
                                CXTranslationUnit_CacheCompletionResults;
            auto tu = clang_parseTranslationUnit(idx.idx, nullptr, argv.data(), argv.size(), 0, 0, options);
            if(!tu)
                throw error("unable to parse tu");
            
            int err = clang_reparseTranslationUnit(tu, 0, 0, clang_defaultReparseOptions(tu));
            if(err)
            {
                clang_disposeTranslationUnit(tu);
                throw error("unable to reparse tu");
            }
            
            std::string filename;
            {
                filename = string(clang_getTranslationUnitSpelling(tu)).str();
                std::cerr << "parsed: " << filename << "\n";
            }
            
            // null action?!
            
            clang_disposeTranslationUnit(tu);
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

