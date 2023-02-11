#include "SPI.h"      // библиотека для обмена данными по протоколу SPI 
#include "nRF24L01.h" // библиотека для nRF24L01+
#include "RF24.h"     // библиотека для радио модуля

RF24 radio(9,10);  // Для MEGA2560 замените на RF24 radio(9,53);
const uint8_t num_channels = 128;
uint8_t values[num_channels];

int serial_putc( char c, FILE * ) {
  Serial.write( c );
  return c;
}

void printf_begin(void) {
  fdevopen( &serial_putc, 0 );
}

void setup(void) {
  Serial.begin(9600);
  printf_begin();
  radio.begin();
  radio.setAutoAck(false);
  radio.startListening(); // включаем прием сигнала

  radio.printDetails(); // если правильно соединили, то увидите настройки модуля
  delay(10000); // задержка на 10 секунд

  radio.stopListening(); // выключаем прием сигнала
  int i = 0; // вывод заголовков всех 127 каналов
  while ( i < num_channels )  {
    printf("%x",i>>4);
    ++i;
  }
  printf("\n\r");
  i = 0;
  while ( i < num_channels ) {
    printf("%x",i&0xf);
    ++i;
  }
  printf("\n\r");
}

const int num_reps = 100;

void loop(void) {
  memset(values, 0, sizeof(values));
  int rep_counter = num_reps;
  while (rep_counter--) {
    int i = num_channels;
    while (i--) {
      radio.setChannel(i);
      radio.startListening(); // включаем прием сигнала
      delayMicroseconds(128);
      radio.stopListening(); // выключаем прием сигнала
      if ( radio.testCarrier() )
        ++values[i];
    }
  }
  int i = 0;
  while ( i < num_channels ) {
    printf("%x", min(0xf, values[i] & 0xf));
    ++i;
  }
  printf("\n\r");
}