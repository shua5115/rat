---@meta

---@class Rational
local rat_mt

rat_mt = {
    ---@type integer
    [1] = nil,
    ---@type integer
    [2] = nil,
    tonumber = function (r) return 0.0 end;
    abs = function (r) return rat_mt end;
    max = function (...) return rat_mt end;
    min = function (...) return rat_mt end;
    compare = function (r1, r2) return 0 end;
}

---Creates a rational number from a float or two integers.
---@param a number If only a is included, then it approximates a as rational.
---@param b integer? If included, uses a as numerator and b as denominator.
---@return Rational
function rat(a, b)
    return rat_mt
end