#include <iostream>
#include <exception>
#include "git.h"

void print_exception(const std::exception& e, int level = 0)
{
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try
    {
        std::rethrow_if_nested(e);
    }
    catch(const std::exception& e)
    {
        print_exception(e, level+1);
    }
    catch(...)
    {
    }
}

int main()
{
    try
    {
        auto repo = cxxide::git::init("/home/nicholas/Development/build/cxxide/crap/");
    }
    catch(const std::exception& e)
    {
        print_exception(e);
    }
    return 0;
}
