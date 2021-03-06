/*
Created 2016
by AlexGyver
AlexGyver Home Labs Inc.
*/

#include <EEPROM.h>   //библиотека для работы со внутренней памятью ардуино

#include <Servo.h>   //библиотека для работы с сервоприводом
Servo servo;  //объявить серво назвать servo
unsigned int min_speed=0;  //минимальная отображаемая скорость, км/ч
unsigned int max_speed=40; //максимальная отображаемая скорость, км/ч

//-----------дисплей-----------
#include <TM74HC595Display-old.h>
int SCLK = 7;
int RCLK = 6;
int DIO = 5;
TM74HC595Display disp(SCLK, RCLK, DIO);
unsigned char LED_0F[29];
//--------дисплей-------

unsigned long lastturn, time_press; //переменные хранения времени
float SPEED; //переменная хранения скорости в виде десятичной дроби
float DIST; //переменная хранения расстояния в виде десятичной дроби
float w_length=2.050; //длина окружности колеса в метрах
boolean flag; //флажок для хранения (что выводим на дисплее, скорость или расстояние)
boolean state, button; //флажки для обработчика нажатия кнопки

void setup() {
  servo.write(0);
  Serial.begin(9600);  //открыть порт
  attachInterrupt(0,sens,RISING); //подключить прерывание на 2 пин при повышении сигнала
  pinMode(3, OUTPUT);   //3 пин как выход
  digitalWrite(3, HIGH);  //подать 5 вольт на 3 пин
  pinMode(8, INPUT);   //сюда подключена кнопка
  servo.attach(4);  //серво на 4 порту 
  //для дисплея цифры----------
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9
  //--------------
  DIST=(float)EEPROM.read(0)/10.0; //вспоминаем пройденное расстояние при запуске системы (деление на 10 нужно для сохранения десятых долей расстояния, см. запись)
}

void sens() {
  if (millis()-lastturn > 80) {  //защита от случайных измерений (основано на том, что велосипед не будет ехать быстрее 120 кмч)
    SPEED=w_length/((float)(millis()-lastturn)/1000)*3.6;  //расчет скорости, км/ч
    lastturn=millis();  //запомнить время последнего оборота
    DIST=DIST+w_length/1000;  //прибавляем длину колеса к дистанции при каждом обороте оного
  }
}

void loop() {
  int pos=map(SPEED, min_speed, max_speed, 180, 0); //перевести значение от минимума до максимума в угол поворота сервы (от 0 до 180)
  servo.write(pos);  //повернуть серву

  disp.digit4(DIST,50);  //вывод расстояния на дисплей


  if ((millis()-lastturn)>2000){ //если сигнала нет больше 2 секунды
    SPEED=0;  //считаем что SPEED 0
     EEPROM.write(0,(float)DIST*10.0); //записываем DIST во внутреннюю память. Сделал так хитро, потому что внутренняя память не любит частой перезаписи. Также *10, чтобы сохранить десятую долю
  }

    if (digitalRead(8)==1) {    // Если кнопка нажата
   DIST=0;    //обнулить расстояние
  }
}

