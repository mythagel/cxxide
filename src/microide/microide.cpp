#include "idecore.h"
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "linenoise.h"

using namespace cxxide;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

void print_exception(const std::exception& e, int level = 0);

/*

use linenoise 

microide --create blah
microide [--path x] [--build=path x] [--exec "cmd"]

microide> blah commands

source create header blah [--lang c++/c]
source create source blah [--lang c++/c]
source create tu blah [--lang c++/c]

*/

void completion(const char* raw, linenoiseCompletions* lc)
{
    std::string buffer = raw;
    
    if (buffer[0] == 'h')
    {
        linenoiseAddCompletion(lc, "hello");
        linenoiseAddCompletion(lc, "hello there");
    }
}

int main(int argc, char* argv[])
{
    po::options_description options("microide options");
    std::vector<std::string> args(argv, argv + argc);
    args.erase(begin(args));

    linenoiseSetCompletionCallback(completion);

    try
    {
        options.add_options()
            ("help", "display this help and exit")
            ("path", po::value<std::string>()->default_value(fs::current_path().native()), "Project path")
            ("build-path", po::value<std::string>(), "Project build path")
        ;

        po::variables_map vm;
        store(po::command_line_parser(args).options(options).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return 0;
        }
        notify(vm);

        while(auto raw = linenoise("microide> "))
        {
            std::string line = raw;
            free(raw);

            if(!line.empty())
            {
                std::cout << line << "\n";
                // something
            }
        }
    }
    catch(const po::error& e)
    {
        print_exception(e);
        std::cout << options << "\n";
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
