// runtime_lib.c
#include <stdio.h>

// extern "C" // 如果用 C++ 编译器编译这个文件，需要 extern "C"
void log_function_entry(const char *function_name) {
    printf("[LOG] Entering function: %s\n", function_name);
}