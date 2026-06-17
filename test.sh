TEST=1 cmake -S . -B build
TEST=1 cmake --build build
cd build
ctest
