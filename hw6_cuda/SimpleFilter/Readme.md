# Фильтрация изображения
Ищет средневзвешенное значение цвета для каждого пикселя по пикселям в его квадратной окрестности.

## Пример использования
```bash
./SimpleFilter input.bmp output.bmp filter_size pattern.txt
1 0 0 0 1 
0 0 0 0 0 
0 0 0 0 0 
0 0 0 0 0 
1 0 0 0 1 
pattern_sum == 4
```
Для изображения:  
![Исходное изображение](image_24b.bmp "Исходное изображение")  

Размытие:  
```
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
```
![Размытие](filtered_24b_p1.bmp "Размытие")  

Рябь:  
```
1 0 0 0 1
0 0 0 0 0
0 0 0 0 0
0 0 0 0 0
1 0 0 0 1
```
![Рябь](filtered_24b_p2.bmp "Рябь")  