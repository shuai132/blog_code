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
