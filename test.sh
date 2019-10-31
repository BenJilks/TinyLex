cd build
cmake ..
make
./TinyLex

cd test_project
g++ *.cpp -o test
./test

