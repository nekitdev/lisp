#include "parse.hpp"
#include <cctype>
#include <expected>
#include <format>
#include <variant>

using namespace lisp;

static String OPEN = "(";
static String CLOSE = ")";
static String QUOTE = "'";

static char OPEN_CHAR = '(';
static char CLOSE_CHAR = ')';
static char SEMICOLON = ';';

static char QUOTE_CHAR = '\'';
static char DOUBLE_QUOTE = '"';

static char ESCAPE = '\\';

static char NEWLINE = '\n';
static char TAB = '\t';

static char N = 'n';
static char T = 'T';

static String UNTERMINATED_ESCAPE = "unterminated character escape";

TokenizeResult lisp::tokenize(const String& code) {
    Tokens tokens;

    Index index = 0;

    Index code_size = code.size();

    while (index < code_size) {
        auto character = code[index];

        if (std::isspace(character)) {
            index += 1;

            continue;
        }

        if (character == SEMICOLON) {
            while (index < code_size && code[index] != NEWLINE) {
                index += 1;
            }

            continue;
        }

        if (character == OPEN_CHAR) {
            tokens.push_back({ Token::Type::Open, OPEN });

            index += 1;

            continue;
        }

        if (character == CLOSE_CHAR) {
            tokens.push_back({ Token::Type::Close, CLOSE });

            index += 1;

            continue;
        }

        if (character == QUOTE_CHAR) {
            tokens.push_back({ Token::Type::Quote, QUOTE });

            index += 1;

            continue;
        }

        if (character == DOUBLE_QUOTE) {
            std::string string;

            index += 1;

            while (index < code_size) {
                if (code[index] == ESCAPE) {
                    index += 1;

                    if (index >= code_size) {
                        return std::unexpected(Error(UNTERMINATED_ESCAPE));
                    }

                    auto escaped = code[index];

                    index += 1;

                    if (escaped == N) {
                        string.push_back(NEWLINE);
                    } else if (escaped == T) {
                        string.push_back(TAB);
                    } else {
                        string.push_back(escaped);
                    }

                    continue;
                }

                if (code[index] == DOUBLE_QUOTE) {
                    index += 1;

                    break;
                }

                string.push_back(code[index]);

                index += 1;
            }

            tokens.push_back({ Token::Type::String, string });

            continue;
        }

        const Index start = index;

        while (
            index < code_size
            && !std::isspace(code[index])
            && code[index] != OPEN_CHAR
            && code[index] != CLOSE_CHAR
            && code[index] != QUOTE_CHAR
        ) {
            index += 1;
        }

        tokens.push_back({ Token::Type::Atom, code.substr(start, index - start) });
    }

    return tokens;
}

static String NIL = "nil";
static String TRUE = "true";
static String FALSE = "false";

Value parse_atom(const String& atom) {
    if (atom == NIL) {
        return Value::nil();
    }

    if (atom == TRUE) {
        return Value(true);
    }

    if (atom == FALSE) {
        return Value(false);
    }

    auto atom_size = atom.size();

    Index parsed = 0;

    try {
        auto integer = std::stoll(atom, &parsed);

        if (parsed == atom_size) {
            return Value(integer);
        }
    } catch (const std::exception&) {
        // fall-through
    }

    parsed = 0;

    try {
        const auto floating = std::stod(atom, &parsed);

        if (parsed == atom_size) {
            return Value(floating);
        }
    } catch (const std::exception&) {
        // fall-through
    }

    auto symbol = Symbol { atom };

    return Value(symbol);
}

const String UNEXPECTED_END_OF_INPUT = "unexpected end of input";
const String UNEXPECTED_CLOSE = "unexpected closing parenthesis";
const String UNTERMINATED_LIST = "unterminated list";

const String QUOTE_NAME = "quote";

Result parse_expression(const Tokens& tokens, Index& index) {
    auto tokens_size = tokens.size();

    if (index >= tokens_size) {
        return std::unexpected(Error(UNEXPECTED_END_OF_INPUT));
    }

    auto token = tokens[index];

    index += 1;

    switch (token.type) {
        case Token::Type::Open: {
            Items items;

            while (index < tokens_size && tokens[index].type != Token::Type::Close) {
                auto result = parse_expression(tokens, index);

                if (!result) {
                    return result;
                }

                items.push_back(result.value());
            }

            if (index >= tokens_size || tokens[index].type != Token::Type::Close) {
                return std::unexpected(Error(UNTERMINATED_LIST));
            }

            index += 1;

            return Value::list(std::move(items));
        }

        case Token::Type::Close: {
            return std::unexpected(Error(UNEXPECTED_CLOSE));
        }

        case Token::Type::Quote: {
            auto result = parse_expression(tokens, index);

            if (!result) {
                return result;
            }

            auto quoted = result.value();

            auto quote = Symbol { QUOTE_NAME };

            auto value = Value(quote);

            return Value::list({ value, quoted });
        }

        case Token::Type::String: {
            return Value(token.string);
        }

        case Token::Type::Atom: {
            return parse_atom(token.string);
        }
    }
}

Result lisp::eval_block(EnvPtr env, const Values& expressions) {
    auto output = Value::nil();

    for (const auto& expression: expressions) {
        auto result = eval(env, expression);

        if (!result) {
            return result;
        }

        output = result.value();
    }

    return output;
}

SymbolResult require_symbol(const Value& value) {
    auto data = value.data;

    if (std::holds_alternative<Symbol>(data)) {
        auto symbol = std::get<Symbol>(data);

        return symbol.name;
    }

    auto message = std::format("expected symbol, got `{}`", value.to_string());

    return std::unexpected(Error(message));
}

ValuesResult require_list(const Value& value) {
    auto data = value.data;

    if (std::holds_alternative<ListPtr>(data)) {
        auto list = std::get<ListPtr>(data);

        return list->items;
    }

    auto message = std::format("expected list, got `{}`", value.to_string());

    return std::unexpected(Error(message));
}

Result apply(EnvPtr env, const Value& function, const Values& arguments) {
    auto data = function.data;

    if (std::holds_alternative<NativePtr>(data)) {
        auto native = std::get<NativePtr>(data);

        return native->call(env, arguments);
    }

    if (std::holds_alternative<LambdaPtr>(data)) {
        auto lambda = std::get<LambdaPtr>(data);

        auto closure = lambda->closure;

        auto names = lambda->arguments;

        auto names_size = names.size();
        auto arguments_size = arguments.size();

        if (names_size != arguments_size) {
            auto message = std::format(
                "expected {} arguments, got {}",
                names_size,
                arguments_size
            );

            return std::unexpected(Error(message));
        }

        for (Index index = 0; index < arguments_size; index += 1) {
            closure->define(names[index], arguments[index]);
        }

        auto body = lambda->body;

        return eval_block(closure, body);
    }

    auto message = std::format("attempted to call non-function `{}`", function.to_string());

    return std::unexpected(Error(message));
}

ValuesResult lisp::parse(const Tokens &tokens) {
    Index index = 0;

    Values expressions;

    auto tokens_size = tokens.size();

    while (index < tokens_size) {
        auto result = parse_expression(tokens, index);

        if (!result) {
            return std::unexpected(result.error());
        }

        expressions.push_back(result.value());
    }

    return expressions;
}

static String IF_NAME = "if";
static String DEFINE_NAME = "define";
static String SET_NAME = "set";
static String LAMBDA_NAME = "lambda";
static String BEGIN_NAME = "begin";
static String AND_NAME = "and";
static String OR_NAME = "or";
static String DEFUN_NAME = "defun";

Result lisp::eval(EnvPtr env, const Value& expression) {
    auto data = expression.data;

    if (std::holds_alternative<Symbol>(data)) {
        auto symbol = std::get<Symbol>(data);

        return env->get(symbol.name);
    }

    if (!std::holds_alternative<ListPtr>(data)) {
        return expression;
    }

    auto list = std::get<ListPtr>(data);

    auto items = list->items;

    if (items.empty()) {
        return Value::nil();
    }

    auto items_size = items.size();

    auto cdr_size = items_size - 1;

    auto car = items.front();
    auto car_data = car.data;

    if (std::holds_alternative<Symbol>(car_data)) {
        auto symbol = std::get<Symbol>(car_data);

        auto name = symbol.name;

        if (name == QUOTE_NAME) {
            if (cdr_size != 1) {
                auto message = std::format(
                    "`{}` expected 1 argument, got {}", QUOTE_NAME, cdr_size
                );

                return std::unexpected(Error(message));
            }

            return items[1];
        }

        if (name == IF_NAME) {
            if (cdr_size < 2 || cdr_size > 3) {
                auto message = std::format(
                    "`{}` expected 2 or 3 arguments, got {}", IF_NAME, cdr_size
                );

                return std::unexpected(Error(message));
            }

            auto result = eval(env, items[1]);

            if (!result) {
                return result;
            }

            auto condition = result.value();

            if (condition.truthy()) {
                return eval(env, items[2]);
            }

            if (cdr_size == 3) {
                return eval(env, items[3]);
            }

            return Value::nil();
        }

        if (name == DEFINE_NAME) {
            if (cdr_size != 2) {
                auto message = std::format(
                    "`{}` expected 2 arguments, got {}", DEFINE_NAME, cdr_size
                );

                return std::unexpected(Error(message));
            }

            auto result = require_symbol(items[1]);

            if (!result) {
                return std::unexpected(result.error());
            }

            auto name = result.value();

            auto value = items[2];

            env->define(name, value);

            return value;
        }

        if (name == SET_NAME) {
            if (cdr_size != 2) {
                auto message = std::format(
                    "`{}` expected 2 arguments, got {}", SET_NAME, cdr_size
                );

                return std::unexpected(Error(message));
            }

            auto result = require_symbol(items[1]);

            if (!result) {
                return std::unexpected(result.error());
            }

            auto name = result.value();

            auto value = items[2];

            if (!env->set(name, value)) {
                auto message = std::format("`{}` on undefined symbol `{}`", SET_NAME, name);

                return std::unexpected(Error(message));
            }

            return value;
        }

        if (name == LAMBDA_NAME) {
            if (cdr_size < 2) {
                auto message = std::format(
                    "`{}` expected at least 2 items, got {}", LAMBDA_NAME, cdr_size
                );

                return std::unexpected(Error(message));
            }

            auto result = require_list(items[1]);

            if (!result) {
                return std::unexpected(result.error());
            }

            auto names = result.value();

            Names arguments;

            arguments.reserve(names.size());

            for (const auto& name: names) {
                auto result = require_symbol(name);

                if (!result) {
                    return std::unexpected(result.error());
                }

                arguments.push_back(result.value());
            }

            auto body = Values(items.begin() + 2, items.end());

            auto closure = std::make_shared<Env>(env);

            auto lambda = Lambda { closure, arguments, body };

            return Value(std::make_shared<Lambda>(lambda));
        }

        if (name == DEFUN_NAME) {
            if (cdr_size < 3) {
                auto message = std::format(
                    "`{}` expected at least 3 items, got {}", DEFUN_NAME, cdr_size
                );

                return std::unexpected(Error(message));
            }

            auto result = require_symbol(items[1]);

            if (!result) {
                return std::unexpected(result.error());
            }

            auto symbol = result.value();

            auto names_result = require_list(items[2]);

            if (!names_result) {
                return std::unexpected(names_result.error());
            }

            auto names = names_result.value();

            Names arguments;

            arguments.reserve(names.size());

            for (const auto& name: names) {
                auto result = require_symbol(name);

                if (!result) {
                    return std::unexpected(result.error());
                }

                arguments.push_back(result.value());
            }

            auto body = Values(items.begin() + 3, items.end());

            auto closure = std::make_shared<Env>(env);

            auto lambda = Lambda { closure, arguments, body };

            auto value = Value(std::make_shared<Lambda>(lambda));

            env->define(symbol, value);

            return value;
        }

        if (name == BEGIN_NAME) {
            if (!cdr_size) {
                return Value::nil();
            }

            auto body = Values(items.begin() + 1, items.end());

            return eval_block(env, body);
        }

        if (name == AND_NAME) {
            auto output = Value(true);

            for (Index index = 1; index < items_size; index += 1) {
                auto result = eval(env, items[index]);

                if (!result) {
                    return result;
                }

                output = result.value();

                if (!output.truthy()) {
                    return output;
                }
            }

            return output;
        }

        if (name == OR_NAME) {
            auto output = Value(false);

            for (Index index = 1; index < items_size; index += 1) {
                auto result = eval(env, items[index]);

                if (!result) {
                    return result;
                }

                output = result.value();

                if (output.truthy()) {
                    return output;
                }
            }

            return output;
        }
    }

    auto result = eval(env, car);

    if (!result) {
        return result;
    }

    auto function = result.value();

    Values arguments;

    arguments.reserve(cdr_size);

    for (Index index = 1; index < items_size; index += 1) {
        auto result = eval(env, items[index]);

        if (!result) {
            return result;
        }

        arguments.push_back(result.value());
    }

    return apply(env, function, arguments);
}

Result lisp::run(EnvPtr env, const String &code) {
    auto tokenize_result = tokenize(code);

    if (!tokenize_result) {
        return std::unexpected(tokenize_result.error());
    }

    auto tokens = tokenize_result.value();

    auto result = parse(tokens);

    if (!result) {
        return std::unexpected(result.error());
    }

    auto expressions = result.value();

    return eval_block(env, expressions);
}
