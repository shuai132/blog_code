#include <iostream>

struct B {
    int x;
    int operator()() {
        return ++x;
    }
};

int main() {
    auto a = [] {
        int x = 0;
        return B {x};
    };
    auto b = a();
    std::cout<<b()<<std::endl;
    std::cout<<b()<<std::endl;
    return 0;
}
