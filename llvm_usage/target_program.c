// target_program.c
#include <stdio.h>

void bar(int x) {
    printf("  Inside bar(%d)\n", x);
}

void foo() {
    printf("  Inside foo()\n");
    bar(10);
    bar(20);
}

int main() {
    printf("Starting main()\n");
    foo();
    printf("Finishing main()\n");
    return 0;
}