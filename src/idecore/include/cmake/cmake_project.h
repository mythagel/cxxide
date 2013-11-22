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
    
    std::vector<std::string> definitions() const;
    void definitions(const std::vector<std::string>& defs);
    
    std::string compile_flags() const;
    void compile_flags(const std::string& flags);
};

class target_t
{
private:
    std::reference_wrapper<config::configuration_t> configuration;
    std::reference_wrapper<config::directory_t> directory;
    std::reference_wrapper<config::target_t> target;
public:
    target_t(config::configuration_t& configuration, config::directory_t& directory, config::target_t& target);

    std::string name() const;
    // Note renaming a target can fail.
    void name(const std::string& name) const;

    std::string label() const;
    void label(const std::string& label);
    std::string output_name() const;
    void output_name(const std::string& name);
    config::target_t::type_t type() const;
    void type(config::target_t::type_t type);
    config::target_t::version_t version() const;
    void version(const config::target_t::version_t& version);

    std::vector<std::string> sources() const;
    void sources(const std::vector<std::string>& sources);
    void source_add(const std::string& source);
    void source_remove(const std::string& source);

    std::vector<std::string> definitions() const;
    void definitions(const std::vector<std::string>& definitions);
    void definition_add(const std::string& define);
    void definition_remove(const std::string& define);

    std::vector<std::string> includes() const;
    void includes(const std::vector<std::string>& includes);
    void include_add(const std::string& include);
    void include_remove(const std::string& include);

    std::string compile_flags() const;
    void compile_flags(const std::string& flags);
    std::string link_flags() const;
    void link_flags(const std::string& flags);

    std::vector<std::string> libs() const;
    void libs(const std::vector<std::string>& libs);
    void lib_add(const std::string& lib);
    void lib_remove(const std::string& lib);

    std::vector<std::string> packages() const;
    void packages(const std::vector<std::string>& packages);
    void package_add(const std::string& package);
    void package_remove(const std::string& package);
};

class directory_t
{
private:
    std::reference_wrapper<config::configuration_t> configuration;
    std::reference_wrapper<config::directory_t> directory;
public:
    directory_t(config::configuration_t& configuration, config::directory_t& directory);

    std::vector<std::string> definitions() const;
    void definitions(const std::vector<std::string>& definitions);
    void definition_add(const std::string& define);
    void definition_remove(const std::string& define);

    std::vector<std::string> includes() const;
    void includes(const std::vector<std::string>& includes);
    void include_add(const std::string& include);
    void include_remove(const std::string& include);

    std::string compile_flags_cxx() const;
    std::string compile_flags_c() const;
    void compile_flags_cxx(const std::string& flags);
    void compile_flags_c(const std::string& flags);

    std::vector<config::directory_t::configure_file_t> configure_files() const;
    void configure_file_add(const std::string& input, const std::string& output);
    void configure_file_remove(const std::string& input);

    file_t file_add(const std::string& name);
    file_t file_get(const std::string& name);
    std::vector<config::file_t> files() const;
    void file_remove(const std::string& name);

    target_t target_add(const std::string& name, config::target_t::type_t type);
    target_t target_get(const std::string& name);
    std::vector<config::target_t> targets() const;
    void target_remove(const std::string& name);

    std::vector<std::string> subdirectories() const;
    /* Retrieve the subdirectory entry for the given canonical, relative path.
     * It is an error to attempt to retrieve a path that does not exist. */
    directory_t subdirectory(const boost::filesystem::path& path);
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
    boost::filesystem::path root() const;

    void packages(const std::set<std::string>& pkgs);
    std::set<std::string> packages() const;

    /* Create and return a new canonical, relative path.
     * Nested paths can be created with this function.
     * Note that the configuration will need to be written after this operation */
    directory_t directory_create(boost::filesystem::path path);

    void directory_remove(boost::filesystem::path path);

    /* Retrieve the directory entry for the given canonical, relative path.
     * It is an error to attempt to retrieve a path that does not exist. */
    directory_t directory(boost::filesystem::path path = {});

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
