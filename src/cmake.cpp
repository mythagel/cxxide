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
    bool skip;
public:
    list_rewriter_t(std::ostream& os, const configuration_t& config)
     : os(os), config(config), is_managed(false), skip(false)
    {
    }

    bool managed() const
    {
        return is_managed;
    }

    void whitespace(const char* c, const char* end) override
    {
        if(skip) return;
        os << std::string(c, end);
    }
    void comment(const char* c, const char* end) override
    {
        auto cmt = std::string(c, end);
        
        if(cmt.find("#<< ") == 0)
        {
            if(cmt == "#<< Managed Configuration >>##")
            {
                is_managed = true;
            }
            else if(cmt == "#<< Referenced Packages >>##")
            {
                if(skip)
                {
                    os << "\n";
                    for(auto& package : config.packages)
                        os << "FIND_PACKAGE(" << package << ")\n";
                }
                skip = !skip;
            }
            else if(cmt == "#<< Directory Properties >>##")
            {
                if(skip)
                {
                    os << "\n";
                    os << "Generated content here!\n";
                }
                skip = !skip;
            }
            else if(cmt == "#<< File Properties >>##")
            {
                if(skip)
                {
                    os << "\n";
                    os << "Generated content here!\n";
                }
                skip = !skip;
            }
            else if(cmt == "#<< Target Properties >>##")
            {
                if(skip)
                {
                    os << "\n";
                    os << "Generated content here!\n";
                }
                skip = !skip;
            }
            
            os << '#' << cmt;
        }
        else
        {
            if(skip) return;
            os << '#' << cmt;
        }
    }
    void begin_command(const char* c, const char* end) override
    {
        if(skip) return;
        os << std::string(c, end);
    }
    void open_bracket() override
    {
        if(skip) return;
        os << '(';
    }
    void close_bracket() override
    {
        if(skip) return;
        os << ')';
    }
    void argument(const char* c, const char* end, bool quoted) override
    {
        if(skip) return;
        
        if(quoted)
            os << '"' << std::string(c, end) << '"';
        else
            os << std::string(c, end);
    }
    void end_command() override
    {
        if(skip) return;
        // ni
    }

    virtual ~list_rewriter_t() = default;
};

void write(const std::string& root_path, const configuration_t& config)
{
    if(!config.managed)
        throw error("Invalid attempt to write unmanaged configuration.");
    /*
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
        if(!ifs)
            throw error("Unable to open CMakeLists.txt");
        std::noskipws(ifs);
        
        std::string source{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
        
        std::ofstream os(root_path + "/CMakeLists.txt.tmp");
        if(!os)
            throw error("Unable to open CMakeLists.txt.tmp");
        list_rewriter_t rewrite(os, config);
        
        auto c = source.c_str();
        auto end = c + source.size();
        
        // Parses CMakeLists.txt and writes new content into CMakeLists.txt.tmp
        rewrite.parse(c, end);
        
        if(rewrite.managed())
        {
            // update subdirs
            
            // TODO recursize rename tmp -> actual
//            int err = rename(const char *oldpath, const char *newpath);
//            if(err)
//                throw std::system_error(errno, std::system_category(), "rename");
        }
        else
        {
            throw error("Cannot write; Configuration is unmanaged.");
        }
    }
    catch(...)
    {
//        unlink((root_path + "/CMakeLists.txt.tmp").c_str());
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

    {
        std::ofstream os(source_path + '/' + "CMakeLists.txt");
        os << "CMAKE_MINIMUM_REQUIRED(VERSION 2.8)\n";
        os << "PROJECT(" << name << ")\n";
        os << "\n";
        os << "##<< Managed Configuration >>##\n";
        os << "SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} \"${CMAKE_SOURCE_DIR}/cmake/Modules/\")\n";
        os << "include(DeclTarget)\n";
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

        std::ofstream os(source_path + '/' + "cmake" + '/' + "Modules" + '/' + "DeclTarget.cmake");
        os << "FUNCTION(DECLARE_TARGET target_name)\n";
        os << "STRING(TOUPPER \"${target_name}\" TARGET)\n";
        os << "\n";
        os << "IF(\"${TARGET}_TYPE\" STREQUAL \"EXECUTABLE\")\n";
        os << "    ADD_EXECUTABLE(\"${target_name}\" ${TARGET}_SOURCES)\n";
        os << "\n";    
        os << "    IF(DEFINED \"${TARGET}_VERSION\")\n";
        os << "        SET_PROPERTY(TARGET foo PROPERTY VERSION ${TARGET}_VERSION)\n";
        os << "    ENDIF()\n";
        os << "\n";
        os << "ELSEIF(\"${TARGET}_TYPE\" STREQUAL \"SHARED_LIBRARY\")\n";
        os << "    ADD_LIBRARY(\"${target_name}\" SHARED ${TARGET}_SOURCES)\n";
        os << "\n";    
        os << "    IF(DEFINED \"${TARGET}_VERSION\")\n";
        os << "        STRING(REPLACE \".\" \";\" VERSION \"${TARGET}_VERSION\")\n";
        os << "        list(GET ${VERSION} 0 VERSION_MAJOR)\n";
        os << "        SET_PROPERTY(TARGET foo PROPERTY SOVERSION ${VERSION_MAJOR})\n";
        os << "        SET_PROPERTY(TARGET foo PROPERTY VERSION ${TARGET}_VERSION)\n";
        os << "    ENDIF()\n";
        os << "\n";
        os << "ELSEIF(\"${TARGET}_TYPE\" STREQUAL \"STATIC_LIBRARY\")\n";
        os << "    ADD_LIBRARY(\"${target_name}\" STATIC ${TARGET}_SOURCES)\n";
        os << "\n";    
        os << "    IF(DEFINED \"${TARGET}_VERSION\")\n";
        os << "        SET_PROPERTY(TARGET foo PROPERTY VERSION ${TARGET}_VERSION)\n";
        os << "    ENDIF()\n";
        os << "\n";
        os << "ELSE()\n";
        os << "    MESSAGE(FATAL_ERROR \"Unknown target type '${TARGET}_TYPE' in target '${target_name}'\")\n";
        os << "ENDIF()\n";
        os << "\n";
        os << "IF(DEFINED \"${TARGET}_LABEL\")\n";
        os << "    SET_PROPERTY(TARGET ${target_name} PROPERTY PROJECT_LABEL \"${TARGET}_LABEL\")\n";
        os << "ENDIF()\n";
        os << "\n";
        os << "IF(DEFINED \"${TARGET}_DEFINES\")\n";
        os << "    SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY COMPILE_DEFINITIONS ${TARGET}_DEFINES)\n";
        os << "ENDIF()\n";
        os << "\n";
        os << "IF(DEFINED \"${TARGET}_INCLUDES\")\n";
        os << "    SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY INCLUDE_DIRECTORIES ${TARGET}_INCLUDES)\n";
        os << "ENDIF()\n";
        os << "\n";
        os << "IF(DEFINED \"${TARGET}_COMPILEFLAGS\")\n";
        os << "    SET_PROPERTY(TARGET ${target_name} APPEND_STRING PROPERTY COMPILE_FLAGS \"${TARGET}_COMPILEFLAGS\")\n";
        os << "ENDIF()\n";
        os << "\n";
        os << "IF(DEFINED \"${TARGET}_LINKFLAGS\")\n";
        os << "    SET_PROPERTY(TARGET ${target_name} APPEND_STRING PROPERTY LINK_FLAGS \"${TARGET}_LINKFLAGS\")\n";
        os << "ENDIF()\n";
        os << "\n";
        os << "FOREACH(library ${TARGET}_LIBS)\n";
        os << "    FIND_LIBRARY(\"${library}\" ${library})\n";
        os << "    LIST(APPEND \"${TARGET}_RESOLVED_LIBS\" ${${library}})\n";
        os << "ENDFOREACH()\n";
        os << "\n";
        os << "FOREACH(package ${TARGET}_PACKAGES)\n";
        os << "    STRING(TOUPPER \"${package}\" upper_package)\n";
        os << "\n";    
        os << "    IF(\"${package}_FOUND\")\n";
        os << "        SET(PKG ${package})\n";
        os << "    ELSEIF(\"${upper_package}_FOUND\")\n";
        os << "        SET(PKG ${upper_package})\n";
        os << "    ELSE()\n";
        os << "        # Not actually an accurate message - more accurate is that it is not recognised.\n";
        os << "        MESSAGE(FATAL_ERROR \"Required package '${package}' in target '${target_name} NOTFOUND or UNRESOLVED'\")\n";
        os << "    ENDIF()\n";
        os << "\n";    
        os << "    IF(DEFINED \"${PKG}_DEFINITIONS\")\n";
        os << "        SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY COMPILE_DEFINITIONS ${PKG}_DEFINITIONS)\n";
        os << "    ENDIF()\n";
        os << "\n";    
        os << "    IF(DEFINED \"${PKG}_INCLUDE_DIRS\")\n";
        os << "        SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY INCLUDE_DIRECTORIES ${PKG}_INCLUDE_DIRS)\n";
        os << "    ELSEIF(DEFINED \"${PKG}_INCLUDE_DIR\")\n";
        os << "        SET_PROPERTY(TARGET ${target_name} APPEND PROPERTY INCLUDE_DIRECTORIES ${PKG}_INCLUDE_DIR)\n";
        os << "    ENDIF()\n";
        os << "\n";    
        os << "    IF(DEFINED \"${PKG}_LIBRARIES\")\n";
        os << "        LIST(APPEND \"${TARGET}_RESOLVED_LIBS\" ${PKG}_LIBRARIES)\n";
        os << "    ELSEIF(DEFINED \"${PKG}_LIBRARY\")\n";
        os << "        LIST(APPEND \"${TARGET}_RESOLVED_LIBS\" ${PKG}_LIBRARY)\n";
        os << "    ENDIF()\n";
        os << "\n";    
        os << "ENDFOREACH()\n";
        os << "\n";
        os << "IF(DEFINED \"RESOLVED_LIBS\")\n";
        os << "    TARGET_LINK_LIBRARIES(${target_name} ${TARGET}_RESOLVED_LIBS)\n";
        os << "ENDIF()\n";
        os << "\n";
        os << "IF(DEFINED \"${TARGET}_DEPENDS\")\n";
        os << "    ADD_DEPENDENCIES(${target_name} ${TARGET}_DEPENDS)\n";
        os << "ENDIF()\n";
        os << "ENDFUNCTION()\n";
    }

    return project;
}

}
}

