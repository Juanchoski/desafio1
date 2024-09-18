#include <Adafruit_LiquidCrystal.h>

int pinEntrada = A0;       // Pin de entrada de la señal
int botonInicio = 2;       // Boton de inicio
int botonFin = 3;          // Boton de fin
int valor = 0;             // Valor analogico leido
int valorAnterior = 0;	   // Valor anterior dle analogico
int valorMaximo = 0;       // Maximo valor detectado
int valorMinimo = 1023;    // Minimo valor detectado
int estadoBotonInicio = 0; // Estado del boton de inicio
int estadoBotonFin = 0;    // Estado del boton de fin
bool maxArreglo = false;   // Limite del arreglo para no llenar la memoria
unsigned int dimension = 10;        // Tamaño del arreglo
unsigned int posicionA = 0;		   // Posicion del arreglo
float frecuencia = 0;        // Frecuencia en Hz
Adafruit_LiquidCrystal lcd(0);  // Inicializacion del LCD

int* grafica = new int[dimension];  // Puntero a la memoria dinamica

float tiempoInicio = 0;    // Tiempo inicial para la medicion
float tiempoFin = 0;       // Tiempo final para la medicion
int estadoSenial = LOW;     // Estado actual de la senal
int ultimoEstadoSenial = LOW; // Estado anterior de la senal
int ciclos = 0;            // Contador de ciclos de la senal
float amplitud = 0;        // Amplitud de la senal
bool leyendo = false;      // Variable para saber si está leyendo datos

void setup() {
  pinMode(botonInicio, INPUT);
  pinMode(botonFin, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Bienvenido");
}

void loop() {
  estadoBotonInicio = digitalRead(botonInicio);
  estadoBotonFin = digitalRead(botonFin);

  if (!leyendo && estadoBotonInicio == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Leyendo...");
    tiempoInicio = millis();  
    ciclos = 0;
    valorMaximo = 0;
    valorMinimo = 1023;
    leyendo = true;
    Serial.println("Midiendo...");
  }

  if (leyendo) {
    valor = analogRead(pinEntrada);
    if(valor!=valorAnterior && posicionA!=dimension){
      valorAnterior=valor;
      grafica[posicionA]=valor;
      posicionA++;
      if(posicionA==dimension && maxArreglo==0){
        maxArreglo = aumentoArreglo(grafica,dimension);
      }
      else if(posicionA==dimension && maxArreglo==1){
        Serial.println("Memoria llena, se dejan de tomar valores de las ondas");
      }
    }
    if (valor > valorMaximo) valorMaximo = valor;
    if (valor < valorMinimo) valorMinimo = valor;

    estadoSenial = (valor > (valorMinimo + valorMaximo) / 2) ? HIGH : LOW;

    if (estadoSenial != ultimoEstadoSenial && estadoSenial == HIGH) {
      ciclos++;
    }

    ultimoEstadoSenial = estadoSenial;
  }

  if (leyendo && estadoBotonFin == HIGH) {
    tiempoFin = millis();  
    leyendo = false;

    float tiempoTranscurrido = (tiempoFin - tiempoInicio) / 1000.0;
    frecuencia = ciclos / tiempoTranscurrido;
    amplitud = ((valorMaximo - valorMinimo) / 1000.0) * 5.0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Freq:");
    lcd.print(frecuencia);
    lcd.print(" Hz");

    lcd.setCursor(0, 1);
    lcd.print("Amp:");
    lcd.print(amplitud, 2);
    lcd.print(" V");

    tipoOnda(grafica, posicionA);
  
    posicionA = 0;
    dimension = 10;
    maxArreglo = false;
    delete[] grafica;
    grafica = new int[dimension];
    ciclos = 0;
  }
  
}


void tipoOnda(int* grafica, unsigned int dimension) {
  bool esCuadrada = true;
  bool esTriangular = true;
  bool esSenoidal = true;
  int valor1 = grafica[0];
  int valor2 = grafica[1];

  for (int i = 2; i < dimension; i++) {
    if ((i % 2 == 0 && grafica[i] != valor1) || (i % 2 != 0 && grafica[i] != valor2)) {
      esCuadrada = false;
      break;
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  if (esCuadrada) {
    lcd.print("Onda Cuadrada");
    return;
  } 	
  
  //para senoidal si no fue cuadrada
  float amplitudEsperada = (valorMaximo - valorMinimo) / 2.0;
  float frecuenciaEsperada = frecuencia;
  int puntos = dimension;
  float* valoresEsperados = new float[puntos];
  for (int t = 0; t < puntos; t++) {
    valoresEsperados[t] = amplitudEsperada * sin(2 * PI * frecuenciaEsperada * t / puntos) + amplitudEsperada;
  }

  float umbral = 10;
  for (int s = 0; s < puntos; s++) {
    if (abs(grafica[s] - valoresEsperados[s]) > umbral) {
      esSenoidal = false;
      break;
    }
  }
  delete[] valoresEsperados;
  if(esSenoidal){
    lcd.print("Onda Senoidal");
    return;
  }
  
  
  //para triangular si no fue cuadrada ni triangular
  if (dimension < 3) esTriangular = false;
  bool subiendo = true;
  int diferencia = valor2 - valor1;

  for (int t = 1; t < dimension - 1; t++) {
    int diferenciaActual = grafica[t + 1] - grafica[t];

    if (subiendo) {
      if (diferenciaActual < 0) {
        subiendo = false;
        diferencia = -diferencia;
      } else if (diferenciaActual != diferencia) {
        esTriangular = false;
      }
    } else {
      if (diferenciaActual != -diferencia) {
        esTriangular = false;
      }
    }
  }
  
  if (esTriangular) {
    lcd.print("Onda Triangular");
    return;

  }
  //si no fue ningun tipo de onda
  lcd.print("No reconocida");
  return;
  
}

bool aumentoArreglo(int*& grafica, unsigned int &dimension) {
  if (dimension * 2 >= 300) {
    return false; 
  }

  int nuevoTamano = dimension * 2;
  int* G = new int[nuevoTamano];
  for (int a = 0; a < dimension; a++) {
    G[a] = grafica[a];
  }

  delete[] grafica;
  grafica = G;
  dimension = nuevoTamano;
  return true;
}


