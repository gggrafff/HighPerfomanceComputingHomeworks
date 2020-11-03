# Оценка числа Пи методом Монте-Карло
Запуск программы:  
```bash
path_to_program point_count [verbose]
```
Примеры с возможным выводом:  
```bash
$ ./MonteCarlo 10000000
Pi is calculated by 10000000 points in 234.41 ms.
Pi approximately equal 3.14151
```
```bash
$ ./MonteCarlo 100 verbose
Num of CPU: 4
Num of OMP threads: 4
Thread №0 generated point (-0.242146;-0.566704). 
Thread №0 generated point (0.902049;-0.357222). 
Thread №0 generated point (0.223101;0.40425). 
Thread №0 generated point (0.559708;-0.233804). 
Thread №1 generated point (-0.574096;0.773374). 
Thread №0 generated point (0.153164;-0.0424839). 
Thread №0 generated point (0.351042;-0.348775). 
Thread №0 generated point (-0.229932;0.3788). 
Thread №0 generated point (-0.281738;0.0148205). 
Thread №0 generated point (-0.20806;0.164529). 
Thread №0 generated point (0.0623214;0.520394). 
Thread №0 generated point (-0.412772;0.250357). 
Thread №0 generated point (0.802841;-0.364278). 
Thread №2 generated point (-0.972964;0.359559). 
Thread №3 generated point (-0.542899;0.678091). 
Thread №2 generated point (0.245691;0.400704). 
Thread №2 generated point (0.60181;-0.874207). 
Thread №1 generated point (-0.711435;-0.03942). 
Thread №1 generated point (-0.103893;-0.446909). 
Thread №1 generated point (-0.603941;0.210214). 
Thread №1 generated point (-0.437767;-0.170592). 
Thread №1 generated point (0.619081;-0.841155). 
Thread №1 generated point (0.557848;-0.414784). 
Thread №1 generated point (-0.482379;-0.378647). 
Thread №1 generated point (-0.655928;0.711766). 
Thread №1 generated point (0.141475;-0.689202). 
Thread №1 generated point (-0.785529;-0.936876). 
Thread №1 generated point (0.582736;-0.507084). 
Thread №1 generated point (-0.219745;0.233673). 
Thread №1 generated point (-0.942833;-0.444574). 
Thread №1 generated point (-0.488348;-0.0611734). 
Thread №1 generated point (0.70911;0.926703). 
Thread №1 generated point (-0.493396;0.236462). 
Thread №1 generated point (0.671005;-0.888546). 
Thread №1 generated point (0.509626;-0.158962). 
Thread №1 generated point (0.0865388;-0.439709). 
Thread №1 generated point (-0.795161;0.487766). 
Thread №1 generated point (-0.513404;-0.72702). 
Thread №1 generated point (-0.403418;0.437027). 
Thread №1 generated point (0.765996;0.324042). 
Thread №1 generated point (0.609792;0.206525). 
Thread №0 generated point (0.206731;0.0570043). 
Thread №2 generated point (0.918467;-0.902705). 
Thread №0 generated point (0.485455;0.369929). 
Thread №2 generated point (0.81847;0.728445). 
Thread №0 generated point (0.571406;-0.216027). 
Thread №2 generated point (0.495154;0.459314). 
Thread №0 generated point (0.122763;-0.590277). 
Thread №2 generated point (0.776436;-0.597129). 
Thread №2 generated point (-0.127975;-0.0326395). 
Thread №2 generated point (-0.883544;0.459235). 
Thread №0 generated point (-0.223725;-0.786448). 
Thread №2 generated point (-0.838997;0.0148997). 
Thread №0 generated point (-0.489452;0.243324). 
Thread №2 generated point (0.559453;0.367566). 
Thread №0 generated point (0.0957527;-0.978044). 
Thread №2 generated point (0.163887;-0.101301). 
Thread №0 generated point (-0.368606;-0.581269). 
Thread №2 generated point (-0.248349;-0.43632). 
Thread №0 generated point (0.844152;-0.23192). 
Thread №2 generated point (-0.0346268;-0.939436). 
Thread №0 generated point (0.704619;0.155269). 
Thread №2 generated point (0.864969;0.193754). 
Thread №0 generated point (-0.333769;0.730655). 
Thread №2 generated point (-0.208324;-0.338454). 
Thread №0 generated point (-0.254575;0.08363). 
Thread №2 generated point (-0.479293;0.650924). 
Thread №0 generated point (-0.441595;-0.653558). 
Thread №2 generated point (0.508194;-0.245775). 
Thread №2 generated point (-0.391819;-0.664936). 
Thread №2 generated point (0.91144;-0.777471). 
Thread №2 generated point (0.69644;-0.372988). 
Thread №2 generated point (0.0950402;0.718198). 
Thread №2 generated point (0.446157;0.903832). 
Thread №2 generated point (-0.29664;0.621148). 
Thread №2 generated point (0.795135;0.602393). 
Thread №3 generated point (-0.522506;0.969253). 
Thread №3 generated point (0.186889;-0.21898). 
Thread №3 generated point (0.723087;-0.992403). 
Thread №3 generated point (0.246215;0.618714). 
Thread №3 generated point (0.268068;0.794824). 
Thread №3 generated point (-0.359179;-0.353524). 
Thread №3 generated point (0.59821;0.833278). 
Thread №3 generated point (-0.694953;0.43849). 
Thread №3 generated point (-0.762096;0.542984). 
Thread №3 generated point (-0.940526;-0.226384). 
Thread №3 generated point (0.131817;-0.369676). 
Thread №3 generated point (0.63567;0.00153422). 
Thread №3 generated point (-0.396549;0.644799). 
Thread №3 generated point (0.428484;-0.486634). 
Thread №3 generated point (0.408666;0.0793041). 
Thread №3 generated point (0.706312;-0.332102). 
Thread №3 generated point (0.645055;-0.642202). 
Thread №3 generated point (-0.603695;0.431242). 
Thread №3 generated point (-0.427272;0.379569). 
Thread №3 generated point (-0.305065;0.518189). 
Thread №3 generated point (0.872528;0.00317085). 
Thread №3 generated point (0.464759;-0.037297). 
Thread №3 generated point (-0.176949;-0.842935). 
Thread №3 generated point (-0.711636;0.768559). 
Pi is calculated by 100 points in 2.53 ms.
Pi approximately equal 3.4
```