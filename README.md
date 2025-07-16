**Envío de comandos mediante teclas**
- sendCommands.m :  
  Este script en MATLAB permite controlar la pinza robótica mediante el teclado de la computadora a través de una conexión serial con Arduino o ESP32.

**Envío de comandos simulados**
- generateFakeData.m:  
Genera el vector de comandos aleatorios.

- preTestRT.m :  
Invoca a generateFakeData y grafica el vector de comandos.

- testRT.m :  
Ejecutar únicamente después de haber ejecutado a preTestRT.m y cuando se tenga conectada la ESP32 de la pinza robótica. De otra forma, el programa mandará a error.


**El archivo testSpyderConnection.py realiza la tarea de los tres archivos anteriores, pero desde Python. Igualmente, se tiene que conectar la ESP32 cuando se ejecute.*

**Final_Code_Gripper**
- codigoFinal.ino :  
Este archivo contiene el código diseñado en Arduino IDE destinado al control del gripper basado en ESP32, incluyendo la adquisición de sensores, el control de servomotores y la transmisión de datos mediante ESP-NOW.

**showMAC**
- showMAC.ino:  
Este archivo contiene un código diseñado en Arduino IDE con la finalidad de obtener y mostrar en el monitor serial la dirección MAC del módulo ESP32.
