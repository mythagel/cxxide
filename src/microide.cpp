#include "idecore.h"
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <map>

using namespace cxxide;

void print_exception(const std::exception& e, int level = 0)
{
    std::cerr << std::string(level, ' ') << "error: " << e.what() << '\n';
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

/*
Interface

microide command [args]

microide create project_name

microide rename blah2
microide package add blah
microide package remove blah
microide target init blah
microide target add source
microide target set cflags blah
microide source

source create header blah [--lang c++/c]
source create source blah [--lang c++/c]
source create tu blah [--lang c++/c]

*/

typedef void command_t(const std::vector<std::string>& args);

void create(const std::vector<std::string>& args)
{
    std::string name;
    auto project = project::create(const std::string& name, const std::string& path, const std::string& build_path);
}

void usage()
{
    std::cerr << R"(usage: microide <command> [...]

)";
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args(argv, argv + argc);
    args.erase(args.begin());
    
    if(args.empty())
    {
        usage();
        return 1;
    }
    
    auto cmd = args[0];
    args.erase(args.begin());
    
    std::map<std::string, command_t*> commands = 
    {
        {"create", create}
    };
    
    try
    {
        auto command = commands.find(cmd);
        if(command != commands.end())
        {
            command->second(args);
            return 0;
        }
        else
        {
            std::cerr << "Unrecognised command " << cmd << "\n";
            usage();
        }
    }
    catch(const std::exception& e)
    {
        print_exception(e);
    }
    return 1;
}

