#include <expected>
#include <format>
#include <iomanip>
#include <string>
#include <variant>

#include "values.hpp"

using namespace lisp;

Result Native::call(const EnvPtr& env, const Values& arguments) const {
    return function(env, arguments);
}

Value Value::nil() {
    auto nil = Nil {};
    return Value(nil);
}

Value Value::list(Items items) {
    auto list = List { std::move(items) };

    return Value(std::make_shared<List>(list));
}

Value Value::native(const String &name, Function function) {
    auto native = Native { name, std::move(function) };

    return Value(std::make_shared<Native>(native));
}

bool Value::is_nil() const {
    return std::holds_alternative<Nil>(data);
}

bool Value::truthy() const {
    if (std::holds_alternative<Bool>(data)) {
        return std::get<Bool>(data);
    }

    return !is_nil();
}

void Env::define(const String& name, Value value) {
    map[name] = std::move(value);
}

bool Env::set(const String& name, Value value) {
    if (auto pair = map.find(name); pair != map.end()) {
        pair->second = std::move(value);

        return true;
    }

    if (parent) {
        return parent->set(name, std::move(value));
    }

    return false;
}

Result Env::get(const String& name) const {
    if (auto pair = map.find(name); pair != map.end()) {
        return pair->second;
    }

    if (parent) {
        return parent->get(name);
    }

    auto unknown = std::format("unknown symbol `{}`", name);

    return std::unexpected(Error(unknown));
}

static String NIL = "nil";
static String TRUE = "true";
static String FALSE = "false";

static String LAMBDA = "<lambda>";
static String UNKNOWN = "<unknown>";

static char OPEN = '(';
static char CLOSE = ')';
static char SPACE = ' ';

String Value::to_string() const {
    if (is_nil()) {
        return NIL;
    }

    if (std::holds_alternative<Bool>(data)) {
        auto boolean = std::get<Bool>(data);

        return boolean ? TRUE : FALSE;
    }

    if (std::holds_alternative<Int>(data)) {
        auto integer = std::get<Int>(data);

        return std::to_string(integer);
    }

    if (std::holds_alternative<Float>(data)) {
        auto floating = std::get<Float>(data);

        return std::to_string(floating);
    }

    if (std::holds_alternative<String>(data)) {
        auto string = std::get<String>(data);

        std::stringstream stream;

        stream << std::quoted(string);

        return stream.str();
    }

    if (std::holds_alternative<Symbol>(data)) {
        auto symbol = std::get<Symbol>(data);

        return symbol.name;
    }

    if (std::holds_alternative<ListPtr>(data)) {
        auto list = std::get<ListPtr>(data);

        std::stringstream stream;

        stream << OPEN;

        bool space = false;

        for (const auto& item: list->items) {
            if (space) {
                stream << SPACE;
            }

            space = true;

            stream << item.to_string();
        }

        stream << CLOSE;

        return stream.str();
    }

    if (std::holds_alternative<NativePtr>(data)) {
        auto native = std::get<NativePtr>(data);

        auto string = std::format("<native:{}>", native->name);

        return string;
    }

    if (std::holds_alternative<LambdaPtr>(data)) {
        return LAMBDA;
    }

    return UNKNOWN;
}

String lisp::to_string(const Value& value) {
    return value.to_string();
}
