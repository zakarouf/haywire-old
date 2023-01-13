# Syntax
Some possible ways the syntax can be implemented for haywire

## 1. Modern
```rust
import std
import std.error

export main
fn main(args: [string]) ?error {
    let x = 32
    let y = 53
    let z = x + y
}
```

## 2. Lispy
```lisp
(import std)
(export main)
(let main (fn (args: [string]) std.error (
  (let x 32)
  (let y 53)
  (let z (+ x y)))))
```

## 3. Evaluation
```haskell
std => str
str.error => error

=> main

(args: [string]) => {
  32 => x
  53 => y
  x + y => z

  z <=> 85
   => { => nil }
  |=> { => error.anyerror } => return
} => ?error => main
```
