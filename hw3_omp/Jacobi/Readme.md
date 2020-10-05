# Метод Якоби
Реализовано решение СЛАУ методом Якоби в двух варинтах: 
в один поток и с распараллеливанием матричных операций.  
Первый аргумент программы - размер матрицы.  
Запускать так:  
```bash
$ path_to_program system_size
```
Пример:  
```bash
$ ./Jacobi 4096
Work in 1 thread without omp.
x is calculated in 2.01 s.
Work in 4 thread with omp and openblas threading.
x is calculated in 894.19 ms.
```