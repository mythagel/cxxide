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
 * cursor.cpp
 *
 *  Created on: 2014-03-04
 *      Author: nicholas
 */

#include "cursor.h"
#include <tuple>

namespace {

auto to_tuple(const cursor& c) -> decltype(std::tie(c.line, c.column)) {
	return std::tie(c.line, c.column);
}

}

bool operator<(const cursor& l, const cursor& r) {
    return to_tuple(l) < to_tuple(r);
}
bool operator>(const cursor& l, const cursor& r) {
    return to_tuple(l) > to_tuple(r);
}
bool operator<=(const cursor& l, const cursor& r) {
    return to_tuple(l) <= to_tuple(r);
}
bool operator>=(const cursor& l, const cursor& r) {
    return to_tuple(l) >= to_tuple(r);
}
bool operator==(const cursor& l, const cursor& r) {
    return to_tuple(l) == to_tuple(r);
}
