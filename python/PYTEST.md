# pytest Python Testing Framework

<img src="figures/pytest.png" alt="pytest logo" style="width: 40%; display: block; margin: 10 auto">

**pytest** is a powerful and flexible testing *framework* for Python, widely used by developers to write simple unit tests as well as complex functional test suites. It supports a straightforward syntax that allows tests to be written as plain functions, making them easy to read and maintain. With pytest, there's no need to write classes or boilerplate code unless it's desired, allowing developers to focus on the logic they want to validate.

In addition to **basic assertions**, pytest offers advanced features such as **fixtures** for *setup and teardown* logic, **parameterized testing** for running the same test with multiple sets of data, and powerful plugins that extend its capabilities. The framework also integrates well with *continuous integration systems*, enabling automated test execution as part of the development pipelines. Detailed tracebacks and intuitive error messages help developers quickly identify and fix issues in their code.

## Basic Assertions

At the heart of any test written with pytest is the assertion statement. pytest builds on Python's native `assert` keyword, but enhances its output significantly when a test fails. Instead of a generic failure message, pytest analyzes the assertion expression and provides detailed information about what went wrong — down to the exact values being compared. This enables the ability to write concise, readable tests without needing to manually include print statements or custom error messaging.

```python
# test_sample.py
def inc(x):
    return x + 1

def test_answer():
    assert inc(3) == 5
```

Using `assert` in pytest is both intuitive and expressive. For example, if we want to check if our function `inc(x)` returns the correct result we can create a simple assertion to test for an expected value like so: `assert inc(3) == 5`. Running the test will produce an output that details the result of the testing session.

```python
$ pytest test_sample.py
=========================== test session starts ============================
platform linux -- Python 3.x.y, pytest-8.x.y, pluggy-1.x.y
rootdir: /home/sweet/project
collected 1 item

test_sample.py F                                                     [100%]

================================= FAILURES =================================
_______________________________ test_answer ________________________________

    def test_answer():
>       assert func(3) == 5
E       assert 4 == 5
E        +  where 4 = func(3)

test_sample.py:6: AssertionError
========================= short test summary info ==========================
FAILED test_sample.py::test_answer - assert 4 == 5
============================ 1 failed in 0.12s =============================
```

When developing multiple tests, you can opt to group them into a class. By using the conventions for Python test discovery, pytest will automatically detect the test class and perform the test actions. 

```python
class TestClass:
    value = 0

    def test_one(self):
        self.value = 1
        assert self.value == 1

    def test_two(self):
        assert self.value == 1
```

While this is a valuable technique to organize related tests, it is important to realize that **each test has a unique instance of the class**. In the code sample above, `test_two(self)` will fail because when run, `self.value` will be initialized to `0` as a new class instance has been created, entirely independent from `test_one(self)`'s instance.

## [Fixtures](https://docs.pytest.org/en/stable/how-to/fixtures.html)

pytest fixtures provide a robust way to **manage setup and teardown logic** that can be shared across multiple tests. A fixture is a function decorated with `@pytest.fixture` that can *return data or resources* needed by test functions (objects, database connections, network calls, etc.). When a test depends on a fixture, it lists the fixture name as an argument, and pytest automatically calls the fixture and injects its return value into the test. Fixtures can also *request other fixtures* (multiple at a time if needed) to simplify complex testing requirements.

```python
@pytest.fixture
def basic_json():
    data = {"key": "apple", "number": 1}
    yield data

def test_one(basic_json):
    assert basic_json["key"] == "apple"
    assert basic_json["number"] == 1

def test_two(basic_json):
    assert isinstance(basic_json, dict)
    assert "key" in basic_json
    assert "number" in basic_json

```

Fixtures come in handy when preparing environments, like creating temporary files, initializing database connections, or setting up complex object graphs. They also support **scoping**, so a fixture can be *reused* at the function, class, module, or session level — minimizing redundant operations and improving test performance. This is extremely useful to make sure tests are not being affected by one another. Fixtures can be composed together, allowing teams to build modular, reusable testing infrastructure that keeps tests clean and focused.

### [Setup and Teardown](https://pytest-with-eric.com/pytest-best-practices/pytest-setup-teardown/)

Setup and teardown refer to the preparation and cleanup activities that you may require to execute dependent tests. The **setup** allows you to create and configure the necessary resources and conditions for the test like class objects, database and network connections. When the test is started, you can be sure that the required resources are ready. **Teardown** helps you clean and reset the resources and configurations created with setup. It means you can gracefully terminate the changes in the environment that you made to run your test.

Understanding the importance of setup and teardown in unit testing provides an advantage to building robust code:

* **Isolation of Tests**: Setup and teardown create a clean and isolated environment for each test function to ensure that the state of one test does not interfere with another.

* **Resource Management**: No matter the resources your test requires (connections, files, memory), setup and teardown provide a structured way to allocate and release resources. This prevents *resource leaks and conflicts* between tests.

* **Teardown for Clean-Up**: The teardown method is useful for cleaning up resources like database transactions or temp files. This stops resource leaks that negatively impact the performance of our tests.

* **Test Order Independence**: Tests can be written in a way that is independent of the order of test execution. Properly managed setup and teardown resets the environment between tests, allowing them to be executed in any order.

#### [Tearing Down Using `yield` Keyword](https://docs.pytest.org/en/stable/how-to/fixtures.html#yield-fixtures-recommended)

When you use the `yield` keyword inside of a `@pytest.fixture`, there is no need to explicitly define any *function* for teardown. That is because yield fixtures `yield` instead of `return`. Any teardown code required for the fixture is placed *after* the `yield`.

```python
@pytest.fixture
def setup_data():
    data = 5
    yield data

    # fixture teardown happens below - happens after test finishes
```

Once pytest determines the linear order for the fixtures, it will run each one up until it `return`'s or `yield`'s and then move on to the next fixture in the list to do the same thing. Once the test has finished, pytest will go back down the list of fixtures, but in *reverse order*, taking each one that yielded, and running the code inside it that happened *after* the `yield`d statement.

#### [Tearing Down Using Finalizers](https://docs.pytest.org/en/stable/how-to/fixtures.html#adding-finalizers-directly)

The other option to teardown and cleanup resources is to add a finalizer function directly to the test's `request-context` object. To use it, we have to request the `request-context` object in the fixture we need to add teardown code for, and then pass a callable, containing that teardown code to its `addfinalizer` method.

```python
@pytest.fixture
def resource_setup(request):
    resource = "Database Connection"

    def teardown_resource():
        pass

    request.addfinalizer(teardown_resource)
    return resource

def test_resource_usage(resource_setup):
    assert resource_setup == "Database Connection"
```

Caution needs to be taken because pytest will run that finalizer once it's been added, even if that fixture raises an exception after adding the finalizer. Make sure you don't run the finalizer code when you don't need to — we olny add the finalizer when a fixture has done something that requires a teardown.

## [Parameterized Tests](https://docs.pytest.org/en/stable/how-to/parametrize.html)

Parameterized tests in pytest allow a signle test function to be run *multiple timees with different sets of input data*. This reduces duplication and ensures comprehensive coverage across a range of scenerios. Using the `@pytest.mark.parameterize` decorator, developers can specify multiple combinations of inputs and expected outputs that will be automatically fed into the test function. 

```python
@pytest.mark.parametrize("test_input,expected", [("3+5", 8), ("2+4", 6), ("6*9", 42)])
def test_eval(test_input, expected):
    assert eval(test_input) == expected
```

Here, the `@parameterize` decorator defines three different `(test_input,expected)` tuples so that the `test_eval` function will run three times (once for each input). Note that the parameter values are passed by object reference instead of a copy. For example, if you pass a list or a dict as a parameter value, and the test case code mutates it, the mutations will be reflected in subsequent test case calls.

This feature is especially useful when testing functions that need to handle a variety of edgve cases or input conditions. Instead of writing separate tests for each condition, a developer can define one test that handles them all in a loop-like structure. Parameterized tests also improve test readability to help identify patterns in failures when a particular input case causes unexpected behavior.