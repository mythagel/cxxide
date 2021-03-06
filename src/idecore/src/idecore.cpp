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
 * idecore.cpp
 *
 *  Created on: 2013-10-03
 *      Author: nicholas
 */

#include "idecore.h"
#include <exception>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

namespace cxxide
{
namespace project
{

error::error(const std::string& what)
 : std::runtime_error(what)
{
}
error::~error() noexcept
{
}

std::string project_t::name() const
{
    return config.name();
}
fs::path project_t::root() const
{
    return config.root();
}

bool project_t::managed() const
{
    return config.managed();
}

void project_t::packages(const std::set<std::string>& pkgs)
{
    config.packages(pkgs);
}
std::set<std::string> project_t::packages() const
{
    return config.packages();
}

cmake::directory_t project_t::directory_create(const fs::path& path)
{
    return config.directory_create(path);
}

void project_t::directory_remove(boost::filesystem::path path)
{
    config.directory_remove(path);
}

cmake::directory_t project_t::directory(const fs::path& path)
{
    return config.directory(path);
}

void project_t::read_config()
{
    config.read();
}

void project_t::write_config()
{
    config.write();
}

void project_t::generate()
{
    config.generate();
}
void project_t::build()
{
    config.build();
}

project_t::~project_t()
{
}

project_t create(const std::string& name, const fs::path& path, const fs::path& build_path)
{
    if(!exists(path))
        throw error("source path does not exist");
    if(exists(path / name))
        throw std::runtime_error("Project folder already exists.");
    
    if(!build_path.empty() && !exists(build_path))
        throw error("build path does not exist");
    if(!build_path.empty() && exists(build_path / name))
        throw error("Project build folder already exists.");

    try
    {
        project_t project;
        
        create_directory(path / name);
        
        project.repo = git::init(path / name);
        
        if(!build_path.empty())
        {
            create_directory(build_path / name);
            project.config = cmake::create(name, path / name, build_path / name);
        }
        else
        {
            project.config = cmake::create(name, path / name);
        }

        {
            fs::ofstream os(path / name / "README.md");
            os << name << "\n";
            os << std::string(name.size(), '=') << "\n";
        }
        
        // TODO commit basic project structure to git.
        
        if(!build_path.empty())
        {
            project.config.generate();
            project.config.build();
        }

        return project;
    }
    catch(...)
    {
        try
        {
            remove_all(path / name);
            
            if(!build_path.empty())
                remove_all(build_path / name);
        }
        catch(...)
        {
            std::throw_with_nested(error("project::create failed (cleanup failed.)"));
        }
        std::throw_with_nested(error("project::create failed"));
    }
}

project_t open(const fs::path& path, const fs::path& build_path)
{
    if(!exists(path))
        throw error("source path does not exist");
    if(!build_path.empty() && !exists(build_path))
        throw error("build path does not exist");

    try
    {
        project_t project;
        
        /* Uses git to locate the project root path given any child path
         * Then passes that root path to the cmake subsystem to open. */
        project.repo = git::open(path);
        project.config = cmake::open(project.repo.root, build_path);

        return project;
    }
    catch(...)
    {
        std::throw_with_nested(error("project::open failed"));
    }
}

}
}

