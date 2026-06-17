#include <iostream>
#include <memory>

#include "env.hpp"

using namespace lisp;

void add_native(const EnvPtr &env, const String &name, Function function) {
    env->define(name, Value::native(name, function));
}

static char SPACE = ' ';

EnvPtr lisp::default_env() {
    auto env = std::make_shared<Env>();

    // add_native(env, "+", [](const EnvPtr& _env, const Values& values) {});

    add_native(env, "print", [](const EnvPtr& _env, const Values& values) {
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
    });

    return env;
}
