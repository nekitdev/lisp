#ifndef LISP_ERRORS
#define LISP_ERRORS

#include <exception>
#include <expected>
#include <string>

namespace lisp {
    class Error: public std::exception {
        public:
            std::string message;

            explicit Error(std::string message): message(std::move(message)) {}

            const char* what() const noexcept override {
                return message.c_str();
            }
    };

    template <typename T>
    using GenericResult = std::expected<T, Error>;
}

#endif // LISP_ERRORS
