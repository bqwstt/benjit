# BenJIT: Benji's JIT-compiled Language
BenJIT is a toy programming language built initially for [Recurse Center](https://www.recurse.com/)'s Impossible Stuff Day (an event where participants are encouraged to try something that feels impossible to do in a day).

The original idea was to build a JIT compiler in a day. It surely felt impossible but I knew I could get somewhat far. This is the result of it!

## Where's the JIT, dude?
That's the fun bit! There's none, yet. I now know writing a JIT in a day is truly impossible. But we'll get to it soon. For now, this is what the language looks like:

```benl
algorithm my_awesome_func() is
    variable i := 0
    for i < 10 do
        i := i + 1
        if i > 5 then
            return i + 1
        end
    end

    print(i) # Won't get printed
    return i
end

algorithm my_other_awesome_func() is
    variable i := 42
    if i < 100 then
        print(i)
        i := i + 40
    end
    return i
end

variable dude_thats_awesome := my_awesome_func()
variable it_surely_is := my_other_awesome_func()
print(dude_thats_awesome)
print(it_surely_is)

# Program prints:
# 42
# 7
# 82
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
