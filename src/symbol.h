#ifndef LISP_SYMBOL_H
#define LISP_SYMBOL_H

#include <string>

namespace lisp::types {
    class Symbol {
        public:
            std::string name;

            Symbol(std::string name): name(std::move(name)) {}
    };
}

// specialize std::hash for Symbol
namespace std {
    template<>
    struct hash<lisp::types::Symbol> {
        size_t operator()(const lisp::types::Symbol& symbol) const {
            return std::hash<std::string>()(symbol.name);
        }
    };
}

#endif // LISP_SYMBOL_H
