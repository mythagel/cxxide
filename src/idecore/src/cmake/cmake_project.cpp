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
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <system_error>

namespace cxxide
{
namespace cmake
{

std::string project_t::name() const
{
    return configuration.name;
}

void project_t::generate()
{
    try
    {
        if(build_path.empty())
            throw error("build path empty");
        
        system::stream_t stream;
        
        auto args = std::vector<std::string>({"cmake", source_path, "-GNinja", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"});
        
        int err = system::exec(build_path, args, &stream);
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
            throw error("build path empty");
        
        system::stream_t stream;
        
        auto args = std::vector<std::string>({"cmake", "--build", build_path});
        
        int err = system::exec(build_path, args, &stream);
        if(err) throw error("cmake: " + stream.err);
    }
    catch(...)
    {
        std::throw_with_nested(error("cmake::build failed"));
    }
}

project_t create(const std::string& name, const std::string& source_path, const std::string& build_path)
{
    try
    {
        project_t project;
        project.source_path = source_path;
        project.build_path = build_path;
        
        project.configuration.name = name;
        write(source_path, project.configuration);

        return project;
    }
    catch(...)
    {
        std::throw_with_nested(error("cmake::create failed"));
    }
}

project_t open(const std::string& source_path, const std::string& build_path)
{
    try
    {
        project_t project;
        project.source_path = source_path;
        project.build_path = build_path;
        project.configuration = cmake::read(project.source_path);
        
        return project;
    }
    catch(...)
    {
        std::throw_with_nested(error("cmake::open failed"));
    }
}

}
}


