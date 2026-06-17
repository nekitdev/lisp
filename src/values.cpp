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

        return std::format("{}", floating);
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

static Float EPSILON = 1e-12;

bool Value::operator==(const Value& other) const {
    auto other_data = other.data;

    if (data.index() != other_data.index()) {
        if (std::holds_alternative<Float>(data) && std::holds_alternative<Int>(other_data)) {
            auto floating = std::get<Float>(data);
            auto integer = std::get<Int>(other_data);

            return std::abs(floating - integer) < EPSILON;
        }

        if (std::holds_alternative<Int>(data) && std::holds_alternative<Float>(other_data)) {
            auto integer = std::get<Int>(data);
            auto floating = std::get<Float>(other_data);

            return std::abs(integer - floating) < EPSILON;
        }

        return false;
    }

    if (is_nil()) {
        return true;
    }

    if (std::holds_alternative<Bool>(data)) {
        auto boolean = std::get<Bool>(data);
        auto other_boolean = std::get<Bool>(other_data);

        return boolean == other_boolean;
    }

    if (std::holds_alternative<Int>(data)) {
        auto integer = std::get<Int>(data);
        auto other_integer = std::get<Int>(other_data);

        return integer == other_integer;
    }

    if (std::holds_alternative<Float>(data)) {
        auto floating = std::get<Float>(data);
        auto other_floating = std::get<Float>(other_data);

        return std::abs(floating - other_floating) < EPSILON;
    }

    if (std::holds_alternative<String>(data)) {
        auto string = std::get<String>(data);
        auto other_string = std::get<String>(other_data);

        return string == other_string;
    }

    if (std::holds_alternative<Symbol>(data)) {
        auto symbol = std::get<Symbol>(data);
        auto other_symbol = std::get<Symbol>(other_data);

        return symbol.name == other_symbol.name;
    }

    if (std::holds_alternative<ListPtr>(data)) {
        auto list = std::get<ListPtr>(data);
        auto other_list = std::get<ListPtr>(other_data);

        auto items = list->items;
        auto other_items = other_list->items;

        if (items.size() != other_items.size()) {
            return false;
        }

        for (Index index = 0; index < items.size(); index += 1) {
            auto item = items[index];
            auto other_item = other_items[index];

            auto equal = item == other_item;

            if (!equal) {
                return false;
            }
        }

        return true;
    }

    if (std::holds_alternative<LambdaPtr>(data)) {
        auto lambda = std::get<LambdaPtr>(data);
        auto other_lambda = std::get<LambdaPtr>(other_data);

        return lambda.get() == other_lambda.get();
    }

    if (std::holds_alternative<NativePtr>(data)) {
        auto native = std::get<NativePtr>(data);
        auto other_native = std::get<NativePtr>(other_data);

        return native.get() == other_native.get();
    }

    return false;
}

bool Value::operator!=(const Value& other) const {
    return !(*this == other);
}

String lisp::to_string(const Value& value) {
    return value.to_string();
}
