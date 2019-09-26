#include <iostream>

using namespace std;

class BigObject {
public:
    char bigObj[1024] = {0};

    BigObject() {
        // cout << "Constructor" << endl;
    }

    ~BigObject() {
        // cout << "~Constructor" << endl;
    }

    BigObject(const BigObject& a) {
        cout << "Copy Constructor" << endl;
    }

    BigObject& operator=(const BigObject& a) {
        cout << "Copy Assignment operator" << endl;
        return *this;
    }

    BigObject(BigObject&& a) noexcept {
        cout << "Move Constructor" << endl;
    }

    BigObject& operator=(BigObject&& a) noexcept {
        cout << "Move Assignment operator" << endl;
        return *this;
    }
};

int main() {
    auto testCase = [](auto name, auto func) {
        cout << endl << "// " << name << endl;
        func();
    };

    testCase("bad way...", [] {
        BigObject bigObject;
        auto func = [bigObject]() {
            cout << "do something" << endl;
        };
        cout << "sizeof func:" << sizeof(func) << endl;
        func();
    });

    testCase("good way(C++14)...", [] {
        BigObject bigObject;
        auto func = [bigObject = std::move(bigObject)] {
            cout << "do something" << endl;
        };
        cout << "sizeof func:" << sizeof(func) << endl;
        func();
    });

    testCase("good way(C++11)...", [] {
        BigObject bigObject;
        auto func = std::bind([](BigObject& object) {
            cout << "do something" << endl;
        }, std::move(bigObject));
        cout << "sizeof func:" << sizeof(func) << endl;
        func();
    });

    return 0;
}

/*
// test bad way...
Copy Constructor
sizeof func:1024
do something

// test good way(C++14)...
Move Constructor
sizeof func:1024
do something

// test good way(C++11)...
Move Constructor
sizeof func:1025
do something
 */
