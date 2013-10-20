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
#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include "listwriter.h"
#include "listreader.h"

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

target_t::target_t()
 : type(executable)
{
}

configuration_t::configuration_t()
 : managed(true)
{
}

void rollback(const std::string& path, const directory_t& directory)
{
    int err = unlink((path + "/CMakeLists.txt.tmp").c_str());
    if(err && errno != ENOENT)
        throw std::system_error(errno, std::system_category(), std::string("unlink(") + path + "/CMakeLists.txt.tmp" + ")");
    
    for(auto& subdir : directory.subdirectories)
        rollback(path + '/' + subdir.first, subdir.second);
}

void commit(const std::string& path, const directory_t& directory)
{
    int err = rename((path + "/CMakeLists.txt.tmp").c_str(), (path + "/CMakeLists.txt").c_str());
    if(err)
        throw std::system_error(errno, std::system_category(), std::string("rename(") + path + "/CMakeLists.txt.tmp" + ", " + path + "/CMakeLists.txt" + ")" );
    
    for(auto& subdir : directory.subdirectories)
        commit(path + '/' + subdir.first, subdir.second);
}

void write_subdirectory(const std::string& path, const directory_t& directory)
{
    try
    {
        if(path.empty() || path == "/")
            throw std::logic_error("root path in subdirectory!");
    
        std::ifstream ifs(path + "/CMakeLists.txt", std::ios::in | std::ios::binary);
        std::ofstream os(path + "/CMakeLists.txt.tmp");
        if(!os)
            throw error(std::string("Unable to open ") + path + "/CMakeLists.txt.tmp");
        
        if(!ifs)
        {
            os << "\n";
            os << "##<< Directory Properties >>##\n";
            os << "##<< Directory Properties >>##\n";
            os << "\n";
            os << "##<< File Properties >>##\n";
            os << "##<< File Properties >>##\n";
            os << "\n";
            os << "##<< Target Properties >>##\n";
            os << "##<< Target Properties >>##\n";
            os << "\n";
        }
        else
        {
            std::noskipws(ifs);
            std::string source{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
            
            list_rewriter_t rewrite(os, nullptr, directory);
            
            auto c = source.c_str();
            auto end = c + source.size();
            
            // Parses CMakeLists.txt and writes new content into CMakeLists.txt.tmp
            rewrite.parse(c, end);
            
        }
        
        // Recursively write subdirectory files.
        for(auto& subdir : directory.subdirectories)
            write_subdirectory(path + '/' + subdir.first, subdir.second);
        
    }
    catch(...)
    {
        std::throw_with_nested(error(std::string("cmake::write_subdirectory(") + path + ") failed"));
    }
}

void write(const std::string& root_path, const configuration_t& config)
{
    if(!config.managed)
        throw error("Invalid attempt to write unmanaged configuration.");
    
    try
    {
        std::ifstream ifs(root_path + "/CMakeLists.txt", std::ios::in | std::ios::binary);
        if(!ifs)
            throw error("Unable to open CMakeLists.txt");

        std::noskipws(ifs);
        std::string source{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
        
        std::ofstream os(root_path + "/CMakeLists.txt.tmp");
        if(!os)
            throw error("Unable to open CMakeLists.txt.tmp");
        
        list_rewriter_t rewrite(os, &config, config.directory);
        
        auto c = source.c_str();
        auto end = c + source.size();
        
        // Parses CMakeLists.txt and writes new content into CMakeLists.txt.tmp
        rewrite.parse(c, end);
        
        if(rewrite.managed())
        {
            // Recursively write subdirectory files.
            for(auto& subdir : config.directory.subdirectories)
                write_subdirectory(root_path + '/' + subdir.first, subdir.second);
            
            // Rename CMakeLists.txt.tmp -> CMakeLists.txt recursively
            commit(root_path, config.directory);
        }
        else
        {
            throw error("Cannot write; Configuration is unmanaged.");
        }
    }
    catch(...)
    {
        try
        {
            rollback(root_path, config.directory);
        }
        catch(...)
        {
            std::throw_with_nested(error("cmake::write failed (cleanup failed.)"));
        }
        std::throw_with_nested(error("cmake::write failed"));
    }
}

void read_subdirectory(const std::string& path, directory_t* directory)
{
    try
    {
        if(path.empty() || path == "/")
            throw std::logic_error("root path in subdirectory!");
    
        std::ifstream ifs(path + "/CMakeLists.txt", std::ios::in | std::ios::binary);
        if(!ifs)
            throw error(std::string("Unable to open ") + path + "/CMakeLists.txt");
        
        std::noskipws(ifs);
        std::string source{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
        
        list_reader_t reader(nullptr, directory);
        
        auto c = source.c_str();
        auto end = c + source.size();
        
        // Read CMakeLists.txt and fill config
        reader.parse(c, end);
        
        // Recursively read subdirectory files.
        for(auto& subdir : directory->subdirectories)
            read_subdirectory(path + '/' + subdir.first, &subdir.second);
        
    }
    catch(...)
    {
        std::throw_with_nested(error(std::string("cmake::read_subdirectory(") + path + ") failed"));
    }
}

configuration_t read(const std::string& root_path)
{
    try
    {
        configuration_t config;
        
        std::ifstream ifs(root_path + "/CMakeLists.txt", std::ios::in | std::ios::binary);
        if(!ifs)
            throw error("Unable to open CMakeLists.txt");

        std::noskipws(ifs);
        std::string source{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
        
        list_reader_t reader(&config, &config.directory);
        
        auto c = source.c_str();
        auto end = c + source.size();
        
        // Read CMakeLists.txt and fill config
        reader.parse(c, end);
        
        if(reader.managed())
        {
            // Recursively read subdirectory files.
            for(auto& subdir : config.directory.subdirectories)
                read_subdirectory(root_path + '/' + subdir.first, &subdir.second);
            
            return config;
        }
        else
        {
            config.managed = false;
            return config;
        }
    }
    catch(...)
    {
        std::throw_with_nested(error("cmake::read failed"));
    }
}

void project_t::generate()
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
        std::throw_with_nested(error("project::generate failed"));
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
        std::throw_with_nested(error("project::build failed"));
    }
}

project_t create(const std::string& name, const std::string& source_path, const std::string& build_path)
{
    project_t project;
    project.source_path = source_path;
    project.build_path = build_path;

    {
        std::ofstream os(source_path + '/' + "CMakeLists.txt");
        os << "CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )\n";
        os << "PROJECT( " << name << " )\n";
        os << "\n";
        os << "##<< Managed Configuration >>##\n";
        os << "SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} \"${CMAKE_SOURCE_DIR}/cmake/Modules/\" )\n";
        os << "INCLUDE( cxxide )\n";
        os << "\n";
        os << "##<< Referenced Packages >>##\n";
        os << "##<< Referenced Packages >>##\n";
        os << "\n";
        os << "##<< Directory Properties >>##\n";
        os << "##<< Directory Properties >>##\n";
        os << "\n";
        os << "##<< File Properties >>##\n";
        os << "##<< File Properties >>##\n";
        os << "\n";
        os << "##<< Target Properties >>##\n";
        os << "##<< Target Properties >>##\n";
        os << "\n";
    }

    {
        int err;
        err = mkdir((source_path + '/' + "cmake").c_str(), 0777);
        if(err)
            throw std::system_error(errno, std::system_category(), std::string("mkdir(") + source_path + '/' + "cmake" + ")");
        err = mkdir((source_path + '/' + "cmake" + '/' + "Modules").c_str(), 0777);
        if(err)
            throw std::system_error(errno, std::system_category(), std::string("mkdir(") + source_path + '/' + "cmake" + '/' + "Modules" + ")");

        std::ofstream os(source_path + '/' + "cmake" + '/' + "Modules" + '/' + "cxxide.cmake");
        os << "INCLUDE( CMakeParseArguments )\n";
        os << "FUNCTION( SET_TARGET_LIBRARIES )\n";
        os << "    CMAKE_PARSE_ARGUMENTS( arg \"\" \"TARGET\" \"LIBS\" ${ARGV})\n";
        os << "    FOREACH(library IN LISTS arg_LIBS)\n";
        os << "        FIND_LIBRARY(LIB ${library})\n";
        os << "        LIST(APPEND LIBS ${LIB})\n";
        os << "    ENDFOREACH()\n";
        os << "    TARGET_LINK_LIBRARIES(${arg_TARGET} ${LIBS})\n";
        os << "ENDFUNCTION()\n";
        os << "\n";
        os << "FUNCTION( SET_TARGET_PACKAGES )\n";
        os << "    CMAKE_PARSE_ARGUMENTS( arg \"\" \"TARGET\" \"PACKAGES\" ${ARGV})\n";
        os << "    FOREACH(package IN LISTS arg_PACKAGES)\n";
        os << "        STRING(TOUPPER \"${package}\" upper_package)\n";
        os << "\n";        
        os << "        IF(\"${package}_FOUND\")\n";
        os << "            SET(PKG ${package})\n";
        os << "        ELSEIF(\"${upper_package}_FOUND\")\n";
        os << "            SET(PKG ${upper_package})\n";
        os << "        ELSE()\n";
        os << "            # Not actually an accurate message - more accurate is that it is not recognised.\n";
        os << "            MESSAGE(FATAL_ERROR \"Required package '${package}' in target '${arg_TARGET}' NOTFOUND\")\n";
        os << "        ENDIF()\n";
        os << "\n";        
        os << "        IF(DEFINED \"${PKG}_DEFINITIONS\")\n";
        os << "            SET_PROPERTY(TARGET ${arg_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS ${${PKG}_DEFINITIONS})\n";
        os << "        ENDIF()\n";
        os << "\n";        
        os << "        IF(DEFINED \"${PKG}_INCLUDE_DIRS\")\n";
        os << "            SET_PROPERTY(TARGET ${arg_TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES ${${PKG}_INCLUDE_DIRS})\n";
        os << "        ELSEIF(DEFINED \"${PKG}_INCLUDE_DIR\")\n";
        os << "            SET_PROPERTY(TARGET ${arg_TARGET} APPEND PROPERTY INCLUDE_DIRECTORIES ${${PKG}_INCLUDE_DIR})\n";
        os << "        ENDIF()\n";
        os << "\n";        
        os << "        IF(DEFINED \"${PKG}_LIBRARIES\")\n";
        os << "            TARGET_LINK_LIBRARIES(${arg_TARGET} ${${PKG}_LIBRARIES})\n";
        os << "        ELSEIF(DEFINED \"${PKG}_LIBRARY\")\n";
        os << "            TARGET_LINK_LIBRARIES(${arg_TARGET} ${${PKG}_LIBRARY})\n";
        os << "        ENDIF()\n";
        os << "    ENDFOREACH()\n";
        os << "ENDFUNCTION()\n";
    }

    return project;
}

}
}

