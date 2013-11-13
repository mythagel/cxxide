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
 * listwriter.h
 *
 *  Created on: 2013-10-19
 *      Author: nicholas
 */

#ifndef LISTWRITER_H_
#define LISTWRITER_H_
#include "listparser.h"
#include <iosfwd>
#include <set>

namespace cxxide
{
namespace cmake
{

struct configuration_t;
struct directory_t;

class list_rewriter_t : public listparser_t
{
private:
    std::ostream& os;
    const configuration_t* config;
    const directory_t& directory;
    bool is_managed;
    bool skip;
    std::set<std::string> written_targets;
public:
    list_rewriter_t(std::ostream& os, const configuration_t* config, const directory_t& directory);

    bool managed() const;

    void whitespace(const char* c, const char* end) override;
    void comment(const char* c, const char* end) override;
    void begin_command(const char* c, const char* end) override;
    void open_bracket() override;
    void close_bracket() override;
    void argument(const char* c, const char* end, bool quoted) override;
    void end_command() override;
    void eof() override;

    virtual ~list_rewriter_t() = default;
};

}
}

#endif /* LISTWRITER_H_ */
