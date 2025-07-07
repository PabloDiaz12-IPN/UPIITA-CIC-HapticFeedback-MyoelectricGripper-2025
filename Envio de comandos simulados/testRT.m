% testRT - Envía datos carácter por carácter a través de un puerto serial
%
% Este script envía secuencialmente cada elemento de un vector de caracteres (como los
% generados por `generateFakeData`) a través de un puerto serial, por ejemplo a un ESP32,
% simulando una transmisión de comandos o señales codificadas.
%
% Durante la transmisión, se actualiza dinámicamente una línea en una figura para mostrar
% visualmente el progreso de la secuencia, y opcionalmente se muestran las respuestas del ESP32.
%
% Características:
%   - Comunicación serial con configuración personalizada (puerto y baudios).
%   - Transmisión carácter por carácter con control visual del progreso.
%   - Opción de lectura de respuestas del dispositivo (útil para depuración).
%
% Variables importantes:
%   vector    - Vector de caracteres a enviar (debe estar definido en el workspace).
%   COM12     - Puerto COM a utilizar (modificar según tu sistema).
%   baudRate  - Velocidad de transmisión serial (default: 115200).
%   hLine     - Objeto de línea gráfica que debe estar definido previamente para
%               mostrar el avance. El script no crea la figura, solo la actualiza.
%
% Recomendaciones:
%   - Asegúrate de que el vector y el objeto `hLine` existan antes de ejecutar el script.
%   - Si se desea mayor eficiencia, se puede evaluar enviar bloques de caracteres en lugar
%     de uno por uno, o deshabilitar temporalmente la lectura de respuestas.
%
% Uso típico:
%   1. Ejecutar `generateFakeData` y el script de visualización inicial.
%   2. Ejecutar este script para iniciar la transmisión hacia el ESP32.
%
% Nota:
%   Al finalizar, el puerto serial se cierra y se limpia el entorno.


pause(4); % Pausa para que la simulación de movimientos no comience inmediatamente

% Configuración del puerto serial
puerto = "COM12";  
baudRate = 115200;
s = serialport(puerto, baudRate);
configureTerminator(s, "LF");

% Espera para que el ESP32 se reinicie y se vacíe el buffer
pause(0.05);
flush(s);

% Enviar cada dato individualmente y actualizar la línea dinámica
for i = 1:length(vector)
    % Enviar el caracter actual
    writeline(s, vector(i)); % Podría enviarse los 3 de uno? Es más eficiente? (tiempo aquí y allá)

    % Actualizar la posición de la línea dinámica para indicar el progreso
    set(hLine, 'XData', [i i]);
    drawnow;  % Forzar la actualización inmediata de la figura
    
    % Leer respuesta del ESP32 (si la hay)
    if s.NumBytesAvailable > 0
        data = readline(s);
        disp("Respuesta del ESP32: " + data) % Tal vez si se quita esta línea se puede acelerar
    end
end

% Cerrar la conexión serial
clear s;
disp("Comunicación cerrada.");

pause(4);
clear, clc, close all;
