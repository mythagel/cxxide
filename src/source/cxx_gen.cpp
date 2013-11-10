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
 * cxx_gen.cpp
 *
 *  Created on: 2013-11-10
 *      Author: nicholas
 */

#include "cxx_gen.h"
#include <sstream>

namespace cxxide
{
namespace source
{
namespace cxxgen
{

std::string comment_t::str() const
{
    std::ostringstream os;
    std::istringstream is(comment);
    std::string line;

    switch(style)
    {
        case style_Block:
        {
            while (getline(is, line))
            {
            }
            break;
        }
        case style_Line:
        {
            while (getline(is, line))
                os << "// " << line << '\n';
            break;
        }
    }
    
    return os.str();
}

}
}
}
