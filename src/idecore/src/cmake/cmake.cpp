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

#include "cmake/cmake.h"
#include <vector>
#include <exception>
#include <iterator>
#include "listparser.h"
#include "listwriter.h"
#include "listreader.h"
#include <boost/filesystem/fstream.hpp>

// Glibc defines these unwanted macros
#undef major
#undef minor

namespace fs = boost::filesystem;

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

target_t::version_t::version_t()
 : major(), minor(), patch()
{
}


target_t::version_t::version_t(int major, int minor, int patch)
 : major(major), minor(minor), patch(patch)
{
}

bool target_t::version_t::empty() const
{
    return !(major || minor || patch);
}

configuration_t::configuration_t()
 : managed(true)
{
}

namespace
{

void rollback(const fs::path& path, const directory_t& directory)
{
    fs::remove(path / "CMakeLists.txt.tmp");
    
    for(auto& subdir : directory.subdirectories)
        rollback(path / subdir.first, subdir.second);
}

void commit(const fs::path& path, const directory_t& directory)
{
    fs::rename(path / "CMakeLists.txt.tmp", path / "CMakeLists.txt");
    
    for(auto& subdir : directory.subdirectories)
        commit(path / subdir.first, subdir.second);
}

void write_subdirectory(const fs::path& path, const directory_t& directory)
{
    try
    {
        fs::ifstream ifs(path / "CMakeLists.txt", std::ios::in | std::ios::binary);
        fs::ofstream os(path / "CMakeLists.txt.tmp");
        if(!os)
            throw error(std::string("Unable to open ") + (path / "CMakeLists.txt.tmp").native());
        
        std::string source;
        if(ifs)
        {
            std::noskipws(ifs);
            source = std::string{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
        }

        list_rewriter_t rewrite(os, nullptr, directory);

        auto c = source.c_str();
        auto end = c + source.size();

        // Parses CMakeLists.txt and writes new content into CMakeLists.txt.tmp
        rewrite.parse(c, end);
        
        // Recursively write subdirectory files.
        for(auto& subdir : directory.subdirectories)
            write_subdirectory(path / subdir.first, subdir.second);
        
    }
    catch(...)
    {
        std::throw_with_nested(error(std::string("cmake::write_subdirectory(") + path.native() + ") failed"));
    }
}

void read_subdirectory(const fs::path& path, directory_t* directory)
{
    try
    {
        fs::ifstream ifs(path / "CMakeLists.txt", std::ios::in | std::ios::binary);
        if(!ifs)
            throw error(std::string("Unable to open ") + (path / "CMakeLists.txt").native());
        
        std::noskipws(ifs);
        std::string source{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
        
        list_reader_t reader(nullptr, directory);
        
        auto c = source.c_str();
        auto end = c + source.size();
        
        // Read CMakeLists.txt and fill config
        reader.parse(c, end);
        
        // Recursively read subdirectory files.
        for(auto& subdir : directory->subdirectories)
            read_subdirectory(path / subdir.first, &subdir.second);
        
    }
    catch(...)
    {
        std::throw_with_nested(error(std::string("cmake::read_subdirectory(") + path.native() + ") failed"));
    }
}

void write_template(const std::string& name, const fs::path& path)
{
    {
        fs::ofstream os(path / "CMakeLists.txt");
        if(!os)
            throw error("Unable to write CMakeLists.txt");
        
        os << "CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )\n";
        os << "PROJECT( " << name << " )\n";
        os << "\n";
        os << "##<< Managed Configuration >>##\n";
        os << "SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} \"${CMAKE_SOURCE_DIR}/cmake/modules/\" )\n";
        os << "INCLUDE( idecore )\n";
        os << "\n";
    }
    
    if(!exists(path / "cmake" / "modules" / "idecore.cmake"))
    {
        create_directories(path / "cmake" / "modules");

        // TODO raw string literal.
        // TODO stat ~/idecore/idecore.cmake replacement
        fs::ofstream os(path / "cmake" / "modules" / "idecore.cmake");
        if(!os)
            throw error("Unable to write cmake/modules/idecore.cmake");
        
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
}

}

void write(const fs::path& root_path, const configuration_t& config)
{
    if(!config.managed)
        throw error("Invalid attempt to write unmanaged configuration.");
    
    try
    {
        fs::ifstream ifs(root_path / "CMakeLists.txt", std::ios::in | std::ios::binary);
        if(!ifs)
        {
            write_template(config.name, root_path);
            ifs.open(root_path / "CMakeLists.txt", std::ios::in | std::ios::binary);
            if(!ifs)
                throw error("Unable to open CMakeLists.txt after write.");
        }

        std::noskipws(ifs);
        std::string source{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
        
        fs::ofstream os(root_path / "CMakeLists.txt.tmp");
        if(!os)
            throw error("Unable to write CMakeLists.txt.tmp");
        
        list_rewriter_t rewrite(os, &config, config.directory);
        
        auto c = source.c_str();
        auto end = c + source.size();
        
        // Parses CMakeLists.txt and writes new content into CMakeLists.txt.tmp
        rewrite.parse(c, end);
        
        if(rewrite.managed())
        {
            // Recursively write subdirectory files.
            for(auto& subdir : config.directory.subdirectories)
                write_subdirectory(root_path / subdir.first, subdir.second);
            
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

configuration_t read(const fs::path& root_path)
{
    try
    {
        configuration_t config;
        
        fs::ifstream ifs(root_path / "CMakeLists.txt", std::ios::in | std::ios::binary);
        if(!ifs)
            throw error("Unable to read CMakeLists.txt");

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
                read_subdirectory(root_path / subdir.first, &subdir.second);
            
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

}
}

