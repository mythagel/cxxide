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

bool whitespace_t::_parse(const char*& c, const char* end, whitespace_t* wsp)
{
	if(!ws_p(*c))
		return false;

	auto begin = c;
	while(c != end && ws_p(*c))
		++c;
	wsp->wsp.assign(begin, c);
	return true;
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

bool comment_t::_parse(const char*& c, const char* end, comment_t* cmt)
{
	if(*c != '#')
		return false;

	++c;
	auto begin = c;
	while(c != end && *c != '\n')
		++c;
	cmt->cmt.assign(begin, c);
	return true;
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

bool identifier_t::_parse(const char*& c, const char* end, identifier_t* ident)
{
	if(!(isalpha(*c) || *c == '_'))
		return false;

	auto begin = c;
	while(c != end && ident_p(*c))
		++c;
	ident->ident.assign(begin, c);
	return true;
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

bool string_t::_parse(const char*& c, const char* end, string_t* str)
{
	if(*c != '"')
		return false;

	++c;
	auto begin = c;
	while(c != end)
	{
		if(*c == '"')
			break;
		if(*c == '\\')
		{
			++c;	// TODO: validate character
			if(c == end)
				throw parse_error("string: unexpected eof; expected <char>");
		}
		++c;
	}
	str->s.assign(begin, c);

	if(c == end)
		throw parse_error("string: unexpected eof; expected '\"'");
	if(*c != '"')
		throw parse_error("string: expected '\"'");

	++c;
	return true;
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

bool escape_char_p(const char*& c, const char* end, std::string* escp)
{
	if(*c != '\\')
		return false;
	auto begin = c;
	++c;
	if(c == end)
		throw parse_error("escape_char: Unexpected eof");

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
			throw parse_error("escape_char: Expected [ \"#$()0;@\\^nrt]");
	}

	++c;
	escp->assign(begin, c);
	return true;
}

variable_t::variable_t()
 : list_item(Type::variable)
{
}

bool variable_t::var_p(char c)
{
	return isalnum(c) || c == '_';
}

bool variable_t::_parse(const char*& c, const char* end, variable_t* var)
{
	if(*c != '$')
		return false;
	++c;

	if(c == end)
		throw parse_error("variable: unexpected eof");

	if(*c != '(')
		return false;
	++c;

	if(c == end)
		throw parse_error("variable: unexpected eof");

	auto begin = c;
	while(c != end && var_p(*c))
		++c;
	var->var.assign(begin, c);

	if(c == end)
		throw parse_error("variable: unexpected eof; expected ')'");
	if(*c != ')')
		throw parse_error("variable: expected ')'");
	++c;

	return true;
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

bool quoted_t::_parse(const char*& c, const char* end, quoted_t* quot)
{
	if(*c != '"')
		return false;
	++c;

	quot->parts.clear();

	variable_t var;
	std::string escp;

	while(c != end)
	{
		if(variable_t::parse(c, end, &var))
		{
			quot->parts.emplace_back(std::make_shared<variable_t>(var));
		}
		else if(argq_p(*c))
		{
			if(quot->parts.empty() || quot->parts.back()->type != Type::raw_string)
			{
				quot->parts.emplace_back(std::make_shared<raw_string_t>(std::string(1, *c)));
			}
			else
			{
				auto strng = std::dynamic_pointer_cast<raw_string_t>(quot->parts.back());
				strng->s += *c;
			}
			++c;
		}
		else if(escape_char_p(c, end, &escp))
		{
			if(quot->parts.empty() || quot->parts.back()->type != Type::raw_string)
			{
				quot->parts.emplace_back(std::make_shared<raw_string_t>(escp));
			}
			else
			{
				auto strng = std::dynamic_pointer_cast<raw_string_t>(quot->parts.back());
				strng->s += escp;
			}
		}
		else if(*c == '"')
		{
			++c;
			return true;
		}
		else
		{
			throw parse_error("quoted: unexpected char");
		}
	}
	throw parse_error("quoted: unexpected eof; expected '\"'");
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

bool unquoted_argument_t::_parse(const char*& c, const char* end, unquoted_argument_t* unq)
{
	unq->parts.clear();

	variable_t var;
	std::string escp;

	if(variable_t::parse(c, end, &var))
	{
		unq->parts.emplace_back(std::make_shared<variable_t>(var));
	}
	else if(arg_p(*c))
	{
		unq->parts.emplace_back(std::make_shared<raw_string_t>(std::string(1, *c)));
		++c;
	}
	else if(escape_char_p(c, end, &escp))
	{
		unq->parts.emplace_back(std::make_shared<raw_string_t>(escp));
	}
	else
	{
		return false;
	}

	while(c != end)
	{
		quoted_t quot;

		if(variable_t::parse(c, end, &var))
		{
			unq->parts.emplace_back(std::make_shared<variable_t>(var));
		}
		else if(arg_p(*c))
		{
			if(unq->parts.empty() || unq->parts.back()->type != Type::raw_string)
			{
				unq->parts.emplace_back(std::make_shared<raw_string_t>(std::string(1, *c)));
			}
			else
			{
				auto strng = std::dynamic_pointer_cast<raw_string_t>(unq->parts.back());
				strng->s += *c;
			}
			++c;
		}
		else if(escape_char_p(c, end, &escp))
		{
			if(unq->parts.empty() || unq->parts.back()->type != Type::raw_string)
			{
				unq->parts.emplace_back(std::make_shared<raw_string_t>(escp));
			}
			else
			{
				auto strng = std::dynamic_pointer_cast<raw_string_t>(unq->parts.back());
				strng->s += escp;
			}
		}
		else if(quoted_t::parse(c, end, &quot))
		{
			unq->parts.emplace_back(std::make_shared<quoted_t>(quot));
		}
		else
		{
			break;
		}
	}

	return true;
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

bool bracketed_args_t::_parse(const char*& c, const char* end, bracketed_args_t* brac)
{
	if(*c != '(')
		return false;
	++c;

	brac->args.clear();
	while(c != end)
	{
		whitespace_t wsp;
		comment_t cmt;
		string_t str;
		unquoted_argument_t unq;
		bracketed_args_t brac_nest;

		if(whitespace_t::parse(c, end, &wsp))
		{
			brac->args.emplace_back(std::make_shared<whitespace_t>(wsp));
		}
		else if(comment_t::parse(c, end, &cmt))
		{
			brac->args.emplace_back(std::make_shared<comment_t>(cmt));
		}
		else if(string_t::parse(c, end, &str))
		{
			brac->args.emplace_back(std::make_shared<string_t>(str));
		}
		else if(unquoted_argument_t::parse(c, end, &unq))
		{
			brac->args.emplace_back(std::make_shared<unquoted_argument_t>(unq));
		}
		else if(bracketed_args_t::parse(c, end, &brac_nest))
		{
			brac->args.emplace_back(std::make_shared<bracketed_args_t>(brac_nest));
		}
		else if(*c == ')')
		{
			++c;
			return true;
		}
		else
		{
			throw parse_error("bracketed: expected ')'");
		}
	}
	throw parse_error("bracketed: unexpected eof");
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

bool command_t::_parse(const char*& c, const char* end, command_t* cmd)
{
	if(!identifier_t::parse(c, end, &cmd->name))
		return false;

	while(c != end)
	{
		whitespace_t wsp;
		comment_t cmt;

		if(whitespace_t::parse(c, end, &wsp))
		{
			cmd->name_wsp.emplace_back(std::make_shared<whitespace_t>(wsp));
		}
		else if(comment_t::parse(c, end, &cmt))
		{
			cmd->name_wsp.emplace_back(std::make_shared<comment_t>(cmt));
		}
		else if(*c == '(')
		{
			break;
		}
		else
		{
			throw parse_error("command: unexpected char; expected '('");
		}
	}

	if(c == end)
		throw parse_error("command: unexpected eof");

	if(*c != '(')
		throw parse_error("command: expected '('");
	++c;

	while(c != end)
	{
		whitespace_t wsp;
		comment_t cmt;
		string_t str;
		unquoted_argument_t unq;
		bracketed_args_t brac;

		if(whitespace_t::parse(c, end, &wsp))
		{
			cmd->args.emplace_back(std::make_shared<whitespace_t>(wsp));
		}
		else if(comment_t::parse(c, end, &cmt))
		{
			cmd->args.emplace_back(std::make_shared<comment_t>(cmt));
		}
		else if(string_t::parse(c, end, &str))
		{
			cmd->args.emplace_back(std::make_shared<string_t>(str));
		}
		else if(unquoted_argument_t::parse(c, end, &unq))
		{
			cmd->args.emplace_back(std::make_shared<unquoted_argument_t>(unq));
		}
		else if(bracketed_args_t::parse(c, end, &brac))
		{
			cmd->args.emplace_back(std::make_shared<bracketed_args_t>(brac));
		}
		else if(*c == ')')
		{
			++c;
			return true;
		}
		else
		{
			throw parse_error("command: unexpected char");
		}
	}

	throw parse_error("command: unexpected eof");
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

bool listfile_t::_parse(const char*& c, const char* end, listfile_t* lst)
{
	while(c != end)
	{
		comment_t cmt;
		whitespace_t wsp;
		command_t cmd;

		if(whitespace_t::parse(c, end, &wsp))
		{
			lst->items.emplace_back(std::make_shared<whitespace_t>(wsp));
		}
		else if(comment_t::parse(c, end, &cmt))
		{
			lst->items.emplace_back(std::make_shared<comment_t>(cmt));
		}
		else if(command_t::parse(c, end, &cmd))
		{
			lst->items.emplace_back(std::make_shared<command_t>(cmd));
		}
		else
		{
			throw parse_error("top-level");
		}
	}
	return true;
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
