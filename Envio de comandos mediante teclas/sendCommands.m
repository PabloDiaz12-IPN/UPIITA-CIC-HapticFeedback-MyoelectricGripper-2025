% % matlab_to_arduino_keypress.m
% % Script MATLAB para enviar por comunicación serial comandos para controlar la pinza robótica mediante las teclas A/S/D,
% % Se envía "0" si no hay tecla presionado
% % Presionar tecla P para parar.

% --- Configuración del puerto serie ---
port = "COM12";      % Ajusta al tuyo
%baud = 115200;
baud = 230400;

% Abrir puerto
s = serialport(port, baud);
configureTerminator(s, "LF");
flush(s);

% --- Variables globales accesibles desde los callbacks ---
global running currentCode
running = true;
currentCode = '3';

% --- Crear figura mínima para captura de teclado ---
hFig = figure('Name','Key Capture','NumberTitle','off', ...
    'KeyPressFcn',   @onKeyPress, ...
    'KeyReleaseFcn', @onKeyRelease);
set(hFig, 'Position', [100 100 1 1], 'MenuBar','none', 'ToolBar','none');

% --- Bucle principal ---
while running
    write(s, currentCode, "char");
    %disp(currentCode);
    if s.NumBytesAvailable > 0
        data = readline(s);
        disp(data); % Tal vez si se quita esta línea se puede acelerar
    end
    %pause(0.05);  % ritmo ~20 Hz
    pause(0.2);  % ritmo ~ 5 Hz
end

% --- Limpieza ---
clear s
if isvalid(hFig)
    close(hFig)
end
disp("Programa terminado. Puerto serie cerrado.");
clear, clc

% -------------------------------------------------------------------------
% Callbacks (tienen que ir al final del script)
% -------------------------------------------------------------------------
function onKeyPress(~, evt)
    global running currentCode
    switch evt.Key
        case 'a' %  
            currentCode = '1'; % Flexión
        case 's'
            currentCode = '2'; % Puño
        case 'd'
            currentCode = '0'; % Extensión
        case 'p'
            running = false;
    end
end

function onKeyRelease(~, evt)
    global currentCode
    if any(strcmp(evt.Key, {'a','s','d'}))
        currentCode = '3'; % REPOSO
    end
end


