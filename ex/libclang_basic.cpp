#include "clang-c/Index.h"
#include "clang-c/CXCompilationDatabase.h"
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
    argv.push_back(nullptr);
    
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
                auto str_arg = clang_CompileCommand_getArg(cmd, i);
                args.push_back(clang_getCString(str_arg));
                clang_disposeString(str_arg);
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
            
            std::cerr << "parsed\n";
            
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
    clang::compilation_db db("/home/nicholas/Development/build/cxxide");
    
    db.index_db(idx);
    
    return 0;
}

