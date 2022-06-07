#include "../include/json/JSON.hpp"

#include <regex>
#include <sstream>

using namespace json;
using namespace json::parser;

std::istream& json::operator>>(std::istream& is, Node& n)
{
#	pragma warning(disable:26800)
	std::stringstream ss;
	ss << is.rdbuf();
	n = parser::Parser(parser::Tokenizer(std::move(ss)).tokenize(), NodeType::Object).parse();
	return is;
#	pragma warning(default:26800)
}

/**
 * @brief
 * @param is
 * @param n		Any ContainerNode-derived type.
 * @returns		std::istream&
 */
template<json_container_type T> std::istream& json::operator>>(std::istream& is, ContainerNode<T>& n)
{
	return is >> n.operator json::Node & ();
}

template std::istream& json::operator>><array_t>(std::istream&, ContainerNode<array_t>&);
template std::istream& json::operator>><object_t>(std::istream&, ContainerNode<object_t>&);

Parser::OutputT json::parser::Parser::parse()
{
	if (node_type == NodeType::Undefined)
		throw make_exception("Parser::parse() invalid operation:  Node type '", NodeType::Undefined, "' is never a valid parser output type!");
	else if (node_type == NodeType::Array) {
		// ARRAY:
		array_t arr;
		arr.reserve(tokens.size());
		bool fst{ true };
		bool comma{ false };

		const auto& resetState{ [&fst, &comma]() { comma = fst = false; } };

		for (const auto& it : tokens) {
			switch (it.type) {
			case TOKEN::ARRAY:
				if (!fst && !comma)
					throw make_exception("Missing comma!");

				arr.emplace_back(Parser(Tokenizer(it.str).tokenize(), NodeType::Array).parse());
				resetState();
				break;
			case TOKEN::OBJECT:
				if (!fst && !comma)
					throw make_exception("Missing comma!");

				arr.emplace_back(Parser(Tokenizer(it.str).tokenize(), NodeType::Object).parse());
				resetState();
				break;
			case TOKEN::NUMBER:
				if (!fst && !comma)
					throw make_exception("Missing comma!");

				if (str::contains(it.str, '.'))
					arr.emplace_back(Node{ str::stold(it.str) });
				else
					arr.emplace_back(Node{ str::stoll(it.str) });
				resetState();
				break;
			case TOKEN::KEYWORD:
				if (!fst && !comma)
					throw make_exception("Missing comma!");

				arr.emplace_back(parseKeyword(it.str));
				resetState();
				break;
			case TOKEN::STRING:
				if (!fst && !comma)
					throw make_exception("Missing comma!");

				arr.emplace_back(Node{ it.str });
				resetState();
				break;
			case TOKEN::COLON:
				throw make_exception("Setter character ':' isn't allowed within an array!");
			case TOKEN::COMMA:
				comma = true;
				break;
			default:
				break;
			}
		}

		return{ arr };
	}
	else if (node_type == NodeType::Object) {
		// OBJECT:
		object_t obj;

		std::pair<string_t, Node> pr;

		bool fst{ true };
		bool comma{ false };
		bool colon{ false };

		const auto& insert_pr{ [&obj, &pr, &fst, &comma, &colon]() {
			if (!obj.insert(pr).second) throw make_exception("Parser::parse() error:  Duplicated key '", pr.first, "' isn't valid!");
			fst = comma = colon = false;
			pr = {};
		} };

		for (const auto& it : tokens) {
			switch (it.type) {
			case TOKEN::ARRAY:
				if (!fst && !comma) throw make_exception("Parser::parse() syntax error:  Missing comma!");

				if (fst && !colon && !comma)
					return Parser(Tokenizer(it.str).tokenize(), NodeType::Array).parse();
				else if (!colon) throw make_exception("Parser::parse() syntax error:  Cannot use array '", it.str, "' as a key; only string keys are allowed!");
				else {
					pr.second = Parser(Tokenizer(it.str).tokenize(), NodeType::Array).parse();
					insert_pr();
				}
				break;
			case TOKEN::OBJECT:
				if (!fst && !comma) throw make_exception("Parser::parse() syntax error:  Missing comma!");

				if (fst && !colon && !comma)
					return Parser(Tokenizer(it.str).tokenize(), NodeType::Object).parse();
				else if (!colon) throw make_exception("Parser::parse() syntax error:  Cannot use object '", it.str, "' as a key; only string keys are allowed!");
				else {
					pr.second = Parser(Tokenizer(it.str).tokenize(), NodeType::Object).parse();
					insert_pr();
				}
				break;
			case TOKEN::NUMBER:
				if (!fst && !comma) throw make_exception("Parser::parse() syntax error:  Missing comma!");

				if (!colon) throw make_exception("Parser::parse() syntax error:  Cannot use number '", it.str, "' as a key; only string keys are allowed!");
				else {
					if (str::contains(it.str, '.'))
						pr.second = Node{ str::stold(it.str) };
					else
						pr.second = Node{ str::stoll(it.str) };
					insert_pr();
				}
				break;
			case TOKEN::KEYWORD:
				if (!fst && !comma) throw make_exception("Parser::parse() syntax error:  Missing comma!");

				if (!colon) throw make_exception("Parser::parse() syntax error:  Cannot use keyword '", it.str, "' as a key; only string keys are allowed!");
				else {
					pr.second = parseKeyword(it.str);
					insert_pr();
				}
				break;
			case TOKEN::STRING:
				if (!fst && !comma) throw make_exception("Parser::parse() syntax error:  Missing comma!");

				if (!colon)
					pr.first = it.str;
				else {
					pr.second = Node{ it.str };
					insert_pr();
				}
				break;
			case TOKEN::COLON:
				colon = true;
				break;
			case TOKEN::COMMA:
				comma = true;
				break;
			}
		}

		return{ obj };
	}
	else if (tokens.size() == 1) {
		// RAW:
		Token t{ tokens.front() };
		if (node_type == NodeType::Null) {
			// NULL:
			if (t.type != TOKEN::KEYWORD && t.type != TOKEN::NUMBER && t.type != TOKEN::STRING)
				throw make_exception("Parser::parse() type error:  Target type '", NodeType::Null, "' requires a token of type 'KEYWORD', 'NUMBER', or 'STRING'!");

			if (t.str.empty() || str::equalsAny<true>(t.str, "null", "0"))
				return{ null };
			else throw make_exception("Parser::parse() syntax error:  Cannot parse '", t.str, "' to type '", NodeType::Null, "'!");
		}
		else if (node_type == NodeType::Boolean) {
			// BOOLEAN:
			if (t.type != TOKEN::KEYWORD && t.type != TOKEN::NUMBER && t.type != TOKEN::STRING)
				throw make_exception("Parser::parse() type error:  Target type '", NodeType::Boolean, "' requires a token of type 'KEYWORD', 'NUMBER', or 'STRING'!");

			if (str::equalsAny<true>(t.str, "true", "1"))
				return{ true };
			else if (str::equalsAny<true>(t.str, "false", "0"))
				return{ false };
			else throw make_exception("Parser::parse() syntax error:  Cannot parse '", t.str, "' to type '", NodeType::Boolean, "'!");
		}
		else if (node_type == NodeType::Number) {
			// NUMBER:
			if (t.type != TOKEN::NUMBER && t.type != TOKEN::STRING)
				throw make_exception("Parser::parse() type error:  Target type '", NodeType::Number, "' requires a token of type 'NUMBER' or 'STRING'!");

			if (str::contains(t.str, '.')) {
				if (const auto& result{ str::optional::stold(t.str) }; result.has_value())
					return{ result.value() };
				else throw make_exception("Parser::parse() syntax error:  Cannot parse invalid floating-point number '", t.str, "'!");
			}
			else {
				if (const auto& result{ str::optional::stoll(t.str) }; result.has_value())
					return{ result.value() };
				else throw make_exception("Parser::parse() syntax error:  Cannot parse invalid integral number '", t.str, "'!");
			}
		}
		else if (node_type == NodeType::String)
			// STRING:
			return{ t.str };
		else throw make_exception("Parser::parse() invalid operation:  Type '", node_type, "' doesn't support direct-parsing. Try using a container type instead (Array/Object).");
	}
	else throw make_exception("Parser::parse() invalid operation:  Cannot parse ", tokens.size(), " tokens into non-container-type '", node_type, "'! (Use Array or Object types or reduce the number of input tokens to 1.)");
	return{}; //< returns null in the event of control flow escape
}
