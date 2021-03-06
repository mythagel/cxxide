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

struct target_t
{
    std::string name;
    std::string label;
    std::string output_name;
    enum type_t
    {
        executable,
        shared_library,
        static_library
    } type;
    struct version_t
    {
        int major;
        int minor;
        int patch;

        version_t();
        explicit version_t(const std::string& v);
        version_t(int major, int minor, int patch);

        bool empty() const;
    } version;
    std::vector<std::string> sources;
    std::vector<std::string> definitions;
    std::vector<std::string> includes;
    std::string compile_flags;
    std::string link_flags;
    std::vector<std::string> libs;
    std::vector<std::string> packages;

    target_t();
};

struct file_t
{
    std::string name;
    std::vector<std::string> definitions;
    std::string compile_flags;
};

struct directory_t
{
    std::vector<std::string> definitions;
    std::vector<std::string> includes;
    struct
    {
        std::string cxx;
        std::string c;
    } compile_flags;

    struct configure_file_t
    {
        std::string input;
        std::string output;
    };
    std::vector<configure_file_t> configure_files;

    std::vector<file_t> files;
    std::vector<target_t> targets;

    std::vector<std::pair<std::string, directory_t>> subdirectories;
};

struct configuration_t
{
    std::string name;
    bool managed;

    std::set<std::string> packages;
    directory_t directory;

    configuration_t();
};

void write(const boost::filesystem::path& root_path, const configuration_t& config);
configuration_t read(const boost::filesystem::path& root_path);

}
}
}

#endif /* CMAKE_H_ */
