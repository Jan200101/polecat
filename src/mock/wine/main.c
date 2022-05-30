#include <stdio.h>

int main(int argc, char** argv)
{
    puts("Mock");

    for (int i = 0; i < argc; ++i)
        printf("%s ", argv[i]);
    puts("");
}
