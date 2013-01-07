/*
 * item.h
 *
 *  Created on: 07/01/2013
 *      Author: nicholas
 */

#ifndef ITEM_H_
#define ITEM_H_
#include <string>
#include <vector>
#include <memory>

namespace project
{
namespace cmake
{

class item_t
{
public:
	enum Type
	{
		type_whitespace_t,
		type_comment_t,
		type_argument_t,
		type_semicolon_list_t,
		type_command_t
	};

	Type type;
protected:
	item_t(Type type);
public:
	virtual std::string str() const =0;
	virtual std::string describe() const;
	virtual ~item_t();
};

typedef std::shared_ptr<item_t> item;

struct whitespace_t : public item_t
{
	whitespace_t();
	whitespace_t(char ws_char, size_t count);

	char ws_char = ' ';
	size_t count = 1;

	std::string str() const override;
	std::string describe() const override;
};
struct comment_t : public item_t
{
	comment_t();
	comment_t(const std::string& comment);

	std::string comment;

	std::string str() const override;
};
struct argument_t : public item_t
{
	argument_t();
	argument_t(const std::string& value, bool quoted);

	std::string value;
	bool quoted = false;

	std::string str() const override;
};
struct semicolon_list_t : public item_t
{
	semicolon_list_t();

	std::vector<item> items;
	bool quoted = false;

	std::string str() const override;
	std::string describe() const override;
};
struct command_t : public item_t
{
	command_t();
	command_t(const std::string& name);

	std::string name;
	std::vector<whitespace_t> name_ws;
	std::vector<item> items;

	std::string str() const override;
	std::string describe() const override;
};

item make_command(const std::string& command);
item make_ws(char ws_char, size_t count);
item make_argument(const std::string& value, bool quoted);
item make_comment(const std::string& comment);

} /* namespace cmake */
} /* namespace project */
#endif /* ITEM_H_ */
