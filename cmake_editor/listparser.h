/*
 * listparser.h
 *
 *  Created on: 15/03/2013
 *      Author: nicholas
 */

#ifndef LISTPARSER_H_
#define LISTPARSER_H_

namespace cmake2
{

struct unexpected_eof
{
};
struct expected
{
	const char* what;
};
struct unexpected
{
	char c;
};

class listparser_t
{
private:
	bool parse_whitespace(const char*& c, const char* end);
	bool parse_comment(const char*& c, const char* end);
	bool parse_quoted_argument(const char*& c, const char* end);
	bool parse_unquoted_argument(const char*& c, const char* end);
	bool parse_command(const char*& c, const char* end);
public:

	virtual void whitespace(const char* c, const char* end) =0;
	virtual void comment(const char* c, const char* end) =0;
	virtual void begin_command(const char* c, const char* end) =0;
	virtual void open_bracket() =0;
	virtual void close_bracket() =0;
	virtual void argument(const char* c, const char* end, bool quoted) =0;
	virtual void end_command() =0;

	/*
	 * TODO doc.
	 */
	void parse(const char* c, const char* end);

	virtual ~listparser_t();
};

} /* namespace cmake2 */
#endif /* LISTPARSER_H_ */
