# -*- coding: utf-8 -*-
"""
Created on Mon Feb 24 17:18:17 2025

@author: Manuel
"""

import random
import matplotlib.pyplot as plt
import serial
import time

def generate_fake_data():
    """
    Genera un vector de caracteres de longitud N siguiendo la lógica de MATLAB.
    """
    N = 200
    # Inicializar el vector con '0'
    vector = ['0'] * N
    ocupado = [False] * N

    # Definir parámetros de segmentos para cada caracter
    segmentos = [
        {"caracter": "2", "porcentaje": 15, "numSegmentos": 3},
        {"caracter": "1", "porcentaje": 15, "numSegmentos": 4},
        {"caracter": "3", "porcentaje": 15, "numSegmentos": 2}
    ]
    p_dominante = 0.8

    for segData in segmentos:
        total_seg_length = round(N * (segData["porcentaje"] / 100))
        seg_length = round(total_seg_length / segData["numSegmentos"])
        for _ in range(segData["numSegmentos"]):
            placed = False
            max_attempts = 1000
            attempts = 0
            while not placed and attempts < max_attempts:
                attempts += 1
                # Los índices en Python inician en 0
                start_index = random.randint(0, N - seg_length)
                if not any(ocupado[start_index : start_index + seg_length]):
                    # Asignar el segmento: en cada posición se pone el caracter predominante
                    # con probabilidad p_dominante o se introduce ruido con probabilidad 1 - p_dominante
                    for idx in range(start_index, start_index + seg_length):
                        if random.random() < p_dominante:
                            vector[idx] = segData["caracter"]
                        else:
                            # Elegir aleatoriamente entre los otros caracteres (excluyendo el predominante)
                            otros = [c for c in ['0', '1', '2', '3'] if c != segData["caracter"]]
                            vector[idx] = random.choice(otros)
                    # Marcar estas posiciones como ocupadas
                    for idx in range(start_index, start_index + seg_length):
                        ocupado[idx] = True
                    placed = True
            if not placed:
                print(f"Warning: No se pudo colocar un segmento para el caracter {segData['caracter']} después de {max_attempts} intentos.")
    return vector

def show_movements(vector):
    """
    Grafica los datos estáticos: para cada posición se dibuja una línea vertical
    cuyo color depende del caracter.
    """
    # Definir alturas para cada caracter
    heights = {'0': 0.1, '1': 1, '2': 1, '3': 1}

    fig, ax = plt.subplots()
    ax.set_facecolor('#DEE1E3')
    ax.grid(color=[0.7, 0.7, 0.7])

    # Crear "handles" dummy para la leyenda
    h0, = ax.plot([], [], 'o', markeredgecolor='#424861', markerfacecolor='#424861', markersize=10, label='Reposo')
    h1, = ax.plot([], [], 'o', markeredgecolor='#C9F2C7', markerfacecolor='#C9F2C7', markersize=10, label='Flexión')
    h2, = ax.plot([], [], 'o', markeredgecolor='#A799B7', markerfacecolor='#A799B7', markersize=10, label='Extensión')
    h3, = ax.plot([], [], 'o', markeredgecolor='#3C6997', markerfacecolor='#3C6997', markersize=10, label='Mano Cerrada')

    # Recorrer cada posición del vector y dibujar una línea vertical
    for i, char in enumerate(vector, start=1):  # start=1 para imitar el índice de MATLAB
        if char == '0':
            color = '#424861'
        elif char == '1':
            color = '#C9F2C7'
        elif char == '2':
            color = '#A799B7'
        elif char == '3':
            color = '#3C6997'
        else:
            color = '#000000'
        h_val = heights.get(char, 1)
        ax.plot([i, i], [0, h_val], color=color, linewidth=2)

    max_height = max(heights.values())
    ax.set_ylim(0, max_height * 1.4)
    ax.set_xlabel('Muestras')
    ax.set_ylabel('Relevancia/Etiqueta')
    ax.set_title('SIMULACIÓN DE MOVIMIENTOS')
    #ax.legend(loc='upper center', ncol=4, frameon=False)
    plt.show(block=False)

    return fig, ax

def main():
    start_time = time.time()
    # Generar los datos
    vector = generate_fake_data()
    # Mostrar la gráfica estática
    fig, ax = show_movements(vector)
    # Agregar la línea dinámica (inicialmente en x=1)
    dynamic_line = ax.axvline(x=1, color='r', linewidth=2)
    plt.draw()
    #plt.pause(0.1)

    # Configurar el puerto serial
    puerto = "COM8"  # Reemplaza con el puerto adecuado
    baud_rate = 115200
    try:
        ser = serial.Serial(puerto, baud_rate, timeout=0.1)
    except Exception as e:
        print("Error al abrir el puerto serial:", e)
        return

    # Espera para que el ESP32 se reinicie y se vacíe el buffer
    time.sleep(0.05)
    ser.reset_input_buffer()

    # Enviar cada dato individualmente
    for i, char in enumerate(vector, start=1):
        # Enviar el caracter seguido de salto de línea
        ser.write((char + "\n").encode('utf-8'))
        time.sleep(0.05)  # Pausa para permitir que el ESP32 procese el dato

        # Actualizar la posición de la línea dinámica
        if i%1 == 100:
            dynamic_line.set_xdata([i, i])
            plt.draw()
            plt.pause(0.001)

        # Leer respuesta del ESP32 (si la hay)
        if ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').strip()
            print("Respuesta del ESP32:", data)

    # Cerrar la conexión serial
    ser.close()
    print("Comunicación cerrada.")
    
    # Detener el temporizador y mostrar el tiempo transcurrido
    end_time = time.time()
    elapsed_time = end_time - start_time
    print("Tiempo total de ejecución: {:.2f} segundos".format(elapsed_time))
    
    # Mantener la gráfica abierta
    plt.show()

if __name__ == '__main__':
    main()
