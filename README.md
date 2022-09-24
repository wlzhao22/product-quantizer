# Product Quantizer
This project is a C++ implementation of Product quantizer for nearest neighbor search.


### Compile and Install
This project can be compiled with GCC 3.8 or later under Linux and MacOS. Although no test has been made on Windows, 
it should be compiled smoothly on Windows by MinGW with a few modifications. 

```
cd product-quantizer/
make release
cp bin/Release/pq $HOME/bin
```


### Performance Evaluation on SIFT1M by PQ
#### k'=8192, dataset=1M SIFT, site for data: http://corpus-texmex.irisa.fr/
--------------------------------------------
|Top   |  1    |   10   |   50 | 100  |  w |
-------|-------|--------|------|------|----|
|asymm |  28.6  |  66.6 | 86.7 | 91.2 | 16 |
|asymm | 22.0   | 40.0  | 42.3 | 42.3 | 1  |
|symm  |  16.7  | 35.4  | 41.6 | 42.3 |	1  |
--------------------------------------------------------
#### k'=1024, dataset=1M SIFT
-------|------|-------|-------|-------|---|
|asymm | 28.3 |  68.5 |  88.6 | 92.8  | 16|
|asymm | 21.1 |  42.5 |  47.7 | 48.2  | 1 |
|symm  | 13.9 |  33.9 |  44.5 | 47.8	| 1 |
----------------------------------------------------------


### How to use PQ

The sample configuration files are available under 'etc/' subfolder. A PDF manual is found under 'manual/' subfolder.

### Copyrights and License
This project is fully supported by Odd Concepts Inc. from Seoul, South Korea during 2014∼2015. The Source Code may be used for academic and research purposes only, and any commercial use is strictly prohibited without the permission of Odd Concepts Inc.

### Author
Wan-Lei Zhao
