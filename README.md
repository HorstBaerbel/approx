# C++ testbed for (transcendental) function approximations

Tests some transcendental function approximations, e.g. for sqrt() for speed and precision. Note that **these are approximations only with varying degrees of precision and speed.** I consciously left out tricks like using assembler code or SSE / NEON. These should be obvious and can be used in conjuction with some of the the methods here. Also a good compiler makes those optimizations for you anyway. Note that you should **compile with optimizations on** (```-O2 -ffast-math```), otherwise your results will be off!

## License

All my own work is under the [MIT License](LICENSE), but I am standing on the shoulders of giants:

* [cxxopts](https://github.com/jarro2783/cxxopts) ([MIT License](./cxxopts/LICENSE))
* [sciplot](https://github.com/sciplot/sciplot) ([MIT License](./sciplot/LICENSE))
* There's no licenses for most of the approximation functions, but I've tried at least attributing them or stating the source in the comments.

## Building

### Prequisites

* Clone repo with all submodules using ```git clone --recursive https://github.com/HorstBaerbel/approx``` or update all submodules after cloning using ```git submodule init && git submodule update```.
* [GNUplot](http://gnuplot.sourceforge.net) installed if you want to plot results.

### From the command line

Navigate to the approx folder, then:

```sh
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### From Visual Studio Code

* **Must**: Install the "C/C++ extension" by Microsoft.
* **Recommended**: If you want intellisense functionality install the "C++ intellisense" extension by austin.
* **Must**: Install the "CMake Tools" extension by Microsoft.
* You might need to restart / reload Visual Studio Code if you have installed extensions.
* Open the approx using "Open folder...".
* Choose a kit of your choice as your active CMake kit if asked, or let CMake decide on its own.
* You should be able to build now using F7 and build + run using F5.

## Usage

Run ```approx``` with the following options:

* ```-h``` or ```--help```: Show help.
* ```-f FUNC``` or ```--function FUNC```: Test function, where FUNC can be:
  * ```log10f```: Test approximations for the float log10 function.
  * ```invsqrtf```: Test approximations for the float 1 / square root function.
  * ```sqrtf```: Test approximations for the float square root function.
  * All other approximations are currently WIP...
* ```-p``` or ```--plot```: Plot results to PDF files using [GNUplot](http://gnuplot.sourceforge.net).

The result is a listing of every function tested, its min/max absolute and relative errors, the standard deviation and the execution time per call, e.g.

```
Input range: (1.17549e-38, 65535), 1000000 samples in range
Tested functions:

log10f #0 - Standard library log10f
Absolute error: (3.89436e-13, 4.00927e-07)
Relative error: (9.97665e-14, 1.20875e-07)
Standard deviation: 1.30251e-07
Execution time: 6.11383 ns / call

...

log10f #4 - David Goldberg mul
Absolute error: (4.16929e-10, 0.00154069)
Relative error: (9.79513e-11, 0.00880727)
Standard deviation: 0.000579952
Execution time: 1.88797 ns / call
```

## Todo

* Check error calculations.
* Make timing more reliable / reproduceable.
* Make command line parameters work.
* Add more functions (isqrt(x), 1/x, sin / cos / tan / etc.).
* Improve error and timing statistics plots.
* Write output HTML or PDF with multiple results + plots.
