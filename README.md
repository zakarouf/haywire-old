<div align="center">
  <h1>Haywire</h1>
  A Small Language Implementation written in Modern C.
</div>

## About
A small language implementation written in C.
* Very Small Binary Size (100K)
* Fast & Small VM
* Static Typed
* Modular Code design, can be easily improved upon and mutted to comply the user need
* Multi-purpose language; Can be used as a -> Data File Loader, Configure File or A Programming Language
* Built-in bytecode compiler is a VM itself which can evaluate code as it compiles.
* No Macros, instead usage of compile-time called functions to generate code
* Exhaustive Type system which can be easily used to implement user defined types.

## Installation

```
./ebuild.sh
```
> Make sure you have [z_](https://github.com/zakarouf/z_) installed

## Programming Prospectus
While not yet totally implemented `haywire` will have c-like syntax; akin to javascript.

```javascript
// `//` or `#`` for writting comments
import std        // Importing modules
import std.error  // Importing specific symbols or sub-modules from modules.

export main       // export symbols of the current module.

let main = fn(args: [string]) ?error {
    if(args.len() < 4) return error.any();
    let x = args[2].parseInt();
    let y = args[3].parseInt();
    std.println("Sum of {x} any {y} is {x + y}");
}
```
Running it.
```sh
$ haywire ./test 40 23
Sum of 40 and 23 is 63
```

