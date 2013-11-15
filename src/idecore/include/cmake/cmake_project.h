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
#include <functional>

namespace cxxide
{
namespace cmake
{

struct error : std::runtime_error
{
    error(const std::string& what);
    virtual ~error() noexcept;
};

class file_t
{
private:
    std::reference_wrapper<config::configuration_t> configuration;
    std::reference_wrapper<config::directory_t> directory;
    std::reference_wrapper<config::file_t> file;
public:
    file_t(config::configuration_t& configuration, config::directory_t& directory, config::file_t& file);
};

class target_t
{
private:
    std::reference_wrapper<config::configuration_t> configuration;
    std::reference_wrapper<config::directory_t> directory;
    std::reference_wrapper<config::target_t> target;
public:
    target_t(config::configuration_t& configuration, config::directory_t& directory, config::target_t& target);
};

class directory_t
{
private:
    std::reference_wrapper<config::configuration_t> configuration;
    std::reference_wrapper<config::directory_t> directory;
public:
    directory_t(config::configuration_t& configuration, config::directory_t& directory);

    std::vector<std::string> definitions() const;
    void definitions(const std::vector<std::string>& defs);

    std::vector<std::string> includes() const;
    void includes(const std::vector<std::string>& incls);

    std::string compile_flags_cxx() const;
    std::string compile_flags_c() const;
    void compile_flags_cxx(const std::string& flags);
    void compile_flags_c(const std::string& flags);

    std::vector<config::directory_t::configure_file_t> configure_files() const;
    void configure_file_add(const std::string& input, const std::string& output);
    void configure_file_remove(const std::string& input);

    file_t file_add(const std::string& name);
    file_t file_get(const std::string& name);
    std::vector<file_t> files();
    file_t file_remove(const std::string& name);

    target_t target_add(const std::string& name);
    target_t target_get(const std::string& name);
    std::vector<target_t> targets();
    target_t target_remove(const std::string& name);

    /* Retrieve the directory entry for the given canonical, relative path.
     * It is an error to attempt to retrieve a path that does not exist. */
    directory_t directory(const boost::filesystem::path& path);
};

class project_t
{
friend project_t create(const std::string& name, const boost::filesystem::path& source_path, const boost::filesystem::path& build_path);
friend project_t open(const boost::filesystem::path& source_path, const boost::filesystem::path& build_path);

private:
    boost::filesystem::path source_path;
    boost::filesystem::path build_path;
    
    config::configuration_t configuration;
public:

    std::string name() const;
    bool managed() const;

    void packages(const std::set<std::string>& pkgs);
    std::set<std::string> packages() const;

    /* Retrieve the directory entry for the given canonical, relative path.
     * It is an error to attempt to retrieve a path that does not exist. */
    directory_t directory(const boost::filesystem::path& path = {});

    /* Read the configuration from disc.
     * Discards any unwritten changes. */
    void read();

    /* Write the in memory configuration to disc.
     * Only the managed sections are rewritten, all user configuration is maintained
     * May throw on errors or if the configuration is unmanaged.*/
    void write();

    /* Generate the cmake configuration.
     * Generally only needed when configuring a new build path.
     * TODO progress report & output. */
    void generate();

    /* Execute the build tool through cmake.
     * Very basic interface. It is expected that higher layers
     * will take finer control over the build process. */
    void build();
};

project_t create(const std::string& name, const boost::filesystem::path& source_path, const boost::filesystem::path& build_path = {});
project_t open(const boost::filesystem::path& source_path, const boost::filesystem::path& build_path = {});

}
}

#endif /* CMAKE_PROJECT_H_ */
