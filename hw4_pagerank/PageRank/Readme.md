# Рассчёт PageRank
Генерирует случайный граф связей между страницами и рассчитывает PageRank разными способами.  
Использование:  
PageRank [-n <количество_узлов>] [-p <вероятность_ребра>] [-v]  
Параметр -n - это количество узлов в графе.  
Параметр -p - это вероятность того, что между двумя узлами появляется ребро.  
Параметр -v - если нужно напечатать матрицы на экран.  
Все параметры необязательные.  
Параметры по умолчанию:  
PageRank -n 8 -p 0.3  
# Примеры и результаты
```bash
$ ./PageRank -v
   Устанавливаем количество потоков для omp и openblas.
   Количество используемых ядер: 4
   
   Везде далее коэффициент демпфирования равен 0.85.
   Исходный граф ссылок: 
   
   0 0 0 0 0 0 0 0 
   1 1 0 0 0 0 1 1 
   0 1 0 1 1 0 0 0 
   0 0 0 1 1 1 1 0 
   0 0 1 0 0 0 0 0 
   0 0 1 0 0 1 0 0 
   0 0 1 0 0 0 0 0 
   1 0 0 0 1 0 1 1 
   
   Численное решение СЛАУ(метод простых итераций).
   Подготовка матрицы для метода простых итераций заняла 60.01 ms.
   Матрица для метода простых итераций: 
   
   0 0 0 0 0 0 0 0 
   0.5 0 0 0 0 0 0.333333 1 
   0 1 0 1 0.333333 0 0 0 
   0 0 0 0 0.333333 1 0.333333 0 
   0 0 0.333333 0 0 0 0 0 
   0 0 0.333333 0 0 0 0 0 
   0 0 0.333333 0 0 0 0 0 
   0.5 0 0 0 0.333333 0 0.333333 0 
   
   Решение методом простых итераций для упрощённого pr заняло 401.21 ms.
   Результат метода простых итераций для упрощённого pr: 
   
   0 
   0.107141 
   0.321429 
   0.178566 
   0.107146 
   0.107146 
   0.107146 
   0.0714265 
   
   Решение методом простых итераций для демпингованного pr заняло 246 us.
   Результат метода простых итераций для демпингованного pr: 
   
   0.01875 
   0.127969 
   0.296181 
   0.164192 
   0.102671 
   0.102671 
   0.102671 
   0.0848957 
   
   Строгое решение СЛАУ.
   Подготовка матрицы для строгого решения СЛАУ упрощённого pr заняла 11 us.
   Матрица коэффициентов линейной системы для упрощённого PR: 
         -1        0        0        0        0        0        0        0
        0.5       -1        0        0        0        0 0.333333        1
          0        1       -1        1 0.333333        0        0        0
          0        0        0       -1 0.333333        1 0.333333        0
          0        0 0.333333        0       -1        0        0        0
          0        0 0.333333        0        0       -1        0        0
          0        0 0.333333        0        0        0       -1        0
        0.5        0        0        0 0.333333        0 0.333333       -1
   
   Решение СЛАУ для упрощённого pr с помощью eigen заняло 12 us.
   Результат решения СЛАУ eigen для упрощённого pr: 
    0
    0
    0
    0
    0
    0
    0
   -0
   
   Видим, что решение вырожденное, это недостаток упрощённого метода рассчёта pr. 
   В демпингованном pr для обхода этой проблемы даже страницы, на которые никто не ссылается, имеют ненулевой pr. 
   Подготовка матрицы для строгого решения СЛАУ демпингованного pr заняла 31 us.
   Матрица коэффициентов линейной системы для демпингованного PR: 
         -1        0        0        0        0        0        0        0
      0.425       -1        0        0        0        0 0.283333     0.85
          0     0.85       -1     0.85 0.283333        0        0        0
          0        0        0       -1 0.283333     0.85 0.283333        0
          0        0 0.283333        0       -1        0        0        0
          0        0 0.283333        0        0       -1        0        0
          0        0 0.283333        0        0        0       -1        0
      0.425        0        0        0 0.283333        0 0.283333       -1
   
   Решение СЛАУ для демпингованного pr с помощью eigen заняло 13 us.
   Результат решения СЛАУ eigen для демпингованного pr: 
     0.01875
    0.127971
    0.296181
    0.164196
    0.102668
    0.102668
    0.102668
   0.0848973
   
   Решение СЛАУ для демпингованного pr с помощью gmm++ заняло 2 us.
   Результат решения СЛАУ gmm для демпингованного pr: 
   0.01875
   0.127971
   0.296181
   0.164196
   0.102668
   0.102668
   0.102668
   0.0848973
   
   Все результаты получены при работе с плотными матрицами. 
   Как выяснили в задаче BinPower, работа с разреженными матрицами становится выгодной только на размерах матриц, которые проблематично умножать на домашнем ПК.
   Поэтому результаты для разреженных матриц не приводим.
```
```bash
$ ./PageRank -n 512
   Устанавливаем количество потоков для omp и openblas.
   Количество используемых ядер: 4
   
   Везде далее коэффициент демпфирования равен 0.85.
   Численное решение СЛАУ(метод простых итераций).
   Подготовка матрицы для метода простых итераций заняла 15.02 ms.
   Решение методом простых итераций для упрощённого pr заняло 50.01 ms.
   Решение методом простых итераций для демпингованного pr заняло 1.99 ms.
   Строгое решение СЛАУ.
   Подготовка матрицы для строгого решения СЛАУ упрощённого pr заняла 4.02 ms.
   Решение СЛАУ для упрощённого pr с помощью eigen заняло 9.71 ms.
   Подготовка матрицы для строгого решения СЛАУ демпингованного pr заняла 4.22 ms.
   Решение СЛАУ для демпингованного pr с помощью eigen заняло 9.69 ms.
   Решение СЛАУ для демпингованного pr с помощью gmm++ заняло 9.67 ms.
   Все результаты получены при работе с плотными матрицами. 
   Как выяснили в задаче BinPower, работа с разреженными матрицами становится выгодной только на размерах матриц, которые проблематично умножать на домашнем ПК.
   Поэтому результаты для разреженных матриц не приводим.
```