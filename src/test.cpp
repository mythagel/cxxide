#include <iostream>
#include <exception>
#include "project.h"
#include <string>
#include <vector>

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

int main(int argc, char* argv[])
{
    std::vector<std::string> args(argv, argv + argc);
    args.erase(args.begin());
    
    std::string project_name;
    if(!args.empty())
    {
        project_name = args[0];
    }
    else
    {
        std::cerr << "Must specify project name to create.\n";
        return 1;
    }

    using namespace cxxide;
    try
    {
        auto project = project::create(project_name, "/home/nicholas/dev/build/cxxide/projects/", "/home/nicholas/dev/build/cxxide/projects/build/");
        std::cout << "Created project '" << project_name << "'.\n";
        
        // testing only.
        cmake::configuration_t config;
        config.name = project_name;
        config.packages.insert("Boost");
        
        config.directory.definitions.push_back("-DBLAH");
        config.directory.includes.push_back("/usr/local/include/");
        config.directory.compile_flags.cxx = "-std=c++11";
        config.directory.compile_flags.c = "-std=c11";
        
//        config.directory.subdirectories.push_back({"src", {}});
        cmake::write(std::string("/home/nicholas/dev/build/cxxide/projects/") + project_name, config);
        
        {
            auto config = cmake::read(std::string("/home/nicholas/dev/build/cxxide/projects/") + project_name);
            for(auto& pkg : config.packages)
                std::cout << "pkg: " << pkg << "\n";
        }
    }
    catch(const std::exception& e)
    {
        print_exception(e);
    }
    return 0;
}
