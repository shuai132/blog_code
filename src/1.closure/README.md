# Lambda和闭包(closure) 及其在C++中的实现原理

## Lambda和闭包
**lambda == 匿名函数。**

不论什么语言，只要支持匿名函数这样的特性，我们就说，它支持lambda（不同程度上）。

为什么会有匿名这种需求呢？因为通常需要使用一简短函数时，定义过于繁琐，而且也会一定程度地污染命名空间。

### Lambda的形式
JavaScript:
```js
func(function() { console.log("I am lambda"); });
```
Lua:
```lua
func(function() print("I am lambda") end)
```
C++11:
```cpp
func([] { printf("I am lambda\n"); });
```
Java8:
```java
func(()->{ System.out.println("I am lambda"); });
```
以上，func里的东东，就是lambda了。

换句话说，lambda就是匿名函数，并不是各种语言里的语法(那只是实现方式)。

**写法怎么样不重要，重要的是要有想象力~**

### 闭包(closure)
**闭包 == 捕获并持有了外部作用域变量的函数。**

举个最简单的例子:
```js
var makeCounter = function() {
    var count = 0;
    var func = function() {
        return ++count;
    };
    return func;
};

var counter = makeCounter();

console.log(counter());     // 1
console.log(counter());     // 2
```
func用到了父作用域的变量count，一般来说makeCounter调用后，变量count会销毁。但是由于返回的func引用到了count。
也就是说count是func引用到的一个资源，这导致count继续存在，且生命期和func一致。这就是闭包的本质和作用。

> 拓展: 闭包如何捕获、何时捕获变量，各种语言的做法并不相同（可参考lua和js闭包的不同、C++11的lambda），不深入讨论。

### 区别联系
看明白了以上论述自然不难总结：
1. lambda == 匿名函数
2. 闭包 == 捕获并持有了外部作用域变量的函数。

这是两个不同的概念，但是呢，常常会一起用。也就是用lambda表示闭包。


## Lambda在C++中的实现方式

以上是lambda和闭包基本概念，下面会比较深入地讲在C++中lambda的实现方式。
以及基于这种实现方式带来的语义(捕获模式)和问题，需要读者对C++11的lambda有一定的基础。

### C++版lambda计数器
* c++
```cpp
#include <iostream>

int main() {
    auto a = [] {
        int x = 0;
        auto b = [x]() mutable {
            return ++x;
        };
        return b;
    };
    auto b = a();
    std::cout<<b()<<std::endl;
    std::cout<<b()<<std::endl;
    return 0;
}
```

输出结果都是1和2。b捕获了父作用域a中的变量x，这种捕获，导致x的生命期被延长。

但是对于c++来讲，其实现原理是不同于FP语言。

### 实现方式和特点
对上一小节的例子有必要深入理解一下c++内部是如何做的：

共同点是：`x的生命周期被延长了`。

值得一提的是，c++并不具备延长栈变量生命周期的能力，具体讲：
b拷贝了一份x到自己的函数对象(闭包)中。
这么做是不得已为之，因为在语义上，lambda是作为一个语句块的，不管怎么说语句块结束后变量都要析构。
也就是x不管怎样都要销毁的。而不像fp语言，能做到延长它的生命！

内部实现如下(只改写b)，b的类结构，其实是个仿函数:
```cpp
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
```

### 理解实现方式很重要么？
是的，对C++使用者来说很重要:
* 首先，很多人（包括我之前）认为它有可能是个类似内联函数的东西，尤其是我们不捕获任何变量的时候。比如:
```cpp
int main() {
    auto func = [] {
        printf("hello");
    };
    func();
    return 0;
}
```
编译器会不会就偷偷优化成这样呢？
```cpp
static void func() {
    printf("hello");
};

int main() {
    func();
    return 0;
}
```
**你永远也不要这么认为，编译器也不会做任何优化。**

之所以说这很重要，是因为一旦认为它是个类似函数地址的东西，就会认为它不会被析构！
事实上，一旦离开作用域，它将立刻被析构。如果引用过这个函数，将会引用到无效地址。

* 仔细思考会发现C++的函数对象，它并不是完美的闭包。但是它提供了多种捕获方式，完美地结合到了语言特性。

因为它并不能真正延长变量的生命期，`它只提供复制和引用的捕获语义`，这和语言特性有关。
因为语言特性的关系(`RAII`)，C++无法延长变量作用域，因为局部变量永远都需要在作用域结束后析构。
尽管我们有办法去解决这个问题，让它看上去和用起来就像其他高级语言的实现一样（比如智能指针）。

之所以说这很重要，是因为如果你理解成fp语言那种，请看这两个写法，结果将是不同的：

* JavaScript: 结果 1 2 3 4
```js
var a = function() {
    var x = 0;

    var b = function() {
        return ++x;
    }
    var c = function() {
        return ++x;
    }
    return [b, c];
}

var pair = a()
var b = pair[0];
var c = pair[1];

console.log(b())
console.log(b())

console.log(c())
console.log(c())
```

* C++11: 结果 1 2 1 2
```cpp
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
```
懂得内部原理的话，就不难理解这种结果的原因（因为b和c各自为自己复制了一份x，各自是独立的）。

能做到像js那样么？那看上去才更符合人类的思维和直觉！

可以的，但是我们要模拟这种行为，有几种方案：
* x改为static，并引用方式捕获
```cpp
    auto a = []() {
        static int value = 0;           // 重点
        auto b = [&]() {                // 重点
            return ++value;
        };
        auto c = [&]() {
            return ++value;
        };
        return std::make_pair(b, c);
    };
```
* 使用C++14，绑定到函数对象，并使用引用方式捕获x
```cpp
    auto a = [x = 0]() mutable {        // 重点
        auto b = [&]() {                // 重点
            return ++x;
        };
        auto c = [&]() {
            return ++x;
        };
        return std::make_pair(b, c);
    };
```
* 使用智能指针
```cpp
    auto a = []() {
        auto x = std::make_shared<int>(0);  // 重点
        auto b = [=]() {
            return ++(*x);
        };
        auto c = [=]() {
            return ++(*x);
        };
        return std::make_pair(b, c);
    };
```

### 一个lambda/函数对象在c++中有多大？有没有必要使用std::move()？
我们需要测试一下函数对象的大小:
```cpp
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
```
输出是（结果可能与编译器和平台有关）:
```text
1
1
8
8
100
```
对结果的解释:
* **不捕获其他值的lambda，等价于一个空对象，一般只占用1个字节。**  
因为c++要确保两个不同对象的地址不同，一个对象至少要有一个字节（哪怕是空的），大部分编译器的实现是1。
* **有捕获的，字节数等于按值捕获的值的字节数 + 按引用捕获的值的个数 * 8**  
因为引用捕获实际上是指针实现。

所以结论是：  
**如果没有捕获变量，比如大多数`std::function<void()>`的形式，是没有必要引用传递的，也没有必要用`std::move()`。**
但是有大对象值捕获的时候，还是有必要使用move的。
但是的但是呢！！！出现这种大的值对象捕获的时候，说明设计很可能是有问题的！除非你知道这将意味着什么，否则不要这么做。

### 其他说明
C++11引入了lambda，极大的方便了编程设计。但是要理解它的前提下使用，否则很容易写出隐蔽的bug。  
**避免使用默认捕获模式（Avoid default capture modes），尤其是引用捕获。**


## 总结和思考
* lambda是函数(特指C/C++语言那种)还是函数对象  
这其实不是个问题，是函数也好，函数对象也好，其重点在于它是匿名的。但是主流语言的实现，都是函数对象。

* 不同语言对lambda、闭包的概念和支持  
在不同语言，对lambda的表述和支持程度都有不同，
    * lua和js中闭包捕获upvalue的行为也不同。
    * python中lambda只支持单条语句，大大限制了其实用性。而且python2中的闭包对基本类型的支持也不够好，直到python3引入了nonlocal。
    * java8引入的lambda，不得不说比较鸡肋，尽管一定程度上也提供了一点点方便，比如配合stream。
    * C++的闭包也与FP语言大有不同。支持了齐全的捕获方式，非常实用。

* lambda是如何实现的？在编程语言中它是如何得到支持的？  
对支持FP的语言讲，没什么特别的，它就是一个临时变量而已。毕竟它们认为函数也是一种变量嘛。
**对于静态语言，比如C++、Java，本质上是匿名的对象。编译器会自动生成特殊的类用于生成对象。**


## Links
* 文章示例代码: [https://github.com/shuai132/blog_code/tree/master/src/1.closure](https://github.com/shuai132/blog_code/tree/master/src/1.closure)
