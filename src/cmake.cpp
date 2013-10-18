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
#include <boost/algorithm/string/case_conv.hpp>

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
    const configuration_t* config;
    const configuration_t::directory_t& directory;
    bool is_managed;
    bool skip;
public:
    list_rewriter_t(std::ostream& os, const configuration_t* config, const configuration_t::directory_t& directory)
     : os(os), config(config), directory(directory), is_managed(false), skip(false)
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
            if(config && cmt == "#<< Managed Configuration >>##")
            {
                is_managed = true;
            }
            else if(config && cmt == "#<< Referenced Packages >>##")
            {
                if(skip)
                {
                    os << "\n";
                    for(auto& package : config->packages)
                        os << "FIND_PACKAGE( " << package << " )\n";
                }
                skip = !skip;
            }
            else if(cmt == "#<< Directory Properties >>##")
            {
                if(skip)
                {
                    os << "\n";
                    
                    if(!directory.definitions.empty())
                    {
                        os << "ADD_DEFINITIONS( ";
                        for(auto& define : directory.definitions)
                            os << define << " ";
                        os << ")\n";
                    }

                    if(!directory.includes.empty())
                    {
                        os << "INCLUDE_DIRECTORIES( ";
                        for(auto& include : directory.includes)
                            os << include << " ";
                        os << ")\n";
                    }
                    
                    if(!directory.compile_flags.cxx.empty())
                        os << "SET( CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} " << directory.compile_flags.cxx << "\" )\n";

                    if(!directory.compile_flags.c.empty())
                        os << "SET( CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} " << directory.compile_flags.c << "\" )\n";

                    for(auto& file : directory.configure_files)
                        os << "CONFIGURE_FILE( " << file.input << " " << file.output << " )\n";

                    for(auto& subdir : directory.subdirectories)
                        os << "ADD_SUBDIRECTORY( " << subdir.first << " )\n";
                    
                }
                skip = !skip;
            }
            else if(cmt == "#<< File Properties >>##")
            {
                if(skip)
                {
                    os << "\n";
                    for(auto& file : directory.files)
                    {
                        if(!file.definitions.empty())
                        {
                            os << "SET_PROPERTY( SOURCE " << file.name << " APPEND PROPERTY COMPILE_DEFINITIONS ";
                            for(auto& define : file.definitions)
                                os << define << " ";
                            os << ")\n";
                        }
                        if(!file.compile_flags.empty())
                            os << "SET_PROPERTY( SOURCE " << file.name << " APPEND_STRING PROPERTY COMPILE_FLAGS \" " << file.compile_flags << "\" )\n";
                    }
                }
                skip = !skip;
            }
            else if(cmt == "#<< Target Properties >>##")
            {
                if(skip)
                {
                    os << "\n";
                    for(auto& target : directory.targets)
                    {
                        os << "TODO Generated content here!\n";
                    }
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

void rollback(const std::string& path, const configuration_t::directory_t& directory)
{
    int err = unlink((path + "/CMakeLists.txt.tmp").c_str());
    if(err && errno != ENOENT)
        throw std::system_error(errno, std::system_category(), std::string("unlink(") + path + "/CMakeLists.txt.tmp" + ")");
    
    for(auto& subdir : directory.subdirectories)
        rollback(path + '/' + subdir.first, subdir.second);
}

void commit(const std::string& path, const configuration_t::directory_t& directory)
{
    int err = rename((path + "/CMakeLists.txt.tmp").c_str(), (path + "/CMakeLists.txt").c_str());
    if(err)
        throw std::system_error(errno, std::system_category(), std::string("rename(") + path + "/CMakeLists.txt.tmp" + ", " + path + "/CMakeLists.txt" + ")" );
    
    for(auto& subdir : directory.subdirectories)
        commit(path + '/' + subdir.first, subdir.second);
}

void write_subdirectory(const std::string& path, const configuration_t::directory_t& directory)
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

class list_reader_t : public listparser_t
{
private:
    configuration_t* config;
    configuration_t::directory_t* directory;
    bool is_managed;
    bool interpret;
    enum 
    {
        section_Packages = -1,
        section_Directory,
        section_File,
        section_Target,
    } section;
    
    struct command_t
    {
        enum
        {
            UNKNOWN,
            PROJECT,
            FIND_PACKAGE,
            ADD_DEFINITIONS,
            INCLUDE_DIRECTORIES,
            SET,
            CONFIGURE_FILE,
            ADD_SUBDIRECTORY,
            SET_PROPERTY
        } type;
        std::string name;
        std::vector<std::string> args;
        
        command_t()
         : type(UNKNOWN)
        {
        }
        
        explicit command_t(std::string cmd_name)
         : type(UNKNOWN), name(cmd_name)
        {
            boost::to_upper(cmd_name);
            
            if(cmd_name == "PROJECT")
                type = PROJECT;
            else if(cmd_name == "FIND_PACKAGE")
                type = FIND_PACKAGE;
            else if(cmd_name == "ADD_DEFINITIONS")
                type = ADD_DEFINITIONS;
            else if(cmd_name == "INCLUDE_DIRECTORIES")
                type = INCLUDE_DIRECTORIES;
            else if(cmd_name == "SET")
                type = SET;
            else if(cmd_name == "CONFIGURE_FILE")
                type = CONFIGURE_FILE;
            else if(cmd_name == "ADD_SUBDIRECTORY")
                type = ADD_SUBDIRECTORY;
            else if(cmd_name == "SET_PROPERTY")
                type = SET_PROPERTY;
        }
    };
    command_t command;
    unsigned int nesting;
public:
    list_reader_t(configuration_t* config, configuration_t::directory_t* directory)
     : config(config), directory(directory), is_managed(false), interpret(false), section(section_Directory), nesting(0)
    {
    }

    bool managed() const
    {
        return is_managed;
    }

    void whitespace(const char*, const char*) override
    {
    }
    void comment(const char* c, const char* end) override
    {
        auto cmt = std::string(c, end);
        
        if(cmt.find("#<< ") == 0)
        {
            if(config && cmt == "#<< Managed Configuration >>##")
            {
                is_managed = true;
            }
            else if(config && cmt == "#<< Referenced Packages >>##")
            {
                interpret = !interpret;
                section = section_Packages;
            }
            else if(cmt == "#<< Directory Properties >>##")
            {
                interpret = !interpret;
                section = section_Directory;
            }
            else if(cmt == "#<< File Properties >>##")
            {
                interpret = !interpret;
                section = section_File;
            }
            else if(cmt == "#<< Target Properties >>##")
            {
                interpret = !interpret;
                section = section_Target;
            }
        }
    }
    void begin_command(const char* c, const char* end) override
    {
        command = command_t{{c, end}};
        nesting = 0;
    }
    void open_bracket() override
    {
        if(nesting) command.args.push_back("(");
        ++nesting;
    }
    void close_bracket() override
    {
        --nesting;
        if(nesting) command.args.push_back(")");
    }
    void argument(const char* c, const char* end, bool /*quoted*/) override
    {
        command.args.push_back({c, end});
    }
    void end_command() override
    {
        if(!interpret)
        {
            if(command.type == command_t::PROJECT)
            {
                if(!command.args.empty() && config)
                    config->name = command.args[0];
            }
            return;
        }
        
        if(command.type == command_t::UNKNOWN)
            throw error("Unrecognised command " + command.name);
        
        switch(section)
        {
            case section_Packages:
            {
                if(command.type != command_t::FIND_PACKAGE)
                    throw error(std::string("Unexpected command in Managed Packages: ") + command.name);
                
                if(command.args.size() != 1)
                    throw error("Unexpected arguments to FIND_PACKAGE command in Managed Packages");
                
                // Safe to dereference config here because section_Package is only set above
                // If config != nullptr
                config->packages.insert(command.args[0]);
                break;
            }
            case section_Directory:
            {
                switch(command.type)
                {
                    case command_t::ADD_DEFINITIONS:
                    {
                        for(auto& define : command.args)
                            directory->definitions.push_back(define);
                        break;
                    }
                    case command_t::INCLUDE_DIRECTORIES:
                    {
                        for(auto& include : command.args)
                            directory->includes.push_back(include);
                        break;
                    }
                    case command_t::SET:
                    {
                        if(command.args.size() != 2)
                            throw error("Unexpected arguments to SET command in Directory Properties");
                        if(command.args[0] == "CMAKE_CXX_FLAGS")
                        {
                            if(command.args[1].find("${CMAKE_CXX_FLAGS}") != 0)
                                throw error("Unexpected argument format to SET command in Directory Properties");
                            
                            auto flags = command.args[1].substr(18);
                            directory->compile_flags.cxx = flags;
                        }
                        else if(command.args[0] == "CMAKE_C_FLAGS")
                        {
                            if(command.args[1].find("${CMAKE_C_FLAGS}") != 0)
                                throw error("Unexpected argument format to SET command in Directory Properties");
                            
                            auto flags = command.args[1].substr(16);
                            directory->compile_flags.c = flags;
                        }
                        else
                        {
                            throw error("Unexpected variable to SET command in Directory Properties");
                        }
                        break;
                    }
                    case command_t::CONFIGURE_FILE:
                    {
                        if(command.args.size() != 2)
                            throw error("Unexpected arguments to CONFIGURE_FILE command in Directory Properties");
                        
                        configuration_t::directory_t::configure_file_t configure_file{command.args[0], command.args[1]};
                        directory->configure_files.push_back(configure_file);
                        
                        break;
                    }
                    case command_t::ADD_SUBDIRECTORY:
                    {
                        if(command.args.size() != 1)
                            throw error("Unexpected arguments to ADD_SUBDIRECTORY command in Directory Properties");
                        
                        directory->subdirectories.push_back(std::make_pair(command.args[0], configuration_t::directory_t()));
                        
                        break;
                    }
                    default:
                        throw error(std::string("Unexpected command in Directory Properties: ") + command.name);
                }
                break;
            }
            case section_File:
            {
                if(command.type != command_t::SET_PROPERTY)
                    throw error(std::string("Unexpected command in File Properties: ") + command.name);
                
                if(command.args.size() < 6)
                    throw error("Unexpected arguments to SET_PROPERTY command in File Properties");
                
                if(command.args[0] != "SOURCE")
                    throw error("Unexpected arguments to SET_PROPERTY command in File Properties");

                auto filename = command.args[1];
                
                configuration_t::directory_t::file_t* file = nullptr;
                for(auto& fl : directory->files)
                {
                    if(fl.name == filename)
                    {
                        file = &fl;
                        break;
                    }
                }
                if(!file)
                {
                    directory->files.emplace_back();
                    file = &directory->files.back();
                    file->name = filename;
                }
                
                if(command.args[2] != "APPEND" && command.args[2] != "APPEND_STRING")
                    throw error("Unexpected arguments to SET_PROPERTY command in File Properties");

                if(command.args[3] != "PROPERTY")
                    throw error("Unexpected arguments to SET_PROPERTY command in File Properties");
                
                if(command.args[4] == "COMPILE_DEFINITIONS")
                {
                    for(size_t i = 5; i < command.args.size(); ++i)
                        file->definitions.push_back(command.args[i]);
                }
                else if(command.args[4] == "COMPILE_FLAGS")
                {
                    file->compile_flags = command.args[5];
                    if(!file->compile_flags.empty() && file->compile_flags[0] == ' ')
                        file->compile_flags = file->compile_flags.substr(1);
                }
                else
                    throw error("Unexpected arguments to SET_PROPERTY command in File Properties");
                
                break;
            }
            case section_Target:
            {
                break;
            }
        }
    }

    virtual ~list_reader_t() = default;
};

void read_subdirectory(const std::string& path, configuration_t::directory_t* directory)
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

