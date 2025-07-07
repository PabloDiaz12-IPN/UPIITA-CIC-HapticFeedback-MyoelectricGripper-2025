#include <WiFi.h>
#include <esp_now.h>
#include <math.h>
#include <ESP32Servo.h>  // Nueva biblioteca

const int entradas[8] = {23, 22, 21, 19, 18, 5, 17, 16}; // Entradas del ULN2803
const int electro = 32; // Pin para activar rele de electroestimulador

const int servoPin1 = 33;  // Pin de señal del servo 1
const int servoPin2 = 25;  // Pin de señal del servo 2
const int servoPin3 = 26;  // Pin de señal del servo 3

Servo servoA;
Servo servoB;
Servo servoC;

unsigned long currentMillis;  // Para almacenar el tiempo actual
unsigned long lastMoveTime = 0;  // Tiempo cuando se movieron los servos
unsigned long lastTemperatureTime = 0;  // Tiempo cuando se controló el electro
bool arrived = false;  // Para saber si los servos llegaron a la posición
int anguloActualA = 0, anguloActualB = 0, anguloActualC = 0;  // Ángulos actuales de los servos

enum EstadoElectro {
  APAGADO,
  ENCENDIDO,
  ESPERA
};
EstadoElectro estadoElectro = APAGADO;
unsigned long tiempoCambio = 0;  // Guarda el tiempo de cambio de estado


int indice = 0;  // Índice para recorrer las muestras
int recoleccion = 0;  // Muestras que han sido tomadas
int umbrales[] = {300, 350, 400, 320, 340, 360, 380};  //Ajustarlos

typedef struct packageData {
  float tempMLX;
  uint16_t FSR_A1;
  uint16_t FSR_A2;
  uint16_t FSR_A3;
  uint16_t FSR_B1;
  uint16_t FSR_B2;
  uint16_t FSR_B3;
  uint16_t FSR_C;
};

packageData sensorsData;

// unsigned long lastElectroTime = 0;
// bool electroActivo = false;
// bool enEspera = false;


unsigned long lastElectroTime = 0;
bool electroActivo = false;
bool enEspera = false;
bool cicloEnCurso = false;

int pulsosTotales = 0;
int pulsosEnviados = 0;
unsigned long intervaloPulso = 0;
bool pulsoActivo = false;

// Prototipos de funciones (por si las tienes al final del código)
void deteccionContacto(int lecturas[], const int entradas[], int cantidad);
int obtenerAnguloPresion(int presion);
int temperaturaNivel(float temperatura);

// Callback que se ejecuta cuando llega un paquete ESP-NOW
void OnRecv(const esp_now_recv_info *macInfoSender, const uint8_t *incomingData, int len) {
  Serial.println("¡Paquete recibido!"); 

  // Copiar datos recibidos a la estructura
  memcpy(&sensorsData, incomingData, sizeof(sensorsData));
  currentMillis = millis();  // Captura el tiempo actual

  // Imprimir todos los valores recibidos para verificar
  // imprimirDatosSensores(sensorsData);  // <- Aquí llamas la nueva función
  // delay(200);

  // Contacto
  int lecturasFSR[] = {
    sensorsData.FSR_A1,
    sensorsData.FSR_A2,
    sensorsData.FSR_A3,
    sensorsData.FSR_B1,
    sensorsData.FSR_B2,
    sensorsData.FSR_B3,
    sensorsData.FSR_C
  };
  imprimirDatosSensores(sensorsData);

  // Presión
  int presionA = max(lecturasFSR[0], max(lecturasFSR[1], lecturasFSR[2]));
  int presionB = max(lecturasFSR[3], max(lecturasFSR[4], lecturasFSR[5]));
  int presionC = lecturasFSR[6];


  // int anguloA = presionNivel(presionA);
  // int anguloB = presionNivel(presionB);
  // int anguloC = presionNivel(presionC);
  deteccionContacto(lecturasFSR, umbrales, 7, presionA, presionB, presionC);


  float temperaturaC = sensorsData.tempMLX;

  int temperaturaTime = temperaturaNivel(temperaturaC);

  // Control electroestimulador
  // switch (estadoElectro) {
  //   case APAGADO:
  //     if (currentMillis - tiempoCambio >= 3000) {
  //       digitalWrite(electro, LOW);
  //       digitalWrite(2, LOW);
  //       estadoElectro = ENCENDIDO;
  //       tiempoCambio = currentMillis;
  //     }
  //     break;

  //   case ENCENDIDO:
  //     if (currentMillis - tiempoCambio >= temperaturaTime * 1000UL) {
  //       digitalWrite(electro, HIGH);
  //       digitalWrite(2, HIGH);
  //       estadoElectro = ESPERA;
  //       tiempoCambio = currentMillis;
  //     }
  //     break;

  //   case ESPERA:
  //     if (currentMillis - tiempoCambio >= 3000) {
  //       estadoElectro = APAGADO;
  //       tiempoCambio = currentMillis;
  //     }
  //     break;
  // }

  // if (temperaturaTime == 0) {
  //   // Asegurarse de que el electro esté apagado
  //   digitalWrite(electro, LOW);
  //   digitalWrite(2, LOW);
  //   estadoElectro = APAGADO;
  //   return;  // Salir de la función
  // }

  // // ✅ Máquina de estados del electroestimulador
  // switch (estadoElectro) {
  //   case APAGADO:
  //     if (currentMillis - tiempoCambio >= 3000) {
  //       digitalWrite(electro, LOW);
  //       digitalWrite(2, LOW);
  //       estadoElectro = ENCENDIDO;
  //       tiempoCambio = currentMillis;
  //     }
  //     break;

  //   case ENCENDIDO:
  //     if (currentMillis - tiempoCambio >= temperaturaTime * 1000UL) {
  //       digitalWrite(electro, HIGH);
  //       digitalWrite(2, HIGH);
  //       estadoElectro = ESPERA;
  //       tiempoCambio = currentMillis;
  //     }
  //     break;

  //   case ESPERA:
  //     if (currentMillis - tiempoCambio >= 3000) {
  //       estadoElectro = APAGADO;
  //       tiempoCambio = currentMillis;
  //     }
  //     break;
  // }


  // Serial.printf("[TEMP] Temperatura: %.2f °C -> Tiempo de activación: %d s\n", temperaturaC, temperaturaTime);

  // if (temperaturaTime == 0) {
  //   // Siempre apagado si la temperatura no es suficiente
  //   if (electroActivo || enEspera) {
  //     Serial.println("[ELECTRO] Temperatura baja: desactivando electroestimulador.");
  //   }
  //   digitalWrite(electro, LOW);
  //   digitalWrite(2, LOW);
  //   electroActivo = false;
  //   enEspera = false;
  //   return;
  // }

  // if (!electroActivo && !enEspera) {
  //   Serial.println("[ELECTRO] Activando electroestimulador.");
  //   digitalWrite(electro, HIGH);
  //   digitalWrite(2, HIGH);
  //   electroActivo = true;
  //   lastElectroTime = currentMillis;
  // }

  // if (electroActivo && currentMillis - lastElectroTime >= temperaturaTime * 1000UL) {
  //   Serial.println("[ELECTRO] Tiempo de activación cumplido. Apagando electroestimulador.");
  //   digitalWrite(electro, LOW);
  //   digitalWrite(2, LOW);
  //   electroActivo = false;
  //   enEspera = true;
  //   lastElectroTime = currentMillis;
  // }

  // if (enEspera && currentMillis - lastElectroTime >= 2000) {
  //   Serial.println("[ELECTRO] Fin del tiempo de espera. Listo para siguiente ciclo.");
  //   enEspera = false;
  // }


  if (temperaturaC <= 30) {
    digitalWrite(electro, LOW);
    digitalWrite(2, LOW);
    electroActivo = false;
    enEspera = false;
    cicloEnCurso = false;
    pulsosTotales = 0;
    pulsosEnviados = 0;
    return;
  } else if (temperaturaC > 30 && temperaturaC <= 40) {
    pulsosTotales = 2;
  } else if (temperaturaC > 40 && temperaturaC <= 50) {
    pulsosTotales = 4;
  } else if (temperaturaC > 50) {
    pulsosTotales = 6;
  }

  intervaloPulso = 2000 / pulsosTotales; // 2 segundos dividido en N pulsos

  if (!cicloEnCurso && !enEspera) {
    Serial.printf("[ELECTRO] Iniciando secuencia de %d pulsos en 2 segundos.\n", pulsosTotales);
    cicloEnCurso = true;
    pulsosEnviados = 0;
    lastElectroTime = currentMillis;
    electroActivo = true;
    digitalWrite(electro, HIGH);
    digitalWrite(2, HIGH);
    pulsoActivo = true;
  }

  if (cicloEnCurso) {
    if (pulsoActivo && currentMillis - lastElectroTime >= intervaloPulso / 2) {
      // Apagar después de medio intervalo
      digitalWrite(electro, LOW);
      digitalWrite(2, LOW);
      pulsoActivo = false;
      lastElectroTime = currentMillis;
    } else if (!pulsoActivo && currentMillis - lastElectroTime >= intervaloPulso / 2) {
      // Encender siguiente pulso
      pulsosEnviados++;
      if (pulsosEnviados >= pulsosTotales) {
        Serial.println("[ELECTRO] Secuencia completada. Iniciando espera.");
        cicloEnCurso = false;
        enEspera = true;
        lastElectroTime = currentMillis;
      } else {
        digitalWrite(electro, HIGH);
        digitalWrite(2, HIGH);
        pulsoActivo = true;
        lastElectroTime = currentMillis;
      }
    }
  }

  if (enEspera && currentMillis - lastElectroTime >= 2000) {
    Serial.println("[ELECTRO] Fin de espera. Listo para próxima secuencia.");
    enEspera = false;
  }
}



void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(2,OUTPUT);


  servoA.attach(servoPin1);
  servoB.attach(servoPin2);
  servoC.attach(servoPin3);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  // Configurar pines
  for (int i = 0; i < 8; i++) {
    pinMode(entradas[i], OUTPUT);
  }
  pinMode(electro, OUTPUT);
  digitalWrite(electro, LOW);
  digitalWrite(2, LOW);
  // Registrar callback para recibir datos
  esp_now_register_recv_cb(OnRecv);

  Serial.println("Receptor ESP-NOW listo");
  delay(1000);

  servoA.write(0);
  servoB.write(0);
  servoC.write(0);
}

void loop() {
  // Puedes agregar código aquí si es necesario
}


// Contacto
void deteccionContacto(int lecturas[], const int umbrales[], int cantidad, int presionA, int presionB, int presionC) {
  int anguloA = 0;
  int anguloB = 0;
  int anguloC = 0;

  for (int i = 0; i < cantidad; i++) {
    if (lecturas[i] > umbrales[i]) { 
      digitalWrite(entradas[i], HIGH);  
      if (i <= 2) {
        anguloA = presionNivel(presionA);  
      } else if (i > 2 && i <= 5) {
        anguloB = presionNivel(presionB);  
      } else {
        anguloC = presionNivel(presionC); 
      }     
    } else {
      digitalWrite(entradas[i], LOW);  // Apagar el motor si no hay presión
    }
  }

  servoA.write(anguloA);
  servoB.write(anguloB);
  servoC.write(anguloC);
  Serial.print(anguloA); Serial.print(", "); Serial.print(anguloB); Serial.print(", "); Serial.println(anguloC);
}

// Presión
int presionNivel(int presion) { 
  if (presion <= 10) {
    return 0;  // sin presión
  } else if (presion > 10 && presion <= 1365) {
    return 50;
  } else if (presion > 1365 && presion <= 2730) {
    return 70;
  } else if (presion > 2730) {
    return 90;
  }
  return 0;
}



// Temperatura
int temperaturaNivel(float temperatura) { 
  if (temperatura <= 10) {
    return 0;  
  // } else if (temperatura > 10 && temperatura <= 30) {
  //   return 3;
  } else if (temperatura > 35 && temperatura <= 50) {
    return 6;
  } else if (temperatura > 50) {
    return 20;
  } 
  return 0;
}

void imprimirDatosSensores(const packageData& datos) {
  Serial.println("====== Datos de Sensores Recibidos ======");
  Serial.printf("Temperatura: %.2f °C\n", datos.tempMLX);
  Serial.printf("FSR_A1: %d\n", datos.FSR_A1);
  Serial.printf("FSR_A2: %d\n", datos.FSR_A2);
  Serial.printf("FSR_A3: %d\n", datos.FSR_A3);
  Serial.printf("FSR_B1: %d\n", datos.FSR_B1);
  Serial.printf("FSR_B2: %d\n", datos.FSR_B2);
  Serial.printf("FSR_B3: %d\n", datos.FSR_B3);
  Serial.printf("FSR_C : %d\n", datos.FSR_C);
  Serial.println("=========================================");
}
