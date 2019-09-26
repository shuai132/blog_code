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
