# Lambda和闭包(closure) 及其在C++中的实现原理

大多数技术，只需要学习本质概念即可。重要的在于推理、发挥想象力！！！

## Lambda和闭包 及其区别和联系
本章讲解lambda和闭包的概念，概念与具体语言无关。

但是为了更好地它说明在C++中的特点，先描述了其他语言的行为和例子，毕竟闭包的概念源于这些函数式的语言。

### 函数是一等公民
对于基础概念一定要咬文嚼字。比如函数和函数对象的区别：

区别于C语言的函数只是一个地址、一个数字，或者称为函数地址。
函数对象有更灵活的用途，比如用于函数式编程（这不在本文讨论范围）至关重要的一句话：函数是一等公民。

**后文所指的函数和函数对象是一个意思，除非特殊说明。**

### Lambda
#### 本质
**lambda或称lambda表达式，本质就是一个匿名函数。**

#### 发挥想象
不论什么语言，只要支持匿名函数这样的特性，我们就说，它支持lambda（不同程度上）。

**匿名函数指的就是定义这个函数的时候，不需要命名。通常就是就地定义，就地使用的函数(function)。**

为什么会有这种需求呢？因为通常我们会需要使用一个代码较少的函数时，定义一个函数过于繁琐，而且也会一定程度地污染命名空间。

比如lua里有这样的表达:
```lua
func(function() print("I am lambda") end)
```
比如c++11之后能写成这样:
```cpp
func([] { printf("I am lambda\n"); });
```
比如java8之后支持这样:
```java
func(()->{ System.out.println("I am lambda"); });
```
以上，func里的东东，就是lambda了。
golang、python、js都有类似的表达，不再一一举例。

**写法怎么样不重要，重要的是要有想象力~**

#### 其他思考
* lambda是函数还是函数对象？
> 这其实不是个问题，是函数也好，函数对象也好，其重点在于它是匿名的。但是主流语言的实现，都是函数对象。
>
> 另外，在不同语言，对lambda的表述和支持程度也不同，比如python中lambda只支持单条语句，大大限制了其实用性。
> 还有下一节讲提到的闭包，事实上python2中的闭包对基本类型的支持也不够好，直到python3引入了nonlocal。
>
> 再比如java8引入的lambda，不得不说也比较鸡肋，尽管一定程度上也解决了一些问题。配合stream使用虽然改进不错，但是还不如支持FP(函数式编程)的语言的自然。
> 反而C++11引入的lambda支持了齐全的捕获方式，更加实用。

* lambda是如何实现的？在编程语言中它是如何得到支持的？
> 对支持FP的语言讲，没什么特别的，它就是一个临时变量而已。毕竟它们认为函数也是一种变量嘛。
> **对于静态语言，比如C++、Java，本质上是匿名的对象。编译器会自动生成特殊的类用于生成对象。**


### 闭包(closure)
闭包或称closure，它的概念比较抽象，具体一点描述:
> 闭包就是能够读取其他函数内部变量的函数。  
> 在本质上，闭包是将函数内部和函数外部连接起来的桥梁。

换成我认为好理解的说法就是：**闭包是一个函数对象，和普通函数不同的是，它能够捕获到外部作用域的变量，延长其生命期。**

注意，上述的重点是，**函数对象可以捕获到外部作用域的变量**，至于如何捕获、何时捕获，可并非大家做法都一样（可参考lua和js闭包的不同）。

首先我们看一个例子，请观察输出结果是什么:  
* python  
注意nonlocal关键字，且要python3，不熟悉python的读者可看lua版本的
```python
    def a():
        x = 0
        def b():
            nonlocal x
            x = x + 1
            print(x)
        return b
    
    b = a()
    b()
    b()
```

* lua
```lua
    local function a()
        local x = 0;

        local function b()
            x = x + 1
            return x
        end
        return b
    end

    local b = a()
    print(b())
    print(b())
```

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

没错，输出结果都是1和2。b捕获了父作用域a中的变量x（lua中称之为upvalue），这种捕获，导致x的生命期被延长。

但是对于c++来讲，其实现原理是不同于fp语言，在下一节讲解。

### 区别联系
看明白了以上论述自然不难总结：
1. lambda == 匿名函数
2. 闭包是用函数对象表示的

所以，lambda可以表示闭包，或者说，闭包可以用lambda表示。
这是两个不同的概念，但是呢，常常会一起说和用。


## Lambda在C++中的实现方式

下面讲在C++中lambda的实现方式，以及基于这种实现方式带来语义(捕获模式)，需要读者对C++11的lambda有一定的基础。

### 实现方式和特点
对上一小节的例子有必要深入理解一下c++内部是如何做的：

共同点和重点是：`x的生命周期被延长了`。

值得一提的是，c++不具备延长栈变量生命周期的能力，具体讲：
实际上，b拷贝了一份x到自己的函数对象(闭包)中。
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

### 理解这个细节很重要么？
是的，很重要:
* 首先，很多人（包括我之前）认为它有可能是个类似内联函数的东西，尤其是我们不捕获任何变量的时候。比如：
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
**实际上是，你永远也不要这么认为，编译器也不会做任何优化。**

之所以说这很重要，是因为你一旦认为它是个类似函数地址的东西，就会觉得它是个全局的东西 -- 不会被析构！
事实上，一旦离开作用域，它将被析构。如果不小引用过这个函数，将会引用到无效地址。

* 你会发现C++的函数对象，它并不是完美的闭包。

因为它并不能真正延长变量的生命期，`它只提供复制和引用的捕获语义`，这和语言特性有关。
因为语言特性的问题，它并没有延长变量作用域的能力，因为局部变量永远都需要在作用域结束后析构。
尽管我们有办法去解决这个问题，让它看上去和用起来就像其他高级语言的实现一样（比如智能指针）。

之所以说这很重要，是因为如果你理解成fp语言那种，请看这两个写法，结果将是不同的：

* lua 结果1 2 3 4
```lua
local function main()
    local function a()
        local x = 0;

        local function b()
            x = x + 1
            return x
        end
        local function c()
            x = x + 1
            return x
        end
        return b, c
    end

    local b, c = a()
    print(b())
    print(b())

    print(c())
    print(c())
end

main()
```

* c++
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
结果将是1 2 1 2，懂得内部原理的话，就不难理解这种结果的原因（因为b和c各自为自己复制了一份x，各自是独立的）。

能做到想lua那样么？毕竟，那样看上去才更符合人类的思维和直觉！

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
> 为什么会有这种问题，因为在c++中有值语义和引用语义。上述和大部分语言都是引用语义的。  
> 值语义下，lambda是有大小的，会影响程序效率。
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
**如果没有捕获东西，比如大多数`std::function<void()>`的形式，是没有必要引用传递的，也没有必要用`std::move()`。**
但是有大对象值捕获的时候，还是有必要使用move的。但是的但是呢！！！出现这种大的值对象捕获的时候，说明设计可能是有问题的！除非你知道这将意味着什么，否则不要这么做。

### 其他说明
C++11引入了lambda，极大的方便了编程设计。但是要理解它的前提下使用，否则很容易写出隐蔽的bug。  
**避免使用默认捕获模式（Avoid default capture modes），尤其是引用捕获。**

## Links
* 文章示例代码: [https://github.com/shuai132/blog_code/tree/master/src/1.closure](https://github.com/shuai132/blog_code/tree/master/src/1.closure)
