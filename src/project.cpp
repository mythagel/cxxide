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
 * project.cpp
 *
 *  Created on: 2013-10-03
 *      Author: nicholas
 */

#include "project.h"
#include <system_error>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <exception>

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

project_t::~project_t()
{
}

struct create_directory_nx
{
    bool release;
    std::string path;
    
    create_directory_nx(const std::string& base, const std::string& dir)
     : release(true)
    {
        int err;
        struct stat sb;
        
        if(base.back() == '/')
            path = base + dir;
        else
            path = base + '/' + dir;
        
        err = stat(path.c_str(), &sb);
        if(!err)
            throw std::runtime_error("Project folder already exists.");

        err = mkdir(path.c_str(), 0777);
        if(err)
            throw std::system_error(errno, std::system_category(), std::string("mkdir: ") + path);
    }
    
    void dismiss()
    {
        release = false;
    }
    
    std::string child(const std::string& file)
    {
        return path + '/' + file;
    }
    
    ~create_directory_nx()
    {
        if(release)
        {
            // TODO recursive
            int err = rmdir(path.c_str());
            if(err)
            {
                fprintf(stderr, "Unable to remove path '%s'\n", path.c_str());
            }
        }
    }
};

project_t create(const std::string& name, const std::string& path, const std::string& build_path)
{
    if(name.empty())
        throw error("project name empty");
    if(path.empty())
        throw error("source path empty");
    if(build_path.empty())
        throw error("build path empty");

    try
    {
        int err;
        struct stat sb;
        err = stat(path.c_str(), &sb);
        if(err)
            throw std::system_error(errno, std::system_category(), std::string("stat: ") + path);
        
        project_t project;
        
        create_directory_nx root(path, name);
        create_directory_nx build_root(build_path, name);

        project.repo = git::init(root.path.c_str());
        project.config = cmake::create(name, root.path, build_root.path);

        {
            std::ofstream os(root.child("README.md"));
            os << name << "\n";
            os << std::string('=', name.size()) << "\n";
        }
        
        // TODO commit basic project structure to git.
        
        project.config.configure();
        project.config.build();
        
        root.dismiss();
        build_root.dismiss();

        return project;
    }
    catch(...)
    {
        std::throw_with_nested(error("project::create failed"));
    }
}

}
}

