/*
 * listfile.h
 *
 *  Created on: 07/01/2013
 *      Author: nicholas
 */

#ifndef LISTFILE_H_
#define LISTFILE_H_
#include <stdexcept>
#include <string>
#include <vector>
#include <istream>
#include <iterator>
#include "item.h"

namespace project
{
namespace cmake
{

/*
 * Doesn't parse correctly yet - cmake contains bracketed argument lists.
 * need to analyse the flex and yacc files in cmake tree and extract a formal
 * grammer for the language (or at least a more complete understanding of the
 * syntax.)
 */
class listfile_t
{
private:
	static void ParseWhitespace(std::istream_iterator<char>& it, const std::istream_iterator<char>& end, whitespace_t* whitespace);
	static void ParseComment(std::istream_iterator<char>& it, const std::istream_iterator<char>& end, comment_t* comment);
	static void ParseCommand(std::istream_iterator<char>& it, const std::istream_iterator<char>& end, command_t* command);
	static void ParseArgument(std::istream_iterator<char>& it, const std::istream_iterator<char>& end, argument_t* argument);
public:
	std::vector<item> items;

	struct error : public std::runtime_error
	{
		error(const std::string& s)
		 : std::runtime_error(s)
		{
		}
	};

	static void Parse(std::istream& is, listfile_t* list);

	std::string str() const;
	std::string describe() const;
};

} /* namespace cmake */
} /* namespace project */
#endif /* LISTFILE_H_ */
