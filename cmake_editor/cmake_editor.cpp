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
namespace cmake
{
struct ws_t
{
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

	template <typename It>
	static bool parse(It& it, const It& end, std::string* ws)
	{
		if(it == end)
			return false;
		if(!ws_p(*it))
			return false;

		It begin = it;
		while(it != end && ws_p(*it++))
			;
		ws->assign(begin, it);
		return true;
	}
};


}

int main(int argc, char* argv[])
{
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
