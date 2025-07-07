Archivos para mover la pinza robótica mediante un conjunto de comandos aleatorios.


Secuencia de ejecución:
1- generateFakeData.m
Genera el vector de comandos aleatorios.


2- preTestRT.m
Invoca a generateFakeData y grafica el vector de comandos.


3- testRT.m
Ejecutar únicamente después de haber ejecutado a preTestRT.m y cuando se tenga conectada la ESP32 de la pinza robótica. De otra forma, el programa mandará a error.


*El archivo testSpyderConnection.py realiza la tarea de los tres archivos anteriores, pero desde Python. Igualmente, se tiene que conectar la ESP32 conectada cuando se ejecuté.