/*
 * item.cpp
 *
 *  Created on: 07/01/2013
 *      Author: nicholas
 */

#include "item.h"
#include <sstream>

namespace project
{
namespace cmake
{

item_t::item_t(Type type)
 : type(type)
{
}

std::string item_t::describe() const
{
	std::ostringstream s;
	switch(type)
	{
		case type_whitespace_t:
			s << "whitespace_t{";
			break;
		case type_comment_t:
			s << "comment_t{";
			break;
		case type_argument_t:
			s << "argument_t{";
			break;
		case type_semicolon_list_t:
			s << "semicolon_list_t{";
			break;
		case type_command_t:
			s << "command_t{";
			break;
	}
	s << str() << "}";
	return s.str();
}

item_t::~item_t()
{
}

whitespace_t::whitespace_t()
 : item_t(type_whitespace_t)
{
}
whitespace_t::whitespace_t(char ws_char, size_t count)
 : item_t(type_whitespace_t), ws_char(ws_char), count(count)
{
}
std::string whitespace_t::str() const
{
	return std::string(count, ws_char);
}
std::string whitespace_t::describe() const
{
	return str();
//	 return item_t::describe();
	std::stringstream s;
	s << "whitespace_t{" << "'" << ws_char << "' x " << count << "}";
	return s.str();
}

comment_t::comment_t()
 : item_t(type_comment_t)
{
}
comment_t::comment_t(const std::string& comment)
 : item_t(type_comment_t), comment(comment)
{
}
std::string comment_t::str() const
{
	return std::string("#") + comment + '\n';
}

argument_t::argument_t()
 : item_t(type_argument_t)
{
}
argument_t::argument_t(const std::string& value, bool quoted)
 : item_t(type_argument_t), value(value), quoted(quoted)
{
}
std::string argument_t::str() const
{
	if(quoted)
	{
		std::ostringstream s;
		s << '"' << value << '"';
		return s.str();
	}
	return value;
}

semicolon_list_t::semicolon_list_t()
 : item_t(type_semicolon_list_t)
{
}
std::string semicolon_list_t::str() const
{
	std::ostringstream s;
	if(quoted)
		s << '"';

	bool first = true;
	for (auto& it : items)
	{
		if (first)
			first = false;
		else
			s << ';';
		s << it->str();
	}
	if(quoted)
		s << '"';
	return s.str();
}
std::string semicolon_list_t::describe() const
{
	std::ostringstream s;
	s << "semicolon_list_t{";
	if(quoted)
		s << '"';

	bool first = true;
	for (auto& it : items)
	{
		if (first)
			first = false;
		else
			s << ';';
		s << it->describe();
	}
	if(quoted)
		s << '"';
	s << "}";
	return s.str();
}

command_t::command_t()
 : item_t(type_command_t)
{
}
command_t::command_t(const std::string& name)
 : item_t(type_command_t), name(name)
{
}
std::string command_t::str() const
{
	std::ostringstream s;

	s << name;
	for(auto& it : name_ws)
		s << it.str();

	s << "(";
	for(auto& it : items)
		s << it->str();
	s << ")";

	return s.str();
}
std::string command_t::describe() const
{
	std::ostringstream s;
	s << "command_t{";

	s << name;
	for(auto& it : name_ws)
		s << it.describe();

	s << "(";
	for(auto& it : items)
		s << it->describe();
	s << ")}";

	return s.str();
}

item make_command(const std::string& command)
{
	return item(new command_t(command));
}

item make_ws(char ws_char, size_t count)
{
	return item(new whitespace_t(ws_char, count));
}

item make_argument(const std::string& value, bool quoted)
{
	return item(new argument_t(value, quoted));
}

item make_comment(const std::string& comment)
{
	return item(new comment_t(comment));
}

} /* namespace cmake */
} /* namespace project */
