#include <iostream>
#include <exception>
#include "project.h"
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>

using namespace cxxide;

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

std::string str(const std::string& name, const cmake::configuration_t::directory_t& dir, int level)
{
    std::stringstream s;
    std::string p(level, ' ');
    
    s << p << name << "\n";
    for(auto& def : dir.definitions)
        s << p << "define: '" << def << "'\n";
    for(auto& inc : dir.includes)
        s << p << "include: '" << inc << "'\n";
    s << p << "cxx_flags: '" << dir.compile_flags.cxx << "'\n";
    s << p << "c_flags: '" << dir.compile_flags.c << "'\n";
    for(auto& f : dir.configure_files)
        s << p << "conf_file: '" << f.input << "' -> '" << f.output << "'\n";
    for(auto& f : dir.files)
    {
        s << p << "file: '" << f.name << "' ";
        for(auto& def : f.definitions)
            s << "'" << def << "' ";
        s << "'" << f.compile_flags << "'\n";
    }
    
    // TODO targets;
    
    for(auto& d : dir.subdirectories)
        s << str(d.first, d.second, ++level);    
    
    return s.str();
}

std::string str(const cmake::configuration_t& config)
{
    std::stringstream s;
    
    s << config.name << "\n";
    s << (config.managed?"managed":"unmanaged") << "\n";
    for(auto& pkg : config.packages)
        s << "pkg: " << pkg << "\n";
    s << str("root", config.directory, 0);
    s << "\n";
    return s.str();
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
        config.directory.configure_files.push_back({"blah.cpp.in", "blah.cpp"});
        
        config.directory.files.push_back({"blah.cpp", {"-DBLEH", "-DFOO"}, "-Wall -Wextra"});
        
        mkdir((std::string("/home/nicholas/dev/build/cxxide/projects/") + project_name + "/src").c_str(), 0777);
        config.directory.subdirectories.push_back({"src", {}});
        
        std::cout << str(config) << "\n";
        
        cmake::write(std::string("/home/nicholas/dev/build/cxxide/projects/") + project_name, config);
        
        {
            std::cout << std::string(40, '-') << "\n";
            auto config = cmake::read(std::string("/home/nicholas/dev/build/cxxide/projects/") + project_name);
            std::cout << str(config) << "\n";
        }
    }
    catch(const std::exception& e)
    {
        print_exception(e);
    }
    return 0;
}
