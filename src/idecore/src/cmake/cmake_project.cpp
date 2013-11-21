/* 
 * Copyright (C) 2013  Nicholas Gill
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * cmake_project.cpp
 *
 *  Created on: 2013-11-11
 *      Author: nicholas
 */

#include "cmake/cmake_project.h"
#include "system/exec.h"
#include <iterator>
#include <algorithm>

namespace fs = boost::filesystem;

namespace cxxide
{
namespace cmake
{

namespace
{

bool target_exists(const std::string& name, const config::directory_t& directory)
{
    for(auto& target : directory.targets)
        if(target.name == name) return true;

    for(auto& dir : directory.subdirectories)
        if(target_exists(name, dir.second)) return true;

    return false;
}
bool target_exists(const std::string& name, const config::configuration_t& configuration)
{
    return target_exists(name, configuration.directory);
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

error::error(const std::string& what)
 : std::runtime_error(what)
{
}
error::~error() noexcept
{
}

file_t::file_t(config::configuration_t& configuration, config::directory_t& directory, config::file_t& file)
 : configuration(std::ref(configuration)), directory(std::ref(directory)), file(std::ref(file))
{
}

std::vector<std::string> file_t::definitions() const
{
    return file.get().definitions;
}
void file_t::definitions(const std::vector<std::string>& defs)
{
    file.get().definitions = defs;
}

std::string file_t::compile_flags() const
{
    return file.get().compile_flags;
}
void file_t::compile_flags(const std::string& flags)
{
    file.get().compile_flags = flags;
}

target_t::target_t(config::configuration_t& configuration, config::directory_t& directory, config::target_t& target)
 : configuration(std::ref(configuration)), directory(std::ref(directory)), target(std::ref(target))
{
}

std::string target_t::name() const
{
    return target.get().name;
}
void target_t::name(const std::string& name) const
{
    if(target_exists(name, configuration)) throw error("named target already exists");

    target.get().name = name;
}

std::string target_t::label() const
{
    return target.get().label;
}
void target_t::label(const std::string& label)
{
    target.get().label = label;
}

std::string target_t::output_name() const
{
    return target.get().output_name;
}
void target_t::output_name(const std::string& name)
{
    target.get().output_name = name;
}

config::target_t::type_t target_t::type() const
{
    return target.get().type;
}
void target_t::type(config::target_t::type_t type)
{
    target.get().type = type;
}

config::target_t::version_t target_t::version() const
{
    return target.get().version;
}
void target_t::version(const config::target_t::version_t& version)
{
    target.get().version = version;
}

std::vector<std::string> target_t::sources() const
{
    return target.get().sources;
}
void target_t::sources(const std::vector<std::string>& sources)
{
    target.get().sources = sources;
}
void target_t::source_add(const std::string& source)
{
    auto& sources = target.get().sources;
    if(std::count(begin(sources), end(sources), source))
        throw error("source exists");
    sources.push_back(source);
}
void target_t::source_remove(const std::string& source)
{
    auto& sources = target.get().sources;
    sources.erase(std::remove(begin(sources), end(sources), source), end(sources));
}

std::vector<std::string> target_t::definitions() const
{
    return target.get().definitions;
}
void target_t::definitions(const std::vector<std::string>& definitions)
{
    target.get().definitions = definitions;
}
void target_t::definition_add(const std::string& define)
{
    auto& definitions = target.get().definitions;
    if(std::count(begin(definitions), end(definitions), define))
        throw error("define exists");
    definitions.push_back(define);
}
void target_t::definition_remove(const std::string& define)
{
    auto& definitions = target.get().definitions;
    definitions.erase(std::remove(begin(definitions), end(definitions), define), end(definitions));
}

std::vector<std::string> target_t::includes() const
{
    return target.get().includes;
}
void target_t::includes(const std::vector<std::string>& includes)
{
    target.get().includes = includes;
}
void target_t::include_add(const std::string& include)
{
    auto& includes = target.get().includes;
    if(std::count(begin(includes), end(includes), include))
        throw error("include exists");
    includes.push_back(include);
}
void target_t::include_remove(const std::string& include)
{
    auto& includes = target.get().includes;
    includes.erase(std::remove(begin(includes), end(includes), include), end(includes));
}

std::string target_t::compile_flags() const
{
    return target.get().compile_flags;
}
void target_t::compile_flags(const std::string& flags)
{
    target.get().compile_flags = flags;
}

std::string target_t::link_flags() const
{
    return target.get().link_flags;
}
void target_t::link_flags(const std::string& flags)
{
    target.get().link_flags = flags;
}

std::vector<std::string> target_t::libs() const
{
    return target.get().libs;
}
void target_t::libs(const std::vector<std::string>& libs)
{
    target.get().libs = libs;
}
void target_t::lib_add(const std::string& lib)
{
    auto& libs = target.get().libs;
    if(std::count(begin(libs), end(libs), lib))
        throw error("lib exists");
    libs.push_back(lib);
}
void target_t::lib_remove(const std::string& lib)
{
    auto& libs = target.get().libs;
    libs.erase(std::remove(begin(libs), end(libs), lib), end(libs));
}

std::vector<std::string> target_t::packages() const
{
    return target.get().packages;
}
void target_t::packages(const std::vector<std::string>& packages)
{
    target.get().packages = packages;
}
void target_t::package_add(const std::string& package)
{
    auto& packages = target.get().packages;
    if(std::count(begin(packages), end(packages), package))
        throw error("package exists");
    packages.push_back(package);
}
void target_t::package_remove(const std::string& package)
{
    auto& packages = target.get().packages;
    packages.erase(std::remove(begin(packages), end(packages), package), end(packages));
}

directory_t::directory_t(config::configuration_t& configuration, config::directory_t& directory)
 : configuration(std::ref(configuration)), directory(std::ref(directory))
{
}

std::vector<std::string> directory_t::definitions() const
{
    return directory.get().definitions;
}
void directory_t::definitions(const std::vector<std::string>& defs)
{
    directory.get().definitions = defs;
}

std::vector<std::string> directory_t::includes() const
{
    return directory.get().includes;
}
void directory_t::includes(const std::vector<std::string>& incls)
{
    directory.get().includes = incls;
}

std::string directory_t::compile_flags_cxx() const
{
    return directory.get().compile_flags.cxx;
}
std::string directory_t::compile_flags_c() const
{
    return directory.get().compile_flags.c;
}
void directory_t::compile_flags_cxx(const std::string& flags)
{
    directory.get().compile_flags.cxx = flags;
}
void directory_t::compile_flags_c(const std::string& flags)
{
    directory.get().compile_flags.c = flags;
}

std::vector<config::directory_t::configure_file_t> directory_t::configure_files() const
{
    return directory.get().configure_files;
}
void directory_t::configure_file_add(const std::string& input, const std::string& output)
{
    auto& configure_files = directory.get().configure_files;

    for(auto& cf : configure_files)
        if(cf.input == input) throw error("configure file exists");

    configure_files.push_back({ input, output });
}
void directory_t::configure_file_remove(const std::string& input)
{
    auto& configure_files = directory.get().configure_files;
    for(auto it = begin(configure_files); it != end(configure_files); )
    {
        if(it->input == input)
            it = configure_files.erase(it);
        else
            ++it;
    }
}

file_t directory_t::file_add(const std::string& name)
{
    auto& files = directory.get().files;

    for(auto& file : files)
        if(file.name == name) throw error("file exists");

    files.emplace_back();
    files.back().name = name;

    return { configuration, directory, files.back() };
}
file_t directory_t::file_get(const std::string& name)
{
    auto& files = directory.get().files;

    for(auto& file : files)
        if(file.name == name) return { configuration, directory, file };

    throw error("file not found");
}
std::vector<config::file_t> directory_t::files() const
{
    return directory.get().files;
}
void directory_t::file_remove(const std::string& name)
{
    auto& files = directory.get().files;
    for(auto it = begin(files); it != end(files); )
    {
        if(it->name == name)
            it = files.erase(it);
        else
            ++it;
    }
}

target_t directory_t::target_add(const std::string& name, config::target_t::type_t type)
{
    auto& targets = directory.get().targets;
    if(target_exists(name, configuration)) throw error("target already exists");

    targets.emplace_back();
    targets.back().name = name;
    targets.back().type = type;
    
    return { configuration, directory, targets.back() };
}
target_t directory_t::target_get(const std::string& name)
{
    auto& targets = directory.get().targets;

    for(auto& target : targets)
        if(target.name == name) return { configuration, directory, target };

    throw error("target not found");
}
std::vector<config::target_t> directory_t::targets() const
{
    return directory.get().targets;
}
void directory_t::target_remove(const std::string& name)
{
    auto& targets = directory.get().targets;
    for(auto it = begin(targets); it != end(targets); )
    {
        if(it->name == name)
            it = targets.erase(it);
        else
            ++it;
    }
}

std::vector<std::string> directory_t::subdirectories() const
{
    std::vector<std::string> ret;
    for(auto& dir : directory.get().subdirectories)
        ret.push_back(dir.first);
    return ret;
}

directory_t directory_t::subdirectory(const fs::path& path)
{
    if(path.is_absolute())
        throw std::logic_error("relative path required.");

    auto cur = directory;
    for(auto& dir : path)
    {
        auto it = find_if(begin(cur.get().subdirectories), end(cur.get().subdirectories),
            [&dir](const std::pair<std::string, config::directory_t>& e) -> bool
            {
                return e.first == dir.native();
            });

        if(it == end(cur.get().subdirectories))
            throw error(path.native() + " : Not found");

        cur = std::ref(it->second);
    }

    return { configuration, cur };
}

std::string project_t::name() const
{
    return configuration.name;
}
bool project_t::managed() const
{
    return configuration.managed;
}
fs::path project_t::root() const
{
    return source_path;
}

void project_t::packages(const std::set<std::string>& pkgs)
{
    configuration.packages = pkgs;
}
std::set<std::string> project_t::packages() const
{
    return configuration.packages;
}

directory_t project_t::directory_create(fs::path path)
{
    path = make_relative(source_path, path);

    try
    {
        // Create the filesystem paths
        auto full_path = source_path / path;
        create_directories(full_path);
        // Create the entries in the configuration.
        auto cur = std::ref(configuration.directory);
        for(auto& dir : path)
        {
            auto it = find_if(begin(cur.get().subdirectories), end(cur.get().subdirectories),
                [&dir](const std::pair<std::string, config::directory_t>& e) -> bool
                {
                    return e.first == dir.native();
                });

            if(it == end(cur.get().subdirectories))
            {
                cur.get().subdirectories.emplace_back(dir.native(), config::directory_t());
                cur = std::ref(cur.get().subdirectories.back().second);
            }
            else
            {
                cur = std::ref(it->second);
            }
        }

        return { configuration, cur };
    }
    catch(...)
    {
        std::throw_with_nested(error("cmake::write failed"));
    }
}

directory_t project_t::directory(fs::path path)
{
    path = make_relative(source_path, path);

    auto cur = std::ref(configuration.directory);
    for(auto& dir : path)
    {
        auto it = find_if(begin(cur.get().subdirectories), end(cur.get().subdirectories),
            [&dir](const std::pair<std::string, config::directory_t>& e) -> bool
            {
                return e.first == dir.native();
            });

        if(it == end(cur.get().subdirectories))
            throw error(path.native() + " : Not found");

        cur = std::ref(it->second);
    }

    return { configuration, cur };
}

void project_t::read()
try
{
    configuration = cmake::config::read(source_path);
}
catch(...)
{
    std::throw_with_nested(error("cmake::read failed"));
}
void project_t::write()
try
{
    cmake::config::write(source_path, configuration);
}
catch(const cmake::config::error_unmanaged& e)
{
    configuration.managed = false;
    std::throw_with_nested(error("cmake::write failed"));
}
catch(...)
{
    std::throw_with_nested(error("cmake::write failed"));
}

void project_t::generate()
try
{
    if(build_path.empty())
        throw std::logic_error("generate: build path empty");

    auto args = std::vector<std::string>({"cmake", source_path.native(), "-GNinja", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"});

    system::stream_t stream;
    int err = system::exec(build_path.native(), args, &stream);
    if(err) throw error("cmake: " + stream.err);
}
catch(...)
{
    std::throw_with_nested(error("cmake::generate failed"));
}
void project_t::build()
try
{
    if(build_path.empty())
        throw std::logic_error("build: build path empty");

    auto args = std::vector<std::string>({"cmake", "--build", build_path.native()});

    system::stream_t stream;
    int err = system::exec(build_path.native(), args, &stream);
    if(err) throw error("cmake: " + stream.err);
}
catch(...)
{
    std::throw_with_nested(error("cmake::build failed"));
}

project_t create(const std::string& name, const fs::path& source_path, const fs::path& build_path)
try
{
    project_t project;
    project.source_path = canonical(source_path);
    project.build_path = canonical(build_path);

    project.configuration.name = name;
    cmake::config::write(source_path, project.configuration);

    return project;
}
catch(...)
{
    std::throw_with_nested(error("cmake::create failed"));
}

project_t open(const fs::path& source_path, const fs::path& build_path)
try
{
    project_t project;
    project.source_path = canonical(source_path);
    project.build_path = canonical(build_path);
    project.configuration = cmake::config::read(project.source_path);

    return project;
}
catch(...)
{
    std::throw_with_nested(error("cmake::open failed"));
}

}
}


