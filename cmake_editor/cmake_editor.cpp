/*
 * cmake_parser.cpp
 *
 *  Created on: 01/03/2013
 *      Author: nicholas
 */

#include "listfile.h"
#include "listparser.h"

#include <string>
#include <vector>
#include <istream>
#include <iterator>
#include <iostream>
#include <fstream>

#include <sstream>


bool diff(const std::string& s, const std::string& file)
{
	std::ifstream ifs(file);

	std::noskipws(ifs);
	std::string contents{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
	return s != contents;
}

int main2(int argc, char* argv[])
{
	std::ifstream ifs;
	std::istream* is = &std::cin;

	if(argc > 1)
	{
		std::cout << "------ " << argv[1] << " ------\n";
		ifs.open(argv[1], std::ios::in | std::ios::binary);
		if(!ifs)
			return 1;
		is = &ifs;
	}

	std::noskipws(*is);
	std::string source{std::istream_iterator<char>(*is), std::istream_iterator<char>()};

	cmake::listfile_t lst;
	try
	{
		auto c = source.c_str();
		auto end = c + source.size();
		if(cmake::listfile_t::parse(c, end, &lst))
		{
			std::string str = lst.str();
			std::cout << str;
			if(argc > 1)
			{
				if(diff(str, argv[1]))
				{
					std::cout << ">>>DIFF\n";
					std::ofstream out("out.cmake");
					out << str;
				}
				else
				{
					std::cout << ">>>SUCCESS\n";
				}
			}
			else
			{
				std::cout << ">>>SUCCESS\n";
			}
			return 0;
		}
		else
		{
			std::cout << lst.describe();
			std::cout << ">>>FAILURE\n";
			return 1;
		}
	}
	catch(const cmake::parse_error& ex)
	{
		std::cout << lst.describe();
		std::cout << ">>>EXCEPTION\n";
		throw;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	struct listp : public cmake2::listparser_t
	{
		std::ostringstream s;

		virtual void whitespace(const char* c, const char* end)
		{
			s << std::string(c, end);
		}
		virtual void comment(const char* c, const char* end)
		{
			s << '#' << std::string(c, end);
		}
		virtual void begin_command(const char* c, const char* end)
		{
			s << std::string(c, end);
		}
		virtual void open_bracket()
		{
			s << '(';
		}
		virtual void close_bracket()
		{
			s << ')';
		}
		virtual void argument(const char* c, const char* end, bool quoted)
		{
			if(quoted)
				s << '"' << std::string(c, end) << '"';
			else
				s << std::string(c, end);
		}
		virtual void end_command()
		{
			// ni
		}

		virtual ~listp() = default;
	};

	struct listq : public cmake2::listparser_t
	{
		std::ostringstream s;

		virtual void whitespace(const char* c, const char* end)
		{
			s << std::string(c, end);
		}
		virtual void comment(const char* c, const char* end)
		{
			s << "cmt{" << '#' << std::string(c, end) << "}";
		}
		virtual void begin_command(const char* c, const char* end)
		{
			s << "cmd{ident{" << std::string(c, end) << "}";
		}
		virtual void open_bracket()
		{
			s << '(';
		}
		virtual void close_bracket()
		{
			s << ')';
		}
		virtual void argument(const char* c, const char* end, bool quoted)
		{
			s << "arg{";
			if(quoted)
				s << '"' << std::string(c, end) << '"';
			else
				s << std::string(c, end);
			s << "}";
		}
		virtual void end_command()
		{
			s << "}";
		}

		virtual ~listq() = default;
	};

	std::ifstream ifs;
	std::istream* is = &std::cin;

	if(argc > 1)
	{
		std::cout << "------ " << argv[1] << " ------\n";
		ifs.open(argv[1], std::ios::in | std::ios::binary);
		if(!ifs)
			return 1;
		is = &ifs;
	}

	std::noskipws(*is);
	std::string source{std::istream_iterator<char>(*is), std::istream_iterator<char>()};

	listp lst;
	try
	{
		auto c = source.c_str();
		auto end = c + source.size();

		lst.parse(c, end);

		std::string str = lst.s.str();
		std::cout << str;
		if(argc > 1)
		{
			if(diff(str, argv[1]))
			{
				std::cout << ">>>DIFF\n";
				std::ofstream out("out.cmake");
				out << str;
			}
			else
			{
				std::cout << ">>>SUCCESS\n";
			}
		}
		else
		{
			std::cout << ">>>SUCCESS\n";
		}
		return 0;
	}
	catch(...)
	{
		std::cout << ">>>EXCEPTION\n";
		throw;
	}
	return 0;
}

