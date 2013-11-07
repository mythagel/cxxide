#include "clang++.h"
#include <iostream>

// c-index-test best example of using libclang

std::ostream& operator<<(std::ostream& os, clang::code_completion_string string)
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
        auto& tu = idx.parse_translation_unit(args);
        std::cout << tu.spelling() << "\n";
        if(tu.spelling() == "/home/nicholas/dev/cxxide/src/project.cpp")
        {
            auto results = tu.codeCompleteAt(tu.spelling(), 82, 25);
            results.sort();
            {
                for(unsigned i = 0; i < results.size(); ++i)
                {
                    std::cout << results[i] << "\n";
                }
            }
            
            auto toks = tu.tokenize(tu.get_cursor().extent());
            for(auto tok : toks)
            {
                std::cout << tok.spelling() << " (" << clang::to_string(tok.kind()) << ")\n";
            }
        }
        // null action?!
    }
    
    return 0;
}

