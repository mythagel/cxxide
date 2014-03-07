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
boost::filesystem::path make_relative( boost::filesystem::path from, boost::filesystem::path to )
{
    from = absolute(from);
    to = absolute(to);
    fs::path relative;

    auto itrFrom = from.begin();
    auto itrTo = to.begin();
    // Find common base
    for( ; itrFrom != from.end() && itrTo != to.end() && *itrFrom == *itrTo; ++itrFrom, ++itrTo );
    // Navigate backwards in directory to reach previously found base
    for( ; itrFrom != from.end(); ++itrFrom )
    {
        if( (*itrFrom) != "." )
            relative /= "..";
    }
    // Now navigate down the directory branch
    for( ; itrTo != to.end() ; ++itrTo )
        relative /= *itrTo;
    return relative;
}

auto parse_target_type(const std::string& token) -> cmake::config::target_t::type_t
{
    if(token == "executable")
        return cmake::config::target_t::executable;
    else if(token == "shared_library")
        return cmake::config::target_t::shared_library;
    else if(token == "static_library")
        return cmake::config::target_t::static_library;

    throw po::validation_error(po::validation_error::invalid_option_value, "type", token);
}

enum class param_action
{
    set,
    append,
    remove
};
auto set_target_params(po::variables_map& vm, cmake::target_t& target, param_action action = param_action::set) -> void
{
    if(vm.count("type"))
        target.type(parse_target_type(vm["type"].as<std::string>()));
    if(vm.count("label"))
    {
        auto label = vm["label"].as<std::string>();
        switch(action)
        {
            case param_action::set:
            case param_action::append:
                target.label(label);
                break;
            case param_action::remove:
                target.label({});
                break;
        }
    }
    if(vm.count("output_name"))
    {
        auto output_name = vm["output_name"].as<std::string>();
        switch(action)
        {
            case param_action::set:
            case param_action::append:
                target.output_name(output_name);
                break;
            case param_action::remove:
                target.output_name({});
                break;
        }
    }
    if(vm.count("version"))
    {
        auto version = cmake::config::target_t::version_t(vm["version"].as<std::string>());
        auto output_name = vm["output_name"].as<std::string>();
        switch(action)
        {
            case param_action::set:
            case param_action::append:
                target.version(version);
                break;
            case param_action::remove:
                target.version({});
                break;
        }
    }
    if(vm.count("source"))
    {
        auto sources = vm["source"].as<std::vector<std::string>>();
        switch(action)
        {
            case param_action::set:
                target.sources(sources);
                break;
            case param_action::append:
                for(auto& source : sources)
                    target.source_add(source);
                break;
            case param_action::remove:
                for(auto& source : sources)
                    target.source_remove(source);
                break;
        }
    }
    if(vm.count("define"))
    {
        auto definitions = vm["define"].as<std::vector<std::string>>();
        switch(action)
        {
            case param_action::set:
                target.definitions(definitions);
                break;
            case param_action::append:
                for(auto& define : definitions)
                    target.definition_add(define);
                break;
            case param_action::remove:
                for(auto& define : definitions)
                    target.definition_remove(define);
                break;
        }
    }
    if(vm.count("include"))
    {
        auto includes = vm["include"].as<std::vector<std::string>>();
        switch(action)
        {
            case param_action::set:
                target.includes(includes);
                break;
            case param_action::append:
                for(auto& include : includes)
                    target.include_add(include);
                break;
            case param_action::remove:
                for(auto& include : includes)
                    target.include_remove(include);
                break;
        }
    }
    if(vm.count("cflags"))
    {
        auto cflags = vm["cflags"].as<std::string>();
        switch(action)
        {
            case param_action::set:
                target.compile_flags(cflags);
                break;
            case param_action::append:
                target.compile_flags(target.compile_flags() + " " + cflags);
                break;
            case param_action::remove:
                target.compile_flags({});
                break;
        }
    }
    if(vm.count("ldflags"))
    {
        auto ldflags = vm["ldflags"].as<std::string>();
        switch(action)
        {
            case param_action::set:
                target.link_flags(ldflags);
                break;
            case param_action::append:
                target.link_flags(target.link_flags() + " " + ldflags);
                break;
            case param_action::remove:
                target.link_flags({});
                break;
        }
    }
    if(vm.count("lib"))
    {
        auto libs = vm["lib"].as<std::vector<std::string>>();
        switch(action)
        {
            case param_action::set:
                target.libs(libs);
                break;
            case param_action::append:
                for(auto& lib : libs)
                    target.lib_add(lib);
                break;
            case param_action::remove:
                for(auto& lib : libs)
                    target.lib_remove(lib);
                break;
        }
    }
    if(vm.count("pkg"))
    {
        auto packages = vm["pkg"].as<std::vector<std::string>>();
        switch(action)
        {
            case param_action::set:
                target.packages(packages);
                break;
            case param_action::append:
                for(auto& package : packages)
                    target.package_add(package);
                break;
            case param_action::remove:
                for(auto& package : packages)
                    target.package_remove(package);
                break;
        }
    }
}

std::string str(const cmake::target_t& target)
{
    std::stringstream s;

    s << "target: " << target.name() << "\n";

    if(!target.label().empty())
        s << "label: " << target.label() << "\n";

    if(!target.output_name().empty())
        s << "output_name: " << target.output_name() << "\n";

    switch(target.type())
    {
        case cmake::config::target_t::executable:
            s << "type: executable\n";
            break;
        case cmake::config::target_t::shared_library:
            s << "type: shared library\n";
            break;
        case cmake::config::target_t::static_library:
            s << "type: static library\n";
            break;
    }
    if(!target.version().empty())
        s << "version: " << target.version().major << "." << target.version().minor << "." << target.version().patch << "\n";

    if(!target.sources().empty())
    {
        auto sources = target.sources();
        s << "sources: ";
        std::copy(begin(sources), end(sources), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }

    if(!target.definitions().empty())
    {
        auto definitions = target.definitions();
        s << "definitions: ";
        std::copy(begin(definitions), end(definitions), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }

    if(!target.includes().empty())
    {
        auto includes = target.includes();
        s << "includes: ";
        std::copy(begin(includes), end(includes), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }

    if(!target.compile_flags().empty())
        s << "compile_flags: " << target.compile_flags() << "\n";
    if(!target.link_flags().empty())
        s << "link_flags: " << target.link_flags() << "\n";

    if(!target.libs().empty())
    {
        auto libs = target.libs();
        s << "libs: ";
        std::copy(begin(libs), end(libs), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }

    if(!target.packages().empty())
    {
        auto packages = target.packages();
        s << "packages: ";
        std::copy(begin(packages), end(packages), std::ostream_iterator<std::string>(s, ", "));
        s << "\n";
    }

    return s.str();
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

struct dir : command
{
    po::positional_options_description p;

    dir()
     : command("dir")
    {
        options.add_options()
            ("help", "display this help and exit")
            ("path", po::value<std::string>()->value_name("name"), "Directory name")
            ("define,D", po::value<std::vector<std::string>>(), "Definition")
            ("include,I", po::value<std::vector<std::string>>(), "Include")
            ("cflags", po::value<std::string>(), "C Compile flags")
            ("cxxflags", po::value<std::string>(), "C++ Compile flags")
            ("set", "Set parameter (default)")
            ("append", "Append to multivalue parameters")
            ("remove", "Remove value from multivalue parameters")
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

        auto source_dir = global.project->directory(path);

        param_action action = param_action::set;

        if(vm.count("set"))
            action = param_action::set;
        else if(vm.count("append"))
            action = param_action::append;
        else if(vm.count("remove"))
            action = param_action::remove;

        if(vm.count("define"))
        {
            auto definitions = vm["define"].as<std::vector<std::string>>();
            switch(action)
            {
                case param_action::set:
                    source_dir.definitions(definitions);
                    break;
                case param_action::append:
                    for(auto& define : definitions)
                        source_dir.definition_add(define);
                    break;
                case param_action::remove:
                    for(auto& define : definitions)
                        source_dir.definition_remove(define);
                    break;
            }
        }
        if(vm.count("include"))
        {
            auto includes = vm["include"].as<std::vector<std::string>>();
            switch(action)
            {
                case param_action::set:
                    source_dir.includes(includes);
                    break;
                case param_action::append:
                    for(auto& include : includes)
                        source_dir.include_add(include);
                    break;
                case param_action::remove:
                    for(auto& include : includes)
                        source_dir.include_remove(include);
                    break;
            }
        }
        if(vm.count("cflags"))
        {
            auto cflags = vm["cflags"].as<std::string>();
            switch(action)
            {
                case param_action::set:
                    source_dir.compile_flags_c(cflags);
                    break;
                case param_action::append:
                    source_dir.compile_flags_c(source_dir.compile_flags_c() + " " + cflags);
                    break;
                case param_action::remove:
                    source_dir.compile_flags_c({});
                    break;
            }
        }
        if(vm.count("cxxflags"))
        {
            auto cxxflags = vm["cxxflags"].as<std::string>();
            switch(action)
            {
                case param_action::set:
                    source_dir.compile_flags_cxx(cxxflags);
                    break;
                case param_action::append:
                    source_dir.compile_flags_cxx(source_dir.compile_flags_cxx() + " " + cxxflags);
                    break;
                case param_action::remove:
                    source_dir.compile_flags_cxx({});
                    break;
            }
        }

// TODO in file  command.
//std::vector<configure_file_t> configure_files;
//std::vector<file_t> files;
        global.project->write_config();
    }

    virtual ~dir()
    {
    }
};

struct rmdir : command
{
    po::positional_options_description p;

    rmdir()
     : command("rmdir")
    {
        options.add_options()
            ("help", "display this help and exit")
            ("name", po::value<std::string>()->value_name("name")->required(), "Directory name")
        ;
        p.add("name", 1);
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

        auto dir = vm["name"].as<std::string>();
        fs::path path = fs::current_path() / dir;
        global.project->directory_remove(path);

        global.project->write_config();
    }

    virtual ~rmdir()
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
            try
            {
                auto source_dir = global.project->directory(path);

                auto definitions = source_dir.definitions();
                for(auto& define : definitions)
                    std::cout << "define: " << define << "\n";

                auto includes = source_dir.includes();
                for(auto& include : includes)
                    std::cout << "include: " << include << "\n";

                if(!source_dir.compile_flags_cxx().empty())
                    std::cout << "cxxflags: " << source_dir.compile_flags_cxx() << "\n";

                if(!source_dir.compile_flags_c().empty())
                    std::cout << "cflags: " << source_dir.compile_flags_c() << "\n";

                auto configure_files = source_dir.configure_files();
                auto files = source_dir.files();

                auto targets = source_dir.targets();
                for(auto& target : targets)
                    std::cout << "target: " << target.name << "\n";

                auto subdirectories = source_dir.subdirectories();

                if (exists(path) && is_directory(path))
                {
                    std::vector<fs::path> children;
                    std::copy(fs::directory_iterator(path), fs::directory_iterator(), std::back_inserter(children));
                    for(auto& child : children)
                    {
                        auto child_path = make_relative(fs::current_path(), child);
                        bool written(false);
                        if(is_directory(child))
                        {
                            for(auto& dir : subdirectories)
                            {
                                if(dir == child_path.filename().native())
                                {
                                    std::cout << "* " << child_path.native() << "\n";
                                    written = true;
                                }
                            }
                        }
                        else
                        {
                            for(auto& cf : configure_files)
                            {
                                if(cf.input == child_path.filename().native())
                                {
                                    std::cout << "  " << child_path.native() << " -> " << cf.output << "\n";
                                    written = true;
                                }
                            }
                            for(auto& f : files)
                            {
                                if(f.name == child_path.filename().native())
                                {
                                    std::cout << "* " << child_path.native() << "\n";
                                    written = true;
                                }
                            }
                        }

                        if(!written)
                            std::cout << "  " << child_path.native() << "\n";
                    }
                }
            }
            catch(const cmake::error& e)
            {
                /* path is not a source directory.
                 * fallback to standard directory listing. */
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
        auto source_dir = global.project->directory(path);
        auto type = parse_target_type(vm["type"].as<std::string>());

        auto target = source_dir.target_add(name, type);
        set_target_params(vm, target);

        global.project->write_config();
    }

    virtual ~mktarget()
    {
    }
};

struct target : command
{
    po::positional_options_description p;

    target()
     : command("target")
    {
        options.add_options()
            ("help", "display this help and exit")
            ("name", po::value<std::string>()->required(), "Name")
            ("type", po::value<std::string>(), "Type {executable / shared_library / static_library}")
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
            ("set", "Set parameter (default)")
            ("append", "Append to multivalue parameters")
            ("remove", "Remove value from multivalue parameters")
        ;
        p.add("name", 1);
    }

    std::vector<std::string> completion(const std::vector<std::string>& args) override
    {
        if(args.size() > 1) return {};
        std::vector<std::string> opts;

        try
        {
            fs::path path = fs::current_path();
            auto source_dir = global.project->directory(path);

            auto targets = source_dir.targets();

            if(args.empty())
            {
                for(auto& target : targets)
                    opts.push_back(target.name);
            }
            else
            {
                auto t = args[0];
                for(auto& target : targets)
                {
                    if(target.name.find(t) == 0)
                        opts.push_back(target.name);
                }
            }
        }
        catch(const std::exception&)
        {
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
        auto name = vm["name"].as<std::string>();
        auto source_dir = global.project->directory(path);

        auto target = source_dir.target_get(name);
        if(vm.count("set"))
            set_target_params(vm, target, param_action::set);
        else if(vm.count("append"))
            set_target_params(vm, target, param_action::append);
        else if(vm.count("remove"))
            set_target_params(vm, target, param_action::remove);
        else    // default: set
            set_target_params(vm, target, param_action::set);

        global.project->write_config();

        std::cout << str(target) << "\n";
    }

    virtual ~target()
    {
    }
};

struct rmtarget : command
{
    po::positional_options_description p;

    rmtarget()
     : command("rmtarget")
    {
        options.add_options()
            ("help", "display this help and exit")
            ("name", po::value<std::string>()->required(), "Name")
        ;
        p.add("name", 1);
    }

    std::vector<std::string> completion(const std::vector<std::string>& args) override
    {
        if(args.size() > 1) return {};
        std::vector<std::string> opts;

        try
        {
            fs::path path = fs::current_path();
            auto source_dir = global.project->directory(path);

            auto targets = source_dir.targets();

            if(args.empty())
            {
                for(auto& target : targets)
                    opts.push_back(target.name);
            }
            else
            {
                auto t = args[0];
                for(auto& target : targets)
                {
                    if(target.name.find(t) == 0)
                        opts.push_back(target.name);
                }
            }
        }
        catch(const std::exception&)
        {
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
        auto name = vm["name"].as<std::string>();

        auto source_dir = global.project->directory(path);
        source_dir.target_remove(name);

        global.project->write_config();
    }

    virtual ~rmtarget()
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
            linenoiseAddCompletion(lc, (cmd->name + " ").c_str());
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
            linenoiseAddCompletion(lc, (cmd->name + " ").c_str());
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
        global.commands.push_back(std::make_shared<commands::dir>());
        global.commands.push_back(std::make_shared<commands::rmdir>());
        global.commands.push_back(std::make_shared<commands::cd>());
        global.commands.push_back(std::make_shared<commands::pwd>());
        global.commands.push_back(std::make_shared<commands::ls>());
        global.commands.push_back(std::make_shared<commands::mktarget>());
        global.commands.push_back(std::make_shared<commands::target>());
        global.commands.push_back(std::make_shared<commands::rmtarget>());

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

