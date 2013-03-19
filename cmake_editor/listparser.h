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
	virtual void open_bracket() =0;
	virtual void close_bracket() =0;
	virtual void argument(const char* c, const char* end, bool quoted) =0;
	virtual void end_command() =0;

//	template<typename T>
//	struct parser_t
//	{
//		static bool parse(const char*& c, const char* end, T* type)
//		{
//			auto c0 = c;
//			if(!T::_parse(c, end, type))
//			{
//				c = c0;
//				return false;
//			}
//			return true;
//		}
//	};

	static bool ws_p(const char c)
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

		const auto begin = c;
		while(c != end && ws_p(*c))
			++c;
		whitespace(begin, c);
		return true;
	}

	bool parse_comment(const char*& c, const char* end)
	{
		if(*c != '#')
			return false;

		const auto begin = ++c;
		while(c != end && *c != '\n')
			++c;
		comment(begin, c);
		return true;
	}

	bool match_escape_character(const char*& c, const char* end) const
	{
		if(*c != '\\')
			return false;
		++c;
		if(c == end)
			throw parser_error("escape_char: Unexpected eof");

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
				throw parser_error("escape_char: Expected [ \"#$()0;@\\^nrt]");
		}

		++c;
		return true;
	}

	bool parse_quoted_argument(const char*& c, const char* end)
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

		if(c == end)
			throw parser_error("string: unexpected eof; expected '\"'");
		if(*c != '"')
			throw parser_error("string: expected '\"'");

		argument(begin, c, true);
		++c;
		return true;
	}

	static bool var_p(const char c)
	{
		return isalnum(c) || c == '_';
	}

	bool match_variable(const char*& c, const char* end) const
	{
		if(*c != '$')
			return false;
		++c;

		if(c == end)
			throw parser_error("variable: unexpected eof");

		if(*c != '(')
			return false;
		++c;

		if(c == end)
			throw parser_error("variable: unexpected eof");

		while(c != end && var_p(*c))
			++c;

		if(c == end)
			throw parser_error("variable: unexpected eof; expected ')'");
		if(*c != ')')
			throw parser_error("variable: expected ')'");
		++c;

		return true;
	}

	static bool argq_p(const char c)
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

	bool match_quoted(const char*& c, const char* end) const
	{
		if(*c != '"')
			return false;
		++c;

		while(c != end)
		{
			if(match_variable(c, end))
			{
			}
			else if(argq_p(*c))
			{
				++c;
			}
			else if(match_escape_character(c, end))
			{
			}
			else if(*c == '"')
			{
				++c;
				return true;
			}
			else
			{
				throw parser_error("quoted: unexpected char");
			}
		}
		throw parser_error("quoted: unexpected eof; expected '\"'");
	}

	static bool arg_p(const char c)
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

	bool match_arg(const char*& c, const char* end) const
	{
		if(!arg_p(*c))
			return false;
		++c;

		while(c != end && arg_p(*c))
			++c;
		return true;
	}

	bool parse_unquoted_argument(const char*& c, const char* end)
	{
		const auto begin = c;
		if(match_identifier(c, end))
		{
			argument(begin, c, false);
			return true;
		}

		if(match_variable(c, end) || match_arg(c, end) || match_escape_character(c, end))
		{
		}
		else
		{
			return false;
		}

		while(c != end)
		{
			if(match_variable(c, end) || match_arg(c, end) || match_escape_character(c, end) || match_quoted(c, end))
			{
			}
			else
			{
				break;
			}
		}

		argument(begin, c, false);
		return true;
	}

	static bool ident_p(char c)
	{
		return isalnum(c) || c == '_';
	}

	bool match_identifier(const char*& c, const char* end)
	{
		if(!(isalpha(*c) || *c == '_'))
			return false;

		while(c != end && ident_p(*c))
			++c;
		return true;
	}

	bool parse_command(const char*& c, const char* end)
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
				throw parser_error("command: unexpected char; expected ws / cmt / (");
		}

		if(c == end)
			throw parser_error("command: unexpected eof");

		if(*c != '(')
			throw parser_error("command: expected '('");

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
				throw parser_error("command: unexpected char");
		}

		throw parser_error("command: unexpected eof");
	}

	void parse(const char*& c, const char* end)
	{
		while(c != end)
		{
			if(parse_whitespace(c, end) || parse_comment(c, end) || parse_command(c, end))
				continue;
			else
				throw parser_error("parse: expected wsp / comment / command");
		}
	}

	virtual ~listparser() = default;
};

namespace lex
{
inline bool ws_p(char c)
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

inline bool ident_p(char c)
{
	return isalnum(c) || c == '_';
}
inline bool escp_p(char c)
{
	switch(c)
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
			return true;
		default:
			return false;
	}
}
inline bool var_p(char c)
{
	return isalnum(c) || c == '_';
}

struct token
{
	const char* c;
	const char* end;

	const char* token_start;
	const char* token_end;
	enum class Type
	{
		whitespace,
		comment,
		identifier,
		lparen,
		rparen,
		string,
		variable,
		unquoted,
		eof
	} token_type;

	void set(const char* begin, const char* end, Type t)
	{
		token_start = begin;
		token_end = end;
		token_type = t;
	}

	inline bool eof() const
	{
		return c == end;
	}

	token(const char* c, const char* end)
	 : c(c), end(end)
	{
	}

	bool operator++()
	{
		if(eof())
		{
			set(nullptr, nullptr, Type::eof);
			return true;
		}
		else if(ws_p(*c))
		{
			const auto begin = c;
			while(c != end && ws_p(*c))
				++c;
			set(begin, c, Type::whitespace);
			return true;
		}
		else if(*c == '#')
		{
			const auto begin = ++c;
			while(c != end && *c != '\n')
				++c;
			set(begin, c, Type::comment);
			return true;
		}
		else if(isalpha(*c) || *c == '_')
		{
			const auto begin = c;
			while(c != end && ident_p(*c))
				++c;
			set(begin, c, Type::identifier);
		}
		else if(*c == '(')
		{
			set(nullptr, nullptr, Type::lparen);
			return true;
		}
		else if(*c == ')')
		{
			set(nullptr, nullptr, Type::rparen);
			return true;
		}
		else if(*c == '"')
		{
			++c;
			const auto begin = c;
			while(c != end)
			{
				if(*c == '"')
					break;
				else if(*c == '\\')
				{
					++c;
					if(eof())
						return false;
					if(!escp_p(*c))
						return false;
				}
				++c;
			}

			if(eof())
				return false;
			if(*c != '"')
				return false;

			set(begin, c, Type::string);
			++c;
			return true;
		}
		else if(*c == '$')
		{
			++c;

			if(eof())
				return false;

			if(*c != '(')
				return false;
			++c;

			if(eof())
				return false;

			const auto begin = c;
			while(c != end && var_p(*c))
				++c;

			if(eof())
				return false;
			if(*c != ')')
				return false;

			set(begin, c, Type::variable);
			++c;
			return true;
		}

		return false;
	}
};
}

} /* namespace cmake2 */
#endif /* LISTPARSER_H_ */
