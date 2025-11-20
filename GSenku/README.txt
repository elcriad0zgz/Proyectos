Este programa soluciona problemas de "GSenku" mediante bactracking.

El fichero Makefile crea el ejecutable mainGSenku.exe .  Este ejecutable se usa así:

./mainGSenku [string:ficherotablero.txt] [string:ficheromovimientos.txt] [int:retardo] [string:ficheroresultados.txt]

El fichero de resultados es tanto creable como "reescribible", y el retardo se mide en milisegundos. Si se eligen 0 milisegundos como retardo,
el programa no muestra el proceso que sigue. Los movimientos que el programa ha realizado para llegar a la solución final, se expresan:

x_inicial,y_inicial:x_final,y_final.

Si no existe solución para la combinación tablero-movimientos, el programa escribirá "-1" en el fichero de resultados.

IMPORTANTE: Para esta versión, se espera que todos los archivos de código, movimientos y tablero estén en el mismo directorio.