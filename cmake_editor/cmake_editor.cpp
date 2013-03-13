/*
 * cmake_parser.cpp
 *
 *  Created on: 01/03/2013
 *      Author: nicholas
 */
#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <memory>
#include <istream>
#include <iterator>
#include <iostream>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

struct parse_error : std::runtime_error
{
	parse_error(const std::string& what)
	 : std::runtime_error(what)
	{
	}
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
	list_item(Type type)
	 : type(type)
	{
	}
public:

	virtual std::string str() const =0;
	virtual ~list_item() = default;
};

struct raw_string_t : list_item
{
	std::string s;

	raw_string_t()
	 : list_item(Type::raw_string)
	{
	}
	raw_string_t(const std::string& s)
	 : list_item(Type::raw_string), s(s)
	{
	}

	std::string str() const
	{
		return s;
	}
};

struct whitespace_t : list_item
{
	std::string wsp;

	whitespace_t()
	 : list_item(Type::whitespace)
	{
	}

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
	static bool parse(It& it, const It& end, whitespace_t* wsp)
	{
		if(!ws_p(*it))
			return false;

		wsp->wsp.clear();
		while(it != end && ws_p(*it))
			wsp->wsp += *it++;
		return true;
	}

	std::string str() const
	{
		return wsp;
	}
};

struct comment_t : list_item
{
	std::string cmt;

	comment_t()
	 : list_item(Type::comment)
	{
	}

	template <typename It>
	static bool parse(It& it, const It& end, comment_t* cmt)
	{
		if(*it != '#')
			return false;

		It begin = ++it;
		while(it != end && *it != '\n')
			++it;
		cmt->cmt.assign(begin, it);
		return true;
	}

	std::string str() const
	{
		std::ostringstream s;
		s << '#' << cmt;
		return s.str();
	}
};

struct identifier_t : list_item
{
	std::string ident;

	identifier_t()
	 : list_item(Type::identifier)
	{
	}

	static bool ident_p(char c)
	{
		return isalnum(c) || c == '_';
	}

	template <typename It>
	static bool parse(It& it, const It& end, identifier_t* ident)
	{
		if(!(isalpha(*it) || *it == '_'))
			return false;

		ident->ident.clear();

		while(it != end && ident_p(*it))
			ident->ident += *it++;

		return true;
	}

	std::string str() const
	{
		return ident;
	}
};

struct string_t : list_item
{
	std::string s;

	string_t()
	 : list_item(Type::string)
	{
	}

	template <typename It>
	static bool parse(It& it, const It& end, string_t* str)
	{
		if(*it != '"')
			return false;

		It begin = it;
		bool last_escape(false);
		while(it != end)
		{
			if(*it == '"' && !last_escape)
				break;
			last_escape = (*it == '\\');
		}

		if(it == end)
			throw parse_error("string: unexpected eof; expected '\"'");
		if(*it != '"')
			throw parse_error("string: expected '\"'");

		str->s.assign(begin, it);
		++it;
		return true;
	}

	std::string str() const
	{
		std::ostringstream ss;
		ss << '"' << s << '"';
		return ss.str();
	}
};

template <typename It>
bool escape_char_p(It& it, const It& end, std::string* escp)
{
	if(*it != '\\')
		return false;
	It begin = it;
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

	variable_t()
	 : list_item(Type::variable)
	{
	}

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
			throw parse_error("variable: expected '('");
		++it;

		if(it == end)
			throw parse_error("variable: unexpected eof");

		It begin = it;
		while(it != end && var_p(*it))
			++it;

		if(it == end)
			throw parse_error("variable: unexpected eof; expected ')'");
		if(*it != ')')
			throw parse_error("variable: expected ')'");

		var->var.assign(begin, it);
		++it;
		return true;
	}

	std::string str() const
	{
		std::ostringstream s;
		s << "$(" << var << ")";
		return s.str();
	}
};

struct quoted_t : list_item
{
	std::vector<std::shared_ptr<list_item> > parts;

	quoted_t()
	 : list_item(Type::quoted)
	{
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

	std::string str() const
	{
		std::ostringstream s;
		s << '"';
		for(auto& part : parts)
			s << part->str();
		s << '"';
		return s.str();
	}
};

struct unquoted_argument_t : list_item
{
	std::vector<std::shared_ptr<list_item> > parts;

	unquoted_argument_t()
	 : list_item(Type::unquoted)
	{
	}

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

	std::string str() const
	{
		std::ostringstream s;
		for(auto& part : parts)
			s << part->str();
		return s.str();
	}
};

struct bracketed_args_t : list_item
{
	std::vector<std::shared_ptr<list_item> > args;

	bracketed_args_t()
	 : list_item(Type::bracketed)
	{
	}

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
				break;
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

	std::string str() const
	{
		std::ostringstream s;
		s << '(';
		for(auto& arg : args)
			s << arg->str();
		s << ')';
		return s.str();
	}
};

struct command_t : list_item
{
	identifier_t name;
	std::vector<std::shared_ptr<list_item> > name_wsp;

	std::vector<std::shared_ptr<list_item> > args;

	command_t()
	 : list_item(Type::command)
	{
	}

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
			else if(bracketed_args_t::parse(it, end, &brac)) // parse-bracketed
			{
				cmd->args.emplace_back(std::make_shared<bracketed_args_t>(brac));
				break;
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

	std::string str() const
	{
		std::ostringstream s;
		s << name.str();
		for(auto& wsp : name_wsp)
			s << wsp->str();
		s << '(';
		for(auto& arg : args)
			s << arg->str();
		s << ')';
		return s.str();
	}
};

struct list_file_t
{
	std::vector<std::shared_ptr<list_item> > items;

	template <typename It>
	static bool parse(It& it, const It& end, list_file_t* lst)
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
		return false;
	}

	std::string str() const
	{
		std::ostringstream s;
		for(auto& item : items)
			s << item->str();
		return s.str();
	}
};

int main()
{
	std::noskipws(std::cin);
	std::istream_iterator<char> it(std::cin);
	const std::istream_iterator<char> end;

	list_file_t lst;
	try
	{
		if(list_file_t::parse(it, end, &lst))
		{
			std::cout << lst.str();
		}
		else
		{
			return 1;
		}
	}
	catch(const parse_error& ex)
	{
		std::cout << lst.str();
		throw;
	}
	return 0;
}


