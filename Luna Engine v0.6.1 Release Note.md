# Luna Engine v0.6.1 Release Node

2021/5/11 JXMaster

The following changed are made in Luna Engine v0.6.1:

1. Switch the build system to CMake.
2. Rename all primitive types:
   1. `int8`, `int16`, `int32`, `int64` to `i8`, `i16`, `i32`, `i64`.
   2. `uint8`, `uint16`, `uint32`, `uint64` to `u8`, `u16`, `u32`, `u64`.
   3. `float32`, `float64` to `f32` and `f64`.
   4. `size_t`, `ptrdiff_t` to `usize`, `isize`.
3. Add `c8`, `c16` and `c32` primitive character types.
4. Replace the original `Base` module with `Runtime` module, which is taken from Luna Engine v0.8. The following changes are made due to this replacement:
   1. `IAllocator` and `IHeap` is discarded. `get_module_allocator` is discarded. Now all modules allocate memory from one global memory heap, which is accessed by `memalloc`, `memfree`, `memrealloc`, `memnew` and `memdelete` (`#include <Runtime/Memory.hpp>`).
   2. `IString`, `IStringBuffer`, `IBlob` and `IBuffer` are discarded. New non-reference types `String` and `Blob` are introduced to replace those interfaces. `strlib` submodule in `Core` module is discarded too.
   3. `result_t` is discarded, new `errcode_t` is introduced to replace `result_t`, along with a more scalable and robust error handling system.
   4. `IName` is discarded. The new non-reference type `Name` and new name system is introduced to replace `IName`.
   5. The original module system in `Core` is discarded and replaced by a new module system in `Runtime`. The new module system handles module initialization and closing order automatically, rather than requiring the user to initialize all modules manually in order when the application starts.
   6. The logging interface `ILogger` is discarded.
5. `IVariant` in `Core` module is discarded and replaced by the non-reference type `Variant`.
6. `IError` is `Core` module is discarded and replaced by the non-reference type `Error`.
7. All namespaces are renamed to Pascal case.
8. Remove `Color` aliasing for `Float4`, using `Color` as the namespace for build-in color options (for example `Color::blue`).
9. Add built-in support for HLSL shader compilation in `Studio` (requires CMake 3.20.1).

