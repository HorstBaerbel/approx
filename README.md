# C++ testbed for (transcendental) function approximations

Tests some transcendental function approximations, e.g. for sqrt() for speed and precision. Note that **these are approximations only with varying degrees of precision and speed and all have their own strengths and weaknesses or may have certain range requirements**. I consciously left out tricks like using assembler code or SSE / NEON. These should be obvious and can be used in conjuction with some of the the methods here. Also a good compiler makes some of those optimizations for you anyway. Note that you should **compile with optimizations on** (```-O2 -ffast-math```), otherwise your results will be off!

## License

All my own work is under the [MIT License](LICENSE), but I am standing on the shoulders of giants:

* [cxxopts](https://github.com/jarro2783/cxxopts) ([MIT License](./cxxopts/LICENSE))
* [sciplot](https://github.com/sciplot/sciplot) ([MIT License](./sciplot/LICENSE))
* There's no licenses for most of the approximation functions, but I've tried at least attributing them or stating the source in the comments.

## Building

### Prequisites

* Clone repo with all submodules using ```git clone --recursive https://github.com/HorstBaerbel/approx``` or update all submodules after cloning using ```git submodule init && git submodule update```.
* Use a C++17-capable compiler.
* [GNUplot](http://gnuplot.sourceforge.net) 4.2 or higher installed if you want to plot results.

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
Testing: 1 / sqrtf
Input range: (1.17549e-38, 2), 100000 samples in range
Tested functions:

#0 - std 1 / sqrtf
Absolute error: (0, 4.8513e-06), mean: 4.15788e-08, median: 2.86144e-08, variance: 6.7012e-10
Relative error: (0, 8.84594e-08), mean: 2.94772e-08, median: 2.62721e-08, variance: 1.27401e-10
Standard deviation: 8.18614e-08
Execution time: 8.57375 ns / call

...

#2 - Quake3 + Newton
Absolute error: (3.1869e-12, 4.01322e+13), mean: 4.01322e+08, median: 1.55585e-06, variance: 1.61059e+27
Relative error: (3.45024e-12, 4.72321e-06), mean: 1.78569e-06, median: 1.45295e-06, variance: 5.54854e-07
Standard deviation: 1.2691e+11
Execution time: 3.26045 ns / call
```

Also a result.pdf file will be saved to the current directory:  

![result.png](result.png)

## Todo

* Check error calculations.
* Make timing more reliable / reproduceable.
* Make command line parameters work.
* Add more functions (isqrt(x), 1/x, sin / cos / tan / etc.).
* Improve error and timing statistics plots.
* Write output HTML or PDF with multiple results + plots.
