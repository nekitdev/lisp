#ifndef LISP_PARSE
#define LISP_PARSE

#include "values.hpp"
#include <vector>

namespace lisp {
    struct Token {
        enum class Type {
            Open,
            Close,
            Quote,
            Atom,
            String,
        } type;

        String string;
    };

    using Tokens = std::vector<Token>;

    using SymbolResult = GenericResult<String>;
    using ValuesResult = GenericResult<Values>;

    using TokenizeResult = GenericResult<Tokens>;

    TokenizeResult tokenize(const String& code);

    ValuesResult parse(const Tokens& tokens);

    Result eval_block(EnvPtr env, const Values& values);
    Result eval(EnvPtr env, const Value& value);
    Result run(EnvPtr env, const String& code);
}

#endif // LISP_PARSE
