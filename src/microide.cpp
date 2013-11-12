#include "idecore.h"
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace cxxide;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

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

typedef void command_t(int argc, char* argv[]);

void create(int argc, char* argv[])
{
    po::options_description generic("Allowed options");
    generic.add_options()
        ("help", "produce help message")
        ("name", po::value<std::string>(), "project name")
        ("path", po::value<std::string>()->default_value(fs::current_path().native()), "project path (default: cwd)")
    ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("command", po::value<std::string>(), "Command to execute")
    ;

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(hidden);

    po::options_description visible;
    visible.add(generic);

    po::positional_options_description p;
    p.add("command", 1);

    po::variables_map vm;
    store(po::command_line_parser(argc, argv). positional(p).options(cmdline_options).run(), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::cout << visible << "\n";
        return;
    }

    //auto path = ;
    std::string name;
    
    //auto project = project::create(name, path);
}

void usage()
{
    std::cerr << R"(usage: microide <command> [...]

)";
}

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        usage();
        return 1;
    }
    
    auto cmd = argv[1];
    
    std::map<std::string, command_t*> commands = 
    {
        {"create", create}
    };
    
    try
    {
        auto command = commands.find(cmd);
        if(command != commands.end())
        {
            command->second(argc, argv);
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

