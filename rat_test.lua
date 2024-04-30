---@diagnostic disable: lowercase-global

local rat = require "rat"

function rat_test(label, val)
    print(label..":", val, val:tonumber())
end

a = rat(1,2)
b = rat(1/3)

rat_test("a", a)
rat_test("b", b)
rat_test("a+b", a+b)
rat_test("a-b", a-b)
rat_test("a*b", a*b)
rat_test("a/b", a/b)
rat_test("a%b", a%b)
rat_test("-a", -a)
rat_test("min(a,b)", a:min(b))
rat_test("max(a,b)", a:max(b))
c = b-a
rat_test("abs(b-a)", c:abs())
print("a<b", a<b)
print("a==b", a==b)
print("a>b", a>b)
print("a..b:", a..b)

print("indices:")
b[1] = 1
b[2] = 3
print("b:", b, "b[1]:", b[1], "b[2]:", b[2])