#include "clang++.h"
#include <iostream>

// c-index-test best example of using libclang

std::ostream& operator<<(std::ostream& os, const clang::code_completion_string& string)
{
    unsigned nchunks = string.chunks();
    for (unsigned i = 0; i < nchunks; ++i)
    {
        auto kind = string.chunkKind(i);

        if (kind == CXCompletionChunk_Optional)
            os << "{Optional " << string.chunkCompletionString(i) << "}";
        else if (kind == CXCompletionChunk_VerticalSpace)
            os << "{VerticalSpace  }";
        else
            os << "{" << clang::to_string(kind) << " " << string.chunkText(i) << "}";
    }
    return os;
}
std::ostream& operator<<(std::ostream& os, const clang::source_location& loc)
{
    CXFile file;
    unsigned line;
    unsigned column;
    unsigned offset;
    
    loc.spelling_location(&file, &line, &column, &offset);
    clang::file f = {file};
    os << f.filename() << ": " << line << ":" << column;
    return os;
}

int main()
{
    clang::index idx;

    clang::compilation_db db("/home/nicholas/dev/build/cxxide");
    auto cmds = db.getAll();
    for (const auto& cmd : cmds)
    {
        std::vector<std::string> args;
        for(const auto& arg : cmd)
            args.push_back(arg);
        args.push_back("-I/usr/include/clang/3.0/include/");
        auto& tu = idx.parse_translation_unit(args);
        std::cout << "parsed: " << tu.spelling() << "\n";
        if(tu.spelling() == "/home/nicholas/dev/cxxide/src/project.cpp" || tu.spelling() == "/home/nicholas/Development/cxxide/src/project.cpp")
        {
            auto results = tu.codeCompleteAt(tu.spelling(), 141, 35);
            results.sort();
            {
                for(unsigned i = 0; i < results.size(); ++i)
                {
                    std::cout << "cmpl: " << results[i] << "\n";
                }
            }
            
            auto toks = tu.tokenize(tu.get_cursor().extent());
            for(auto tok : toks)
            {
                std::cout << "tok: " << tok.spelling() << " (" << clang::to_string(tok.kind()) << ")\n";
            }
            
            {
                auto cursor = tu.get_cursor();
                
                unsigned depth = 1;
                std::function<CXChildVisitResult(const clang::cursor& cur, const clang::cursor& parent)> visitor = [&depth, &visitor](clang::cursor cur, clang::cursor parent) -> CXChildVisitResult
                {
                    if(!cur.isUnexposed())
                    {
                        std::cout << std::string(depth, '\t') << cur.location() << " " << clang::to_string(cur.kind()) << " " << cur.spelling() << "\n";
                    }
                    
                    ++depth;
                    cur.visitChildren(visitor);
                    --depth;
                    
                    return CXChildVisit_Continue;
                };
                cursor.visitChildren(visitor);
            }
            
            auto file = tu.get_file(tu.spelling());
            auto loc = tu.get_location(file, 162, 36);
            auto cursor = tu.get_cursor(loc);
            
            findReferencesInFile(cursor, file, [](clang::cursor cur, clang::source_range range) -> CXVisitorResult
            {
                std::cout << "ref: " << cur.location() << " " << clang::to_string(cur.kind()) << " " << cur.spelling() << "\n";
                return CXVisit_Continue;
            });
            
            {
                unsigned depth = 0;
                std::function<CXVisitorResult(clang::cursor, clang::source_range)> visitor = [&tu, &depth, &visitor](clang::cursor cur, clang::source_range range) -> CXVisitorResult
                {
                    std::cout << "incl: " << std::string(depth, '\t') << cur.location() << " " << clang::to_string(cur.kind()) << " " << cur.spelling() << "\n";
                    
                    auto ifile = cur.includedFile();
                    ++depth;
                    //findIncludesInFile(tu, ifile, visitor);
                    --depth;
                    
                    return CXVisit_Continue;
                };
                findIncludesInFile(tu, tu.get_file(tu.spelling()), visitor);
            }
        }
        // null action?!
    }
    
    return 0;
}

