/*
 * nrf.c
 *
 *  Created on: 25 ���. 2020 �.
 *      Author: ysatt
 */
#include "base_nrf.h"
#include <stdint.h>
#include <avr/io.h>
#include "SPI.h"
#include "defining.h"
#include <util/delay.h>

#define RADIO_PORT PORTD
#define RADIO_DDR DDRD
#define RADIO_PIN PIND

#define RADIO_CSN 1
#define RADIO_CE 2
#define RADIO_IRQ 3

// ��������� ������� ��� ������ � csn �� �������������� ������������ � ���� ������, �� ����� �������� static

// �������� �������� ��������� (������� �������) �� ����� CE
inline void radio_assert_ce() {
	RADIO_PORT |= (1 << RADIO_CE); // ��������� �������� ������ �� ����� CE
}

// �������� ���������� ��������� (������ �������) �� ����� CE
inline void radio_deassert_ce() {
	RADIO_PORT &= ~(1 << RADIO_CE); // ��������� ������� ������ �� ����� CE
}

// �������� �������� ��������� (������ �������) �� ����� CSN
inline static void csn_assert() {
	RADIO_PORT &= ~(1 << RADIO_CSN); // ��������� ������� ������ �� ����� CSN
}

// �������� ���������� ��������� (������� �������) �� ����� CSN
inline static void csn_deassert() {
	RADIO_PORT |= (1 << RADIO_CSN); // ��������� �������� ������ �� ����� CSN
}

// �������������� �����
void radio_init() {
	RADIO_DDR |= (1 << RADIO_CSN) | (1 << RADIO_CE); // ����� CSN � CE �� �����
	RADIO_DDR &= ~(1 << RADIO_IRQ); // IRQ - �� ����
	csn_deassert();
	radio_deassert_ce();
	spi_init();
}

// ��������� ������� cmd, � ������ count ���� ������, ������� �� � ����� buf, ���������� ������� �������
uint8_t radio_read_buf(uint8_t cmd, uint8_t *buf, uint8_t count) {
	csn_assert();
	uint8_t status = spi_send_recv(cmd);
	while (count--) {
		*(buf++) = spi_send_recv(0xFF);
	}
	csn_deassert();
	return status;
}

// ��������� ������� cmd, � ������� count ���� ���������� �� ������ buf, ���������� ������� �������
uint8_t radio_write_buf(uint8_t cmd, uint8_t *buf, uint8_t count) {
	csn_assert();
	uint8_t status = spi_send_recv(cmd);
	while (count--) {
		spi_send_recv(*(buf++));
	}
	csn_deassert();
	return status;
}

// ������ �������� ������������� �������� reg (�� 0 �� 31) � ���������� ���
uint8_t radio_readreg(uint8_t reg) {
	csn_assert();
	spi_send_recv((reg & 31) | R_REGISTER);
	uint8_t answ = spi_send_recv(0xFF);
	csn_deassert();
	return answ;
}

// ���������� �������� ������������� �������� reg (�� 0 �� 31), ���������� ������� �������
uint8_t radio_writereg(uint8_t reg, uint8_t val) {
	csn_assert();
	uint8_t status = spi_send_recv((reg & 31) | W_REGISTER);
	spi_send_recv(val);
	csn_deassert();
	return status;
}

// ������ count ���� �������������� �������� reg (�� 0 �� 31) � ��������� ��� � ����� buf,
// ���������� ������� �������
uint8_t radio_readreg_buf(uint8_t reg, uint8_t *buf, uint8_t count) {
	return radio_read_buf((reg & 31) | R_REGISTER, buf, count);
}

// ���������� count ���� �� ������ buf � ������������� ������� reg (�� 0 �� 31), ���������� ������� �������
uint8_t radio_writereg_buf(uint8_t reg, uint8_t *buf, uint8_t count) {
	return radio_write_buf((reg & 31) | W_REGISTER, buf, count);
}

// ���������� ������ ������ � ������ FIFO ������� ��������
uint8_t radio_read_rx_payload_width() {
	csn_assert();
	spi_send_recv(R_RX_PL_WID);
	uint8_t answ = spi_send_recv(0xFF);
	csn_deassert();
	return answ;
}

// ��������� �������. ���������� ������� �������
uint8_t radio_cmd(uint8_t cmd) {
	csn_assert();
	uint8_t status = spi_send_recv(cmd);
	csn_deassert();
	return status;
}

// ���������� 1, ���� �� ����� IRQ �������� (������) �������.
uint8_t radio_is_interrupt() {
	return (RADIO_PIN & RADIO_IRQ) ? 0 : 1;
}

// ������� ���������� �������������� ��������� ����������. ���������� 1, � ������ ������, 0 � ������ ������
uint8_t radio_start() {
	uint8_t self_addr[] = { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 }; // ����������� �����
	uint8_t remote_addr[] = { 0xC2, 0xC2, 0xC2, 0xC2, 0xC2 }; // ����� �������� �������
	uint8_t chan = 3; // ����� �����-������ (� ��������� 0 - 125)

	radio_deassert_ce();
	for (uint8_t cnt = 100;;) {
		radio_writereg(CONFIG, (1 << EN_CRC) | (1 << CRCO) | (1 << PRIM_RX)); // ���������� �������
		if (radio_readreg(CONFIG)
				== ((1 << EN_CRC) | (1 << CRCO) | (1 << PRIM_RX)))
			break;
		// ���� ��������� �� �� ��� ��������, �� ������ ���� �����-��� ��� ����������������, ���� �� ��������.
		if (!cnt--)
			return 0; // ���� ����� 100 ������� �� ������� �������� ��� �����, �� ������� � �������
		_delay_ms(1);
	}

	radio_writereg(EN_AA, (1 << ENAA_P1)); // ��������� ����������������� ������ �� ������ 1
	radio_writereg(EN_RXADDR, (1 << ERX_P0) | (1 << ERX_P1)); // ��������� ������� 0 � 1
	radio_writereg(SETUP_AW, SETUP_AW_5BYTES_ADDRESS); // ����� ����� ������ 5 ����
	radio_writereg(SETUP_RETR,
			SETUP_RETR_DELAY_250MKS | SETUP_RETR_UP_TO_2_RETRANSMIT);
	radio_writereg(RF_CH, chan); // ����� ���������� ������
	radio_writereg(RF_SETUP, RF_SETUP_1MBPS | RF_SETUP_0DBM); // ����� �������� 1 ����/� � �������� 0dBm

	radio_writereg_buf(RX_ADDR_P0, &remote_addr[0], 5); // ������������� �������� �� ����� 0
	radio_writereg_buf(TX_ADDR, &remote_addr[0], 5);

	radio_writereg_buf(RX_ADDR_P1, &self_addr[0], 5);

	radio_writereg(RX_PW_P0, 0);
	radio_writereg(RX_PW_P1, 32);
	radio_writereg(DYNPD, (1 << DPL_P0) | (1 << DPL_P1)); // ��������� ������������ ����� ��� ������� 0 � 1
	radio_writereg(FEATURE, 0x04); // ���������� ������������ ����� ������ ������

	radio_writereg(CONFIG,
			(1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX)); // ��������� �������
	return (radio_readreg(CONFIG)
			== ((1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX))) ?
			1 : 0;
}
