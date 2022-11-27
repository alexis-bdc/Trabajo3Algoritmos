/*
        Trabajo 3 Algoritmos Distribuidos
        Alumnos:    Alexis Bolados
                    Florencia Cespedes
        Profesor:   Ruben Carvajal Schiaffino
        Entrega:    4 Diciembre 2022

        Ejecucion: mpirun -np K ./t3.exe -p -o -m < data.txt
                K: numero de nodos | numero de procesos
                p: particion de matrices | como se reparten desde el archivo {H,V}
                o: operacion a realizar {S,M}
                m: modo de ejecucion | silent or verbose {S,V}

        Requerimientos:
                algoritmos distribuido en memora privada, para operar matrices mapeadas en nodos
                operaciones: [SUMA][], [MULTIPLICACION][]
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>