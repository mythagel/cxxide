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
 * buffer.cpp
 *
 *  Created on: 2014-02-27
 *      Author: nicholas
 */

#include "buffer.h"
#include <fstream>

namespace ex {

buffer buffer_read(const std::string& file) {
    buffer b;
    b.name = file;
    std::ifstream is(file);
    is.exceptions(std::ios_base::failbit);

    std::string line;
    while(std::getline(is, line)) {
        b.lines.push_back(std::move(line));
    }
    return b;
}

}
