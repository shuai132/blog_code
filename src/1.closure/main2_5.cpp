#include <cstdio>
#include <cstdint>

int main() {
    uint8_t  a = 0;
    uint64_t b = 0;

    char array[100];

    auto func1 = []{};
    auto func2 = [a]{};
    auto func3 = [&a]{};
    auto func4 = [b]{};
    auto func5 = [array]{};

    printf("%d\n", sizeof(func1));
    printf("%d\n", sizeof(func2));
    printf("%d\n", sizeof(func3));
    printf("%d\n", sizeof(func4));
    printf("%d\n", sizeof(func5));
    return 0;
}

/*
输出:
1
1
8
8
100
*/
