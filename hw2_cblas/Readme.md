# Описание программы

## MatrixLibrary
Реализовано умножение матриц тремя способами:  
* по определению;  
* с использованием cBLAS;  
* алгоритмом Винограда-Штрассена;  
Работа с матрицами изолирована в классе Matrix (создание, удаление, сложение, вычитание, выбор элемента и т.д.).

## TimeMeasurer
Инициализирует две случайные квадратные матрицы и умножает разными способами. Производит замер времени умножения.
Как пользоваться:  
```bash
$ path_to_program matrix_size experiment_count
```
Нулевой передаваемый параметр - путь к программе TimeMeasurer.  
Первый передаваемый параметр - размер матриц.  
Второй передаваемый параметр - количество экспериментов для усреднения результатов.  
Пример:  
```bash
$ ./TimeMeasurer 512 3
```
# Зависимости
Требуемые библиотеки:  
libopenblas-base - Optimized BLAS (linear algebra) library based on GotoBLAS2  
libopenblas-dev - Optimized BLAS (linear algebra) library based on GotoBLAS2  
libblas-test - Basic Linear Algebra Subroutines 3, testing programs  

Установка:
```bash
sudo apt-get install libopenblas-dev  
sudo apt-get install libblas-test  
```
# Замеры времени
## На домашнем ПК
### Процессор
```bash
$ lscpu
Архитектура:         x86_64
CPU op-mode(s):      32-bit, 64-bit
Порядок байт:        Little Endian
CPU(s):              4
On-line CPU(s) list: 0-3
Потоков на ядро:     1
Ядер на сокет:       4
Сокетов:             1
NUMA node(s):        1
ID прроизводителя:   GenuineIntel
Семейство ЦПУ:       6
Модель:              158
Имя модели:          Intel(R) Core(TM) i5-7400 CPU @ 3.00GHz
Степпинг:            9
CPU МГц:             800.037
CPU max MHz:         3500,0000
CPU min MHz:         800,0000
BogoMIPS:            6000.00
Виртуализация:       VT-x
L1d cache:           32K
L1i cache:           32K
L2 cache:            256K
L3 cache:            6144K
NUMA node0 CPU(s):   0-3
Флаги:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault invpcid_single pti ssbd ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d

```
### Операционная система
```bash
$ cat /etc/*-release
DISTRIB_ID=LinuxMint
DISTRIB_RELEASE=19.3
DISTRIB_CODENAME=tricia
DISTRIB_DESCRIPTION="Linux Mint 19.3 Tricia"
NAME="Linux Mint"
VERSION="19.3 (Tricia)"
ID=linuxmint
ID_LIKE=ubuntu
PRETTY_NAME="Linux Mint 19.3"
VERSION_ID="19.3"
HOME_URL="https://www.linuxmint.com/"
SUPPORT_URL="https://forums.ubuntu.com/"
BUG_REPORT_URL="http://linuxmint-troubleshooting-guide.readthedocs.io/en/latest/"
PRIVACY_POLICY_URL="https://www.linuxmint.com/"
VERSION_CODENAME=tricia
UBUNTU_CODENAME=bionic
```
### Результаты
#### Debug сборка (-g)
```bash
$ ./TimeMeasurer 512 3
3 launches were carried out.
The multiplication by definition average duration of two 512x512 square matrices is 1.73 s.
The multiplication with BLAS average duration of two 512x512 square matrices is 2.13 ms.
The multiplication with Strassen's algorithm average duration of two 512x512 square matrices is 1.33 s.
$ ./TimeMeasurer 1024 3
3 launches were carried out.
The multiplication by definition average duration of two 1024x1024 square matrices is 13.85 s.
The multiplication with BLAS average duration of two 1024x1024 square matrices is 11.82 ms.
The multiplication with Strassen's algorithm average duration of two 1024x1024 square matrices is 9.33 s.
$ ./TimeMeasurer 2048 3
3 launches were carried out.
The multiplication by definition average duration of two 2048x2048 square matrices is 1.84 min.
The multiplication with BLAS average duration of two 2048x2048 square matrices is 74.81 ms.
The multiplication with Strassen's algorithm average duration of two 2048x2048 square matrices is 1.09 min.
$ ./TimeMeasurer 4096 3
3 launches were carried out.
The multiplication by definition average duration of two 4096x4096 square matrices is 14.7 min.
The multiplication with BLAS average duration of two 4096x4096 square matrices is 534.94 ms.
The multiplication with Strassen's algorithm average duration of two 4096x4096 square matrices is 7.61 min.
```
#### Release сборка (-O3)
```bash
$ ./TimeMeasurer 512 3
3 launches were carried out.
The multiplication by definition average duration of two 512x512 square matrices is 396.90 ms.
The multiplication with BLAS average duration of two 512x512 square matrices is 1.03 ms.
The multiplication with Strassen's algorithm average duration of two 512x512 square matrices is 281.12 ms.
$ ./TimeMeasurer 1024 3
3 launches were carried out.
The multiplication by definition average duration of two 1024x1024 square matrices is 3.25 s.
The multiplication with BLAS average duration of two 1024x1024 square matrices is 7.74 ms.
The multiplication with Strassen's algorithm average duration of two 1024x1024 square matrices is 1.97 s.
$ ./TimeMeasurer 2048 3
3 launches were carried out.
The multiplication by definition average duration of two 2048x2048 square matrices is 25.84 s.
The multiplication with BLAS average duration of two 2048x2048 square matrices is 57.23 ms.
The multiplication with Strassen's algorithm average duration of two 2048x2048 square matrices is 13.78 s.
$ ./TimeMeasurer 4096 3
3 launches were carried out.
The multiplication by definition average duration of two 4096x4096 square matrices is 3.41 min.
The multiplication with BLAS average duration of two 4096x4096 square matrices is 456.19 ms.
The multiplication with Strassen's algorithm average duration of two 4096x4096 square matrices is 1.61 min.
```
## На виртуальной машине
### Процессор
```bash
$ lscpu
Архитектура:i686
CPU op-mode(s):        32-bit, 64-bit
Порядок байтов:Little Endian
CPU(s):                2
On-line CPU(s) list:   0,1
Потоков на ядро:1
Ядер на сокет:2
Сокет(ы):        1
Vendor ID:             GenuineIntel
Семейство CPU:6
Модель:          158
Model name:            Intel(R) Core(TM) i5-7400 CPU @ 3.00GHz
Stepping:              9
CPU МГц:            2999.998
BogoMIPS:              5999.99
Hypervisor vendor:     KVM
Тип виртуализации:full
L1d cache:             32K
L1i cache:             32K
L2 cache:              256K
L3 cache:              6144K
Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx rdtscp lm constant_tsc xtopology nonstop_tsc pni pclmulqdq ssse3 cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt aes xsave avx rdrand hypervisor lahf_lm abm 3dnowprefetch fsgsbase avx2 invpcid rdseed clflushopt
```
### Операционная система
```bash
$ cat /etc/*-release
DISTRIB_ID=Ubuntu
DISTRIB_RELEASE=16.04
DISTRIB_CODENAME=xenial
DISTRIB_DESCRIPTION="Ubuntu 16.04.1 LTS"
NAME="Ubuntu"
VERSION="16.04.1 LTS (Xenial Xerus)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 16.04.1 LTS"
VERSION_ID="16.04"
HOME_URL="http://www.ubuntu.com/"
SUPPORT_URL="http://help.ubuntu.com/"
BUG_REPORT_URL="http://bugs.launchpad.net/ubuntu/"
UBUNTU_CODENAME=xenial
```
### Результаты
#### Debug сборка (-g)
```bash
$ ./TimeMeasurer 512 3
3 launches were carried out.
The multiplication by definition average duration of two 512x512 square matrices is 2.22 s.
The multiplication with BLAS average duration of two 512x512 square matrices is 8.26 ms.
The multiplication with Strassen's algorithm average duration of two 512x512 square matrices is 1.63 s.
$ ./TimeMeasurer 1024 3
3 launches were carried out.
The multiplication by definition average duration of two 1024x1024 square matrices is 18.15 s.
The multiplication with BLAS average duration of two 1024x1024 square matrices is 57.44 ms.
The multiplication with Strassen's algorithm average duration of two 1024x1024 square matrices is 11.93 s.
$ ./TimeMeasurer 2048 3
3 launches were carried out.
The multiplication by definition average duration of two 2048x2048 square matrices is 2.4 min.
The multiplication with BLAS average duration of two 2048x2048 square matrices is 413.45 ms.
The multiplication with Strassen's algorithm average duration of two 2048x2048 square matrices is 1.35 min.
$ ./TimeMeasurer 4096 3
3 launches were carried out.
The multiplication by definition average duration of two 4096x4096 square matrices is 19.19 min.
The multiplication with BLAS average duration of two 4096x4096 square matrices is 3.13 s.
The multiplication with Strassen's algorithm average duration of two 4096x4096 square matrices is 9.44 min.
```
#### Release сборка (-O3)
```bash
$ ./TimeMeasurer 512 3
3 launches were carried out.
The multiplication by definition average duration of two 512x512 square matrices is 601.82 ms.
The multiplication with BLAS average duration of two 512x512 square matrices is 7.73 ms.
The multiplication with Strassen's algorithm average duration of two 512x512 square matrices is 427.6 ms.
$ ./TimeMeasurer 1024 3
3 launches were carried out.
The multiplication by definition average duration of two 1024x1024 square matrices is 4.85 s.
The multiplication with BLAS average duration of two 1024x1024 square matrices is 46.76 ms.
The multiplication with Strassen's algorithm average duration of two 1024x1024 square matrices is 2.97 s.
$ ./TimeMeasurer 2048 3
3 launches were carried out.
The multiplication by definition average duration of two 2048x2048 square matrices is 38.56 s.
The multiplication with BLAS average duration of two 2048x2048 square matrices is 373.8 ms.
The multiplication with Strassen's algorithm average duration of two 2048x2048 square matrices is 20.77 s.
$ ./TimeMeasurer 4096 3
3 launches were carried out.
The multiplication by definition average duration of two 4096x4096 square matrices is 5.16 min.
The multiplication with BLAS average duration of two 4096x4096 square matrices is 3.04 s.
The multiplication with Strassen's algorithm average duration of two 4096x4096 square matrices is 2.44 min.
```
## В docker-контейнере
Шпаргалка по запуску контейнеров для начинающих:  
https://losst.ru/ustanovka-docker-na-ubuntu-16-04  
https://losst.ru/zapusk-kontejnera-docker  
### Процессор
```bash
# lscpu
Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
CPU(s):              4
On-line CPU(s) list: 0-3
Thread(s) per core:  1
Core(s) per socket:  4
Socket(s):           1
NUMA node(s):        1
Vendor ID:           GenuineIntel
CPU family:          6
Model:               158
Model name:          Intel(R) Core(TM) i5-7400 CPU @ 3.00GHz
Stepping:            9
CPU MHz:             800.034
CPU max MHz:         3500.0000
CPU min MHz:         800.0000
BogoMIPS:            6000.00
Virtualization:      VT-x
L1d cache:           32K
L1i cache:           32K
L2 cache:            256K
L3 cache:            6144K
NUMA node0 CPU(s):   0-3
Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault invpcid_single pti ssbd ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d
```
### Операционная система
```bash
# cat /etc/*-release
DISTRIB_ID=Ubuntu
DISTRIB_RELEASE=18.04
DISTRIB_CODENAME=bionic
DISTRIB_DESCRIPTION="Ubuntu 18.04.5 LTS"
NAME="Ubuntu"
VERSION="18.04.5 LTS (Bionic Beaver)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 18.04.5 LTS"
VERSION_ID="18.04"
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
VERSION_CODENAME=bionic
UBUNTU_CODENAME=bionic
```
### Результаты
#### Debug сборка (-g)
```bash
# ./TimeMeasurer 512 3
3 launches were carried out.
The multiplication by definition average duration of two 512x512 square matrices is 1.94 s.
The multiplication with BLAS average duration of two 512x512 square matrices is 3.1 ms.
The multiplication with Strassen's algorithm average duration of two 512x512 square matrices is 1.53 s.
# ./TimeMeasurer 1024 3
3 launches were carried out.
The multiplication by definition average duration of two 1024x1024 square matrices is 15.89 s.
The multiplication with BLAS average duration of two 1024x1024 square matrices is 16.56 ms.
The multiplication with Strassen's algorithm average duration of two 1024x1024 square matrices is 11.19 s.
# ./TimeMeasurer 2048 3
3 launches were carried out.
The multiplication by definition average duration of two 2048x2048 square matrices is 2.07 min.
The multiplication with BLAS average duration of two 2048x2048 square matrices is 88.74 ms.
The multiplication with Strassen's algorithm average duration of two 2048x2048 square matrices is 1.27 min.
# ./TimeMeasurer 4096 3
3 launches were carried out.
The multiplication by definition average duration of two 4096x4096 square matrices is 16.48 min.
The multiplication with BLAS average duration of two 4096x4096 square matrices is 592.29 ms.
The multiplication with Strassen's algorithm average duration of two 4096x4096 square matrices is 8.95 min.
```
#### Release сборка (-O3)
```bash
# ./TimeMeasurer 512 3
3 launches were carried out.
The multiplication by definition average duration of two 512x512 square matrices is 679.64 ms.
The multiplication with BLAS average duration of two 512x512 square matrices is 1.03 ms.
The multiplication with Strassen's algorithm average duration of two 512x512 square matrices is 469.85 ms.
# ./TimeMeasurer 1024 3
3 launches were carried out.
The multiplication by definition average duration of two 1024x1024 square matrices is 5.54 s.
The multiplication with BLAS average duration of two 1024x1024 square matrices is 15.34 ms.
The multiplication with Strassen's algorithm average duration of two 1024x1024 square matrices is 3.4 s.
# ./TimeMeasurer 2048 3
3 launches were carried out.
The multiplication by definition average duration of two 2048x2048 square matrices is 44.04 s.
The multiplication with BLAS average duration of two 2048x2048 square matrices is 103.64 ms.
The multiplication with Strassen's algorithm average duration of two 2048x2048 square matrices is 23.09 s.
# ./TimeMeasurer 4096 3
3 launches were carried out.
The multiplication by definition average duration of two 4096x4096 square matrices is 5.97 min.
The multiplication with BLAS average duration of two 4096x4096 square matrices is 868.60 ms.
The multiplication with Strassen's algorithm average duration of two 4096x4096 square matrices is 2.74 min.
```