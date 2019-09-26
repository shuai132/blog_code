#include <iostream>

int main() {
    auto a = []() {
        auto x = std::make_shared<int>(0);
        auto b = [=]() {
            return ++(*x);
        };
        auto c = [=]() {
            return ++(*x);
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
