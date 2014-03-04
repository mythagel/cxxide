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
 * cursor.h
 *
 *  Created on: 2014-03-04
 *      Author: nicholas
 */

#ifndef CURSOR_H_
#define CURSOR_H_

struct cursor {
    unsigned int line;
    unsigned int column;
};

bool operator<(const cursor& l, const cursor& r);
bool operator>(const cursor& l, const cursor& r);
bool operator<=(const cursor& l, const cursor& r);
bool operator>=(const cursor& l, const cursor& r);
bool operator==(const cursor& l, const cursor& r);


#endif /* CURSOR_H_ */
