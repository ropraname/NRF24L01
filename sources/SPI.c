/*
 * SPI.c
 *
 *  Created on: 25 июн. 2020 г.
 *      Author: ysatt
 */
#include <avr/io.h>
#include "SPI.h"
#define SPI_DDR DDRB

#define SPI_SS PB0
#define SPI_MOSI PB2
#define SPI_SCK PB1

// Инициализация интерфейса
void spi_init() {
  SPI_DDR |= (1 << SPI_MOSI) | (1 <<  SPI_SCK) | (1 << SPI_SS);
  SPCR = (1 << SPE) | (1 << MSTR); // режим 0, мастер, частота 1/4 от частоты ЦП
}

// Передаёт и принимает 1 байт по SPI, возвращает полученное значение
uint8_t spi_send_recv(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
  return SPDR;
}

