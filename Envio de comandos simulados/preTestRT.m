% preTestRT - Gráfica no dinámica de un vector de comandos 
%
% Este script grafica un vector de datos simbólicos,
% como los producidos por `generateFakeData`, donde cada carácter ('0', '1', '2', '3') se
% muestra con un color y una altura específicos para facilitar la interpretación visual.
%
% Se utiliza una codificación por colores y alturas para representar distintos tipos de comandos
% o etiquetas en el tiempo, lo que permite observar patrones o secuencias en los datos simulados.
%
% Características:
%   - Cada carácter se representa con una línea vertical coloreada.
%   - La altura de la línea varía según el valor del carácter.
%   - Incluye leyenda personalizada y fondo estilizado para mejorar la legibilidad.
%
% Requisitos:
%   - El vector `vector` debe estar previamente cargado en el workspace, preferentemente
%     generado con el script `generateFakeData.m`.
%
% Uso:
%   1. Ejecutar `generateFakeData` para crear el vector.
%   2. Ejecutar este script para visualizar los datos.
%
% Resultado:
%   Una figura con líneas verticales codificadas por color y altura, que representan
%   secuencias de comandos o clases a lo largo del eje temporal (índice del vector).



% --- GRÁFICA ESTÁTICA DE LOS DATOS ---
generateFakeData;

% Definición de alturas de cada caracter (para mejorar visualización)
heights = containers.Map;
heights('0') = 0.1;  % Altura para el comando '0'
heights('1') = 1;    % Altura para el comando '1'
heights('2') = 1;    % Altura para el comando '2'
heights('3') = 1;    % Altura para el comando '3'

% Crear figura para la gráfica
figure;
hold on;

% Cambiar el color de fondo de la gráfica (para ejes y figura)
set(gca, 'Color', '#DEE1E3','GridColor',[0.7,0.7,0.7]);

% Crear "handles" para la leyenda utilizando círculos dummy (invisibles)
h0 = plot(nan, nan, 'o', 'MarkerEdgeColor', '#424861', 'MarkerFaceColor', '#424861', 'MarkerSize', 10);
h1 = plot(nan, nan, 'o', 'MarkerEdgeColor', '#C9F2C7', 'MarkerFaceColor', '#C9F2C7', 'MarkerSize', 10);
h2 = plot(nan, nan, 'o', 'MarkerEdgeColor', '#A799B7', 'MarkerFaceColor', '#A799B7', 'MarkerSize', 10);
h3 = plot(nan, nan, 'o', 'MarkerEdgeColor', '#3C6997', 'MarkerFaceColor', '#3C6997', 'MarkerSize', 10);
h4 = plot(nan, nan, 'o', 'MarkerEdgeColor', '#000000', 'MarkerFaceColor', '#000000', 'MarkerSize', 10);

% Recorrer cada posición del vector para graficar una línea vertical con el color según el caracter
for i = 1:length(vector)
    switch vector(i)
        case '0'
            color = '#424861';  
        case '1'
            color = '#C9F2C7';  
        case '2'
            color = '#A799B7';  
        case '3'
            color = '#3C6997';  
        otherwise
            color = '#000000';  % Negro para cualquier otro caracter
    end
    
    if heights.isKey(vector(i))
        h = heights(vector(i));
    else
        h = 1;
    end
    
    line([i i], [0 h], 'Color', color, 'LineWidth', 2);
end

% Ajustar el rango del eje Y para mejorar la visualización
maxHeight = max(cell2mat(values(heights)));
ylim([0, maxHeight * 1.4]);

xlabel('Muestras');
ylabel('Relevancia/Etiqueta');
title('SIMULACIÓN DE COMANDOS');
% set(gcf,'WindowState','Maximized'); 
grid on;


yl = ylim;  % Obtener límites actuales del eje Y
hLine = line([1 1], yl, 'Color', 'k', 'LineWidth', 1);
