#ifndef LISP_PRIMITIVES_H
#define LISP_PRIMITIVES_H

#include <cstdint>
#include <string>

namespace lisp::types {
    using Bool = bool;
    using Int = int64_t;
    using Float = double;
    using String = std::string;
}

#endif // LISP_PRIMITIVES_H
