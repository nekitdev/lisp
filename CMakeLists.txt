cmake_minimum_required(VERSION 4.1.2)
project(lisp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# set(SOURCES
# )

set(HEADERS
    src/errors.h
    src/primitives.h
    src/symbol.h
    src/value.h
    src/types.h
)

set(EXECUTABLE src/main.cpp)

# add_library(lisp_library STATIC ${SOURCES} ${HEADERS})
# target_include_directories(lisp_library PUBLIC src)

add_executable(lisp ${EXECUTABLE})
# target_link_libraries(lisp lisp_library)

# enable_testing()
# add_subdirectory(tests)

install(TARGETS lisp
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
