# BenJIT: Benji's JIT-compiled Language
BenJIT is a toy programming language built initially for [Recurse Center](https://www.recurse.com/)'s Impossible Stuff Day (an event where participants are encouraged to try something that feels impossible to do in a day).

The original idea was to build a JIT compiler in a day. It surely felt impossible but I knew I could get somewhat far. This is the result of it!

## Where's the JIT, dude?
That's the fun bit! There's none, yet. I now know writing a JIT in a day is truly impossible. But we'll get to it soon. For now, this is what the language looks like:

```benl
algorithm is_prime(n) is
    if n < 2 then
        return false
    end

    variable i := 2
    for i * i < n + 1 do
        variable divided := n // i
        variable remainder := n - divided * i
        if remainder == 0 then
            return false
        end
        i := i + 1
    end

    return true
end

algorithm count_primes(limit) is
    variable count := 0
    variable n := 2
    for n < limit + 1 do
        if is_prime(n) then
            count := count + 1
        end
        n := n + 1
    end
    return count
end

print(count_primes(50))
# Program prints:
# 15
```

BenJIT supports:
- variables with the `variable` keyword. The only types possible are `double`s and `bool`eans
- functions with `algorithm ... is`
- for loops with `for ... do` and `break/continue` statements
- if/else branching with basic boolean checks
- prints! but just for integers, heh

## Building
```sh
$ make
$ ./jit <path_to_file>
```

BenJIT is written in C++23, so you need a recent `clang` compiler. LLVM19+ will work. BenJIT uses `libc++`, so unfortunately GCC is not supported.
