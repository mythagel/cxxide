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
#include <iterator>
#include "listparser.h"

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

configuration_t::configuration_t()
 : managed(true)
{
}

class list_rewriter_t : public listparser_t
{
private:
    std::ostream& os;
    const configuration_t& config;
    bool is_managed;
public:
    list_rewriter_t(std::ostream& os, const configuration_t& config)
     : os(os), config(config), is_managed(false)
    {
    }

    bool managed() const
    {
        return is_managed;
    }

    virtual void whitespace(const char* c, const char* end)
    {
        os << std::string(c, end);
    }
    virtual void comment(const char* c, const char* end)
    {
        auto cmt = std::string(c, end);
        os << '#' << cmt;
        if(cmt == "#<< Managed Configuration >>##")
            is_managed = true;
    }
    virtual void begin_command(const char* c, const char* end)
    {
        os << std::string(c, end);
    }
    virtual void open_bracket()
    {
        os << '(';
    }
    virtual void close_bracket()
    {
        os << ')';
    }
    virtual void argument(const char* c, const char* end, bool quoted)
    {
        if(quoted)
            os << '"' << std::string(c, end) << '"';
        else
            os << std::string(c, end);
    }
    virtual void end_command()
    {
        // ni
    }

    virtual ~list_rewriter_t() = default;
};

void write(const std::string& root_path, const configuration_t& config)
{
    if(!config.managed)
        throw error("Invalid attempt to write unmanaged configuration.");
    /*
    if configuration unmanaged - throw error - should not have been called.
    
    read all CMakeLsits files referenced in configuration
    check all files are managed (master has tag and all parsable) - if not throw error
    
    open root_path/CMakeLists.txt
      not exist - create with default contents
      exist - attempt to parse - if configuration unmanaged throw error
    
    open root config, update sections
    continue recursivly for managed directories
    */
    
    try
    {
        std::ifstream ifs(root_path + "/CMakeLists.txt", std::ios::in | std::ios::binary);
        std::noskipws(ifs);
        
        std::string source{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
        
        std::ofstream os(root_path + "/CMakeLists.txt.new");
        list_rewriter_t rewrite(os, config);
        
        auto c = source.c_str();
        auto end = c + source.size();
        
        // Parses CMakeLists.txt and writes content into CMakeLists.txt.new
        rewrite.parse(c, end);
        
        if(rewrite.managed())
        {
            // TODO atomic rename
        }
        else
        {
            throw error("Cannot write; Configuration is unmanaged.");
        }
    }
    catch(...)
    {
        std::throw_with_nested(error("cmake::write failed"));
    }
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

