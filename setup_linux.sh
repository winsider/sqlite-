rm -rf ./build
mkdir build
cd build
mkdir release
cd release
cmake --DCMAKE_BUILD_TYPE=Release ../..
make
cd ..
mkdir debug
cd debug
cmake --DCMAKE_BUILD_TYPE=Debug ../..
make
cd ..
cd ..



