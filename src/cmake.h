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
 * cmake.h
 *
 *  Created on: 2013-10-05
 *      Author: nicholas
 */

#ifndef CMAKE_H_
#define CMAKE_H_
#include <string>
#include <stdexcept>

namespace cxxide
{
namespace cmake
{

struct error : std::runtime_error
{
    error(const std::string& what);
    virtual ~error() noexcept;
};

class project_t
{
friend project_t create(const std::string& name, const std::string& source_path, const std::string& build_path);;

private:
    std::string source_path;
    std::string build_path;
public:
    void configure();
    void build();
};

project_t create(const std::string& name, const std::string& source_path, const std::string& build_path);

}
}

#endif /* CMAKE_H_ */
