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

namespace
{

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

bool is_child_of(const fs::path& child, const fs::path& path)
{
    using std::begin;
    using std::end;
    auto path_mag = std::distance(begin(path), end(path));
    auto child_mag = std::distance(begin(child), end(child));
    if(path_mag > child_mag) return false;

    return std::equal(begin(path), end(path), begin(child));
}

}

/*

microide [--path x] [--build=path x] [--create x]
microide> blah commands


microide --create blah
> mkdir src
> cd src
> pwd
/src
> mktarget --type executable test
> 

source create header blah [--lang c++/c]
source create source blah [--lang c++/c]
source create tu blah [--lang c++/c]

*/

namespace commands
{

struct command
{
    std::string name;
    po::options_description options;
    
    command(const std::string& name)
     : name(name), options(name + " options")
    {
    }
    
    virtual std::vector<std::string> completion(const std::string& buffer) =0;
    virtual void execute(const std::vector<std::string>& args) =0;
    virtual ~command()
    {
    
    }
};

struct mkdir : command
{
    po::positional_options_description p;

    mkdir()
     : command("mkdir")
    {
        options.add_options()
            ("help", "display this help and exit")
            ("name", po::value<std::vector<std::string>>()->value_name("name"), "Directory name")
        ;
        p.add("name", -1);
    }
    
    std::vector<std::string> completion(const std::string& buffer) override
    {
        // todo
    }
    void execute(const std::vector<std::string>& args) override
    {
        po::variables_map vm;
        po::store(po::command_line_parser(ac, av). 
                  options(options).positional(p).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return;
        }

        po::notify(vm);

        auto dirs = vm["name"].as<std::vector<std::string>>();
        for(auto& dir : dirs)
        {
            fs::path path = dir;
            project.directory_create(absolute(path));
        }
    }
    
    virtual ~mkdir()
    {
    }
};

struct cd : command
{
    po::positional_options_description p;

    cd()
     : command("cd")
    {
        options.add_options()
            ("help", "display this help and exit")
            ("path", po::value<std::string>()->value_name("name"), "Directory name")
        ;
        p.add("path", 1);
    }
    
    std::vector<std::string> completion(const std::string& buffer) override
    {
        // todo expand path
    }
    void execute(const std::vector<std::string>& args) override
    {
        po::variables_map vm;
        po::store(po::command_line_parser(ac, av). 
                  options(options).positional(p).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return;
        }

        po::notify(vm);

        fs::path path = vm["path"].as<std::string>();
        if(!is_child_of(path, project.root()))
            throw std::runtime_error("cd outside of project path");
        current_path(path);
    }
    
    virtual ~cd()
    {
    }
};

}

void completion(const char* raw, linenoiseCompletions* lc)
{
    std::string buffer = raw;
    
    if(buffer.empty())
    {
        // list out top level commands.
        linenoiseAddCompletion(lc, "option1");
        return;
    }

    auto args = po::split_unix(buffer);
    
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
            ("path", po::value<std::string>()->value_name("path")->default_value(fs::current_path().native()), "Project path")
            ("build-path", po::value<std::string>()->value_name("path"), "Project build path")
            ("create", po::value<std::string>()->value_name("name"), "Create a new project")
        ;

        po::variables_map vm;
        store(po::command_line_parser(args).options(options).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return 0;
        }
        notify(vm);

        fs::path path = vm["path"].as<std::string>();
        fs::path build_path;
        if(vm.count("build-path"))
            build_path = vm["build-path"].as<std::string>();

        if(vm.count("create"))
        {
            auto name = vm["create"].as<std::string>();
            project::create(name, path, build_path);
            
            path /= name;
        }

        auto project = project::open(path, build_path);

        while(auto raw = linenoise("microide> "))
        {
            std::string line = raw;
            free(raw);

            if(!line.empty())
            {
                linenoiseHistoryAdd(line.c_str());
                
                // parse and execute a command.
                std::cout << line << "\n";

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

//int create_dir(const std::vector<std::string>& args)
//{
//    fs::path path = vm["path"].as<std::string>();
//    fs::path build_path;
//    if(vm.count("build-path"))
//        build_path = vm["build-path"].as<std::string>();
//    auto name = vm["name"].as<std::string>();

//    auto project = project::open(path, build_path);

//    project.directory_create(path / name);
//    project.write_config();

//    return 0;
//}

//int create_target(const std::vector<std::string>& args)
//{
//    fs::path path = vm["path"].as<std::string>();
//    fs::path build_path;
//    if(vm.count("build-path"))
//        build_path = vm["build-path"].as<std::string>();
//    auto name = vm["name"].as<std::string>();
//    auto type = [](const std::string& token) -> cmake::config::target_t::type_t
//    {
//        if(token == "executable")
//            return cmake::config::target_t::executable;
//        else if(token == "shared_library")
//            return cmake::config::target_t::shared_library;
//        else if(token == "static_library")
//            return cmake::config::target_t::static_library;

//        throw po::validation_error(po::validation_error::invalid_option_value, "type", token);
//    }(vm["type"].as<std::string>());

//    auto project = project::open(path, build_path);

//    auto source_dir = project.directory(path);
//    source_dir.target_add(name, type);
//    project.write_config();

//    return 0;
//}

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

