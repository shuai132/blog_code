#include <iostream>

int main() {
    auto a = [] {
        int x = 0;
        return [x]() mutable {
            return ++x;
        };
    };
    auto b = a();
    std::cout<<b()<<std::endl;
    std::cout<<b()<<std::endl;
    return 0;
}
