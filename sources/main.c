/*
 * main.c
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

// �������� ����� � ������� ��������.
// buf - ����� � �������, size - ����� ������ (�� 1 �� 32)
uint8_t send_data(uint8_t *buf, uint8_t size) {
	radio_deassert_ce(); // ���� � ������ �����, �� ��������� ���
	uint8_t conf = radio_readreg(CONFIG);
	// ���������� ��� PRIM_RX, � �������� ������� ���������� PWR_UP
	uint8_t status = radio_writereg(CONFIG,
			(conf & ~(1 << PRIM_RX)) | (1 << PWR_UP));
	if (status & (1 << TX_FULL_STATUS)) // ���� ������� ����������� ���������, ������������ � �������
		return 0;
	if (!(conf & (1 << PWR_UP))) // ���� ������� �� ���� ��������, �� ���, ���� ���������� ����������
		_delay_ms(2);
	radio_write_buf(W_TX_PAYLOAD, buf, size); // ������ ������ �� ��������
	radio_assert_ce(); // ������� �� ����� CE ������� � ������ ��������
	_delay_us(15); // ����� ������� 10���, ������ � �������
	radio_deassert_ce();
	return 1;
}

void check_radio() {
	if (!radio_is_interrupt()) // ���� ���������� ���, �� �� �������������
		return;
	uint8_t status = radio_cmd(NOP);
	radio_writereg(STATUS, status); // ������ ������� ������� �������, ��� ����� ������� ���� ����������

	if (status & ((1 << TX_DS) | (1 << MAX_RT))) { // ��������� �������� �������, ��� ���,
		if (status & (1 << MAX_RT)) { // ���� ���������� ������������ ����� �������
			radio_cmd(FLUSH_TX); // ������ ��������� ����� �� �������
			//on_send_error(); // ������� ����������
		}
		if (!(radio_readreg(FIFO_STATUS) & (1 << TX_EMPTY))) { // ���� � ������� ����������� ���� ��� ����������
			radio_assert_ce(); // ������� �� ����� CE ������� � ������ ��������
			_delay_us(15); // ����� ������� 10���, ������ � �������
			radio_deassert_ce();
		} else {
			uint8_t conf = radio_readreg(CONFIG);
			radio_writereg(CONFIG, conf & ~(1 << PWR_UP)); // ���� �����, ��������� �������
		}
	}
	uint8_t protect = 4; // � ������� FIFO �� ������ ���� ����� 3 �������. ���� ������, ������ ���-�� �� ���
	while (((status & (7 << RX_P_NO)) != (7 << RX_P_NO)) && protect--) { // ���� � ������� ���� �������� �����
		radio_cmd(FLUSH_RX); // �� ���� ������� ���������� ��������� �����.
		status = radio_cmd(NOP);
	}
}

// �������� ����
int main(void) {
	radio_init();
	while (!radio_start()) {
		_delay_ms(1000);
	}
	// ����� ���������� ������� ���� � �������� CE ������ ������ ����� ����������� ��� ������ ������ �����������
	// ��� �������� ����������� � ������������� �������������� �� ����� 30��� ���������� 1.5 ��
	_delay_ms(2);

	radio_assert_ce();

	for (;;) {
		check_radio();

		// TODO ����� �������� ��� ���������
	}
}
