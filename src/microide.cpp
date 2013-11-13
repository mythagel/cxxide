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

typedef int command_t(int argc, char* argv[]);

int create(int argc, char* argv[]);
int dir(int argc, char* argv[]);

po::options_description common_options("Global options");
po::options_description user_options("microide options");

int main(int argc, char* argv[])
{
    try
    {
        common_options.add_options()
            ("help", "display this help and exit")
            ("path", po::value<std::string>()->default_value(fs::current_path().native()), "Project path")
            ("build-path", po::value<std::string>(), "Project build path")
        ;

        po::options_description hidden;
        hidden.add_options()
            ("command", po::value<std::string>()->required(), "Command to execute")
            ("command_args", po::value<std::vector<std::string>>(), "Command arguments")
        ;

        user_options.add(common_options);

        po::positional_options_description p;
        p.add("command", 1);
        p.add("command_args", -1);
        po::options_description desc;
        desc.add(common_options).add(hidden);

        po::variables_map vm;
        store(po::command_line_parser(argc, argv).positional(p).options(desc).allow_unregistered().run(), vm);

        if (vm.count("help") && !vm.count("command"))
        {
            std::cout << user_options << "\n";
            return 0;
        }
        notify(vm);

        if (!vm.count("help") && !vm.count("command"))
        {
            std::cout << user_options << "\n";
            return 1;
        }

        auto cmd = vm["command"].as<std::string>();

        std::map<std::string, command_t*> commands =
        {
            {"create", create},
            {"dir", dir}
        };

        auto command = commands.find(cmd);
        if(command != commands.end())
        {
            return command->second(argc, argv);
        }
        else
        {
            std::cerr << "Unrecognised command " << cmd << "\n";
            std::cout << user_options << "\n";
            return 1;
        }
    }
    catch(const po::error& e)
    {
        print_exception(e);
        std::cout << user_options << "\n";
        return 1;
    }
    catch(const std::exception& e)
    {
        print_exception(e);
        return 1;
    }
}

int create(int argc, char* argv[])
{
    po::options_description command_options("Create options");
    command_options.add_options()
        ("name", po::value<std::string>()->required(), "Project name")
    ;

    po::options_description hidden;
    hidden.add_options()
        ("command", po::value<std::string>()->required(), "Command to execute")
    ;

    user_options.add(command_options);

    po::positional_options_description p;
    p.add("command", 1);
    p.add("name", 1);
    po::options_description desc;
    desc.add(common_options).add(command_options).add(hidden);

    po::variables_map vm;
    store(po::command_line_parser(argc, argv). positional(p).options(desc).run(), vm);

    if(vm.count("help"))
    {
        std::cout << user_options << "\n";
        return 0;
    }
    notify(vm);

    fs::path path = vm["path"].as<std::string>();
    auto name = vm["name"].as<std::string>();
    fs::path build_path;
    if(vm.count("build-path"))
        build_path = vm["build-path"].as<std::string>();

    auto project = project::create(name, path, build_path);

    return 0;
}

int dir(int argc, char* argv[])
{
    po::options_description command_options("Directory options");
    command_options.add_options()
//        ("name", po::value<std::string>()->required(), "Directory name")
    ;

    po::options_description hidden;
    hidden.add_options()
        ("command", po::value<std::string>()->required(), "Command to execute")
        ("subcommand", po::value<std::string>()->required(), "Subcommand to execute")
        ("subcommand_args", po::value<std::vector<std::string>>(), "Subcommand arguments")
    ;

    user_options.add(command_options);

    po::positional_options_description p;
    p.add("command", 1);
    p.add("subcommand", 1);
    p.add("subcommand_args", -1);
    po::options_description desc;
    desc.add(common_options).add(command_options).add(hidden);

    po::variables_map vm;
    store(po::command_line_parser(argc, argv). positional(p).options(desc).run(), vm);

    if (vm.count("help"))
    {
        std::cout << user_options << "\n";
        return 0;
    }
    notify(vm);

    /* Better idea would be to part the initial generic directory arguments
     * Then reparse based on the selected subcommand. */

    fs::path path = vm["path"].as<std::string>();
    fs::path build_path;
    if(vm.count("build-path"))
        build_path = vm["build-path"].as<std::string>();

    auto cmd = vm["subcommand"].as<std::string>();
    auto args = vm["subcommand_args"].as<std::vector<std::string>>();

    auto project = project::open(path, build_path);

    if(cmd == "create")
    {
        if(args.size() != 1)
        {
            std::cout << user_options << "\n";
            return 1;
        }
        // TODO
    }
    else
    {
        std::cerr << "Unrecognised subcommand " << cmd << "\n";
        std::cout << user_options << "\n";
        return 1;
    }

    return 0;
}

