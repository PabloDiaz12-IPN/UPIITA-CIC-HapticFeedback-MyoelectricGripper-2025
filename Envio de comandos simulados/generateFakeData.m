% generateFakeData - Genera un vector de comandos aleatorios.
%
% Este script crea un vector de tamaño fijo (N = 800) compuesto por los caracteres '0', '1', '2' y '3',
% donde se insertan segmentos definidos con un caracter predominante según una probabilidad dada.
% Cada segmento se distribuye aleatoriamente a lo largo del vector, sin superposición con otros.
%
% Los parámetros como el porcentaje total ocupado por cada tipo de caracter, la cantidad de segmentos
% por tipo, y la probabilidad de aparición del caracter predominante pueden ajustarse en el bloque
% de configuración.
%
% El vector resultante puede utilizarse para entrenar o probar redes neuronales en tareas de detección,
% clasificación o segmentación de patrones en secuencias.
%
% Variables principales:
%   N             - Tamaño total del vector.
%   vector        - Vector resultante con caracteres y segmentos.
%   p_dominante   - Probabilidad de que una posición del segmento contenga el caracter predominante.
%
% Ejecución:
%   Simplemente corre el script en la consola o editor de MATLAB.





% Tamaño total de muestras del vector
N = 800;

% Inicializar el vector lleno de '0'
vector = repmat('0', 1, N);

% Array para controlar qué posiciones ya tienen un segmento asignado (evitar superposición)
ocupado = false(1, N);

% Definir parámetros de segmentos para cada caracter
% Cada estructura tiene:
%  - 'caracter': el caracter predominante
%  - 'porcentaje': porcentaje del vector total destinado a este caracter
%  - 'numSegmentos': cantidad de segmentos (cada uno de aproximadamente el mismo tamaño)
segmentos = {
    struct('caracter', '2', 'porcentaje', 15, 'numSegmentos', 4),  % 10% del vector, 3 segmentos 10 
    struct('caracter', '1', 'porcentaje', 15, 'numSegmentos', 4),  % 15% del vector, 4 segmentos 25 
    struct('caracter', '3', 'porcentaje', 15,  'numSegmentos', 4)   % 5% del vector, 2 segmentos 10
};

% Probabilidad de asignar el caracter predominante en cada posición del segmento
p_dominante = 0.80; % Relacionada a la eficiencia que alcance la red

% Recorrer cada definición de segmento
for i = 1:length(segmentos)
    segData = segmentos{i};
    
    % Calcular la longitud total destinada a este caracter (porcentaje del total)
    total_seg_length = round(N * (segData.porcentaje / 100));
    
    % Calcular la longitud aproximada de cada segmento
    seg_length = round(total_seg_length / segData.numSegmentos);
    
    % Para cada uno de los segmentos de este caracter
    for j = 1:segData.numSegmentos
        placed = false;
        max_attempts = 1000;
        attempts = 0;
        % Buscar una posición de inicio aleatoria que no interfiera con otros segmentos
        while ~placed && attempts < max_attempts
            attempts = attempts + 1;
            start_index = randi([1, N - seg_length + 1]);
            if ~any(ocupado(start_index:start_index+seg_length-1))
                % Asignar el segmento: en cada posición se pone el caracter predominante
                % con probabilidad p_dominante o se introduce ruido (otro caracter) con probabilidad 1 - p_dominante
                for idx = start_index:(start_index+seg_length-1)
                    if rand < p_dominante
                        vector(idx) = segData.caracter;
                    else
                        % Elegir aleatoriamente entre los otros caracteres (excluyendo el predominante)
                        otros = ['0', '1', '2', '3'];
                        otros(otros == segData.caracter) = [];
                        vector(idx) = otros(randi(length(otros)));
                    end
                end
                % Marcar estas posiciones como ocupadas
                ocupado(start_index:start_index+seg_length-1) = true;
                placed = true;
            end
        end
        if ~placed
            warning('No se pudo colocar un segmento para el caracter %s después de %d intentos.', segData.caracter, max_attempts);
        end
    end
end

% Mostrar una parte del vector para visualizar el resultado
clearvars -except vector



