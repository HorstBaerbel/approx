# C++ testbed for (transcendental) function approximations

Tests some transcendental function approximations, e.g. for sqrt() for speed and precision. Note that **these are approximations only with varying degrees of precision and speed.** I consciously left out tricks like using assembler code or SSE / NEON. These should be obvious and can be used in conjuctions with some of the the methods here. Also a good compiler makes those optimizations for you anyway. Note that you should **compile with optimizations on**, otherwise your results will be off!

# License

All my own work is under the [MIT License](LICENSE), but I am standing on the shoulders of giants:  
* cxxopts ([MIT License](./cxxopts/LICENSE))

# How to build?

## Prequisites

* Clone repo with all submodules using ```git clone --recursive https://github.com/HorstBaerbel/approx``` or update all submodules after cloning using ```git submodule init && git submodule update```.

# From the command line

Navigate to the approx folder, then:

```sh
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## From Visual Studio Code

* **Must**: Install the "C/C++ extension" by Microsoft.
* **Recommended**: If you want intellisense functionality install the "C++ intellisense" extension by austin.
* **Must**: Install the "CMake Tools" extension by Microsoft.
* You might need to restart / reload Visual Studio Code if you have installed extensions.
* Open the approx using "Open folder...".
* Choose a kit of your choice as your active CMake kit if asked, or let CMake decide on its own.
* You should be able to build now using F7 and build + run using F5.

# Todo

* Check error calculations.
* Make timing more reliable / reproduceable.
* Make command line parameters work.
* Add more functions (isqrt(x) 1/sqrt(x), 1/x, sin / cos / tan / etc.).
* Plot error statistics to images (GNUplot?).
* Write output HTML with results + plots.
