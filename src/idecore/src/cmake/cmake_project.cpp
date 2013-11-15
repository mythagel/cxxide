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

namespace fs = boost::filesystem;

namespace cxxide
{
namespace cmake
{

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

target_t directory_t::target_add(const std::string& name)
{
    auto& targets = directory.get().targets;
    if(target_exists(name, configuration)) throw error("named target already exists");

    targets.emplace_back();
    targets.back().name = name;
    
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

void project_t::packages(const std::set<std::string>& pkgs)
{
    configuration.packages = pkgs;
}
std::set<std::string> project_t::packages() const
{
    return configuration.packages;
}

directory_t project_t::directory(const fs::path& path)
{
    if(path.is_absolute())
        throw std::logic_error("relative path required.");

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
{
    try
    {
        configuration = cmake::config::read(source_path);
    }
    catch(...)
    {
        std::throw_with_nested(error("cmake::read failed"));
    }
}
void project_t::write()
{
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
}

void project_t::generate()
{
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
}
void project_t::build()
{
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
}

project_t create(const std::string& name, const fs::path& source_path, const fs::path& build_path)
{
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
}

project_t open(const fs::path& source_path, const fs::path& build_path)
{
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
}


