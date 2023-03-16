/*   Данный скетч делает следующее: передатчик (TX) отправляет массив
     данных, который генерируется согласно показаниям с кнопки и с
     двух потенциомтеров. Приёмник (RX) получает массив, и записывает
     данные на реле, сервомашинку и генерирует ШИМ сигнал на транзистор.
    by AlexGyver 2016
*/

// #include <SPI.h>
// #include "nRF24L01.h"
// #include "RF24.h"
// #include <Servo.h>

// RF24 radio(9, 10);  // "создать" модуль на пинах 9 и 10 Для Уно
// //RF24 radio(9,53); // для Меги

// byte recieved_data[3];  // массив принятых данных
// byte relay = 2;         // реле на 2 цифровом
// byte servo = 3;         // сервопривод на 3 цифровом
// byte mosfet = 5;        // мосфет на 5 цифровом (ТУТ ЕСТЬ ШИМ!!!)

// Servo myservo;

// byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

// void setup() {
//   Serial.begin(9600);       // открываем порт для связи с ПК

//   // pinMode(relay, OUTPUT);   // настроить пин реле как выход
//   // pinMode(mosfet, OUTPUT);  // настроить пин мосфета как выход

//   // myservo.attach(servo);

//   radio.begin(); //активировать модуль
//   radio.setAutoAck(1);        // режим подтверждения приёма, 1 вкл 0 выкл
//   radio.setRetries(0, 15);    // (время между попыткой достучаться, число попыток)
//   radio.enableAckPayload();   // разрешить отсылку данных в ответ на входящий сигнал
//   radio.setPayloadSize(32);   // размер пакета, в байтах

//   radio.openReadingPipe(1, address[0]);     // хотим слушать трубу 0
//   radio.setChannel(0x60);  // выбираем канал (в котором нет шумов!)

//   radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
//   radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
//   //должна быть одинакова на приёмнике и передатчике!
//   //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

//   radio.powerUp();          // начать работу
//   radio.startListening();   // начинаем слушать эфир, мы приёмный модуль
//   radio.printDetails(); // Вот эта строка напечатает нам что-то, если все правильно соединили.
//   if (radio.available()) {
//     Serial.println("(setup) radio is available!");
//   } else {
//     Serial.println("(setup) radio is not available!");
//   }
// }

// void loop() {
//   byte pipeNo;

//   // if (radio.available()) Serial.println("(loop) radio is available!");
//   // else Serial.println("(loop) radio is not available!");

//   while ( radio.available(&pipeNo)) { // есть входящие данные
//     // чиатем входящий сигнал
//     radio.read(&recieved_data, sizeof(recieved_data));

//     Serial.println(String("button: ") + recieved_data[0]);
//     Serial.println(String("A0: ") + recieved_data[1]);
//     Serial.println(String("A1: ") + recieved_data[2]);

//     delay(1000);

//     // подать на реле сигнал с 0 места массива
//     // digitalWrite(relay, recieved_data[0]);

//     // повернуть серво на угол 0..180
//     // значение получено с 1 элемента массива
//     // myservo.write(recieved_data[1]);

//     // подать на мосфет ШИМ сигнал
//     // в соответствии с принятыми данными со 2 места массива, диапазон 0...255
//     // analogWrite(mosfet, recieved_data[2]);
//   }
// }

// ************************

#include "SPI.h"       // библиотека для протокола SPI
#include "nRF24L01.h"  // библиотека для nRF24L01+
#include "RF24.h"      // библиотека для радио модуля
// #include <Servo.h>

const uint64_t pipe = 0xF0F1F2F3F4LL;  // идентификатор передачи
RF24 radio(9, 10);                     // Для MEGA2560 замените на RF24 radio(9,53);
byte recieved_data[7];                 // массив принятых данных
// byte relay = 2;         // реле на 2 цифровом
// byte servo = 3;         // сервопривод на 3 цифровом
// byte mosfet = 5;        // мосфет на 5 цифровом (ТУТ ЕСТЬ ШИМ!!!)

void setup() {
  Serial.begin(9600);   // запускаем последовательный порт
  radio.begin();        // включаем радио модуль
  radio.setChannel(0);  // выбираем канал (от 0 до 127)

  // speed: RF24_250KBPS, RF24_1MBPS or RF24_2MBPS
  radio.setDataRate(RF24_2MBPS);
  // power: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_HIGH);

  radio.openReadingPipe(1, pipe);
  radio.powerUp();
  radio.startListening();
  Serial.println("Radio init!");
  // radio.printDetails();
  Serial.println("-----------------------");
}

void loop() {
  // int data;
  // byte pipeNo;

  // while (radio.available(&pipeNo)) {  // есть входящие данные
  if (radio.available()) {  // есть входящие данные
    // чиатем входящий сигнал
    radio.read(&recieved_data, sizeof(recieved_data));

    Serial.print("button left: ");
    Serial.println(recieved_data[0]);
    Serial.print("button right: ");
    Serial.println(recieved_data[1]);
    Serial.print("potent: ");
    Serial.println(recieved_data[2]);
    Serial.print("X left: ");
    Serial.println(recieved_data[3]);
    Serial.print("Y left: ");
    Serial.println(recieved_data[4]);
    Serial.print("X right: ");
    Serial.println(recieved_data[5]);
    Serial.print("Y right: ");
    Serial.println(recieved_data[6]);
    Serial.println("-----------------------");
    delay(1000);

    // подать на реле сигнал с 0 места массива
    // digitalWrite(relay, recieved_data[0]);

    // повернуть серво на угол 0..180
    // значение получено с 1 элемента массива
    // myservo.write(recieved_data[1]);

    // подать на мосфет ШИМ сигнал
    // в соответствии с принятыми данными со 2 места массива, диапазон 0...255
    // analogWrite(mosfet, recieved_data[2]);
  } else {
    Serial.println("ERROR!");
    delay(1000);
  }
}