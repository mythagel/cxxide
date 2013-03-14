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

struct raw_string_t : list_item
{
	std::string s;

	raw_string_t();
	raw_string_t(const std::string& s);

	std::string str() const;
	std::string describe() const;
};

struct whitespace_t : list_item
{
	std::string wsp;

	whitespace_t();

	static bool ws_p(char c);

	template <typename It>
	static bool parse(It& it, const It& end, whitespace_t* wsp)
	{
		if(!ws_p(*it))
			return false;

		auto begin = it;
		while(it != end && ws_p(*it))
			++it;
		wsp->wsp.assign(begin, it);
		return true;
	}

	std::string str() const;
	std::string describe() const;
};

struct comment_t : list_item
{
	std::string cmt;

	comment_t();

	template <typename It>
	static bool parse(It& it, const It& end, comment_t* cmt)
	{
		if(*it != '#')
			return false;

		++it;
		auto begin = it;
		while(it != end && *it != '\n')
			++it;
		cmt->cmt.assign(begin, it);
		return true;
	}

	std::string str() const;
	std::string describe() const;
};

struct identifier_t : list_item
{
	std::string ident;

	identifier_t();

	static bool ident_p(char c);

	template <typename It>
	static bool parse(It& it, const It& end, identifier_t* ident)
	{
		if(!(isalpha(*it) || *it == '_'))
			return false;

		auto begin = it;
		while(it != end && ident_p(*it))
			++it;
		ident->ident.assign(begin, it);
		return true;
	}

	std::string str() const;
	std::string describe() const;
};

struct string_t : list_item
{
	std::string s;

	string_t();

	template <typename It>
	static bool parse(It& it, const It& end, string_t* str)
	{
		if(*it != '"')
			return false;

		++it;
		auto begin = it;
		while(it != end)
		{
			if(*it == '"')
				break;
			if(*it == '\\')
			{
				++it;
				if(it == end)
					throw parse_error("string: unexpected eof; expected <char>");
			}
			++it;
		}
		str->s.assign(begin, it);

		if(it == end)
			throw parse_error("string: unexpected eof; expected '\"'");
		if(*it != '"')
			throw parse_error("string: expected '\"'");

		++it;
		return true;
	}

	std::string str() const;
	std::string describe() const;
};

template <typename It>
bool escape_char_p(It& it, const It& end, std::string* escp)
{
	if(*it != '\\')
		return false;
	auto begin = it;
	++it;
	if(it == end)
		throw parse_error("escape_char: Expected [char]");
	++it;
	escp->assign(begin, it);
	return true;
}

struct variable_t : list_item
{
	std::string var;

	variable_t();

	static bool var_p(char c);

	template <typename It>
	static bool parse(It& it, const It& end, variable_t* var)
	{
		auto begin = it;
		if(*it != '$')
			return false;
		++it;

		if(it == end)
			throw parse_error("variable: unexpected eof");

		if(*it != '(')
		{
			it = begin;
			return false;
		}
		++it;

		if(it == end)
			throw parse_error("variable: unexpected eof");

		begin = it;
		while(it != end && var_p(*it))
			++it;
		var->var.assign(begin, it);

		if(it == end)
			throw parse_error("variable: unexpected eof; expected ')'");
		if(*it != ')')
			throw parse_error("variable: expected ')'");
		++it;

		return true;
	}

	std::string str() const;
	std::string describe() const;
};

struct quoted_t : list_item
{
	std::vector<std::shared_ptr<list_item> > parts;

	quoted_t();

	static bool argq_p(char c);

	template <typename It>
	static bool parse(It& it, const It& end, quoted_t* quot)
	{
		if(*it != '"')
			return false;
		++it;

		quot->parts.clear();

		variable_t var;
		std::string escp;

		while(it != end)
		{
			if(variable_t::parse(it, end, &var))
			{
				quot->parts.emplace_back(std::make_shared<variable_t>(var));
			}
			else if(argq_p(*it))
			{
				if(quot->parts.empty() || quot->parts.back()->type != Type::raw_string)
				{
					quot->parts.emplace_back(std::make_shared<raw_string_t>(std::string(1, *it)));
				}
				else
				{
					auto strng = std::dynamic_pointer_cast<raw_string_t>(quot->parts.back());
					strng->s += *it;
				}
				++it;
			}
			else if(escape_char_p(it, end, &escp))
			{
				if(quot->parts.empty() || quot->parts.back()->type != Type::raw_string)
				{
					quot->parts.emplace_back(std::make_shared<raw_string_t>(escp));
				}
				else
				{
					auto strng = std::dynamic_pointer_cast<raw_string_t>(quot->parts.back());
					strng->s += escp;
				}
			}
			else if(*it == '"')
			{
				++it;
				return true;
			}
			else
			{
				throw parse_error("quoted: unexpected char");
			}
		}
		throw parse_error("quoted: unexpected eof; expected '\"'");
	}

	std::string str() const;
	std::string describe() const;
};

struct unquoted_argument_t : list_item
{
	std::vector<std::shared_ptr<list_item> > parts;

	unquoted_argument_t();

	static bool arg_p(char c);

	template <typename It>
	static bool parse(It& it, const It& end, unquoted_argument_t* unq)
	{
		unq->parts.clear();

		variable_t var;
		std::string escp;

		if(variable_t::parse(it, end, &var))
		{
			unq->parts.emplace_back(std::make_shared<variable_t>(var));
		}
		else if(arg_p(*it))
		{
			unq->parts.emplace_back(std::make_shared<raw_string_t>(std::string(1, *it)));
			++it;
		}
		else if(escape_char_p(it, end, &escp))
		{
			unq->parts.emplace_back(std::make_shared<raw_string_t>(escp));
		}
		else
		{
			return false;
		}

		while(it != end)
		{
			quoted_t quot;

			if(variable_t::parse(it, end, &var))
			{
				unq->parts.emplace_back(std::make_shared<variable_t>(var));
			}
			else if(arg_p(*it))
			{
				if(unq->parts.empty() || unq->parts.back()->type != Type::raw_string)
				{
					unq->parts.emplace_back(std::make_shared<raw_string_t>(std::string(1, *it)));
				}
				else
				{
					auto strng = std::dynamic_pointer_cast<raw_string_t>(unq->parts.back());
					strng->s += *it;
				}
				++it;
			}
			else if(escape_char_p(it, end, &escp))
			{
				if(unq->parts.empty() || unq->parts.back()->type != Type::raw_string)
				{
					unq->parts.emplace_back(std::make_shared<raw_string_t>(escp));
				}
				else
				{
					auto strng = std::dynamic_pointer_cast<raw_string_t>(unq->parts.back());
					strng->s += escp;
				}
			}
			else if(quoted_t::parse(it, end, &quot))
			{
				unq->parts.emplace_back(std::make_shared<quoted_t>(quot));
			}
			else
			{
				break;
			}
		}

		return true;
	}

	std::string str() const;
	std::string describe() const;
};

struct bracketed_args_t : list_item
{
	std::vector<std::shared_ptr<list_item> > args;

	bracketed_args_t();

	template <typename It>
	static bool parse(It& it, const It& end, bracketed_args_t* brac)
	{
		if(*it != '(')
			return false;
		++it;

		brac->args.clear();
		while(it != end)
		{
			whitespace_t wsp;
			comment_t cmt;
			string_t str;
			unquoted_argument_t unq;
			bracketed_args_t brac_nest;

			if(whitespace_t::parse(it, end, &wsp))
			{
				brac->args.emplace_back(std::make_shared<whitespace_t>(wsp));
			}
			else if(comment_t::parse(it, end, &cmt))
			{
				brac->args.emplace_back(std::make_shared<comment_t>(cmt));
			}
			else if(string_t::parse(it, end, &str))
			{
				brac->args.emplace_back(std::make_shared<string_t>(str));
			}
			else if(unquoted_argument_t::parse(it, end, &unq))
			{
				brac->args.emplace_back(std::make_shared<unquoted_argument_t>(unq));
			}
			else if(bracketed_args_t::parse(it, end, &brac_nest))
			{
				brac->args.emplace_back(std::make_shared<bracketed_args_t>(brac_nest));
			}
			else if(*it == ')')
			{
				++it;
				return true;
			}
			else
			{
				throw parse_error("bracketed: expected ')'");
			}
		}
		throw parse_error("bracketed: unexpected eof");
	}

	std::string str() const;
	std::string describe() const;
};

struct command_t : list_item
{
	identifier_t name;
	std::vector<std::shared_ptr<list_item> > name_wsp;
	std::vector<std::shared_ptr<list_item> > args;

	command_t();

	template <typename It>
	static bool parse(It& it, const It& end, command_t* cmd)
	{
		It begin = it;
		if(!identifier_t::parse(it, end, &cmd->name))
		{
			it = begin;
			return false;
		}

		if(it == end)
			throw parse_error("command: unexpected eof");

		while(it != end)
		{
			whitespace_t wsp;
			comment_t cmt;

			if(whitespace_t::parse(it, end, &wsp))
			{
				cmd->name_wsp.emplace_back(std::make_shared<whitespace_t>(wsp));
			}
			else if(comment_t::parse(it, end, &cmt))
			{
				cmd->name_wsp.emplace_back(std::make_shared<comment_t>(cmt));
			}
			else if(*it == '(')
			{
				break;
			}
			else
			{
				throw parse_error("command: unexpected char; expected '('");
			}
		}

		if(it == end)
			throw parse_error("command: unexpected eof");

		if(*it != '(')
			throw parse_error("command: expected '('");
		++it;

		while(it != end)
		{
			whitespace_t wsp;
			comment_t cmt;
			string_t str;
			unquoted_argument_t unq;
			bracketed_args_t brac;

			if(whitespace_t::parse(it, end, &wsp))
			{
				cmd->args.emplace_back(std::make_shared<whitespace_t>(wsp));
			}
			else if(comment_t::parse(it, end, &cmt))
			{
				cmd->args.emplace_back(std::make_shared<comment_t>(cmt));
			}
			else if(string_t::parse(it, end, &str))
			{
				cmd->args.emplace_back(std::make_shared<string_t>(str));
			}
			else if(unquoted_argument_t::parse(it, end, &unq))
			{
				cmd->args.emplace_back(std::make_shared<unquoted_argument_t>(unq));
			}
			else if(bracketed_args_t::parse(it, end, &brac))
			{
				cmd->args.emplace_back(std::make_shared<bracketed_args_t>(brac));
			}
			else if(*it == ')')
			{
				++it;
				return true;
			}
			else
			{
				throw parse_error("command: unexpected char");
			}
		}

		throw parse_error("command: unexpected eof");
	}

	std::string str() const;
	std::string describe() const;
};

struct listfile_t
{
	std::vector<std::shared_ptr<list_item> > items;

	/*
	 * Note: requires > 1 char lookahead so doesn't work with istream iterators
	 */
	template <typename It>
	static bool parse(It& it, const It& end, listfile_t* lst)
	{
		while(it != end)
		{
			comment_t cmt;
			whitespace_t wsp;
			command_t cmd;

			if(whitespace_t::parse(it, end, &wsp))
			{
				lst->items.emplace_back(std::make_shared<whitespace_t>(wsp));
			}
			else if(comment_t::parse(it, end, &cmt))
			{
				lst->items.emplace_back(std::make_shared<comment_t>(cmt));
			}
			else if(command_t::parse(it, end, &cmd))
			{
				lst->items.emplace_back(std::make_shared<command_t>(cmd));
			}
			else
			{
				throw parse_error("top-level");
			}
		}
		return true;
	}

	std::string str() const;
	std::string describe() const;
};

} /* namespace cmake */
#endif /* LISTFILE_H_ */
