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

directory_t::directory_t(config::configuration_t& configuration, config::directory_t& directory)
 : configuration(std::ref(configuration)), directory(std::ref(directory))
{
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

directory_t project_t::directory(const boost::filesystem::path& path)
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


