/*
 * listfile.h
 *
 *  Created on: 14/03/2013
 *      Author: nicholas
 */

#ifndef LISTFILE_H_
#define LISTFILE_H_
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

namespace cmake
{

struct parse_error : std::runtime_error
{
	parse_error(const std::string& what);
};

struct list_item
{
	enum class Type
	{
		whitespace,
		comment,
		identifier,
		command,
		string,
		variable,
		unquoted,
		quoted,
		raw_string,
		bracketed
	} type;
protected:
	list_item(Type type);
public:

	virtual std::string str() const =0;
	virtual std::string describe() const =0;
	virtual ~list_item() = default;
};

template<typename T>
struct parser
{
	static bool parse(const char*& c, const char* end, T* type)
	{
		auto c0 = c;
		if(!T::_parse(c, end, type))
		{
			c = c0;
			return false;
		}
		return true;
	}
};

struct raw_string_t : list_item
{
	std::string s;

	raw_string_t();
	raw_string_t(const std::string& s);

	std::string str() const;
	std::string describe() const;
};

struct whitespace_t : list_item, parser<whitespace_t>
{
	std::string wsp;

	whitespace_t();

	static bool ws_p(char c);

	static bool _parse(const char*& c, const char* end, whitespace_t* wsp);

	std::string str() const;
	std::string describe() const;
};

struct comment_t : list_item, parser<comment_t>
{
	std::string cmt;

	comment_t();

	static bool _parse(const char*& c, const char* end, comment_t* cmt);

	std::string str() const;
	std::string describe() const;
};

struct identifier_t : list_item, parser<identifier_t>
{
	std::string ident;

	identifier_t();

	static bool ident_p(char c);

	static bool _parse(const char*& c, const char* end, identifier_t* ident);

	std::string str() const;
	std::string describe() const;
};

struct string_t : list_item, parser<string_t>
{
	std::string s;

	string_t();

	static bool _parse(const char*& c, const char* end, string_t* str);

	std::string str() const;
	std::string describe() const;
};

bool escape_char_p(const char*& c, const char* end, std::string* escp);

struct variable_t : list_item, parser<variable_t>
{
	std::string var;

	variable_t();

	static bool var_p(char c);

	static bool _parse(const char*& c, const char* end, variable_t* var);

	std::string str() const;
	std::string describe() const;
};

struct quoted_t : list_item, parser<quoted_t>
{
	std::vector<std::shared_ptr<list_item> > parts;

	quoted_t();

	static bool argq_p(char c);

	static bool _parse(const char*& c, const char* end, quoted_t* quot);

	std::string str() const;
	std::string describe() const;
};

struct unquoted_argument_t : list_item, parser<unquoted_argument_t>
{
	std::vector<std::shared_ptr<list_item> > parts;

	unquoted_argument_t();

	static bool arg_p(char c);

	static bool _parse(const char*& c, const char* end, unquoted_argument_t* unq);

	std::string str() const;
	std::string describe() const;
};

struct bracketed_args_t : list_item, parser<bracketed_args_t>
{
	std::vector<std::shared_ptr<list_item> > args;

	bracketed_args_t();

	static bool _parse(const char*& c, const char* end, bracketed_args_t* brac);

	std::string str() const;
	std::string describe() const;
};

struct command_t : list_item, parser<command_t>
{
	identifier_t name;
	std::vector<std::shared_ptr<list_item> > name_wsp;
	std::vector<std::shared_ptr<list_item> > args;

	command_t();

	static bool _parse(const char*& c, const char* end, command_t* cmd);

	std::string str() const;
	std::string describe() const;
};

struct listfile_t : parser<listfile_t>
{
	std::vector<std::shared_ptr<list_item> > items;

	static bool _parse(const char*& c, const char* end, listfile_t* lst);

	std::string str() const;
	std::string describe() const;
};

} /* namespace cmake */
#endif /* LISTFILE_H_ */
