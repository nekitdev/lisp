#ifndef LISP_VALUE_H
#define LISP_VALUE_H

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

#include "errors.h"
#include "primitives.h"
#include "symbol.h"

namespace lisp::types {
    class Value {
        public:
            using List = std::list<Value>;
            using Map = std::map<Value, Value>;

            using EnvMap = std::unordered_map<Symbol, Value>;

            using Names = std::vector<Symbol>;
            using Body = std::shared_ptr<Value>;

            using Values = std::vector<Value>;

            using Result = errors::Result<Value>;

            class Env {
                public:
                    using Parent = std::shared_ptr<Env>;

                    Parent parent;
                    EnvMap map;

                    Env(Parent parent = nullptr): parent(std::move(parent)) {}
            };

            using Native = std::function<Result(Env, Values)>;

            class Lambda {
                public:
                    Env closure;
                    Names names;
                    Body body;
            };

            using Data = std::variant<Bool, Int, Float, String, Symbol, List, Map, Lambda, Native>;

            Data data;

            Value(Data data): data(std::move(data)) {}
    };
}

#endif // LISP_VALUE_H
