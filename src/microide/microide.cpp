#include "idecore.h"
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "linenoise.h"

using namespace cxxide;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace commands {
struct command;
}

void print_exception(const std::exception& e, int level = 0);

struct
{
    project::project_t* project = nullptr;
    std::vector<std::shared_ptr<commands::command>> commands;
} global;

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

// Return path when appended to a_From will resolve to same as a_To
boost::filesystem::path make_relative( boost::filesystem::path a_From, boost::filesystem::path a_To )
{
    a_From = boost::filesystem::absolute( a_From ); a_To = boost::filesystem::absolute( a_To );
    boost::filesystem::path ret;
    boost::filesystem::path::const_iterator itrFrom( a_From.begin() ), itrTo( a_To.begin() );
    // Find common base
    for( boost::filesystem::path::const_iterator toEnd( a_To.end() ), fromEnd( a_From.end() ) ; itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo );
    // Navigate backwards in directory to reach previously found base
    for( boost::filesystem::path::const_iterator fromEnd( a_From.end() ); itrFrom != fromEnd; ++itrFrom )
    {
        if( (*itrFrom) != "." )
            ret /= "..";
    }
    // Now navigate down the directory branch
    for( ; itrTo != a_To.end() ; ++itrTo )
        ret /= *itrTo;
    return ret;
}

}

/*

microide [--path x] [--build=path x] [--create x]
microide> blah commands


microide --create blah
> ls
CMakeLists.txt
cmake
README.md
> mkdir src
> cd src
> pwd
src
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
    
    virtual std::vector<std::string> completion(const std::vector<std::string>& args) =0;
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
            ("name", po::value<std::vector<std::string>>()->value_name("name")->required(), "Directory name")
        ;
        p.add("name", -1);
    }
    
    std::vector<std::string> completion(const std::vector<std::string>&) override
    {
        return {};
    }
    void execute(const std::vector<std::string>& args) override
    {
        po::variables_map vm;
        po::store(po::command_line_parser(args).options(options).positional(p).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return;
        }

        po::notify(vm);

        auto dirs = vm["name"].as<std::vector<std::string>>();
        for(auto& dir : dirs)
        {
            fs::path path = fs::current_path() / dir;
            global.project->directory_create(path);
        }

        global.project->write_config();
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
            ("path", po::value<std::string>()->value_name("name")->required(), "Directory name")
        ;
        p.add("path", 1);
    }
    
    std::vector<std::string> completion(const std::vector<std::string>& args) override
    {
        if(args.size() > 1) return {};
        std::vector<std::string> opts;

        fs::path path = fs::current_path();
        if(!args.empty())
            path = path / args[0];

        if(exists(path) && !is_child_of(canonical(absolute(path)), global.project->root()))
            return {};
        else if(exists(path.parent_path()) && !is_child_of(canonical(absolute(path.parent_path())), global.project->root()))
            return {};

        try
        {
            if (exists(path) && is_directory(path))
            {
                std::vector<fs::path> children;
                std::copy(fs::directory_iterator(path), fs::directory_iterator(), std::back_inserter(children));
                for(auto& child : children)
                {
                    if(is_directory(child))
                        opts.push_back(make_relative(fs::current_path(), child).native());
                }
            }
            else if(exists(path.parent_path()) && is_directory(path.parent_path()))
            {
                std::string partial = path.filename().native();
                path = path.parent_path();
                std::vector<fs::path> children;
                std::copy(fs::directory_iterator(path), fs::directory_iterator(), std::back_inserter(children));
                for(auto& child : children)
                {
                    if(is_directory(child))
                    {
                        std::string opt = make_relative(fs::current_path(), child).native();
                        if(child.filename().native().find(partial) == 0)
                            opts.push_back(opt);
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            print_exception(e);
        }
        return opts;
    }
    void execute(const std::vector<std::string>& args) override
    {
        po::variables_map vm;
        po::store(po::command_line_parser(args).options(options).positional(p).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return;
        }

        po::notify(vm);

        fs::path path = vm["path"].as<std::string>();
        path = absolute(path);
        if(!is_child_of(canonical(path), global.project->root()))
            throw std::runtime_error("cd outside of project path");

        current_path(path);
    }
    
    virtual ~cd()
    {
    }
};

struct pwd : command
{
    pwd()
     : command("pwd")
    {
        options.add_options()
            ("help", "display this help and exit")
        ;
    }
    
    std::vector<std::string> completion(const std::vector<std::string>&) override
    {
        return {};
    }
    void execute(const std::vector<std::string>& args) override
    {
        po::variables_map vm;
        po::store(po::command_line_parser(args).options(options).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return;
        }

        po::notify(vm);

        std::cout << make_relative(global.project->root(), fs::current_path()).native() << "\n";
    }
    
    virtual ~pwd()
    {
    }
};

struct ls : command
{
    po::positional_options_description p;

    ls()
     : command("ls")
    {
        options.add_options()
            ("help", "display this help and exit")
            ("path", po::value<std::string>()->value_name("name"), "Directory name")
        ;
        p.add("path", 1);
    }
    
    std::vector<std::string> completion(const std::vector<std::string>& args) override
    {
        if(args.size() > 1) return {};
        std::vector<std::string> opts;

        fs::path path;
        if(!args.empty())
            path = global.project->root() / args[0];
        else 
            path = fs::current_path();

        if(exists(path) && !is_child_of(canonical(absolute(path)), global.project->root()))
            return {};
        else if(exists(path.parent_path()) && !is_child_of(canonical(absolute(path.parent_path())), global.project->root()))
            return {};

        try
        {
            if (exists(path) && is_directory(path))
            {
                std::vector<fs::path> children;
                std::copy(fs::directory_iterator(path), fs::directory_iterator(), std::back_inserter(children));
                for(auto& child : children)
                {
                    opts.push_back(make_relative(fs::current_path(), child).native());
                }
            }
            else if(exists(path.parent_path()) && is_directory(path.parent_path()))
            {
                std::string partial = path.filename().native();
                path = path.parent_path();
                std::vector<fs::path> children;
                std::copy(fs::directory_iterator(path), fs::directory_iterator(), std::back_inserter(children));
                for(auto& child : children)
                {
                    std::string opt = make_relative(fs::current_path(), child).native();
                    if(child.filename().native().find(partial) == 0)
                        opts.push_back(opt);
                }
            }
        }
        catch (const std::exception& e)
        {
            print_exception(e);
        }
        return opts;
    }
    void execute(const std::vector<std::string>& args) override
    {
        po::variables_map vm;
        po::store(po::command_line_parser(args).options(options).positional(p).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return;
        }

        po::notify(vm);

        fs::path path = fs::current_path();
        if(vm.count("path"))
            path = path / vm["path"].as<std::string>();

        try
        {
            if (exists(path) && is_directory(path))
            {
                std::vector<fs::path> children;
                std::copy(fs::directory_iterator(path), fs::directory_iterator(), std::back_inserter(children));
                for(auto& child : children)
                {
                    std::cout << make_relative(fs::current_path(), child).native() << "\n";
                }
            }
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    
    virtual ~ls()
    {
    }
};

struct mktarget : command
{
    po::positional_options_description p;

    mktarget()
     : command("mktarget")
    {
        options.add_options()
            ("help", "display this help and exit")
            ("name", po::value<std::string>()->required(), "Name")
            ("type", po::value<std::string>()->required(), "Type {executable / shared_library / static_library}")
            ("label", po::value<std::string>(), "Label")
            ("output_name", po::value<std::string>(), "Output name")
            ("version", po::value<std::string>(), "Version")
            ("source,c", po::value<std::vector<std::string>>(), "Source file")
            ("define,D", po::value<std::vector<std::string>>(), "Definition")
            ("include,I", po::value<std::vector<std::string>>(), "Include")
            ("cflags", po::value<std::string>(), "Compile flags")
            ("ldflags", po::value<std::string>(), "Link flags")
            ("lib,l", po::value<std::vector<std::string>>(), "Libraries")
            ("pkg", po::value<std::vector<std::string>>(), "Packages")
        ;
        p.add("name", 1);
    }
    
    std::vector<std::string> completion(const std::vector<std::string>&) override
    {
        return {};
    }
    void execute(const std::vector<std::string>& args) override
    {
        po::variables_map vm;
        po::store(po::command_line_parser(args).options(options).positional(p).run(), vm);

        if(vm.count("help"))
        {
            std::cout << options << "\n";
            return;
        }

        po::notify(vm);

        fs::path path = fs::current_path();
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
        
        auto source_dir = global.project->directory(path);
        auto target = source_dir.target_add(name, type);
        
        if(vm.count("label"))
            target.label(vm["label"].as<std::string>());
        if(vm.count("output_name"))
            target.output_name(vm["output_name"].as<std::string>());
// todo
//            ("version", po::value<std::string>(), "Version")
//            ("source,c", po::value<std::vector<std::string>>(), "Source file")
//            ("define,D", po::value<std::vector<std::string>>(), "Definition")
//            ("include,I", po::value<std::vector<std::string>>(), "Include")
//            ("cflags", po::value<std::string>(), "Compile flags")
//            ("ldflags", po::value<std::string>(), "Link flags")
//            ("lib,l", po::value<std::vector<std::string>>(), "Libraries")
//            ("pkg", po::value<std::vector<std::string>>(), "Packages")
        
        global.project->write_config();
    }
    
    virtual ~mktarget()
    {
    }
};

}

void completion(const char* raw, linenoiseCompletions* lc)
try
{
    std::string buffer = raw;
    
    if(buffer.empty())
    {
        // list out top level commands.
        for(auto& cmd : global.commands)
            linenoiseAddCompletion(lc, cmd->name.c_str());
        return;
    }

    auto args = po::split_unix(buffer);
    if(args.empty())
        return;

    for(auto& cmd : global.commands)
    {
        if(cmd->name == args[0])
        {
            auto cmd_args = args;
            cmd_args.erase(cmd_args.begin());

            auto opts = cmd->completion(cmd_args);
            for(auto opt : opts)
            {
                opt = args[0] + " " + opt; 
                linenoiseAddCompletion(lc, opt.c_str());
            }
        }
        else if(cmd->name.find(args[0]) == 0)
        {
            linenoiseAddCompletion(lc, cmd->name.c_str());
        }
    }
}
catch(...)
{
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
            if(vm.count("build-path"))
                build_path /= name;
        }

        auto project = project::open(path, build_path);
        global.project = &project;

        current_path(path);

        global.commands.push_back(std::make_shared<commands::mkdir>());
        global.commands.push_back(std::make_shared<commands::cd>());
        global.commands.push_back(std::make_shared<commands::pwd>());
        global.commands.push_back(std::make_shared<commands::ls>());
        global.commands.push_back(std::make_shared<commands::mktarget>());

        while(auto raw = linenoise("microide> "))
        {
            std::string line = raw;
            free(raw);

            if(!line.empty())
            {
                auto args = po::split_unix(line);
                if(args.empty())
                    continue;

                bool command_found = false;
                for(auto& cmd : global.commands)
                {
                    if(cmd->name == args[0])
                    {
                        linenoiseHistoryAdd(line.c_str());

                        auto cmd_args = args;
                        cmd_args.erase(cmd_args.begin());
                        
                        try
                        {
                            cmd->execute(cmd_args);
                        }
                        catch(const std::exception& e)
                        {
                            print_exception(e);
                        }
                        command_found = true;
                    }
                }
                
                if(!command_found)
                    std::cout << "error: Unrecognised command.\n";
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

//int create_target(const std::vector<std::string>& args)
//{
//    fs::path path = vm["path"].as<std::string>();
//    fs::path build_path;
//    if(vm.count("build-path"))
//        build_path = vm["build-path"].as<std::string>();


//    auto project = project::open(path, build_path);



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

