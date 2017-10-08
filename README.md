# BiCOMC

Binary Compatibility Object Model for C++ (BiCOMC) is library for surporting binary compatibility to C++ classes.

## Requirement

* Support **C++ 03**
* Support **\_\_VA\_ARGS\_\_**
* Support **SFINAE**

## How to link your project

### In general

Just add `include` to include directory path.

### In CMake

1. Move to directory of BiCOMC

1. Install BiCOMC

    ```bash
    mkdir build && cd build
    cmake ..
    cmake --build . --target install
    ```

1. Link BiCOMC to your project
    ```cmake
    find_package(BiCOMC CONFIG REQUIRED)
    link_libraries(BiCOMC::BiCOMC)
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
	return 0;
}
```

## License

BiCOMC is distributed under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0) and it uses open source software shown in the [LICENSE](LICENSE) file.
