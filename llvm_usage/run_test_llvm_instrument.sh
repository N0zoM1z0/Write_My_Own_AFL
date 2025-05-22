clang++-10 test_llvm_instrument.cpp     `llvm-config-10 --cxxflags --ldflags --system-libs --libs core irreader support mc BitReader analysis profiledata`     -o instrument_tool
./instrument_tool target_program.c instrumented.ll
clang-10 -c instrumented.ll -o instrumented.o
clang-10 instrumented.o runtime.o -o final_executable
./final_executable

# cleanup
rm -rf runtime.o 