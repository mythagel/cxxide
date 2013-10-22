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
 * template.cpp
 *
 *  Created on: 27/06/2011
 *      Author: nicholas
 */

#include "template.h"

namespace cxxide
{
namespace templ
{

std::string expand(std::string templ, const std::map<std::string, std::string>& parameters)
{
	for(std::string::size_type pos = 0; ; )
	{
		auto start = templ.find("${", pos);
		auto end = templ.find("}", start);
		if(end == std::string::npos)
			break;

		auto key = templ.substr(start+2, (end-(start+2)));
		auto it = parameters.find(key);
		if(it != parameters.end())
		{
			auto& value = it->second;
			
		    templ.replace(start, (end-start)+1, value);
		    pos = start + value.size();
		}
		else
		{
		    pos = end;
		}
	}

	return templ;
}

}
}

