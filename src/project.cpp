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

namespace cxxide
{
namespace project
{

project_t create(const std::string& name, const std::string& path, const std::string& build_path)
{
    int err;
    struct stat sb;
    err = stat(path.c_str(), &sb);
    if(err)
        throw std::system_error(errno, std::system_category(), std::string("stat: ") + path);
    
    project_t project;
    
    std::string root;
    if(path.back() == '/')
        root = path + name;
    else
        root = path + '/' + name;
    
    err = stat(root.c_str(), &sb);
    if(!err)
        throw std::runtime_error("Project folder already exists.");

    err = mkdir(root.c_str(), 0777);
    if(err)
        throw std::system_error(errno, std::system_category(), "mkdir");

    project.repo = git::init(root.c_str());

    {    
        std::ofstream os(root + '/' + "CMakeLists.txt");
        os << "CMAKE_MINIMUM_REQUIRED(VERSION 2.6)\n";
        os << "PROJECT(" << name << ")\n";
    }
    
    // TODO commit basic project structure to git.
    
    return project;
}

}
}

