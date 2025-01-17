# Параллельный одномерный клеточный автомат.
С помощью MPI распараллельте одномерный клеточный автомат Вольфрама.  
Игра происходит следующим образом:
1) Инициализируйте одномерный массив 0 и 1 случайным образом  
2) В зависимости от значений: левого соседа, себя, правого соседа 
на следующем шаге клетка либо меняет значение, либо остается той же.  
Периодические и непериодические граничные условия.  
Работает параллельный код на нескольких процессах.  
Имплементированы клетки-призраки (ghost cells).  
Можно поменять правило игры (сделать одно из 256).  

# Использование
Клеточный автомат Вольфрама с конечной лентой.  
CellularAutomaton [-r <номер_правила>] [-i <количество_итераций>] [-l <размер_автомата>] [-b <0|1|2>] [-v <0|1>]  
Параметр -r - это номер правила для автомата в соответсвии с кодом Вольфрама.  
Параметр -i - количество пересчёта состояний автомата.  
Параметр -l - длина ленты автомата (размер состояния, количество клеток).  
Параметр -b - тип границы: 0 - границы заполняются нулями, 1 - границы заполняются единицами, 2 - границы замкнуты периодически.  
Параметр -v - нужно ли печатать состояния: 1 - нужно, 0 - не нужно.
Все параметры необязательные.  
Параметры по умолчанию:  
CellularAutomaton -r 110 -i 10 -l 20 -b 0 -v 1  

# Примеры работы
```bash
$ mpirun -np 4 ./CellularAutomaton 
0 1 1 0 0 0 1 1 0 1 0 0 0 0 1 0 1 1 0 1 
1 1 1 0 0 1 1 1 1 1 0 0 0 1 1 1 1 1 1 1 
1 0 1 0 0 1 0 0 0 1 0 0 1 1 0 0 0 0 0 1 
1 1 1 0 1 1 0 0 1 1 0 1 1 1 0 0 0 0 1 1 
1 0 1 1 1 1 0 1 1 1 1 1 0 1 0 0 0 1 1 1 
1 1 1 0 0 1 1 1 0 1 0 1 1 1 0 0 1 1 0 1 
1 0 1 0 1 1 0 1 1 1 1 1 0 1 0 1 1 1 1 1 
1 1 1 1 1 1 1 1 0 1 0 1 1 1 1 1 0 0 0 1 
1 0 0 0 0 0 0 1 1 1 1 1 0 0 0 1 0 0 1 1 
1 0 0 0 1 0 1 1 0 0 0 1 0 0 1 1 0 1 1 1 
1 0 0 1 1 1 1 1 0 0 1 1 0 1 1 1 1 1 0 1 
$ mpirun -np 4 ./CellularAutomaton -l 10
1 1 0 0 1 1 0 1 1 0 
1 0 0 1 1 1 1 1 1 0 
1 0 1 1 0 0 0 0 1 0 
1 0 1 1 0 0 0 1 1 0 
1 1 1 1 0 0 1 1 1 0 
1 0 0 1 0 1 1 0 1 0 
1 1 1 1 1 1 1 1 1 0 
1 1 0 0 0 0 0 0 1 0 
1 0 0 0 0 0 0 1 1 0 
1 0 0 0 0 0 1 1 1 0 
1 0 0 0 0 1 1 0 1 0 
$ mpirun -np 4 ./CellularAutomaton -l 10 -i 5
1 1 1 1 0 0 0 1 0 1 
1 0 0 1 0 0 1 1 1 1 
1 1 1 1 0 1 1 0 0 1 
1 1 0 1 1 1 1 0 1 1 
1 0 1 1 0 0 1 1 1 1 
1 0 1 0 0 1 1 0 0 1 
$ mpirun -np 4 ./CellularAutomaton -l 10 -i 5 -b 2
1 0 1 0 1 0 1 1 0 1 
1 1 1 0 1 1 1 1 1 1 
0 0 1 1 1 0 0 0 0 0 
0 1 1 0 1 1 0 0 0 0 
1 1 1 1 1 1 0 0 0 0 
1 0 0 0 0 1 0 0 0 1 
$ mpirun -np 4 ./CellularAutomaton -l 10 -i 5 -b 2 -r 101
1 0 1 1 0 1 0 0 1 0 
1 0 0 1 1 0 0 0 1 1 
1 0 0 0 1 0 1 0 0 0 
1 0 1 0 1 1 1 0 1 0 
1 0 1 1 0 0 1 1 1 1 
1 1 0 0 0 0 0 0 0 0
```

# Ускорение работы программы при увеличении количества процессов
На сервере с большим количеством процессоров код не завёлся, 
потому что на сервере старый gcc, переписывать код под старую версию стандарта не стал.  
Поэтому так:
```bash
$ time mpirun -np 1 ./CellularAutomaton -l 131072 -v 0

real	0m0,827s
user	0m0,811s
sys	0m0,016s
$ time mpirun -np 2 ./CellularAutomaton -l 131072 -v 0

real	0m0,453s
user	0m0,856s
sys	0m0,029s
```

# Картинка эволюции для одного правила
```bash
$ mpirun -np 4 ./CellularAutomaton
0 1 1 0 0 0 1 1 0 1 0 0 0 0 1 0 1 1 0 1 
1 1 1 0 0 1 1 1 1 1 0 0 0 1 1 1 1 1 1 1 
1 0 1 0 0 1 0 0 0 1 0 0 1 1 0 0 0 0 0 1 
1 1 1 0 1 1 0 0 1 1 0 1 1 1 0 0 0 0 1 1 
1 0 1 1 1 1 0 1 1 1 1 1 0 1 0 0 0 1 1 1 
1 1 1 0 0 1 1 1 0 1 0 1 1 1 0 0 1 1 0 1 
1 0 1 0 1 1 0 1 1 1 1 1 0 1 0 1 1 1 1 1 
1 1 1 1 1 1 1 1 0 1 0 1 1 1 1 1 0 0 0 1 
1 0 0 0 0 0 0 1 1 1 1 1 0 0 0 1 0 0 1 1 
1 0 0 0 1 0 1 1 0 0 0 1 0 0 1 1 0 1 1 1 
1 0 0 1 1 1 1 1 0 0 1 1 0 1 1 1 1 1 0 1 
```
![Evolution](./evolution.png)

