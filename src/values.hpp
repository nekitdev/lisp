#ifndef LISP_VALUES
#define LISP_VALUES

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "errors.hpp"

namespace lisp {
    struct Env;
    struct Value;

    using Nil = std::monostate;
    using Bool = bool;
    using Float = double;
    using Int = std::int64_t;
    using Index = std::size_t;
    using String = std::string;

    struct Symbol {
        String name;
    };

    using Items = std::vector<Value>;

    struct List {
        Items items;
    };

    using ListPtr = std::shared_ptr<List>;

    using Result = GenericResult<Value>;

    using Names = std::vector<String>;

    using Values = std::vector<Value>;

    using EnvPtr = std::shared_ptr<Env>;

    struct Lambda {
        EnvPtr closure;
        Names arguments;
        Values body;
    };

    using LambdaPtr = std::shared_ptr<Lambda>;

    using Function = std::function<Result(const EnvPtr&, const Values&)>;

    struct Native {
        String name;
        Function function;

        Result call(const EnvPtr& env, const Values& arguments) const;
    };

    using NativePtr = std::shared_ptr<Native>;

    struct Value {
        using Storage = std::variant<
            Nil,
            Bool,
            Int,
            Float,
            String,
            Symbol,
            ListPtr,
            LambdaPtr,
            NativePtr
        >;

        Storage data;

        Value() = default;

        explicit Value(Storage value): data(std::move(value)) {}

        static Value nil();
        static Value list(Items items);

        static Value native(const String& name, Function function);

        String to_string() const;

        [[nodiscard]] bool is_nil() const;
        [[nodiscard]] bool truthy() const;

        bool operator==(const Value& other) const;
        bool operator!=(const Value& other) const;
    };

    using EnvMap = std::unordered_map<String, Value>;

    struct Env: std::enable_shared_from_this<Env> {
        EnvPtr parent;

        explicit Env(EnvPtr env = nullptr): parent(std::move(env)) {}

        void define(const String& name, Value value);
        bool set(const String& name, Value value);
        Result get(const String& name) const;

        private:
            EnvMap map;
    };

    String to_string(const Value& value);
}

#endif // LISP_VALUES
