#include <iostream>
#include <exception>
#include "project.h"

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
    using namespace cxxide;
    try
    {
        auto project = project::create("crap", "/home/nicholas/dev/build/cxxide/projects/", "/home/nicholas/dev/build/cxxide/projects/build/");
        std::cout << "created project.\n";
    }
    catch(const std::exception& e)
    {
        print_exception(e);
    }
    return 0;
}
