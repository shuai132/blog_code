# C++11 实现初始化捕获（init capture）

## 初始化捕获（init capture）
初始化捕获，是C++14添加的特性，用于把对象移动到闭包。

关于Lambda和闭包，见我的另一篇文章:  
[Lambda和闭包(closure) 及其在C++中的实现原理](https://blog.csdn.net/u012456479/article/details/101479821)

### 使用场景和用法
#### 使用场景
* 当有一个大对象，在某个的lambda中被使用，且后续不会再被使用了。为了避免拷贝开销，最好move到闭包
* 有些只能移动的资源，如`std::unique_ptr`

#### 用法
* C++14提供了初始化捕获方式:
```cpp
    auto func = [bigObject = std::move(bigObject)] {
        cout << "do something" << endl;
    };
```
实现了对象到闭包的转移/移动，但是在C++11中不支持这种捕获方式。

* C++11使用`std::bind`:  
如果拷贝捕获，将发生一次bigObject复制。
```cpp
    auto func = [bigObject]() {
        cout << "do something" << endl;
    };
```
只能另辟蹊径，可以这么做:
```cpp
    auto func = std::bind([](BigObject& object) {
        cout << "do something" << endl;
    }, std::move(bigObject));
```

### 完整用例
* 定义一个大对象，并打印各种构造函数：
```cpp
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
```

* move到闭包测试
```cpp
int main() {
    auto testCase = [](auto name, auto func) {
        cout << endl << "// "<< name << endl;
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

    testCase("good way(C++14)...", []{
        BigObject bigObject;
        auto func = [bigObject = std::move(bigObject)] {
            cout << "do something" << endl;
        };
        cout << "sizeof func:" << sizeof(func) << endl;
        func();
    });

    testCase("good way(C++11)...", []{
        BigObject bigObject;
        auto func = std::bind([](BigObject& object) {
            cout << "do something" << endl;
        }, std::move(bigObject));
        cout << "sizeof func:" << sizeof(func) << endl;
        func();
    });

    return 0;
}
```
输出为:
```text
// test bad way...
// bad way...
Copy Constructor
sizeof func:1024
do something

// good way(C++14)...
Move Constructor
sizeof func:1024
do something

// good way(C++11)...
Move Constructor
sizeof func:1025
do something
```

**可见，后两次都实现了move转移**  

值得一提的是，以上还打印了func的大小，供大家学习思考。大致说就是:
* lambda生成的func或者说仿函数，这个仿函数有个成员就是BigObject。根据C++的对象模型，大小就是1024。
* 而std::bind生成的func或者说仿函数，根据bind的实现原理，它把lambda和BigObject构造了新的仿函数对象，所以大小是1025。

详细的细节请查阅std::bind相关资料，或者反编译查看，这部分也非常复杂，不过多论述，浅尝辄止吧。

## 总结
初始化捕获（init capture），它移动资源、它离不开`std::move`、它看起来、用起来都是在move资源。
所以有些人称之为移动捕获，也有道理。

## Links
* 文章示例代码: [https://github.com/shuai132/blog_code/tree/master/src/3.init_capture](https://github.com/shuai132/blog_code/tree/master/src/3.init_capture)
