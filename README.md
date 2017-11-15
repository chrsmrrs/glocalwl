
# Global Weisfeiler-Lehman Kernels ###
Source code for our IEEE ICDM 2017 paper " [Glocalized Weisfeiler-Lehman Graph Kernels: Global-Local Feature Maps of Graphs](https://arxiv.org/abs/1703.02379) ".

## Compile
Using `cmake` you can simply type `cmake cmake-build-debug`, otherwise (using `gcc`)

```Bash
$ g++ main.cpp src/*.h src/*.cpp -std=c++11 -o wlglobal -O2
```
In order to compile, you need a recent version of [Eigen 3](http://eigen.tuxfamily.org/index.php?title=Main_Page) installed on your system.

## Usage
You can select the kernel and parameters in `main.cpp`.

## More Data Sets
See [Benchmark Data Sets for Graph Kernels](http://graphkernels.cs.tu-dortmund.de) for more data sets.

## Terms and conditions
Please feel free to use our code. We only ask that you cite:

		@inproceedings{Morris+2017,
		    title={Glocalized Weisfeiler-Lehman Graph Kernels: Global-Local Feature Maps of Graphs},
		    author={Christopher Morris and Kristian Kersting and Petra Mutzel},
		    booktitle={IEEE International Conference on Data Mining (ICDM), 2017},
		    pages={},
		    year={2017}
		}



## Contact Information
If you have any questions, send an email to Christopher Morris (christopher.morris at udo.edu).
