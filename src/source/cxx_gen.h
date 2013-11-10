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
 * cxx_gen.h
 *
 *  Created on: 2013-11-10
 *      Author: nicholas
 */

#ifndef CXX_GEN_H_
#define CXX_GEN_H_
#include <string>

namespace cxxide
{
namespace source
{
namespace cxxgen
{

/* C++ code generator - very limited in scope.
 * Exists simply to have confidence in the code generated
 * by the ide. */

struct comment_t
{
    std::string comment;
    enum
    {
        style_Block,
        style_Line
    } style;
    
    std::string str() const;
};

}
}
}

#endif /* CXX_GEN_H_ */
