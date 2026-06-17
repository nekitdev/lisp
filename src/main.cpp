#include <fstream>
#include <iostream>

#include "lisp.hpp"

static String LISP = "lisp> ";
static String QUIT = "quit";
static String EXIT = "exit";

static Index READ_SIZE = 4096;

static char NULL_BYTE = '\0';

String read_file(const String& name) {
    auto stream = std::ifstream(name);

    stream.exceptions(std::ios_base::badbit);

    String output;

    auto buffer = String(READ_SIZE, NULL_BYTE);

    while (stream.read(&buffer[0], READ_SIZE)) {
        output.append(buffer, 0, stream.gcount());
    }

    output.append(buffer, 0, stream.gcount());

    return output;
}

int main(int argc, char* argv[]) {
    auto env = default_env();

    if (argc > 1) {
        auto name = argv[1];

        auto code = read_file(name);

        auto result = run(env, code);

        if (!result) {
            auto error = result.error();

            std::cerr << "error: " << error.message << std::endl;

            return 1;
        }

        auto value = result.value();

        std::cout << value.to_string() << std::endl;

        return 0;
    }

    std::string line;

    std::cout << LISP;

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            std::cout << LISP;

            continue;
        }

        if (line == QUIT || line == EXIT) {
            break;
        }

        auto result = run(env, line);

        if (!result) {
            auto error = result.error();

            std::cerr << "error: " << error.message << std::endl;
        } else {
            auto value = result.value();

            std::cout << value.to_string() << std::endl;
        }

        std::cout << LISP;
    }

    return 0;
}
