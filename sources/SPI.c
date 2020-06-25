/*
 * SPI.c
 *
 *  Created on: 25 ���. 2020 �.
 *      Author: ysatt
 */
#include <avr/io.h>
#include "SPI.h"
#define SPI_DDR DDRB

#define SPI_SS PB0
#define SPI_MOSI PB2
#define SPI_SCK PB1

// ������������� ����������
void spi_init() {
  SPI_DDR |= (1 << SPI_MOSI) | (1 <<  SPI_SCK) | (1 << SPI_SS);
  SPCR = (1 << SPE) | (1 << MSTR); // ����� 0, ������, ������� 1/4 �� ������� ��
}

// ������� � ��������� 1 ���� �� SPI, ���������� ���������� ��������
uint8_t spi_send_recv(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
  return SPDR;
}

