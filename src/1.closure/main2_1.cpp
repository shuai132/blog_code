#include <iostream>

int main() {
    auto a = []() {
        int x = 0;
        auto b = [x]() mutable {
            return ++x;
        };
        auto c = [x]() mutable {
            return ++x;
        };
        return std::make_pair(b, c);
    };

    auto pair = a();
    auto b = pair.first;
    auto c = pair.second;
    std::cout<<b()<<std::endl;
    std::cout<<b()<<std::endl;

    std::cout<<c()<<std::endl;
    std::cout<<c()<<std::endl;
    return 0;
}
