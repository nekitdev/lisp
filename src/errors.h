#ifndef LISP_ERRORS_H
#define LISP_ERRORS_H

#include <exception>
#include <expected>
#include <string>

namespace lisp::errors {
    class Error: public std::exception {
        public:
            std::string message;

            explicit Error(std::string message): message(std::move(message)) {}

            const char* what() const noexcept override {
                return message.c_str();
            }
    };

    template <typename T>
    using Result = std::expected<T, Error>;
}

#endif // LISP_ERRORS_H
