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

po::options_description common_options("Global options");
po::options_description user_options("microide options");

void print_exception(const std::exception& e, int level = 0);

/*
Interface

$ microide init project_name
$ cd project_name
$ microide mkdir src
$ cd src
$ microide init target example
$ microide target example sources add 

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

typedef int command_t(const std::vector<std::string>& args);

namespace commands
{
int create(const std::vector<std::string>& args);
int create_dir(const std::vector<std::string>& args);
int create_target(const std::vector<std::string>& args);

int target_name(const std::vector<std::string>& args);
int target_label(const std::vector<std::string>& args);
int target_output_name(const std::vector<std::string>& args);
int target_type(const std::vector<std::string>& args);
int target_version(const std::vector<std::string>& args);

int target_sources(const std::vector<std::string>& args);
int target_sources_add(const std::vector<std::string>& args);
int target_sources_remove(const std::vector<std::string>& args);

int target_definitions(const std::vector<std::string>& args);
int target_definitions_add(const std::vector<std::string>& args);
int target_definitions_remove(const std::vector<std::string>& args);

int target_includes(const std::vector<std::string>& args);
int target_includes_add(const std::vector<std::string>& args);
int target_includes_remove(const std::vector<std::string>& args);

int target_compile_flags(const std::vector<std::string>& args);
int target_link_flags(const std::vector<std::string>& args);

int target_libs(const std::vector<std::string>& args);
int target_libs_add(const std::vector<std::string>& args);
int target_libs_remove(const std::vector<std::string>& args);

int target_packages(const std::vector<std::string>& args);
int target_packages_add(const std::vector<std::string>& args);
int target_packages_remove(const std::vector<std::string>& args);

}

struct subcommand
{
    std::string name;
    std::string description;
    command_t* command;

    std::vector<subcommand> subcommands;
};

std::ostream& operator<<(std::ostream& os, const subcommand& cmd);


int main(int argc, char* argv[])
{
    std::vector<std::string> args(argv, argv + argc);

    subcommand root;
    root.name = args[0];
    root.command = nullptr;
    root.subcommands =
    {
        {"create", "Create a new project", commands::create,
            {
                {"dir", "Create a new source directory", commands::create_dir, {}},
                {"target", "Create a new target", commands::create_target, {}}
            }
        },
        {"target", "Commands on targets", nullptr,
            {
                {"name", "Target name", commands::target_name, {}},
                {"label", "Descriptive label", commands::target_label, {}},
                {"output_name", "Output filename", commands::target_version, {}},
                {"type", "Target type {executable / shared_library / static_library}", commands::target_type, {}},
                {"version", "Version number (major[.minor[.patch]])", commands::target_version, {}},
                {"sources", "Source list", commands::target_sources, 
                    {
                        {"add", "Add source file", commands::target_sources_add, {}},
                        {"remove", "Remove source file", commands::target_sources_remove, {}}
                    }},
                {"definitions", "", commands::target_definitions, 
                    {
                        {"add", "Add preprocessor defintion", commands::target_definitions_add, {}},
                        {"remove", "Remove preprocessor defintion", commands::target_definitions_remove, {}}
                    }},
                {"includes", "", commands::target_includes, 
                    {
                        {"add", "Add include directory", commands::target_includes_add, {}},
                        {"remove", "Remove include directory", commands::target_includes_remove, {}}
                    }},
                {"compile_flags", "Compiler flags", commands::target_compile_flags, {}},
                {"link_flags", "Linker flags", commands::target_link_flags, {}},
                {"libs", "", commands::target_libs, 
                    {
                        {"add", "Add link library", commands::target_libs_add, {}},
                        {"remove", "Remove link library", commands::target_libs_remove, {}}
                    }},
                {"packages", "", commands::target_packages, 
                    {
                        {"add", "Add package", commands::target_packages_add, {}},
                        {"remove", "Remove package", commands::target_packages_remove, {}}
                    }}
            }
        }
    };
    args.erase(begin(args));

    try
    {
        common_options.add_options()
            ("help", "display this help and exit")
            ("path", po::value<std::string>()->default_value(fs::current_path().native()), "Project path")
            ("build-path", po::value<std::string>(), "Project build path")
        ;

        po::options_description hidden;
        hidden.add_options()
            ("commands", po::value<std::vector<std::string>>()->required(), "Command to execute")
        ;

        user_options.add(common_options);

        po::positional_options_description p;
        p.add("commands", -1);
        po::options_description desc;
        desc.add(common_options).add(hidden);

        po::variables_map vm;
        store(po::command_line_parser(args).positional(p).options(desc).allow_unregistered().run(), vm);

        if (vm.count("help") && !vm.count("commands"))
        {
            std::cout << user_options << "\n";
            std::cout << "\nCommands: \n" << root << "\n";
            return 0;
        }
        notify(vm);

        {
            subcommand* command = &root;

            for(auto it = begin(args); it != end(args); )
            {
                if(it->find('-') == 0)
                    break;

                bool command_found = false;
                for(auto& cmd : command->subcommands)
                {
                    if(cmd.name == *it)
                    {
                        command = &cmd;
                        command_found = true;
                        break;
                    }
                }
                if(command_found)
                    it = args.erase(it);
                else
                    break;
            }

            if(command != &root && command->command)
                return command->command(args);

            std::cout << "error: Unrecognised command.\n";
            std::cout << user_options << "\n";
            std::cout << "\nCommands: \n" << root << "\n";
            return 1;
        }
    }
    catch(const po::error& e)
    {
        print_exception(e);
        std::cout << user_options << "\n";
        std::cout << "\nCommands: \n" << root << "\n";
        return 1;
    }
    catch(const std::exception& e)
    {
        print_exception(e);
        return 1;
    }
}

namespace commands
{

int create(const std::vector<std::string>& args)
{
    po::options_description command_options("Create project options");
    command_options.add_options()
        ("name", po::value<std::string>()->required(), "Project name")
    ;

    user_options.add(command_options);

    po::positional_options_description p;
    p.add("name", 1);
    po::options_description desc;
    desc.add(common_options).add(command_options);

    po::variables_map vm;
    store(po::command_line_parser(args). positional(p).options(desc).run(), vm);

    if(vm.count("help"))
    {
        std::cout << user_options << "\n";
        return 0;
    }
    notify(vm);

    fs::path path = vm["path"].as<std::string>();
    fs::path build_path;
    if(vm.count("build-path"))
        build_path = vm["build-path"].as<std::string>();
    auto name = vm["name"].as<std::string>();

    auto project = project::create(name, path, build_path);

    return 0;
}

int create_dir(const std::vector<std::string>& args)
{
    po::options_description command_options("Create directory options");
    command_options.add_options()
        ("name", po::value<std::string>()->required(), "Directory name")
    ;

    user_options.add(command_options);

    po::positional_options_description p;
    p.add("name", 1);
    po::options_description desc;
    desc.add(common_options).add(command_options);

    po::variables_map vm;
    store(po::command_line_parser(args). positional(p).options(desc).run(), vm);

    if (vm.count("help"))
    {
        std::cout << user_options << "\n";
        return 0;
    }
    notify(vm);

    fs::path path = vm["path"].as<std::string>();
    fs::path build_path;
    if(vm.count("build-path"))
        build_path = vm["build-path"].as<std::string>();
    auto name = vm["name"].as<std::string>();

    auto project = project::open(path, build_path);

    project.directory_create(path / name);
    project.write_config();

    return 0;
}

int create_target(const std::vector<std::string>& args)
{
    po::options_description command_options("Create target options");
    command_options.add_options()
        ("name", po::value<std::string>()->required(), "Target name")
        ("type", po::value<std::string>()->required(), "Target type")
    ;

    user_options.add(command_options);

    po::positional_options_description p;
    p.add("name", 1);
    p.add("type", 1);
    po::options_description desc;
    desc.add(common_options).add(command_options);

    po::variables_map vm;
    store(po::command_line_parser(args). positional(p).options(desc).run(), vm);

    if (vm.count("help"))
    {
        std::cout << user_options << "\n";
        return 0;
    }
    notify(vm);

    fs::path path = vm["path"].as<std::string>();
    fs::path build_path;
    if(vm.count("build-path"))
        build_path = vm["build-path"].as<std::string>();
    auto name = vm["name"].as<std::string>();
    auto type = [](const std::string& token) -> cmake::config::target_t::type_t
    {
        if(token == "executable")
            return cmake::config::target_t::executable;
        else if(token == "shared_library")
            return cmake::config::target_t::shared_library;
        else if(token == "static_library")
            return cmake::config::target_t::static_library;

        throw po::validation_error(po::validation_error::invalid_option_value, "type", token);
    }(vm["type"].as<std::string>());

    auto project = project::open(path, build_path);

    auto source_dir = project.directory(path);
    source_dir.target_add(name, type);
    project.write_config();

    return 0;
}

int target_name(const std::vector<std::string>& args)
{
    std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
    throw std::logic_error("Unimplemented");
}
int target_label(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_output_name(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_type(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_version(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}

int target_sources(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_sources_add(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_sources_remove(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}

int target_definitions(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_definitions_add(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_definitions_remove(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}

int target_includes(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_includes_add(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_includes_remove(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}

int target_compile_flags(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_link_flags(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}

int target_libs(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_libs_add(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_libs_remove(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}

int target_packages(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_packages_add(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}
int target_packages_remove(const std::vector<std::string>& args)
{
    throw std::logic_error("Unimplemented");
}

}

void print_exception(const std::exception& e, int level)
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

namespace
{

void out_nested(std::ostream& os, const subcommand& cmd, size_t n)
{
    std::string indent;
    for(size_t i = 0; i < n; ++i)
        indent += "  ";
    size_t pad = std::abs(20 - (indent.size() + cmd.name.size()));
    if(!cmd.description.empty())
        os << indent << cmd.name << std::string(pad, ' ') << ": " << cmd.description << "\n";
    else
        os << indent << cmd.name << "\n";
    if(!cmd.command)
        os << indent << std::string(cmd.name.size(), '-') << "\n";
    for(auto& sc : cmd.subcommands)
        out_nested(os, sc, n+1);
}

}

std::ostream& operator<<(std::ostream& os, const subcommand& cmd)
{
    out_nested(os, cmd, 0);
    return os;
}
