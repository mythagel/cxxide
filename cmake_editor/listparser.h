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
	virtual void whitespace(const char* c, const char* end) =0;
	virtual void comment(const char* c, const char* end) =0;
	virtual void begin_command() =0;
	virtual void end_command() =0;
	virtual void open_bracket() =0;
	virtual void close_bracket() =0;
	virtual void identifier(const char* c, const char* end) =0;
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

	bool parse_identifier(const char*& c, const char* end)
	{
		if(!(isalpha(*c) || *c == '_'))
			return false;

		auto begin = c;
		while(c != end && ident_p(*c))
			++c;
		identifier(begin, c);
		return true;
	}

//	bool parse_command(const char*& c, const char* end)
//	{
//		if(!parse_identifier(c, end))
//			return false;
//
//		while(c != end)
//		{
//			whitespace_t wsp;
//			comment_t cmt;
//
//			if(whitespace_t::parse(c, end, &wsp))
//			{
//				cmd->name_wsp.emplace_back(std::make_shared<whitespace_t>(wsp));
//			}
//			else if(comment_t::parse(c, end, &cmt))
//			{
//				cmd->name_wsp.emplace_back(std::make_shared<comment_t>(cmt));
//			}
//			else if(*c == '(')
//			{
//				break;
//			}
//			else
//			{
//				throw parse_error("command: unexpected char; expected '('");
//			}
//		}
//
//		if(c == end)
//			throw parse_error("command: unexpected eof");
//
//		if(*c != '(')
//			throw parse_error("command: expected '('");
//		++c;
//
//		while(c != end)
//		{
//			whitespace_t wsp;
//			comment_t cmt;
//			string_t str;
//			unquoted_argument_t unq;
//			bracketed_args_t brac;
//
//			if(whitespace_t::parse(c, end, &wsp))
//			{
//				cmd->args.emplace_back(std::make_shared<whitespace_t>(wsp));
//			}
//			else if(comment_t::parse(c, end, &cmt))
//			{
//				cmd->args.emplace_back(std::make_shared<comment_t>(cmt));
//			}
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
//			else if(*c == ')')
//			{
//				++c;
//				return true;
//			}
//			else
//			{
//				throw parse_error("command: unexpected char");
//			}
//		}
//
//		throw parse_error("command: unexpected eof");
//	}

	bool parse(const char*& c, const char* end)
	{
		while(c != end)
		{
			if(parse_whitespace(c, end))
			{

			}
			else if(parse_comment(c, end))
			{

			}
//			else if(parse_command(c, end))
//			{
//
//			}
			else
			{
				return false;
			}
		}
		return true;
	}

	virtual ~listparser() = default;
};

} /* namespace cmake2 */
#endif /* LISTPARSER_H_ */
