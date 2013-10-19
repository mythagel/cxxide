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
 * listreader.h
 *
 *  Created on: 2013-10-19
 *      Author: nicholas
 */

#ifndef LISTREADER_H_
#define LISTREADER_H_
#include "listparser.h"
#include <string>
#include <vector>

namespace cxxide
{
namespace cmake
{

struct configuration_t;
struct directory_t;

class list_reader_t : public listparser_t
{
private:
    configuration_t* config;
    directory_t* directory;
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
            SET_PROPERTY,
            ADD_EXECUTABLE,
            SET_TARGET_PACKAGES,
            SET_TARGET_LIBRARIES,
            ADD_DEPENDENCIES
        } type;
        std::string name;
        std::vector<std::string> args;
        
        command_t();
        
        explicit command_t(std::string cmd_name);
    };
    command_t command;
    unsigned int nesting;
public:
    list_reader_t(configuration_t* config, directory_t* directory);

    bool managed() const;

    void whitespace(const char*, const char*) override;
    void comment(const char* c, const char* end) override;
    void begin_command(const char* c, const char* end) override;
    void open_bracket() override;
    void close_bracket() override;
    void argument(const char* c, const char* end, bool /*quoted*/) override;
    void end_command() override;

    virtual ~list_reader_t() = default;
};

}
}

#endif /* LISTREADER_H_ */
