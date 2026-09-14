/*
------------------------------------------------------------
Project: Automatic Syringe Pump
Author: Jonnathan Venceslau Souza
Date: February 24, 2026
Description: This program controls a stepper motor via
two commands: manual (up and down) and automatic
(downward movement at a constant flow rate).
------------------------------------------------------------
*/
//Biblioteca
#include <Stepper.h>

int PassosPorVolta = 100;  //Represents the number of steps the motor needs to make a full rotation
const int Pino_botao1 = 4; //Button to trigger the descent command
const int Pino_botao2 = 3; //Button to trigger the upward movement command
const int Pino_botao3 = 2; //Button to activate automatic mode

// Variables controlling the stepper motor coils
int IN1 = 8;
int IN2 = 9;
int IN3 = 10;
int IN4 = 11;

/*Variables for controlling syringe injection speed in automatic mode*/
int Baixa = 6;
int Media = 3;
int Alta = 1;

/*insert the name of the speed control variable (Low, Medium, and High)*/
int velocidade = Baixa;  

/*Variable responsible for controlling the automatic descent of the syringe;
the speed and the type of fluid used must be taken into account.*/
int ajustededescida = 7700; 

//Variables for button state control
int buttonState1 = 0;
int lastButtonState1 = 0;
int motorState1 = LOW;
unsigned long lastDebounceTime1 = 0;
const unsigned long debounceDelay1 = 50;

//Sinalizadores
const int LedPin1 = 5;
const int LedPin2 = 6;
const int LedPin3 = 7;

bool automaticoHabilitado = false; 

int passo[8][4] = {
  { 1, 0, 0, 0 },
  { 1, 1, 0, 0 },
  { 0, 1, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 1, 0 },
  { 0, 0, 1, 1 },
  { 0, 0, 0, 1 },
  { 1, 0, 0, 1 }
};

Stepper MotorStepper(PassosPorVolta, 8, 10, 9, 11);

void setup() {
  
  pinMode(Pino_botao1, INPUT_PULLUP);
  pinMode(Pino_botao2, INPUT_PULLUP);
  pinMode(Pino_botao3, INPUT_PULLUP);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(LedPin1, OUTPUT);
  pinMode(LedPin2, OUTPUT);
  pinMode(LedPin3, OUTPUT);

  digitalWrite(LedPin1, LOW);  //LedPin1 off

  MotorStepper.setSpeed(200);  //Sets the maximum speed in RPM.
}

void loop() {

  Manual();

  // Only runs automatically if enabled
  if (automaticoHabilitado) {
    Automatico();

  } else {
    verificarBotaoInicial();  // Checks if the button was pressed to enable it.
  }
}

void Manual() {

  if (digitalRead(Pino_botao1) == LOW) {
    digitalWrite(LedPin3, HIGH);
    digitalWrite(LedPin2, LOW);

    MotorStepper.step(-PassosPorVolta);
    pararMotor();
  }

  if (digitalRead(Pino_botao2) == LOW) {
    digitalWrite(LedPin3, LOW);
    digitalWrite(LedPin2, HIGH);

    MotorStepper.step(PassosPorVolta);
    pararMotor();
  }
}

void escreverPasso(int passoNum) {
  digitalWrite(IN1, passo[passoNum][0]);
  digitalWrite(IN2, passo[passoNum][1]);
  digitalWrite(IN3, passo[passoNum][2]);
  digitalWrite(IN4, passo[passoNum][3]);
}

void pararMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

//Function that enables automatic mode only after the first click
void verificarBotaoInicial() {
  if (digitalRead(Pino_botao3) == LOW) {
    automaticoHabilitado = true;
    delay(200);  //Avoids multiple reads
  }
}

void Automatico() {
  digitalWrite(LedPin3, LOW);
  digitalWrite(LedPin2, LOW);
  int reading1 = digitalRead(Pino_botao3);

  if (reading1 != lastButtonState1) {
    lastDebounceTime1 = millis();
  }

  if ((millis() - lastDebounceTime1) > debounceDelay1) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;

      if (buttonState1 == HIGH) {
        int contador = 0;
        motorState1 = !motorState1;

        if (motorState1 == HIGH) {
          while (buttonState1 == HIGH && contador < ajustededescida) {
            for (int i = 0; i < 50; i++) {
              for (int j = 7; j >= 0; j--) {
                escreverPasso(j);
                delay(velocidade);
                contador++;
              }
            }
          }
          pararMotor();
          digitalWrite(LedPin1, HIGH);
        } else {
          digitalWrite(LedPin1, LOW);
          pararMotor();
        }
        delay(200);
      }
    }
  }
  lastButtonState1 = reading1;
}