# Using Concepts in C++ 20

Examples and explanations from the textbook *Programming with C++20 by Andreas Fertig*.

## Concepts

Templates enable generic programming (GP), the idea of abstracting *concrete* algorithms to get *generic* algorithms. They can then be combined and used with different types to produce a wide variety of software without providing a dedicated algorithm for each type. One of the drawbacks to using templates in C++ with an unsupported type is that finding the error **requires a good understanding of the compilers error message**. The issue is that we previously had no way of specifying the requirements to prevent a misuse and at the same time, give a clear error message.

If we wanted to simplify code to add a variable number of arguments from previous implementations with the help of **C++ 20 Concepts**, we can utilize the **requires** keyword. In this case, we require that all of the arguments in the function template to be true which is provided by the inline function `are_same_v`.

```cpp
template<typename... Args>
requires are_same_v<Args...>
auto Add(Args&&... args) {
    return (... + args);
}
```

## `requires` Clause vs Expression

With Concepts, we now have two primary tools at our disposal: `requires`-**clauses** and `requires`-**expressions**. While they share syntax similarities, they serve different purposes in how they constrain templates.

### `requires` Clause

The `Add` function above is using a **requires-clause** — a boolean condition that must evaluate to `true` at compile time for the template to be instantiated. `are_same_v` is the helper variable template that makes sure all arguments are of the same type. This lone requirement acts as a **singular requirement** and prevents mixed-type additions such as `Add(2, 9.75)`, which could silently convert types or lead to imprecise behavior.

The syntax for a *requires-clause* is straightforward: the `requires` keyword is followed by a condition, and that condition can be a simple concept or a type trait. When the condition is not satisfied, the template is excluded from the overloaded set — a clean alternative to older `std::enable_if` idioms.

```cpp
template<typename T>
requires std::integral<T> || std::floating_point<T>
T Add(T a, T b) {
    return a + b;
}
```

### `requires` Expression

To implement requirements that span multiple areas, the standard provides us with `requires`-expression. In constrast to the simple `requires`-clause, a `requires`-expression allows us to check *multiple and varied* constraints on template parameters. You can think of it as a compile-time "function body" that verifies the validity of certain operations and types.

```cpp
requires(Args... args) {
    (... + args);                                  // SR: Simple requirement
    requires are_same_v<Args...>;                  // NR: Nested requirement
    requires sizeof...(Args) > 1;                  // NR: At least two arguments
    { (... + args) } noexcept -> same_as<first_arg_t<Args...>>;  // CR: Compound requirement
}
```

This block of code is wrapped inside a `requires(...){ ... }` construct and is evaluated at compile time during template instantiation. Each line represents a different requirement kind.

| **Requirement**          | **Purpose**                                                              |
| ------------------------ | ------------------------------------------------------------------------ | 
| **Simple Requirement**   | Checks if an expression is valid (e.g., if `(... + args)` compiles.)     |
| **Nested Requirement**   | Asserts a boolean expression at compile time.                            |
| **Compound Requirement** | Validates the return type and `noexcept` specification of an expression. |
| **Type Requirement**     | Asserts that a certain type member (like `T::value_type`) exists         |

We can imbed this entire expression into a `requires`-clause like so:

```cpp
template<typename... Args>
requires requires(Args... args) {
    (... + args);
    requires are_same_v<Args...>;
    requires sizeof...(Args) > 1;
    { (... + args) } noexcept -> same_as<first_arg_t<Args...>>;
}
auto Add(Args&&... args) {
    return (... + args);
}
```

This usage, known as an **ad hoc constraint**, allows you to write detailed checks inline without first creating a named concept. While it is convenient, it's recommended to extract such expressions into reusable concepts when appropriate.

#### Named Concepts

If the logic inside a `requires`-expression becomes complicated, it's better to wrap it into a named **concept**, which can then be reused across different templates. A concept always starts with a template-head. The reason is that concepts are predicates (a callable entity like a function that takes in one or more arguments and returns a boolean value) in generic code, which makes them templates. A name must be given to the concept followed by an assignment to the requirements of the concept. Here is how we can refactor our example above into a named concept we can reuse:

```cpp
template<typename... Args>
concept Addable = requires(Args... args) {
    (... + args);
    requires are_same_v<Args...>;
    requires sizeof...(Args) > 1;
    { (... + args) } noexcept -> same_as<first_arg_t<Args...>>;
}

template<typename ...Args>
requires Addable<Args...>
auto Add(Args&&... args) {
    return (... + args);
}
```

By doing it this way, the `Add` function becomes easier to read and maintain. The `Addable` concept can now be reused in other function templates that share similar constraints.

## `concept` Testing Requirements

As concepts are evaluated entirely at compile time, you don't need a runtime testing framework to verify them. Instead you can use `static_assert` to check whether specific types satify your concepts. A common practice is to create stub types that simulate different combinations of properties like whether an operator is defined, the concept is `noexcept`, and the correct return type. The stubs can help verify how your concept behaves when applying valid or invalid types to improve correctness in development. See `cpp/concepts/main.cc` for full example.

## Abbreviated Function Templates

C++ 20 allows you to use *abbreviated function templates* using constrained `auto` parameters. Rather than writing full `template<typename T>` declarations and `requires` clauses, you can now write:

```cpp
template<typename T>
concept Integral = std::integral<T>;

void Print(Integral auto value);
```

This feature improves readability by putting the constraint directly where the type would otherwise go. It also helps communicate the expected interface of a parameter more explicitly, especially during quick reading or code review. This techniques applies to both function parameters and return types, extending the usefulness of concepts even further.

## Concepts + `constexpr if`: Clean Conditional Code

One powerful use of concepts is their combination with `constexpr if`. This makes it easy to call member functions conditionaly without boilerplate. For example, you can write:

```cpp
template<typename T>
void Send(const T& data) {
    if constexpr (SupportsValidation<T>) {
        data.validate();
    }
    // Send logic...
}
```

This avoids the need for dummy `validate()` functions or SFINAE-based overloads. Concepts enable cleaner code by formally expressing interface expectations and removing the guesswork around what is requires of a type.


## Concept Subsumption and Ordering

C++20 introduces a mechanism known as **subsumption**, which determines whick of two overloaded functions is more constrained (i.e., more specific). If one concept logically includes another (e.g., `integral` implies `arithmetic`), then the compiler prefers the more constrained overload. This is a cornerstone of overload resolution with concepts and helps maintain predictable behavior when multiple overloads exist.