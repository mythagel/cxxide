/*
 * argparser.h
 *
 *  Created on: 20/03/2013
 *      Author: nicholas
 */

#ifndef ARGPARSER_H_
#define ARGPARSER_H_
#include <cctype>

namespace cmake2
{

struct argparser_t
{
	virtual void atname(const char* c, const char* const end) =0;
	virtual void env_curly(const char* c, const char* const end) =0;
	virtual void d_curly(const char* c, const char* const end) =0;
	virtual void n_curly(const char* name_begin, const char* const name_end, const char* c, const char* const end) =0;

//	bool parse(const char*& c, const char* const end)
//	{
//		const auto x = c;
//		if(type_parser(c, end))
//			return true;
//		c = x;
//		return false;
//	}

	bool name_p(const char c)
	{
		return isalnum(c) || c == '/' || c == '_' || c == '.' || c == '+' || c == '-';
	}
	//	name = 1*(ALPHA / DIGIT / "/" / "_" / "." / "+" / "-")
	bool match_name(const char*& c, const char* end)
	{
		if(!name_p(*c))
			return false;
		++c;

		while(c != end && name_p(*c))
			++c;
		return true;
	}

	bool symbol_p(const char c)
	{
		return c != '$' && c != '{' && c != '}' && c != '\\' && c != '@';
	}
	//	symbol = 1*<CHAR excluding "$" "{" "}" "\\" "@">
	bool match_symbol(const char*& c, const char* end)
	{
		if(!symbol_p(*c))
			return false;
		++c;

		while(c != end && symbol_p(*c))
			++c;
		return true;
	}

	//	atname = "@" name "@"
	bool _atname(const char*& c, const char* const end)
	{
		if(*c != '@')
			return false;
		++c;
		if(c == end)
			return false;

		const auto begin = c;

		if(!match_name(c, end))
			return false;

		if(c == end || *c != '@')
			return false;

		atname(begin, c);
		++c;
		return true;
	}
	bool parse_atname(const char*& c, const char* const end)
	{
		const auto x = c;
		if(_atname(c, end))
			return true;
		c = x;
		return false;
	}

	//	ncurly = "$" name "{" MultipleIds "}"
	bool _ncurly(const char*& c, const char* end)
	{
		if(*c != '$')
			return false;
		++c;

		const auto name_begin = c;
		if(!match_name(c, end))
			return false;
		const auto name_end = c;

		if(c == end || *c != '{')
			return false;

		const auto begin = c;
		if(!match_multipleids(c, end))
			return false;

		if(c == end || *c != '}')
			return false;

		n_curly(name_begin, name_end, begin, c);
		++c;
		return true;
	}
	bool parse_ncurly(const char*& c, const char* end)
	{
		const auto x = c;
		if(_ncurly(c, end))
			return true;
		c = x;
		return false;
	}

	//	dcurly = "$" "{" MultipleIds "}"
	bool parse_dcurly(const char*& c, const char* end)
	{
		if(*c != '$')
			return false;
		++c;
		if(c == end || *c != '{')
			return false;
		++c;
		if(c == end)
			return false;

		const auto begin = c;
		if(!match_multipleids(c, end))
			return false;

		if(c == end || *c != '}')
			return false;

		d_curly(begin, c);
		++c;
		return true;
	}

	//	Variable = env-curly / ncurly / dcurly / atname
	bool match_variable(const char*& c, const char* const end)
	{
		return parse_env_curly(c, end) || parse_ncurly(c, end) || parse_dcurly(c, end) || parse_atname(c, end);
	}

	//	MultipleIds = *(name / Variable)
	bool match_multipleids(const char*& c, const char* end)
	{
		if(! (match_name(c, end) || match_variable(c, end)))
			return false;

		while(c != end)
		{
			if(match_name(c, end) || match_variable(c, end))
				continue;
			else
				break;
		}
		return true;
	}

	//	EnvVarName = MultipleIds / (symbol EnvVarName)
	bool _envvarname(const char*& c, const char* end)
	{
		return match_multipleids(c, end) || (match_symbol(c, end) && match_envvarname(c, end));
	}
	bool match_envvarname(const char*& c, const char* end)
	{
		const auto x = c;
		if(_envvarname(c, end))
			return true;
		c = x;
		return false;
	}

	//	env-curly = "$" "E" "N" "V" "{" EnvVarName "}"
	bool _env_curly(const char*& c, const char* end)
	{
		if(*c != '$')
			return false;
		++c;
		if(c == end || *c != 'E')
			return false;
		++c;
		if(c == end || *c != 'N')
			return false;
		++c;
		if(c == end || *c != 'V')
			return false;
		++c;
		if(c == end || *c != '{')
			return false;
		++c;
		if(c == end)
			return false;

		const auto begin = c;
		if(!match_envvarname(c, end))
			return false;

		if(c == end || *c != '}')
			return false;

		env_curly(begin, c);
		++c;
		return true;
	}

	bool parse_env_curly(const char*& c, const char* end)
	{
		const auto x = c;
		if(_env_curly(c, end))
			return true;
		c = x;
		return false;
	}

//	Start = (String cal_BSLASH) / String
//
//	String = Variable / OuterText
//


//
//	OuterText = cal_name / "@" / "$" / "{" / "}" / cal_symbol
//



//





};

} /* namespace cmake2 */
#endif /* ARGPARSER_H_ */
