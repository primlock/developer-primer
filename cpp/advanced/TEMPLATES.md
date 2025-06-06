# Templating Functions and Classes for Generics

Templating in C++ allows you to write generic and reusable code. Instead of writing separate functions or classes for each data type, templates let you define them once and use them with any type. This is a feature widely used in the STL like in containers such as `std::vector` or `std::map` or functions like `std::sort`. Templates improve code maintainability and type safety by allowing operations on user-defined or built-in types without code duplication.

## The Basics of Template Syntax

Templates use the `template` keyword followed by `<>` (angled brackets) containing one or more *template parameters*. The most common of the parameters is a type parameter, specified as `typename T` or `class T` (both are interchangeable in this context). To declare a template with a single parameter we would use the syntax `template<typename T>`, where the `T` is a widely followed convention when the parameter represents a type.

```cpp
template<typename T>
T add(T a, T b) {
    return a + b;
}
```

In the code sample above, `T add(T a, T b)` is a generic function that adds two values of the passed type (`T a, T b`) and returns the result with the same type (`T`). During compilation, the  compiler *instantiates* specific versions of `add` depending on if the user passed `int`'s or `double`'s.

The same idea can be applied to classes as well:

```cpp
template <typename T>
class Box {
    T value;
public:
    Box(T v) : value(v) {}
    T get() const { return value; }
};
```

### Non-Type Template Parameters

You can also specify a non-type parameter in a template, also known as **value parameters**. This allows you to pass constant values as arguments to templates in addition to types. These values are *known at compile time* and can be used to customize the behavior of the template. The following are allowed in non-type template parameters:

* **Integral types**: Common non-type parameters include integers (`int`, `size_t`), enums, and booleans.
* **Pointers and References:**: Pointers to functions, objects, and references can also be used as non-type parameters.
* **Literal Classes**: Since C++20, instances of literal classes can be used as non-type template parameters.

```cpp
template <typename T, int N> // N is a non-type parameter
class MyArray {
  T arr[N];
};
```

## [Variadic Templates](https://cppreference.com/w/cpp/language/parameter_pack.html)

C++11 added variadic templates to the standard library to allow you to define functions or classes that accept a variable number of arguments. They are useful when you need to handle a variable number of parameters of possibly different types (like in `std::printf`). They are also referred to as *parameter packs* and come in 2 different flavors depending on the application:
* **Template Parameter Pack**: A template parameter that accepts zero or more template arguments.
* **Function Parameter Pack**: A function parameter that accepts zero or more template arguments.

Variadic templates use an ellipsis (`...`) to denote the parameter pack and can be applied like:

```cpp
template<typename... Args>
void my_function(Args... args) {
  // ...
}
```

where `template<typename... Args>` declares `Args` as a template parameter pack and `Args... args` declares `args` as the function parameter pack. To use the arguments in the pack the ellipsis operator is used to expand it into individual arguments. This is typically performed with recursion or with fold expressions.

## [Fold Expressions](https://en.cppreference.com/w/cpp/language/fold.html)

Fold expressions were introduced in C++17 to simplify the process of working with variadic templates. Before fold expressions, developers typically had to write recursive functions to unpack parameter packs, wich could be verbose and error-prone. Fold expressions allow you to apply a binary operator (like `+`, `*`, `&&` or `||`) across all elements of a parameter pack in a single, compact expression.

The basic syntax of a fold expression involves placing the ellipsis (`...`) either before or after the parameter pack, depending on whether you are performing a *left* or a *right* fold. The expression

```cpp
(... + args);
```

performs a **left fold**, meaning it expands in left-to-right order like `((arg1 + arg2) + arg3) + ...`. The expression 

```cpp
(args + ...);
```

performs a **right-fold** which expands in right-to-left order like `arg1 + (arg2 + (arg3 + (...)))`. 

Fold expressions can also include an initial value that can combine with the first or last (left or right) parameter, like (`0 + .. + args`), which provides a well-defined starting point. This is especially important when the pack might be empty. Returning to our veriadic templates example with `my_function(Args... args)`, we can expand the pack by using a fold expressions like so:

```cpp
template<typename... Args>
void my_function(Args... args) {
    (std::cout << ... << args << " "); // fold-expression to print each argument
}
```