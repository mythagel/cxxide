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
 * cmake_project.h
 *
 *  Created on: 2013-11-11
 *      Author: nicholas
 */

#ifndef CMAKE_PROJECT_H_
#define CMAKE_PROJECT_H_
#include <string>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include "cmake.h"

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
friend project_t create(const std::string& name, const boost::filesystem::path& source_path, const boost::filesystem::path& build_path);
friend project_t open(const boost::filesystem::path& source_path, const boost::filesystem::path& build_path);

private:
    boost::filesystem::path source_path;
    boost::filesystem::path build_path;
    
    config::detail::configuration_t configuration;
public:
    std::string name() const;
    bool managed() const;

    void generate();
    void build();
};

struct 

project_t create(const std::string& name, const boost::filesystem::path& source_path, const boost::filesystem::path& build_path = {});
project_t open(const boost::filesystem::path& source_path, const boost::filesystem::path& build_path = {});

}
}

#endif /* CMAKE_PROJECT_H_ */
