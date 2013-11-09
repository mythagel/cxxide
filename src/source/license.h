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
 * license.h
 *
 *  Created on: 2013-11-09
 *      Author: nicholas
 */

#ifndef LICENSE_H_
#define LICENSE_H_
#include <string>
#include <map>

namespace cxxide
{
namespace source
{
namespace license
{

std::string generate_header(const std::string& name, const std::map<std::string, std::string>& vars);
std::string generate_full(const std::string& name, const std::map<std::string, std::string>& vars);

}
}
}

#endif /* LICENSE_H_ */
