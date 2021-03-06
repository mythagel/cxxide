/*
 * listparser_t.cpp
 *
 *  Created on: 15/03/2013
 *      Author: nicholas
 */

#include "listparser.h"
#include <cctype>

namespace
{
void throw_if(bool cond, const std::string& what)
{
    if(cond) throw cxxide::cmake::parse_error(what);
}

bool ws_p(const char c)
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

bool var_p(const char c)
{
	return isalnum(c) || c == '_';
}

bool argq_p(const char c)
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

bool arg_p(const char c)
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

bool ident_p(char c)
{
	return isalnum(c) || c == '_';
}

bool match_escape_character(const char*& c, const char* const end)
{
	const auto begin = c;
	if(*c != '\\')
		return false;
	++c;
	if(c == end)
	{
		c = begin;
		return false;
	}

	switch(*c)
	{
		case ' ':
		case '"':
		case '#':
		case '$':
		case '(':
		case ')':
		case '0':
		case ';':
		case '@':
		case '\\':
		case '^':
		case 'n':
		case 'r':
		case 't':
			break;
		default:
		{
			c = begin;
			return false;
		}
	}

	++c;
	return true;
}

bool match_variable(const char*& c, const char* const end)
{
	const auto begin = c;
	if(*c != '$')
		return false;
	++c;

	if(c == end)
	{
		c = begin;
		return false;
	}

	if(*c != '(')
	{
		c = begin;
		return false;
	}
	++c;

	if(c == end)
	{
		c = begin;
		return false;
	}

	while(c != end && var_p(*c))
		++c;

	if(c == end || *c != ')')
	{
		c = begin;
		return false;
	}
	++c;

	return true;
}

bool match_argqp(const char*& c, const char* const end)
{
	if(!argq_p(*c))
		return false;
	++c;

	while(c != end && argq_p(*c))
		++c;
	return true;
}

bool match_quoted(const char*& c, const char* const end)
{
	const auto begin = c;
	if(*c != '"')
		return false;
	++c;

	while(c != end)
	{
		if(match_variable(c, end) || match_argqp(c, end) || match_escape_character(c, end))
			continue;
		else if(*c == '"')
		{
			++c;
			return true;
		}
		else
		{
			c = begin;
			return false;
		}
	}

	c = begin;
	return false;
}

bool match_arg(const char*& c, const char* const end)
{
	if(!arg_p(*c))
		return false;
	++c;

	while(c != end && arg_p(*c))
		++c;
	return true;
}

bool match_identifier(const char*& c, const char* const end)
{
	if(!(isalpha(*c) || *c == '_'))
		return false;
	++c;

	while(c != end && ident_p(*c))
		++c;
	return true;
}

}

namespace cxxide
{
namespace cmake
{

parse_error::parse_error(const std::string& what)
 : std::runtime_error(what)
{
}
parse_error::~parse_error() noexcept
{
}

bool listparser_t::parse_whitespace(const char*& c, const char* const end)
{
	if(!ws_p(*c))
		return false;

	const auto begin = c;
	while(c != end && ws_p(*c))
		++c;
	whitespace(begin, c);
	return true;
}

bool listparser_t::parse_comment(const char*& c, const char* const end)
{
	if(*c != '#')
		return false;

	const auto begin = ++c;
	while(c != end && *c != '\n')
		++c;
	comment(begin, c);
	return true;
}

bool listparser_t::parse_quoted_argument(const char*& c, const char* const end)
{
	if(*c != '"')
		return false;

	++c;
	const auto begin = c;
	while(c != end)
	{
		if(*c == '"')
			break;
		else if(match_escape_character(c, end))
			continue;
		++c;
	}

	throw_if(c == end, "unexpected eof");
	throw_if(*c != '"', "expected \"");

	argument(begin, c, true);
	++c;
	return true;
}

bool listparser_t::parse_unquoted_argument(const char*& c, const char* const end)
{
	const auto begin = c;
	if(! (match_variable(c, end) || match_arg(c, end) || match_escape_character(c, end)) )
	{
		if(match_identifier(c, end))
		{
			argument(begin, c, false);
			return true;
		}
		return false;
	}

	while(c != end)
	{
		if(match_variable(c, end) || match_arg(c, end) || match_escape_character(c, end) || match_quoted(c, end))
			continue;
		else
			break;
	}

	argument(begin, c, false);
	return true;
}

bool listparser_t::parse_command(const char*& c, const char* const end)
{
	const auto begin = c;
	if(!match_identifier(c, end))
		return false;

	begin_command(begin, c);

	while(c != end)
	{
		if(parse_whitespace(c, end) || parse_comment(c, end))
			continue;
		else if(*c == '(')
			break;
		else
			throw parse_error{"expected ws / cmt / ("};
	}

	throw_if(c == end, "unexpected eof");
	throw_if(*c != '(', "expected (");

	open_bracket();
	++c;
	std::size_t nesting(0);

	while(c != end)
	{
		if(parse_whitespace(c, end) || parse_comment(c, end) || parse_quoted_argument(c, end) || parse_unquoted_argument(c, end))
			continue;
		else if(*c == '(')
		{
			open_bracket();
			++c;
			++nesting;
		}
		else if(*c == ')')
		{
			close_bracket();
			++c;
			if(nesting == 0)
			{
				end_command();
				return true;
			}
			else
			{
				--nesting;
			}
		}
		else
			throw parse_error{"expected ws / cmt / quoted-argument / unquoted-argument / ( / )"};
	}

	throw parse_error("unexpected eof");
}

void listparser_t::parse(const char* c, const char* const end)
{
	while(c != end)
	{
		if(parse_whitespace(c, end) || parse_comment(c, end) || parse_command(c, end))
			continue;
		else
			throw parse_error{"expected wsp / comment / command"};
	}
	eof();
}

listparser_t::~listparser_t()
{
}

}
}

