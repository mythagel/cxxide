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
 * git.cpp
 *
 *  Created on: 2013-10-04
 *      Author: nicholas
 */

#include "git.h"
#include "exec.h"
#include <exception>

namespace cxxide
{
namespace git
{

error::error(const std::string& what)
 : std::runtime_error(what)
{
}
error::~error()
{
}

repo_t init(const std::string& path)
{
    try
    {
        int err = system::exec(path, {"git", "init", "-q"});
        if(err) throw error("git init failed");
        
        return { path };
    }
    catch(...)
    {
        std::throw_with_nested(error("git init failed"));
    }
}

}
}

