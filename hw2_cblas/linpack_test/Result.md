Подсказка для первого знакомства с linpack: 
https://losst.ru/stress-test-protsessora-v-linux  
Результаты запуска теста:  

```bash
$ ./xlinpack_xeon64 -i ./my_test
Sample data file lininput_xeon64.

Current date/time: Wed Sep 30 23:53:57 2020

CPU frequency:    3.399 GHz
Number of CPUs: 1
Number of cores: 4
Number of threads: 4

Parameters are set to:

Number of tests: 5
Number of equations to solve (problem size) : 1000  2000  5000  10000 20000
Leading dimension of array                  : 1000  2000  5008  10000 20000
Number of trials to run                     : 4     2     2     2     1    
Data alignment value (in Kbytes)            : 4     4     4     4     4    

Maximum memory requested that can be used=3200404096, at the size=20000

=================== Timing linear equation system solver ===================

Size   LDA    Align. Time(s)    GFlops   Residual     Residual(norm) Check
1000   1000   4      0.006      107.4186 1.022959e-12 3.033181e-02   pass
1000   1000   4      0.006      112.2690 1.022959e-12 3.033181e-02   pass
1000   1000   4      0.006      107.3281 1.022959e-12 3.033181e-02   pass
1000   1000   4      0.006      114.8105 1.022959e-12 3.033181e-02   pass
2000   2000   4      0.042      125.9426 5.619838e-12 4.375464e-02   pass
2000   2000   4      0.043      124.2378 5.619838e-12 4.375464e-02   pass
5000   5008   4      0.552      151.1814 2.548040e-11 3.392018e-02   pass
5000   5008   4      0.550      151.5237 2.548040e-11 3.392018e-02   pass
10000  10000  4      4.085      163.2364 1.054555e-10 3.553909e-02   pass
10000  10000  4      4.096      162.8015 1.054555e-10 3.553909e-02   pass
20000  20000  4      30.562     174.5360 3.807211e-10 3.257923e-02   pass

Performance Summary (GFlops)

Size   LDA    Align.  Average  Maximal
1000   1000   4       110.4565 114.8105
2000   2000   4       125.0902 125.9426
5000   5008   4       151.3526 151.5237
10000  10000  4       163.0190 163.2364
20000  20000  4       174.5360 174.5360

Residual checks PASSED

End of tests
```