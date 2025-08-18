cmake -S . -B build -DBUILD_EDITOR=OFF
cmake --build build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..