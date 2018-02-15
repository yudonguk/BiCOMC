# BiCOMC

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](http://www.apache.org/licenses/LICENSE-2.0)
[![Travis CI](https://img.shields.io/travis/yudonguk/BiCOMC/develop.svg?label=Travis+CI)](https://travis-ci.org/yudonguk/BiCOMC)
[![AppVeyor](https://img.shields.io/appveyor/ci/yudonguk/BiCOMC/develop.svg?label=AppVeyor)](https://ci.appveyor.com/project/yudonguk/BiCOMC/branch/develop)
[![CircleCI](https://img.shields.io/circleci/token/7c945317234ebedaff5d0bf9b1521a8f7276bcfd/project/github/yudonguk/BiCOMC/develop.svg?label=CircleCI)](https://circleci.com/gh/yudonguk/BiCOMC/tree/develop)
[![Gitlab CI](https://git.kangwon.ac.kr/iic/bicomc/badges/develop/pipeline.svg)](https://git.kangwon.ac.kr/iic/bicomc/commits/develop)

Binary Compatibility Object Model for C++ (BiCOMC) is library for surporting binary compatibility to C++ classes.

## Requirements

* **C++ 03 or higher** support
* **\_\_VA\_ARGS\_\_** support
* **SFINAE** support

## How to link your project

### In general

Just add `include` to the include directory path.

### In CMake 2.8.8 or higher

1. Move to directory of BiCOMC

1. Install BiCOMC (just once)

    ```bash
    mkdir build && cd build
    cmake ..
    cmake --build . --target install
    ```

1. Link BiCOMC to your project

    ```cmake
    find_package(BiCOMC CONFIG REQUIRED)
    add_executable(YourTarget ...) # or add_library(...)
    target_link_libraries(YourTarget BiCOMC)
    ```

### In CMake all versions

1. Check the absolute path of `include`

1. Include the path to your project using `include_directories`

    ```cmake
    include_directories("path/of/bicomc/include")
    add_executable(YourTarget ...) # or add_library(...)
    ```

## How to use

### To define interface

```cpp
// iprinter.h
#pragma once
#include <bicomc/object.h>
BICOMC_INTERFACE(IPrinter)
	BICOMC_DECL_METHOD(print, void(char const* message), 1)
BICOMC_INTERFACE_END(IPrinter)
```

### To implement interface

```cpp
// printer.h
#pragma once
#include <iostream>
#include "iprinter.h"
class Printer : public IPrinter {
	BICOMC_OVERRIDE(IPrinter)
		BICOMC_OVER_METHOD(print, void(char const*))
	BICOMC_OVERRIDE_END(IPrinter)
public:
	Printer() : BICOMC_OVERRIDE_INIT() {}
	void print(char const* message) {
		std::cout << message;
	}
};
```

### To instantiate, cast, and call

```cpp
// main.cpp
#include "printer.h"
int main() {
	bcc::Object* p = new Printer();
	IPrinter& printer = bicomc_cast<IPrinter&>(*p);
	printer.print("Hellow world!!");
	p->destroy(); // as a 'delete p'
	return 0;
}
```

## License

BiCOMC is distributed under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0) and it uses open source softwares shown in the [NOTICE](NOTICE.md) file.
