# Using Coroutines in C++ 20

Examples, explanations and diagrams from the textbook *Programming with C++20 by Andreas Fertig*.

## Coroutines

A **coroutine** is a special type of function that can *suspend its execution* and later *resume from the point where it left off*, while preserving its internal state. The term *coroutine* has been around since 1958 but only with C++20 has it become a language feature. Coroutines enable a new way of writing functions that produce sequences of values or handle asynchronous logic, allowing for a cleaner separation between **computation logic** and **data consumption**.

## Regular Functions and Their Limitations

Before we dive into what coroutines offer, lets look at the limitations of traditional functions.

```cpp
for (int i = 0; i < 5; i++) {
    useCounterValue(i);
}
```

This loop mixes two distinct responsibilities: **generating numbers** and **consuming them**. When this pattern is repeated with different boundaries or logic, we duplicate code and introduce opportunities for subtle bugs, like off-by-one errors. Moreover, when we try to generalize this with lambdas or static variables to store a counter to store a counter, the solutions become rigid, difficult to manage and non-thread safe. Regular functions *lack the flexibility to pause and resume* with their linear flow of execution, making them poor candidates for generators or event-driven code.

## Coroutines for a Better Control Flow

Coroutines change this paradigm by introducing **resumable functions**. They allow a function to temporarily **yield control** and **return a value**, then pick up where it left off. This means that generation and usage can be separated cleanly.

In contrast to regular functions which execute linearly from start to finish, a coroutine can stop mid-way using `co-yield`, hand a value back to the caller and be *resumed later* (suspension of execution). These behavioral differences between regular functions and coroutines is shown below in figure 2.1.

<img src="figures/coroutine-vs-regular.png" alt="coroutine vs regular function" style="margin: 10 auto;">

### Coroutine Example: Counter Generator

Lets take a look at an example use case for a coroutine; generating a value.

```cpp
IntGenerator counter(int start, int end) {
    while(start < end) {
        co_yield start;
        start++;
    }
}
```

In the code above, `IntGenerator` is the type for the coroutine wrapper (an interface) that allows us to use the coroutine function in a range-based loop. This wrapper contains all the necessary values and methods for the coroutine to perform its task including how it should handle values/exceptions and other internal behavior. `counter` is the coroutine function which can yield a value (with `co_yield`) and suspend its execution. The usage of `co_yield` suspends the coroutine and sends a value back to the caller. Each time the coroutine is resumed, it continues after the last `co_yield`.

```cpp
void UseCounter() {
    auto g = counter(1, 5);
    for (auto i : g) {
        UseCounterValue(i);
    }
}
```

Now everytime `UseCounter` is called, we can pickup from the counter value we left with last time without having to duplicate a for loop or manage global variables.

### Coroutines Maintain State Automatically

Another important advantage of coroutines is **automatic state preservation**. In the regular counter implementation using a static variable, the function could only track one counter state globally. With coroutines, each call to the generator creates an **independant state machine**. You can spawn multiple generator instances, each with their own internal state, without writing any extra logic for managing those states. This is the foundation that makes coroutines powerful for **asyncronous programming, generators** and **iuterative processing**.

Coroutines are stackless which means they do not maintain their own stack. Instead, when a coroutine suspends, it returns control to the caller, and the necessary state for resumption is stored separately, typically on the heap in a **coroutine frame**. This approach differs from stackful coroutines (like Golang's goroutines) which allocate a separate stack for each coroutine, similar to threads.

### Coroutine Keywords

Coroutines in C++ are maked using one of the following three keywords:

| Keyword     | Purpose                  | Behavior   |
| ----------- | ------------------------ | ---------- |
| `co_yield`  | Yields a value           | Suspends   |
| `co_return` | Ends the coroutine       | Terminates |
| `co_await`  | Waits on an async result | Suspends   |

If any of these appear in a function, the function becomes a coroutine. The compiler will transform it into a **Finate State Machine (FSM)** with the coroutine's state held in the *coroutine frame*.

### The Coroutine Generator

C++ coroutines cannot have plain return types like `int` or `std::string`, they are required to be wrapped into a so-called **generator**. This is because coroutines are small abstractions of an **FSM**, a model that describes a systems behavior based on a finite number of states and transitions between them. An FSM can only be in one state at a time and its behavior is determined by the current state and any input it receives.

A generator is a user-defined type that wraps and controls the coroutine. For the coroutine to function properly, its generator (implemented as a class or struct) must conform to an API contract, which gives us the freedom and choice on how we would like to model our coroutine.

```cpp
template<typename T>
struct Generator {
    // A: The promise type
    using promise_type = promise_type_base<T, Generator>;

    // B1: The coroutine handle
    using PromiseTypeHandle = std::coroutine_handle<promise_type>;

    // C: The coroutine constructor and destructor
    explicit Generator(promise_type* p) : mCoroHdl{PromiseTypeHandle::from_promise(*p)} {}
    ~Generator() { if(mCoroHdl) mCoroHdle.destroy(); }

    // B2: The coroutine handle
    PromiseTypeHandle mCoroHdl;
};
```

#### A: The promise type

The `promise_type` is a core component of the coroutine. It defines how the coroutine behaves — including how values are yielded, how the coroutine starts and ends, and how exceptions are handled. Although its only referred to it here via a `using` declaration, the compiler generates and interacts with an instance of this type inside the coroutine frame.

#### B: The coroutine Handle

`mCoroHdl` (shown in B2) is the coroutine handle. It acts as our interface to the coroutine’s state machine, allowing us to control execution by resuming or destroying the coroutine. Its type, aliased in B1 as `PromiseTypeHandle`, can be viewed as a wrapper around a pointer which points to the *coroutine frame* — a memory structure generated by the compiler that stores local variables, control state, and a promise_type instance.

When a coroutine is first invoked the compiler implicitly allocates memory on the **heap** for this coroutine frame using `operator new`. This frame is where all coroutine-related state is held. The coroutine handle (`mCoroHdl`) is our only way to access or interact with this frame. We use it to resume execution (`resume()`), check completion (`done()`), or clean up (`destroy()`). Without it, the coroutine would be unreachable.

#### C: The coroutine constructor and destructor

The constructor and destructor of the `Generator` type are responsible for establishing a properly cleaning up the coroutine's execution context. In the constructor, `promise_type` pointer is converted into a`std::coroutine_handle` using `from_promise(*p)` which effectively binds the coroutine generator object to the coroutine frame, allowing it to control the coroutine's execution (resume, destroy, etc.).

The destructor ensures that if the coroutine handle is valid, the associated coroutine frame is destroyed using `destroy()`. This is a critical step: coroutine frames are **heap-allocated**, and failing to destroy them will lead to a memory leak.

### The Promise Type

The `promise_type` object in the `Generator` struct is a hook for the compiler. When it appears inside of the class, it is checked to determine whether the type satisfies the promise-type iterface.

```cpp
template<typename T, typename G>
struct promise_type_base {
    // A: The yielded value
    T mValue;

    // B: Yield functionality
    auto yield_value(T value) {
        mValue = std::move(value);
        return std::suspend_always{};
    }

    // C: Returning the generator object 
    G get_return_object() { return G{*this}; }

    // D: Coroutine lifecycle hooks
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }
    static auto get_return_object_on_allocation_failure() { return G{nullptr}; }
};
```

#### A: The yielded value

`T mValue` is a member variable used to temporarily store a value that is passed from the coroutine to the caller when `co_yield` is used. Each time the coroutine yields, the value is placed here so that the caller (typically through an iterator) can access it. This variable acts as a bridge between the internal coroutine execution and the external consumer of the generator.

#### B: Yield Functionality

The function `yield_value(T value)` is called automatically by the compiler every time the coroutine uses `co_yield`. Its role is to accept and store the yielded value in `mValue` and then return a suspension point — in this case, `std::suspend_always{}` — which tells the compiler to **suspend** the coroutine after yielding. This gives control back to the caller until they explicitly resume the coroutine. This method is part of the standard coroutine customization points required for all `promise_type`'s that support yielding.

#### C: Returning the Generator Object

The function `get_return_object()` is invoked once during coroutine initialization. It constructs and returns the object that the coroutine appears to return (e.g., `Generator<T>`), even though the actual work is done via suspension and resumption. It typically takes the current `promise_type` instance (`*this`) and wraps it inside the generator type `G`, which manages the coroutine handle and acts as the user's interface to the coroutine.

#### D: Coroutine Lifecycle Hooks

These methods customize the suspension behavior at the start and end of the coroutine:

- `initial_suspend()` is called before the coroutine body begins execution. Returning `std::suspend_always` causes the coroutine to pause immediately after construction, requiring the caller to explicitly `resume()` it.

- `final_suspend()` is called when the coroutine reaches the end of its execution. Returning `std::suspend_always` ensures the coroutine remains suspended, giving the caller a chance to observe the final state before destruction.

- `return_void()` handles `co_return` statements that don’t produce a value. In this context, it's effectively a no-op.

- `unhandled_exception()` is called if an exception is thrown within the coroutine and not caught. The default here calls `std::terminate()`, but custom logic could be added.

- `get_return_object_on_allocation_failure()` is a fallback function invoked if memory allocation for the coroutine frame fails. It provides a way to gracefully return a default-constructed (null) generator object.