/*
 * cmake_parser.cpp
 *
 *  Created on: 01/03/2013
 *      Author: nicholas
 */
#include "listfile.h"

#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <memory>
#include <istream>
#include <iterator>
#include <iostream>
#include <fstream>


bool diff(const std::string& s, const std::string& file)
{
	std::ifstream ifs(file);

	std::noskipws(ifs);
	std::string contents{std::istream_iterator<char>(ifs), std::istream_iterator<char>()};
	return s != contents;
}

int main(int argc, char* argv[])
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
		auto it = source.begin();
		auto end = source.end();
		if(cmake::listfile_t::parse(it, end, &lst))
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


