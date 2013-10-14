/* cxxcam - C++ CAD/CAM driver library.
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
 * cmake.cpp
 *
 *  Created on: 2013-10-05
 *      Author: nicholas
 */

#include "cmake.h"
#include <fstream>
#include "exec.h"
#include <vector>
#include <exception>

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

void project_t::configure()
{
    try
    {
        system::stream_t stream;
        
        auto args = std::vector<std::string>({"cmake", source_path, "-GNinja"});
        
        int err = system::exec(build_path, args, &stream);
        if(err) throw error("cmake: " + stream.err);
    }
    catch(...)
    {
        std::throw_with_nested(error("git::init failed"));
    }
}
void project_t::build()
{
    try
    {
        system::stream_t stream;
        
        auto args = std::vector<std::string>({"cmake", "--build", build_path});
        
        int err = system::exec(build_path, args, &stream);
        if(err) throw error("cmake: " + stream.err);
    }
    catch(...)
    {
        std::throw_with_nested(error("git::init failed"));
    }
}

project_t create(const std::string& name, const std::string& source_path, const std::string& build_path)
{
    project_t project;
    project.source_path = source_path;
    project.build_path = build_path;
    
    std::ofstream os(source_path + '/' + "CMakeLists.txt");
    os << "CMAKE_MINIMUM_REQUIRED(VERSION 2.6)\n";
    os << "PROJECT(" << name << ")\n";

    return project;
}

}
}

