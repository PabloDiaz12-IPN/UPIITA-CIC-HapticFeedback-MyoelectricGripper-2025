// BIBLIOTECAS PARA COMUNICACIÓN & CONTROL
#include "esp32-hal-timer.h"
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

// BIBLIOTECAS DE MÓDULOS
#include <Adafruit_MLX90614.h>        // Sensor infrarrojo de temperatura
#include <Adafruit_PWMServoDriver.h>  // ServoDriver
#include <Adafruit_ADS1X15.h>         // ADC

// CREACIÓN DE OBJETOS
Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver(0x40);  // Dirección I2C del PCA9685
Adafruit_MLX90614 sensorTemp = Adafruit_MLX90614();  
Adafruit_ADS1115 ADC_ext;
TaskHandle_t tareaParalela;

// ESTRUCTURA DE DATOS A TRANSMITIR MEDIANTE ESP-NOW
typedef struct packageData{
  float tempMLX;
  uint16_t FSR_A1;
  uint16_t FSR_A2;
  uint16_t FSR_A3;
  uint16_t FSR_B1;
  uint16_t FSR_B2;
  uint16_t FSR_B3;
  uint16_t FSR_C;
};

// CREACIÓN DEL PAQUETE DE INFORMACION
packageData sensorsData;
// SLAVE MAC ADRESS
// const uint8_t slaveAddress[] = {0xE8, 0x6B, 0xEA, 0xF6, 0xD2, 0x28};
const uint8_t slaveAddress[] = {0x24, 0xDC, 0xC3, 0x45, 0xEF, 0x84};

// DECLARACION DE PINES 
const uint8_t pinFSR_A1=36; // ADC1 CH0 // A1: Superior Anterior
const uint8_t pinFSR_A2=39; // ADC1 CH3 // A2: Superior Medio
const uint8_t pinFSR_A3=34; // ADC1 CH6 // A3: Superior Posterior
const uint8_t pinFSR_B1=35; // ADC1 CH7
const uint8_t pinFSR_B2=32; // ADC1 CH4
const uint8_t pinFSR_B3=33; // ADC1 CH5 

// En ADC externo
const uint8_t pinFSR_C=0;       // ADS CH0
const uint8_t pinPotApertura=1; // ADS CH1  // VARIABLE DE PRUEBA *

// VARIABLES PARA EL CONTROL DE SERVOMOTORES
const uint8_t canal_EfecMenor = 1;
const uint8_t canal_EfecMayor = 2;
const uint8_t canal_BaseRotora = 3;

const uint16_t pos0 = 80;
const uint16_t pos180 = 650;

// MODO SEGURO
int rangos[3][2] = {
  {80, 10},    // Me
  {100, 170},   // Ma
  {30, 180}     // RotExt
};

// @KEVICHO
volatile int porcRot=0; // -50 (Flexión Total), +50 (Extensión Total) 
volatile int porcAper=15; // Casi totalmente abierta 0 - 100
String prev2="3";
String prev1="3";
String actual="3";

volatile int prev2Num=3;
volatile int prev1Num=3;
volatile int actualNum=3;

int stepSize=1; // Tamaño del paso en que se incrementa al moverse
int stepTime=22000; // Microsegundos ---> 80 ms == 80 000 us 15000

volatile int cont = 0;
int pinLight=2;

int contReposo=0;
bool flagAper=true;


// FUNCIONES AUXILIARES
void taskEachTransmission(const uint8_t *mac_addr, esp_now_send_status_t status){
}


void filter(int x0, int x1,int x2){
 if(x0==x1 && x0==x2){ // Este if dentro (o fuera) de una super condición relacionada a los FSR x0==x2
  if(x1==4) delay(1000);
  choice(x1);
 }else{ 
    delay(100);
 }
}


void move(int rot, int aper){
  int angleRot = map(rot, -50, 50, rangos[2][0], rangos[2][1]);
  int angleMe = map(aper, 0, 100, rangos[0][0], rangos[0][1]);
  int angleMa = map(aper, 0, 100, rangos[1][0], rangos[1][1]);
  
  unsigned int pwmValueRot = map(angleRot, 0, 180, pos0, pos180);
  unsigned int pwmValueMe = map(angleMe, 0, 180, pos0, pos180);
  unsigned int pwmValueMa = map(angleMa, 0, 180, pos0, pos180);

  // Mover los servos a las posiciones deseadas
  servos.setPWM(canal_BaseRotora, 0, pwmValueRot);
  servos.setPWM(canal_EfecMenor, 0, pwmValueMe);
  servos.setPWM(canal_EfecMayor, 0, pwmValueMa);
}


// 2 - HC
// 3 - REP
void choice(int command){ // commendare, Kommando
  if (command ==0) { // Flexión
    porcRot -= stepSize;
  } else if (command == 2) { // HC
    porcRot += stepSize;
  } else if (command == 1) { // APER & CIERRE
    //porcAper=porcAper;
    //porcAper -= stepSize*2;
    porcAper+=stepSize;

  }else if(command == 4){
    porcRot=0; // -50 (Flexión Total), +50 (Extensión Total) 
    porcAper=15;
    
    
  } else { // PUÑO
      porcAper=porcAper;
}

  porcRot = constrain(porcRot, -50, 50);
  porcAper = constrain(porcAper, 0, 100);

  move(porcRot,porcAper);
}

void printTask(void *parameter){
  for (;;) {
    printSensorsData();
    vTaskDelay(pdMS_TO_TICKS(250)); // Imprime cada 500ms (ajusta según necesidad)
  }
}

void printSensorsData() {
  Serial.print(sensorsData.tempMLX); Serial.print(", ");
  Serial.print(sensorsData.FSR_A1); Serial.print(", ");
  Serial.print(sensorsData.FSR_A2); Serial.print(", ");
  Serial.print(sensorsData.FSR_A3); Serial.print(", ");
  Serial.print(sensorsData.FSR_B1); Serial.print(", ");
  Serial.print(sensorsData.FSR_B2); Serial.print(", ");
  Serial.print(sensorsData.FSR_B3); Serial.print(", ");
  Serial.print(sensorsData.FSR_C); Serial.print(", ");
  Serial.println(porcAper);
}


void setup() {
  Serial.begin(115200);

  // INICIALIZACIÓN DE MODULOS
  servos.begin(); servos.setPWMFreq(60);  // Frecuencia PWM de 60Hz (16,66ms)
  sensorTemp.begin(0x5A);
  ADC_ext.begin(0x48);
  delay(100);

  // POSICIONAMIENTO INICIAL
  move(porcRot, porcAper); // Posición Inicial

  // CREACIÓN DE LA TAREA PARALELA
  xTaskCreatePinnedToCore(
    loop2,    // 
    "Task_1",
    10000,
    NULL,
    1,
    &tareaParalela,
    0);
  delay(1000);

  xTaskCreatePinnedToCore(
    printTask,
    "PrintTask",
    5000,
    NULL,
    1,
    NULL,
    1); // Ejecuta en el otro núcleo (1)

  // CONFIGURACIÓN DE ESP-NOW SENDER (MASTER)
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    //Serial.println("Hubo un error en la inicialización de ESP-NOW");
    return;
  }  

  // Registro del módulo esclavo & configuración de la comunicación
  esp_now_peer_info_t slaveInfo;
  memcpy(slaveInfo.peer_addr, slaveAddress, 6);
  slaveInfo.channel = 0;  
  slaveInfo.encrypt = false;

  // Emparejamiento       
  if (esp_now_add_peer(&slaveInfo) != ESP_OK){
    //Serial.println("Hubo un error en el emparejamiento");
    return;
  }

  esp_now_register_send_cb(taskEachTransmission); // Función a ejecutar en cada transmisión
  delay(500);
}

// NO USES NINGÚN SERIAL.PRINT EN NINGÚN LADO FUERA DEL LOOP PRINCIPAL
void loop() {
  sensorsData.FSR_A1 = analogRead(pinFSR_A1);
  sensorsData.FSR_A2 = analogRead(pinFSR_A2);
  sensorsData.FSR_A3 = analogRead(pinFSR_A3);
  sensorsData.FSR_B1 = analogRead(pinFSR_B1);
  sensorsData.FSR_B2 = analogRead(pinFSR_B2);
  sensorsData.FSR_B3 = analogRead(pinFSR_B3);
  sensorsData.FSR_C = ADC_ext.readADC_SingleEnded(pinFSR_C);
  if (sensorsData.FSR_C > 10000){
    sensorsData.FSR_C =0;
  }
  sensorsData.tempMLX = sensorTemp.readObjectTempC();

  esp_err_t result = esp_now_send(slaveAddress, (uint8_t *) &sensorsData, sizeof(sensorsData));
  
  if (Serial.available()) {
    prev2 = Serial.readStringUntil('\n');
    prev2Num=prev2.toInt();
    prev1 = Serial.readStringUntil('\n');
    prev1Num=prev1.toInt();
    actual = Serial.readStringUntil('\n');
    actualNum=actual.toInt();
  }
}


void loop2(void *parameter){
  for(;;){
    filter(prev2Num,prev1Num,actualNum);
    delayMicroseconds(stepTime);
  }
  vTaskDelay(10);
}
