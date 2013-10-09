#include <iostream>
#include <exception>
#include "git.h"
#include "exec.h"

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
        auto repo = cxxide::git::init("/home/nicholas/dev/build/cxxide/crap/");
    }
    catch(const std::exception& e)
    {
        print_exception(e);
    }
    
    cxxide::system::stream_t stream;
    int status = cxxide::system::exec({}, {"true", ""}, stream);
    
    std::cout << "out: >>>" << stream.out << "<<<\n";
    std::cout << "err: >>>" << stream.err << "<<<\n";
    return 0;
}
