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
 * exec.h
 *
 *  Created on: 2013-10-08
 *      Author: nicholas
 */

#ifndef EXEC_H_
#define EXEC_H_
#include <string>
#include <vector>
#include <stdexcept>

namespace cxxide
{
namespace system
{

struct error : std::runtime_error
{
    int code;
    error(const std::string& what, int code);
    error(const std::string& what);
    virtual ~error() noexcept;
};

struct stream_t
{
    std::string in;
    std::string out;
    std::string err;
};

int exec(const std::string& wd, const std::vector<std::string>& args);
int exec(const std::string& wd, const std::vector<std::string>& args, stream_t& stream);

}
}

#endif /* EXEC_H_ */
