local function func(closure)
    closure();
end
func(function() print("i am lambda") end)



print("test main2")
local function main2()
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
end

main2()
