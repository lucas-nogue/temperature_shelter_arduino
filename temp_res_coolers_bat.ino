#include <Wire.h>

#include <LiquidCrystal_I2C.h>

//initialize the liquid crystal library
//the first parameter is  the I2C address
//the second parameter is how many columns are on your screen
//the  third parameter is how many rows are on your screen
LiquidCrystal_I2C lcd(0x3F, 20, 4);  // set the LCD address to 0x3F for a 20 chars and 4 line display

// variables globales
float voltInt, tempInt, voltExt, tempExt, voltBat, tempBat;
//pines analogicos
//mas texho
const int pinInt = 0;
const int pinExt = 1;
const int pinBat = 2;
//pines digitales
const int res = 2;
const int cool = 3;
const int bat = 4;
const int boton = 5;
const int lcdPower = 7;
const int rele = 8;
const int ledVida = 9;
//variables milis
extern volatile unsigned long timer0_millis;
unsigned long newValue;

//variables booleanas (estados de pines)
bool resState;
bool coolState;
bool batState;


void setup() {

  pinMode(2, OUTPUT);  //led verde
  pinMode(3, OUTPUT);  //led rojo
  pinMode(4, OUTPUT);  //led rojo
  pinMode(5, INPUT);   //boton
  pinMode(7, OUTPUT);  //lcd
  pinMode(8, OUTPUT);  //relé (killswitch)
  pinMode(9, OUTPUT);  //ledVida
  //initialize lcd screen
  lcd.init();
  lcd.backlight();
  digitalWrite(lcdPower, LOW);

  //variables bool true or false
  bool resState = false;
  bool coolState = false;
  bool batState = false;
}


void loop() {


  //temperaturas
  voltInt = getVoltInt(pinInt);
  voltExt = getVoltExt(pinExt);
  voltBat = getVoltBat(pinBat);

  tempInt = (voltInt - 0.5) * 100.0;
  tempExt = (voltExt - 0.5) * 100.0;
  tempBat = (voltBat - 0.5) * 100.0;

  //milis
  unsigned long currentTime;
  currentTime = millis();
  //resetear tiempo
  if (digitalRead(boton) == LOW) {
    setMillis(newValue);
  }
  if (currentTime < 60000) {
    digitalWrite(lcdPower, HIGH);  //prender lcd durante 1 minuto
  }

  if (currentTime > 60000) {
    digitalWrite(lcdPower, LOW);  //apagar lcd despues de 1 minuto
  }

  //prender apagar resistencias coolers y baterias

  //resistencias prender
  if (tempExt <= -15)  //temperatura exterior menor a 15
  {
    if (tempInt <= 0)  //temp interior menor a 0
    {
      //prender resistencia
      digitalWrite(res, HIGH);  //prende resistencia
      resState = true;          //cambia estado
    }
  }
  //resistencias apagar
  if (resState)  //esto pregunta si la variable bool resState es true or false, si es true sigue
  {
    if (tempInt >= 2) {
      digitalWrite(res, LOW);  //apaga resistencia
      resState = false;        //cambia valor a falso
    }
  }

  //baterias prender
  if (tempBat <= 0)  //si la temperatura de la bateria es menor a 0 grados
  {
    digitalWrite(bat, HIGH);  //prender baterias
    batState = true;          //cambiar variable
  }
  //baterias apagar
  if (batState)  //si las baterias estan prendidas
  {
    if (tempBat >= 5)  //temp mayor a 5
    {
      digitalWrite(bat, LOW);  //apagar baterias
      batState = false;        //cambiar variable
    }
  }
  //coolers prender
  if (tempInt >= 30)  //si la temperatura del cooler es mayor a 30
  {
    digitalWrite(cool, HIGH);  //prender baterias
    coolState = true;          //cambiar variable
  }
  //coolers apagar
  if (coolState)  //si los coolers estan prendidos
  {
    if (tempInt <= 20)  //temp menor a 20
    {
      digitalWrite(cool, LOW);  //apagar coolers
      coolState = false;        //cambiar variable
    }
  }
  //lcd
  //titulo
  digitalWrite(ledVida, HIGH);
  delay(250);
  digitalWrite(ledVida, LOW);
  lcd.setCursor(0, 0);
  lcd.print("sensor de temp.");
  //temperatura interior
  lcd.setCursor(0, 1);
  lcd.print("temp. int:");
  lcd.setCursor(10, 1);
  lcd.print(tempInt);
  lcd.setCursor(15, 1);
  lcd.print("   ");
  //temperatura exterior
  lcd.setCursor(0, 2);
  lcd.print("temp. ext:");
  lcd.setCursor(10, 2);
  lcd.print(tempExt);
  lcd.setCursor(15, 2);
  lcd.print("   ");
  //temperatura de las baterias
  lcd.setCursor(0, 3);
  lcd.print("temp. bat:");
  lcd.setCursor(10, 3);
  lcd.print(tempBat);
  lcd.setCursor(15, 3);
  lcd.print("   ");
}
float getVoltInt(int intPin) {
  return (analogRead(intPin) * 0.004882814);
}
float getVoltExt(int extPin) {
  return (analogRead(extPin) * 0.004882814);
}
float getVoltBat(int batPin) {
  return (analogRead(batPin) * 0.004882814);
}

//millis set timer stuff



void setMillis(unsigned long new_millis) {
  uint8_t oldSREG = SREG;
  cli();
  timer0_millis = new_millis;
  SREG = oldSREG;
}