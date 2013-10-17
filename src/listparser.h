/*
 * listparser.h
 *
 *  Created on: 15/03/2013
 *      Author: nicholas
 */

#ifndef LISTPARSER_H_
#define LISTPARSER_H_
#include <stdexcept>

namespace cxxide
{
namespace cmake
{

struct parse_error : std::runtime_error
{
    parse_error(const std::string& what);
    virtual ~parse_error() noexcept;
};

class listparser_t
{
private:
	bool parse_whitespace(const char*& c, const char* const end);
	bool parse_comment(const char*& c, const char* const end);
	bool parse_quoted_argument(const char*& c, const char* const end);
	bool parse_unquoted_argument(const char*& c, const char* const end);
	bool parse_command(const char*& c, const char* const end);
public:

	virtual void whitespace(const char* c, const char* const end) =0;
	virtual void comment(const char* c, const char* const end) =0;
	virtual void begin_command(const char* c, const char* const end) =0;
	virtual void open_bracket() =0;
	virtual void close_bracket() =0;
	virtual void argument(const char* c, const char* const end, bool quoted) =0;
	virtual void end_command() =0;

	/*
	 * TODO doc.
	 */
	void parse(const char* c, const char* const end);

	virtual ~listparser_t();
};

}
}

#endif /* LISTPARSER_H_ */
