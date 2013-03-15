/*
 * listparser.h
 *
 *  Created on: 15/03/2013
 *      Author: nicholas
 */

#ifndef LISTPARSER_H_
#define LISTPARSER_H_
#include <cctype>

namespace cmake2
{

struct listparser
{
	struct parser_error
	{
		parser_error(const char* c)
		{
		}
	};

	virtual void whitespace(const char* c, const char* end) =0;
	virtual void comment(const char* c, const char* end) =0;
	virtual void begin_command(const char* c, const char* end) =0;
	virtual void end_command() =0;
	virtual void open_bracket() =0;
	virtual void close_bracket() =0;
	virtual void begin_argument(bool quoted) =0;
	virtual void end_argument() =0;
	virtual void string(const char* c, const char* end) =0;
	virtual void variable(const char* c, const char* end) =0;
	virtual void raw_string(const char* c, const char* end) =0;
	virtual void escape_character(char c) =0;

	static bool ws_p(char c)
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
	bool parse_whitespace(const char*& c, const char* end)
	{
		if(!ws_p(*c))
			return false;

		auto begin = c;
		while(c != end && ws_p(*c))
			++c;
		whitespace(begin, c);
		return true;
	}

	bool parse_comment(const char*& c, const char* end)
	{
		if(*c != '#')
			return false;

		auto begin = ++c;
		while(c != end && *c != '\n')
			++c;
		comment(begin, c);
		return true;
	}

	static bool ident_p(char c)
	{
		return isalnum(c) || c == '_';
	}

	bool parse_command(const char*& c, const char* end)
	{
		{
			if(!(isalpha(*c) || *c == '_'))
				return false;

			auto begin = c;
			while(c != end && ident_p(*c))
				++c;
			begin_command(begin, c);
		}

		while(c != end)
		{
			if(parse_whitespace(c, end))
			{
			}
			else if(parse_comment(c, end))
			{
			}
			else if(*c == '(')
			{
				break;
			}
			else
			{
				throw parser_error("command: unexpected char; expected '('");
			}
		}

		if(c == end)
			throw parser_error("command: unexpected eof");

		if(*c != '(')
			throw parser_error("command: expected '('");

		open_bracket();
		++c;

		while(c != end)
		{
			if(parse_whitespace(c, end))
			{
			}
			else if(parse_comment(c, end))
			{
			}
//			else if(string_t::parse(c, end, &str))
//			{
//				cmd->args.emplace_back(std::make_shared<string_t>(str));
//			}
//			else if(unquoted_argument_t::parse(c, end, &unq))
//			{
//				cmd->args.emplace_back(std::make_shared<unquoted_argument_t>(unq));
//			}
//			else if(bracketed_args_t::parse(c, end, &brac))
//			{
//				cmd->args.emplace_back(std::make_shared<bracketed_args_t>(brac));
//			}
			else if(*c == ')')
			{
				close_bracket();
				++c;
				end_command();
				return true;
			}
			else
			{
				throw parser_error("command: unexpected char");
			}
		}

		throw parser_error("command: unexpected eof");
	}

	void parse(const char*& c, const char* end)
	{
		while(c != end)
		{
			if(parse_whitespace(c, end))
			{
			}
			else if(parse_comment(c, end))
			{
			}
			else if(parse_command(c, end))
			{
			}
			else
			{
				throw parser_error("parse: expected wsp / comment / command");
			}
		}
	}

	virtual ~listparser() = default;
};

} /* namespace cmake2 */
#endif /* LISTPARSER_H_ */
