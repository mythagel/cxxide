/*
 * cmake_editor.cpp
 *
 *  Created on: 20/02/2013
 *      Author: nicholas
 */
#include "project/cmake/listfile.h"
#include <fstream>
#include <iostream>
#include <cassert>

#include <vector>
#include <tuple>
#include <cctype>
#include <sstream>

namespace cmake
{
struct parse_error
{
	parse_error(const char*)
	{
	}
};

struct variable_t
{
	std::string variable;

	static bool var_p(char c)
	{
		return isalnum(c) || c == '_';
	}

	template <typename It>
	static bool parse(It& it, const It& end, variable_t* var)
	{
		if(*it != '$')
			return false;
		++it;

		if(it == end)
			throw parse_error("variable: unexpected eof");

		if(*it != '(')
			throw parse_error("variable");
		++it;

		if(it == end)
			throw parse_error("variable: unexpected eof");

		It begin = it;
		while(it != end && var_p(*it))
			++it;

		if(it == end)
			throw parse_error("variable: unexpected eof");
		if(*it != ')')
			throw parse_error("variable");

		var->variable.assign(begin, it);
		++it;
		return true;
	}

	std::string str() const
	{
		std::ostringstream s;
		s << "$(" << variable << ")";
		return s.str();
	}
};

// unquoted-argument  = (variable / arg_p / escape-char )
// *(variable / arg_p / escape-char /
//   DQUOTE *( variable / argq_p / escape-char ) DQUOTE)

struct unquoted_argument_t
{
	static bool arg_p(char c)
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
	static bool argq_p(char c)
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

	template <typename It>
	static bool parse(It& it, const It& end, unquoted_argument_t* unq)
	{
		return false;
	}
};

}

int main(int argc, char* argv[])
{
	{
		using namespace cmake;

		char* c;
		char* end;
		variable_t var;
		variable_t::parse(c, end, &var);
	}

	using namespace project::cmake;
	assert(argc > 1);

	std::cout << argv[1] << "------------------------------------------\n";
	std::ifstream ifs(argv[1]);

	listfile_t CMakeLists;
	try
	{
		listfile_t::Parse(ifs, CMakeLists);
	}
	catch(const listfile_t::error& ex)
	{
		std::cout << ex.what() << '\n';
		std::cout << "ERROR " << argv[1] << "------------------------------------------\n";
		std::cout << CMakeLists.describe() << "\n";
		return 1;
	}

	std::cout << CMakeLists.str();
	std::cout << "SUCCESS " << argv[1] << "------------------------------------------\n";
}
