# Continuous Integration and Continuous Delivery

<img src="https://innovationspace.ansys.com/knowledge/wp-content/uploads/sites/4/2024/02/scade-009-ci-cd-workflow.svg" alt="CI/CD Visual Example" style="width: 60%; display: block; margin: 20 auto;">

Continuous Integration / Continuous Delivery (CICD) is a software development process that *automates building, testing and deploying code changes*. It aims to improve software quality, speed up delivery and make the entire software development lifecycle more efficient and reliable.

## Continuous Integration (CI)

Developers frequently mergve their code changes into a central repository. This triggers automated builds and tests to ensure that new code changes don't break the application. Developers frequently merge their code changes into a shared main branch (e.g., `main` or `master`). Each commit triggers an **automated pipeline** that performs:

- **Build**: Compiles the code (important for statically typed languages like C++).
- **Unit Tests**: Verifies functionality using unit tests (e.g., with Google Test or Catch2).
- **Static Analysis / Linters**: Checks for style issues and potential bugs (e.g., using `clang-tidy`, `cppcheck`).
- **Code Coverage Reports**: Measures which parts of the code are exercised by tests (e.g., with `gcov`, `lcov`).

The goal of CI is to catch integration issues early and maintain a deployable state at all times.

> *For C++ projects, tools like **CMake** (for builds), **CTest**, and **Google Test** are commonly integrated into CI workflows.*

## Continuous Delivery/Deployment (CD)

After a successful build and testing phase in CI, CD automates the release of the application to a testing and then a production environment. **Continuous delivery** delivers the code changets to a testing or production environment.

Once code passes CI checks, Continuous Delivery automates the next stages:

- **Packaging**: Create build artifacts (e.g., executables, libraries, installers).
- **Staging Deployment**: Optionally deploy artifacts to a staging/testing environment.
- **Approval Gates**: In Continuous Delivery, manual approval is typically required before pushing to production.
- **Production Deployment**: In Continuous Deployment, code is *automatically* released to production after CI passes.

> *For C++ applications, delivery could mean packaging a shared library, CLI binary, or even containerizing an application for deployment.*

## What a Developer Should Understand About CI/CD

CI/CD is more than just automation; it acts as a **crucial safety net**, enabling faster shipping without compromising quality. It significantly **reduces manual effort and human error** in testing and deploying code, while also providing essential feedback loops to catch regressions early.

As a developer your responsibilities within a CI/CD workflow are critical. You must **write testable code** as the effectiveness of CI hinges on robust unit and integration tests. It is also your duty to keep builds green, quickly fixing any broken pipelines resulting from your pull requests or commits. In addition, you may also need to **write or update pipelines** to incorporate new tools such as linters or sanitizers. This means that a deep understanding of **how artifacts are built and deployed** is essential for debugging production issues. Finally, **securing secrets and credentials** is crucial; adhere to the best practices like storing them in secrets managers when your code uploads artifacts, runs deployment steps, or requires access tokens.

### Important Concepts for CI/CD

There are a few concepts developers need to be aware of with CI/CD to better understand the process:

- **Pipelines**: Visualize CI/CD as a series of steps: build → test → analyze → package → deploy.                         
- **Build Matrix**: You may need to test on multiple platforms (Windows/Linux/macOS) and compiler versions.                  
- **Caching**: Speeds up pipeline runs by avoiding redoing work (e.g., re-downloading dependencies).                    
- **Artifacts**: Outputs from a build: compiled binaries, test logs, reports, etc. Stored and used later in the pipeline. 
- **Stages vs Jobs**: Understand how to break your process into logical phases (e.g., test stage, deploy stage).               
- **Rollbacks**: Know how deployments can be rolled back safely if something breaks.                                      

## CI/CD Workflows for C++ 

C++ projects bring their own set of challenges to a CI/CD pipeline, largely because of how they compile and handle dependences. **Long build times** are a frequent issue since C++ compliation is inherently slow. To address this, you should look into techniques like **Precompiled Headers (PCH)**, using **ccache** for smart caching, and **splitting builds across cores** with `make -j`.

**Dependency management** can also be tricky as C++ lacks a universal package manager. You might find yourself using *vcpkg*, *conan* or *submodules*—it is crucial to make sure your CI environment sets up and manages these dependencies. Another potential problem is **binary compatibility**; if you distribute shared libraries, application binary interface (ABI) changes can inadvertently break production environments so your CI/CD should include compatibility checks to catch these issues early. Lastly, *platform specific bugs* are common—CI is invaluable for catching these early, so ensure youi have *runners for all supported operating systems* in your pipeline.

To make your CI/CD workflows for C++ projects as effective as possible, several strategies and tools can signficantly improve your workflow:

- **Using CMake**: Standardize your build process by using CMake across both yoiur local development and your CI environments. This ensures consistency and reproducibility.
- **Cross-Platform Builds**: Utilize yoiur CI system to *test your code on multiple platforms* (Linuz, Windows, macOS) by configuring appropriate runners. This helps identify platform-specific issues early on.
- **Compiler Matrix Testing**: Go a step further by running your builds with a *matrix of different compilers* (GCC, Clang, MSVC). This verifies compatibility and catches compiler-specific warnings or errors.
- **Essential Tools to Integrate**: Incorporate the following tools into your CI pipeline for comprehensive testing and analysis:
    - **Google Test / Catch2**: For robust unit testing.
    - **clang-format**: To enforce consistent code style.
    - **clang-tidy** and **cppcheck**: For static analysis to identify potential bugs and code smells.
    - **lcov + gcov** or **gcovr**: To generate and analyze test coverage reports, ensuring your tests adequately cover your codebase.

## Example CI/CD Workflow for C++ (Using GitHub Actions)

Below is a generic example of a CI/CD workflow for C++ that includes:
- CMake Usage.
- Cross-platform testing (Ubuntu, macOS, Windows).
- Compiler matrix testing (GCC, Clang, MSVC).
- Google Test based unit testing.
- Static analysis with `clang-tidy`.
- Code coverage with `lcov` on Linux.

```yaml
# .github/workflows/ci.yml
name: C++ CI/CD Pipeline

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build-and-test:
    name: Build and Test on ${{ matrix.os }} with ${{ matrix.compiler }}
    runs-on: ${{ matrix.os }}

    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        compiler: [gcc, clang]  # On Windows, 'gcc' maps to MSVC fallback

        exclude:
          - os: windows-latest
            compiler: clang  # Clang on Windows typically needs extra setup

    env:
      BUILD_DIR: build

    steps:
      - name: Checkout Code
        uses: actions/checkout@v3

      - name: Set up Compiler
        if: runner.os == 'Linux' || runner.os == 'macOS'
        run: |
          if [[ "${{ matrix.compiler }}" == "gcc" ]]; then
            sudo apt-get update
            sudo apt-get install -y build-essential cmake gcovr lcov
            export CC=gcc
            export CXX=g++
          elif [[ "${{ matrix.compiler }}" == "clang" ]]; then
            sudo apt-get update
            sudo apt-get install -y clang cmake gcovr lcov
            export CC=clang
            export CXX=clang++
          fi

      - name: Set up Compiler (Windows)
        if: runner.os == 'Windows'
        shell: pwsh
        run: |
          choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
          RefreshEnv.cmd

      - name: Configure Project with CMake
        run: cmake -S . -B ${{ env.BUILD_DIR }} -DCMAKE_BUILD_TYPE=Debug

      - name: Build
        run: cmake --build ${{ env.BUILD_DIR }} --config Debug

      - name: Run Unit Tests
        run: ctest --test-dir ${{ env.BUILD_DIR }} --output-on-failure

      - name: Run Static Analysis (clang-tidy)
        if: matrix.os == 'ubuntu-latest' && matrix.compiler == 'clang'
        run: |
          sudo apt-get install -y clang-tidy
          cmake -S . -B ${{ env.BUILD_DIR }} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
          run-clang-tidy -p ${{ env.BUILD_DIR }} -quiet || true

      - name: Run Code Coverage (Linux + GCC only)
        if: matrix.os == 'ubuntu-latest' && matrix.compiler == 'gcc'
        run: |
          sudo apt-get install -y lcov
          lcov --directory ${{ env.BUILD_DIR }} --capture --output-file coverage.info
          lcov --remove coverage.info '/usr/*' --output-file coverage.info
          lcov --list coverage.info

```