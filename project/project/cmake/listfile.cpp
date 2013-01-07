/*
 * listfile.cpp
 *
 *  Created on: 07/01/2013
 *      Author: nicholas
 */

#include "listfile.h"
#include <sstream>

namespace project
{
namespace cmake
{

std::string listfile_t::str() const
{
	std::ostringstream s;

	for(auto& it : items)
		s << it->str();

	return s.str();
}

std::string listfile_t::describe() const
{
	std::ostringstream s;

	for(auto& it : items)
		s << it->describe();

	return s.str();
}

void listfile_t::ParseWhitespace(std::istream_iterator<char>& it, const std::istream_iterator<char>& end, whitespace_t* whitespace)
{
	switch(*it)
	{
		case ' ':
		case '\t':
		case '\n':
		{
			whitespace->ws_char = *it++;
			whitespace->count = 1;
			while(it != end && *it == whitespace->ws_char)
			{
				++whitespace->count;
				++it;
			}
			return;
		}
	}

	throw error("Whitespace parse failure");
}

void listfile_t::ParseComment(std::istream_iterator<char>& it, const std::istream_iterator<char>& end, comment_t* comment)
{
	if(*it != '#')
		throw error("Comment parse failure");

	++it;
	while(it != end && *it != '\n')
		comment->comment += *it++;
	if(it != end)
		++it;
}

void listfile_t::ParseCommand(std::istream_iterator<char>& it, const std::istream_iterator<char>& end, command_t* command)
{
	std::string name;
	name += *it++;
	while(it != end && (isalpha(*it) || *it == '_'))
		name += *it++;

	command->name = name;

	if(it == end)
		throw error("eof");

	while(it != end)
	{
		if(*it == '(')
		{
			++it;
			break;
		}

		switch(*it)
		{
			case ' ':
			case '\t':
			case '\n':
			{
				whitespace_t whitespace;
				ParseWhitespace(it, end, &whitespace);
				command->name_ws.push_back(whitespace);
				continue;
			}
			default:
				throw error("invalid character");
		}

		++it;
	}

	while(it != end)
	{
		if(*it == ')')
			break;

		switch(*it)
		{
			case ' ':
			case '\t':
			case '\n':
			{
				whitespace_t* whitespace = new whitespace_t;
				ParseWhitespace(it, end, whitespace);
				command->items.emplace_back(whitespace);
				continue;
			}
			case '#':
			{
				comment_t* comment = new comment_t;
				ParseComment(it, end, comment);
				command->items.emplace_back(comment);
				continue;
			}
		}

		if(isalnum(*it) || *it == '"' || *it == '_' || *it == '-' || *it == '(' || *it == '.' || *it == '[' || *it == '@' || *it == '$' || *it == '/' || *it == '\\')
		{
			argument_t* argument = new argument_t;
			ParseArgument(it, end, argument);
			command->items.emplace_back(argument);
			continue;
		}

		++it;
	}
}

void listfile_t::ParseArgument(std::istream_iterator<char>& it, const std::istream_iterator<char>& end, argument_t* argument)
{
	bool in_quote(false);
	if(*it == '"')
	{
		++it;
		argument->quoted = true;
		while(it != end)
		{
			if(*it == '"')
			{
				++it;
				// TODO escaped quotes
				break;
			}
			argument->value += *it++;
		}
		return;
	}

	size_t quote_nesting{0};

	while(it != end)
	{
		switch(*it)
		{
			case '"':
				in_quote = !in_quote;
			case '$':
			case '{':
			case '}':
			case '-':
			case '_':
			case '.':
			case '/':
			case '*':
			case '@':
			case ':':
			case '=':
			case '[':
			case ']':
			case '+':
			case '<':
			case '>':
			{
				argument->value += *it++;
				continue;
			}
			case '\\':
			{
				argument->value += *it++;
				if(it == end)
					throw error("argument");
				argument->value += *it++;
				continue;
			}
			case '(':
			{
				if(!in_quote)
					++quote_nesting;
				argument->value += *it++;
				continue;
			}
			case ')':
			{
				if(in_quote)
				{
					argument->value += *it++;
					continue;
				}
				else if(quote_nesting)
				{
					argument->value += *it++;
					--quote_nesting;
					continue;
				}
				else
				{
					return;
				}
			}
		}

		if(isalnum(*it))
		{
			argument->value += *it++;
		}
		else
		{
//			std::cout << *it << '\n';
			return;
		}
	}

	throw error("argument");
}

void listfile_t::Parse(std::istream& is, listfile_t* list)
{
	std::noskipws(is);
	std::istream_iterator<char> end; // end-of-stream iterator
	std::istream_iterator<char> it(is); // stdin iterator

	while(it != end)
	{
		switch(*it)
		{
			case ' ':
			case '\t':
			case '\n':
			{
				whitespace_t* whitespace = new whitespace_t;
				ParseWhitespace(it, end, whitespace);
				list->items.emplace_back(whitespace);
				continue;
			}
			case '#':
			{
				comment_t* comment = new comment_t;
				ParseComment(it, end, comment);
				list->items.emplace_back(comment);
				continue;
			}
		}

		if(isalpha(*it))
		{
			command_t* command = new command_t;
			ParseCommand(it, end, command);
			list->items.emplace_back(command);
			continue;
		}

		++it;
	}
}

} /* namespace cmake */
} /* namespace project */
