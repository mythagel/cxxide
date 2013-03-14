/*
 * listfile.cpp
 *
 *  Created on: 14/03/2013
 *      Author: nicholas
 */

#include "listfile.h"
#include <sstream>
#include <cctype>

namespace cmake
{

parse_error::parse_error(const std::string& what)
 : std::runtime_error(what)
{
}

list_item::list_item(Type type)
 : type(type)
{
}

raw_string_t::raw_string_t()
 : list_item(Type::raw_string)
{
}
raw_string_t::raw_string_t(const std::string& s)
 : list_item(Type::raw_string), s(s)
{
}

std::string raw_string_t::str() const
{
	return s;
}

std::string raw_string_t::describe() const
{
	std::ostringstream s;
	s << "raw_string{" << str() << "}";
	return s.str();
}

whitespace_t::whitespace_t()
 : list_item(Type::whitespace)
{
}

bool whitespace_t::ws_p(char c)
{
	switch(c)
	{
		case '\t':
		case '\n':
		case '\r':
		case ' ':
			return true;
		default:
			return false;
	}
}

std::string whitespace_t::str() const
{
	return wsp;
}

std::string whitespace_t::describe() const
{
	return str();
	std::ostringstream s;
	s << "ws{" << str() << "}";
	return s.str();
}

comment_t::comment_t()
 : list_item(Type::comment)
{
}

std::string comment_t::str() const
{
	std::ostringstream s;
	s << '#' << cmt;
	return s.str();
}

std::string comment_t::describe() const
{
	std::ostringstream s;
	s << "cmt{" << str() << "}";
	return s.str();
}

identifier_t::identifier_t()
 : list_item(Type::identifier)
{
}

bool identifier_t::ident_p(char c)
{
	return isalnum(c) || c == '_';
}

std::string identifier_t::str() const
{
	return ident;
}

std::string identifier_t::describe() const
{
	std::ostringstream s;
	s << "ident{" << str() << "}";
	return s.str();
}

string_t::string_t()
 : list_item(Type::string)
{
}

std::string string_t::str() const
{
	std::ostringstream ss;
	ss << '"' << s << '"';
	return ss.str();
}

std::string string_t::describe() const
{
	std::ostringstream s;
	s << "str{" << str() << "}";
	return s.str();
}

variable_t::variable_t()
 : list_item(Type::variable)
{
}

bool variable_t::var_p(char c)
{
	return isalnum(c) || c == '_';
}

std::string variable_t::str() const
{
	std::ostringstream s;
	s << "$(" << var << ")";
	return s.str();
}

std::string variable_t::describe() const
{
	std::ostringstream s;
	s << "var{" << str() << "}";
	return s.str();
}

quoted_t::quoted_t()
 : list_item(Type::quoted)
{
}

bool quoted_t::argq_p(char c)
{
	switch(c)
	{
		case '\n':
		case '\r':
		case '"':
		case '#':
		case '(':
		case ')':
		case '\\':
			return false;
		default:
			return true;
	}
}

std::string quoted_t::str() const
{
	std::ostringstream s;
	s << '"';
	for(auto& part : parts)
		s << part->str();
	s << '"';
	return s.str();
}

std::string quoted_t::describe() const
{
	std::ostringstream s;
	s << "quot{" << str() << "}";
	return s.str();
}

unquoted_argument_t::unquoted_argument_t()
 : list_item(Type::unquoted)
{
}

bool unquoted_argument_t::arg_p(char c)
{
	switch(c)
	{
		case '\t':
		case '\n':
		case '\r':
		case ' ':
		case '"':
		case '#':
		case '(':
		case ')':
		case '\\':
			return false;
		default:
			return true;
	}
}

std::string unquoted_argument_t::str() const
{
	std::ostringstream s;
	for(auto& part : parts)
		s << part->str();
	return s.str();
}

std::string unquoted_argument_t::describe() const
{
	std::ostringstream s;
	s << "unq{" << str() << "}";
	return s.str();
}

bracketed_args_t::bracketed_args_t()
 : list_item(Type::bracketed)
{
}

std::string bracketed_args_t::str() const
{
	std::ostringstream s;
	s << '(';
	for(auto& arg : args)
		s << arg->str();
	s << ')';
	return s.str();
}

std::string bracketed_args_t::describe() const
{
	std::ostringstream s;
	s << "bracketed{";
	s << '(';
	for(auto& arg : args)
		s << arg->describe();
	s << ')';
	s << "}";
	return s.str();
}

command_t::command_t()
 : list_item(Type::command)
{
}

std::string command_t::str() const
{
	std::ostringstream s;
	s << name.str();
	for(auto& wsp : name_wsp)
		s << wsp->str();
	s << '(';
	for(auto& arg : args)
		s << arg->str();
	s << ')';
	return s.str();
}

std::string command_t::describe() const
{
	std::ostringstream s;
	s << "cmd{";
	s << name.describe();
	for(auto& wsp : name_wsp)
		s << wsp->describe();
	s << '(';
	for(auto& arg : args)
		s << arg->describe();
	s << ')';
	s << "}";
	return s.str();
}

std::string listfile_t::str() const
{
	std::ostringstream s;
	for(auto& item : items)
		s << item->str();
	return s.str();
}

std::string listfile_t::describe() const
{
	std::ostringstream s;
	for(auto& item : items)
		s << item->describe();
	return s.str();
}

} /* namespace cmake */
