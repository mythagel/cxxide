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
 * cmake.h
 *
 *  Created on: 2013-10-05
 *      Author: nicholas
 */

#ifndef CMAKE_H_
#define CMAKE_H_
#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include <memory>
#include <boost/filesystem.hpp>
#include "../../src/cmake/priv.h"

namespace cxxide
{
namespace cmake
{
namespace config
{

struct error : std::runtime_error
{
    error(const std::string& what);
    virtual ~error() noexcept;
};
struct error_unmanaged : error
{
    error_unmanaged();
    virtual ~error_unmanaged() noexcept;
};

//class directory
//{
//private:
//    std::shared_ptr<detail::directory_t> dir;
//public:
//    directory();
//    directory(std::shared_ptr<detail::directory_t> dir);
//};

//class configuration
//{
//private:
//    std::shared_ptr<detail::configuration_t> config;
//public:
//    configuration();
//    explicit configuration(std::shared_ptr<detail::configuration_t> config);

//    std::string name() const;
//    bool managed() const;

//    typedef std::set<std::string>::const_iterator package_iterator;
//    void add_package(const std::string& name);
//    void remove_package(const std::string& name);
//    package_iterator package_begin() const;
//    package_iterator package_end() const;
//};

void write(const boost::filesystem::path& root_path, const detail::configuration_t& config);
detail::configuration_t read(const boost::filesystem::path& root_path);

}
}
}

#endif /* CMAKE_H_ */
