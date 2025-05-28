#include <coroutine>
#include <iostream>

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
    G get_return_object() { return G{this}; }

    // D: Coroutine lifecycle hooks
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }
};

template<typename T>
struct Generator {
    // A: The promise type
    using promise_type = promise_type_base<T, Generator>;

    // B1: The coroutine handle
    using PromiseTypeHandle = std::coroutine_handle<promise_type>;

    // C: The coroutine constructor and destructor
    explicit Generator(promise_type* p) : mCoroHdl{PromiseTypeHandle::from_promise(*p)} {}
    ~Generator() { if(mCoroHdl) mCoroHdl.destroy(); }

    // B2: The coroutine handle
    PromiseTypeHandle mCoroHdl;

    // Give the user access to mValue
    bool done() { return mCoroHdl.done(); }
    T next() {
        mCoroHdl.resume();
        return mCoroHdl.promise().mValue;
    }
};


Generator<int> counter(int start, int end) {
    while (start < end) {
        co_yield start;
        start++;
    }
}

int main() {
    auto g = counter(1, 5);
    while (!g.done()) {
        std::cout << g.next() << " ";
    }

    return 0;
}