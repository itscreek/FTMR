# Find Tractable Multitree Recolorabilities

## Build

You can build FTMR library using CMake.

```
cmake -S . -B build
cmake --build build
```

## Finding tractable polytree recolorabilities

You can examines every polytree with `n` vertices to determine if it meets the conditions (S), (CP) and (CV).

```
./example/find_tractable_polytrees.sh n
```

## Test

Go to the `build/` directory and execute `ctest`.

```
cd build/ && ctest
```

You can also execute the test directly.

```
./build/test/unit_test
```
