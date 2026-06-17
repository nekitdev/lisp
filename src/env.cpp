#include <iostream>
#include <memory>
#include <variant>

#include "env.hpp"

using namespace lisp;

void add_native(const EnvPtr &env, const String &name, Function function) {
    env->define(name, Value::native(name, function));
}

static char SPACE = ' ';

static Float EPSILON = 1e-12;

static String NIL = "nil";
static String TRUE = "true";
static String FALSE = "false";

static String NOT = "not";

static String ADD = "+";
static String SUB = "-";
static String MUL = "*";
static String DIV = "/";

static String PRINT = "print";

static String LIST = "list";
static String CAR = "car";
static String CDR = "cdr";
static String CONS = "cons";
static String LENGTH = "length";

static String EQUAL = "=";
static String DOUBLE_EQUAL = "==";
static String NOT_EQUAL = "!=";

static String DIVISION_BY_ZERO = "division by zero";

Result native_not(const EnvPtr& _env, const Values& values) {
    if (values.size() != 1) {
        auto message = std::format("`{}` expected 1 argument, got {}", NOT, values.size());

        return std::unexpected(Error(message));
    }

    auto value = values.front();

    return Value(!value.truthy());
}

Result native_add(const EnvPtr& _env, const Values& values) {
    if (values.empty()) {
        auto message = std::format("`{}` expected at least 1 argument, got 0", ADD);
        return std::unexpected(Error(message));
    }

    bool all_int = true;

    Int int_sum = 0;
    Float float_sum = 0;

    for (const auto& value: values) {
        auto data = value.data;

        if (std::holds_alternative<Int>(data)) {
            int_sum += std::get<Int>(data);
        } else if (std::holds_alternative<Float>(data)) {
            float_sum += std::get<Float>(data);

            all_int = false;
        } else {
            auto message = std::format("`{}` expected number, got `{}`", ADD, value.to_string());

            return std::unexpected(Error(message));
        }
    }

    if (all_int) {
        return Value(int_sum);
    }

    return Value(float_sum + int_sum);
}

Result native_sub(const EnvPtr& _env, const Values& values) {
    if (values.empty()) {
        return std::unexpected(Error("`-` expected at least 1 argument, got 0"));
    }

    bool all_int = true;

    auto car = values.front();

    auto data = car.data;

    Int int_difference = 0;
    Float float_difference = 0.0;

    if (std::holds_alternative<Int>(data)) {
        int_difference = std::get<Int>(data);
    } else if (std::holds_alternative<Float>(data)) {
        float_difference = std::get<Float>(data);

        all_int = false;
    } else {
        auto message = std::format("`-` expected number, got `{}`", car.to_string());

        return std::unexpected(Error(message));
    }

    auto values_size = values.size();

    if (values_size == 1) {
        return all_int ? Value(-int_difference) : Value(-float_difference);
    }

    for (Index index = 1; index < values_size; index += 1) {
        auto value = values[index];

        auto data = value.data;

        if (std::holds_alternative<Int>(data)) {
            int_difference -= std::get<Int>(data);
        } else if (std::holds_alternative<Float>(data)) {
            float_difference -= std::get<Float>(data);

            all_int = false;
        } else {
            auto message = std::format("`-` expected number, got `{}`", value.to_string());

            return std::unexpected(Error(message));
        }
    }

    if (all_int) {
        return Value(int_difference);
    }

    return Value(float_difference + int_difference);
}

Result native_mul(const EnvPtr& _env, const Values& values) {
    if (values.empty()) {
        auto message = std::format("`{}` expected at least 1 argument, got 0", MUL);

        return std::unexpected(Error(message));
    }

    bool all_int = true;

    Int int_product = 1;
    Float float_product = 1;

    for (const auto& value: values) {
        auto data = value.data;

        if (std::holds_alternative<Int>(data)) {
            int_product *= std::get<Int>(data);
        } else if (std::holds_alternative<Float>(data)) {
            float_product *= std::get<Float>(data);

            all_int = false;
        } else {
            auto message = std::format("`{}` expected number, got `{}`", MUL, value.to_string());

            return std::unexpected(Error(message));
        }
    }

    if (all_int) {
        return Value(int_product);
    }

    return Value(float_product * int_product);
}

Result native_div(const EnvPtr& _env, const Values& values) {
    if (values.empty()) {
        auto message = std::format("`{}` expected at least 1 argument, got 0", DIV);

        return std::unexpected(Error(message));
    }

    auto car = values.front();

    auto data = car.data;

    Float quotient = 0.0;

    if (std::holds_alternative<Int>(data)) {
        auto integer = std::get<Int>(data);

        quotient = static_cast<Float>(integer);
    } else if (std::holds_alternative<Float>(data)) {
        quotient = std::get<Float>(data);
    } else {
        auto message = std::format("`{}` expected number, got `{}`", DIV, car.to_string());

        return std::unexpected(Error(message));
    }

    auto values_size = values.size();

    for (Index index = 1; index < values_size; index += 1) {
        auto value = values[index];

        auto data = value.data;

        if (std::holds_alternative<Int>(data)) {
            auto integer = std::get<Int>(data);

            if (integer == 0) {
                return std::unexpected(Error(DIVISION_BY_ZERO));
            }

            quotient /= static_cast<Float>(integer);
        } else if (std::holds_alternative<Float>(data)) {
            auto floating = std::get<Float>(data);

            if (std::abs(floating) < EPSILON) {
                return std::unexpected(Error(DIVISION_BY_ZERO));
            }

            quotient /= floating;
        } else {
            auto message = std::format("`{}` expected number, got `{}`", DIV, value.to_string());

            return std::unexpected(Error(message));
        }
    }

    return Value(quotient);
}

Result native_print(const EnvPtr& _env, const Values& values) {
    bool space = false;

    for (const auto& value: values) {
        if (space) {
            std::cout << SPACE;
        }

        space = true;

        std::cout << value.to_string();
    }

    std::cout << std::endl;

    return Value::nil();
}

Result native_list(const EnvPtr& _env, const Values& values) {
    return Value::list(values);
}

Result native_car(const EnvPtr& _env, const Values& values) {
    auto values_size = values.size();

    if (values_size != 1) {
        auto message = std::format("`{}` expected 1 argument, got {}", CAR, values_size);

        return std::unexpected(Error(message));
    }

    auto value = values.front();

    if (value.is_nil()) {
        return value;
    }

    auto data = value.data;

    if (!std::holds_alternative<ListPtr>(data)) {
        auto message = std::format("`{}` expected list, got `{}`", CAR, value.to_string());

        return std::unexpected(Error(message));
    }

    auto list = std::get<ListPtr>(data);

    auto items = list->items;

    if (items.empty()) {
        return Value::nil();
    }

    return items.front();
}

Result native_cdr(const EnvPtr& _env, const Values& values) {
    auto values_size = values.size();

    if (values_size != 1) {
        auto message = std::format("`{}` expected 1 argument, got {}", CDR, values_size);

        return std::unexpected(Error(message));
    }

    auto value = values.front();

    if (value.is_nil()) {
        return value;
    }

    auto data = value.data;

    if (!std::holds_alternative<ListPtr>(data)) {
        auto message = std::format("`{}` expected list, got `{}`", CDR, value.to_string());

        return std::unexpected(Error(message));
    }

    auto list = std::get<ListPtr>(data);

    auto items = list->items;

    if (items.empty()) {
        return Value::nil();
    }

    items.erase(items.begin());

    if (items.empty()) {
        return Value::nil();
    }

    return Value::list(items);
}

Result native_cons(const EnvPtr& _env, const Values& values) {
    auto values_size = values.size();

    if (values_size != 2) {
        auto message = std::format("`{}` expected 2 arguments, got {}", CONS, values_size);

        return std::unexpected(Error(message));
    }

    auto car = values[0];
    auto cdr = values[1];

    if (cdr.is_nil()) {
        return Value::list({ car });
    }

    auto data = cdr.data;

    if (!std::holds_alternative<ListPtr>(data)) {
        auto message = std::format("`{}` expected list as second argument, got `{}`", CONS, cdr.to_string());

        return std::unexpected(Error(message));
    }

    auto list = std::get<ListPtr>(data);

    auto items = list->items;

    items.insert(items.begin(), car);

    return Value::list(items);
}

Result native_length(const EnvPtr& _env, const Values& values) {
    auto values_size = values.size();

    if (values_size != 1) {
        auto message = std::format("`{}` expected 1 argument, got {}", LENGTH, values_size);

        return std::unexpected(Error(message));
    }

    auto value = values.front();

    if (value.is_nil()) {
        return Value(0);
    }

    auto data = value.data;

    if (!std::holds_alternative<ListPtr>(data)) {
        auto message = std::format("`{}` expected list, got `{}`", LENGTH, value.to_string());

        return std::unexpected(Error(message));
    }

    auto list = std::get<ListPtr>(data);

    auto items = list->items;

    auto length = static_cast<Int>(items.size());

    return Value(length);
}

Result native_equal(const EnvPtr& _env, const Values& values) {
    if (values.empty()) {
        auto message = std::format("`{}` expected at least 1 argument, got 0", EQUAL);

        return std::unexpected(Error(message));
    }

    auto values_size = values.size();

    auto first = values.front();

    for (Index index = 1; index < values_size; index += 1) {
        auto value = values[index];

        if (first != value) {
            return Value(false);
        }
    }

    return Value(true);
}

Result native_not_equal(const EnvPtr& _env, const Values& values) {
    if (values.empty()) {
        auto message = std::format("`{}` expected at least 1 argument, got 0", NOT_EQUAL);

        return std::unexpected(Error(message));
    }

    auto values_size = values.size();

    auto first = values.front();

    for (Index index = 1; index < values_size; index += 1) {
        auto value = values[index];

        if (first != value) {
            return Value(true);
        }
    }

    return Value(false);
}

EnvPtr lisp::default_env() {
    auto env = std::make_shared<Env>();

    env->define(NIL, Value::nil());
    env->define(TRUE, Value(true));
    env->define(FALSE, Value(false));

    add_native(env, NOT, native_not);

    add_native(env, ADD, native_add);
    add_native(env, SUB, native_sub);
    add_native(env, MUL, native_mul);
    add_native(env, DIV, native_div);

    add_native(env, PRINT, native_print);

    add_native(env, LIST, native_list);
    add_native(env, CAR, native_car);
    add_native(env, CDR, native_cdr);
    add_native(env, CONS, native_cons);
    add_native(env, LENGTH, native_length);

    add_native(env, EQUAL, native_equal);
    add_native(env, DOUBLE_EQUAL, native_equal);
    add_native(env, NOT_EQUAL, native_not_equal);

    return env;
}
