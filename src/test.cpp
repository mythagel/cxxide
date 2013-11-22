#include <iostream>
#include <exception>
#include "idecore.h"
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <iterator>

using namespace cxxide;
using namespace cxxide::cmake::config;

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

std::string str(const target_t& target, int level)
{
    std::stringstream s;
    std::string p(level, ' ');
    
    s << p << "target: " << target.name << "\n";
    
    if(!target.label.empty())
        s << p << "label: " << target.label << "\n";

    if(!target.output_name.empty())
        s << p << "output_name: " << target.output_name << "\n";

    switch(target.type)
    {
        case target_t::executable:
            s << p << "type: executable\n";
            break;
        case target_t::shared_library:
            s << p << "type: shared library\n";
            break;
        case target_t::static_library:
            s << p << "type: static library\n";
            break;
    }
    if(!target.version.empty())
        s << p << "version: " << target.version.major << "." << target.version.minor << "." << target.version.patch << "\n";
    
    if(!target.sources.empty())
    {
        s << p << "sources: ";
        std::copy(begin(target.sources), end(target.sources), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }
    
    if(!target.definitions.empty())
    {
        s << p << "definitions: ";
        std::copy(begin(target.definitions), end(target.definitions), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }
    
    if(!target.includes.empty())
    {
        s << p << "includes: ";
        std::copy(begin(target.includes), end(target.includes), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }
    
    if(!target.compile_flags.empty())
        s << p << "compile_flags: " << target.compile_flags << "\n";
    if(!target.link_flags.empty())
        s << p << "link_flags: " << target.link_flags << "\n";
    
    if(!target.libs.empty())
    {
        s << p << "libs: ";
        std::copy(begin(target.libs), end(target.libs), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }
    
    if(!target.packages.empty())
    {
        s << p << "packages: ";
        std::copy(begin(target.packages), end(target.packages), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }
    
    return s.str();
}

std::string str(const std::string& name, const directory_t& dir, int level)
{
    std::stringstream s;
    std::string p(level, ' ');
    
    s << p << name << "\n";
    for(auto& def : dir.definitions)
        s << p << "define: '" << def << "'\n";
    for(auto& inc : dir.includes)
        s << p << "include: '" << inc << "'\n";
    if(!dir.compile_flags.cxx.empty())
        s << p << "cxx_flags: '" << dir.compile_flags.cxx << "'\n";
    if(!dir.compile_flags.c.empty())
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
    
    for(auto& t : dir.targets)
        s << str(t, level);
    
    for(auto& d : dir.subdirectories)
        s << str(name + '/' + d.first, d.second, ++level);    
    
    return s.str();
}

std::string str(const configuration_t& config)
{
    std::stringstream s;
    
    s << config.name << "\n";
    s << (config.managed?"managed":"unmanaged") << "\n";
    for(auto& pkg : config.packages)
        s << "pkg: " << pkg << "\n";
    s << str("root", config.directory, 1);
    s << "\n";
    return s.str();
}

int main(int argc, char* argv[])
{
    {
        //target_t::version_t t1("");
        //target_t::version_t t2(".");
        target_t::version_t t3("1");
        //target_t::version_t t4("a");
        //target_t::version_t t5("1.");
        //target_t::version_t t6("a.");
        target_t::version_t t7("1.3");
        //target_t::version_t t8("1.a");
        //target_t::version_t t9("1.3.");
        target_t::version_t ta("1.3.1");
        //target_t::version_t tb("1.3.a");
        //target_t::version_t tc("1.a.a");
        //target_t::version_t td("1.3.1.");
        //target_t::version_t te("1.3.1.a");
        //target_t::version_t tf("1.3.1.3");
        //target_t::version_t tg("1.3.1.3a");
    }

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
        configuration_t config;
        config.name = project_name;
        config.packages.insert("Boost");
        config.packages.insert("X11");
        
        config.directory.definitions.push_back("-DBLAH");
        config.directory.includes.push_back("/usr/local/include/");
        config.directory.compile_flags.cxx = "-std=c++11";
        config.directory.compile_flags.c = "-std=c11";
        config.directory.configure_files.push_back({"blah.cpp.in", "blah.cpp"});
        
        config.directory.files.push_back({"blah.cpp", {"-DBLEH", "-DFOO"}, "-Wall -Wextra"});
        config.directory.files.push_back({"b.cpp", {"-DBBLEH", "-DBFOO"}, "-Wall -Wextra"});
        
        target_t foo;
        foo.name = "foo";
        foo.label = "Foo executable";
        foo.output_name = "foo";
        foo.type = target_t::executable;
        foo.version = {1, 2, 3};
        foo.sources = {"a.cpp", "b.cpp", "c.cpp"};
        foo.definitions = {"-DFOOFOO", "-DFOOBAR"};
        foo.includes = {"foo", "/usr/include/foo/"};
        foo.compile_flags = "-Wno-unused-parameters";
        foo.link_flags = "-fPIC";
        foo.libs = {"m", "pthread"};
        foo.packages = {"Boost", "X11"};
        config.directory.targets.push_back(foo);
        
        mkdir((std::string("/home/nicholas/dev/build/cxxide/projects/") + project_name + "/src").c_str(), 0777);
        config.directory.subdirectories.push_back({"src", directory_t()});
        
        std::string written;
        {
            std::ostringstream s;
            s << str(config) << "\n";
            written = s.str();
            std::cout << written;
        }
        
        cmake::config::write(std::string("/home/nicholas/dev/build/cxxide/projects/") + project_name, config);
        
        //project.generate();
        
        {
            std::cout << std::string(40, '-') << "\n";
            auto config = cmake::config::read(std::string("/home/nicholas/dev/build/cxxide/projects/") + project_name);
            std::string read;
            {
                std::ostringstream s;
                s << str(config) << "\n";
                read = s.str();
                std::cout << read;
            }
            if(read != written)
                std::cout << "ERROR - WRITTEN AND READ DIFFER!\n";
        }
        
        {
            auto project = project::open("/home/nicholas/dev/build/cxxide/projects/" + project_name, "/home/nicholas/dev/build/cxxide/projects/build/" + project_name);
            std::cout << "name: " << project.name() << "\n";
        }
    }
    catch(const std::exception& e)
    {
        print_exception(e);
    }
    return 0;
}
