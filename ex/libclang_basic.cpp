#include "clang++.h"
#include <iostream>

// c-index-test best example of using libclang

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
            auto res = tu.codeCompleteAt(tu.spelling(), 82, 25);
            for(unsigned i = 0; i < res.size(); ++i)
            {
                auto opt = res[i];
                for(unsigned i = 0; i < opt.chunks(); ++i)
                {
                    //std::cout << opt.chunkText(i) << "  ";
                }
                //std::cout << "\n";
            }
            // TODO
        }
        // null action?!
    }
    
    return 0;
}

