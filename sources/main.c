/*
 * main.c
 *
 *  Created on: 25 июн. 2020 г.
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

// Вызывается, когда превышено число попыток отправки, а подтверждение так и не было получено.
void on_send_error() {
	// TODO здесь можно описать обработчик неудачной отправки

}

// Выбирает активное состояние (высокий уровень) на линии CE
inline void radio_assert_ce() {
	RADIO_PORT |= (1 << RADIO_CE); // Установка высокого уровня на линии CE
}

// Выбирает неактивное состояние (низкий уровень) на линии CE
inline void radio_deassert_ce() {
	RADIO_PORT &= ~(1 << RADIO_CE); // Установка низкого уровня на линии CE
}

// Помещает пакет в очередь отправки.
// buf - буфер с данными, size - длина данных (от 1 до 32)
uint8_t send_data(uint8_t *buf, uint8_t size) {
	radio_deassert_ce(); // Если в режиме приёма, то выключаем его
	uint8_t conf = radio_readreg(CONFIG);
	// Сбрасываем бит PRIM_RX, и включаем питание установкой PWR_UP
	uint8_t status = radio_writereg(CONFIG,
			(conf & ~(1 << PRIM_RX)) | (1 << PWR_UP));
	if (status & (1 << TX_FULL_STATUS)) // Если очередь передатчика заполнена, возвращаемся с ошибкой
		return 0;
	if (!(conf & (1 << PWR_UP))) // Если питание не было включено, то ждём, пока запустится осциллятор
		_delay_ms(2);
	radio_write_buf(W_TX_PAYLOAD, buf, size); // Запись данных на отправку
	radio_assert_ce(); // Импульс на линии CE приведёт к началу передачи
	_delay_us(15); // Нужно минимум 10мкс, возьмём с запасом
	radio_deassert_ce();
	return 1;
}

void check_radio() {
	if (!radio_is_interrupt()) // Если прерывания нет, то не задерживаемся
		return;
	uint8_t status = radio_cmd(NOP);
	radio_writereg(STATUS, status); // Просто запишем регистр обратно, тем самым сбросив биты прерываний

	if (status & ((1 << TX_DS) | (1 << MAX_RT))) { // Завершена передача успехом, или нет,
		if (status & (1 << MAX_RT)) { // Если достигнуто максимальное число попыток
			radio_cmd(FLUSH_TX); // Удалим последний пакет из очереди
			//on_send_error(); // Вызовем обработчик
		}
		if (!(radio_readreg(FIFO_STATUS) & (1 << TX_EMPTY))) { // Если в очереди передатчика есть что передавать
			radio_assert_ce(); // Импульс на линии CE приведёт к началу передачи
			_delay_us(15); // Нужно минимум 10мкс, возьмём с запасом
			radio_deassert_ce();
		} else {
			uint8_t conf = radio_readreg(CONFIG);
			radio_writereg(CONFIG, conf & ~(1 << PWR_UP)); // Если пусто, отключаем питание
		}
	}
	uint8_t protect = 4; // В очереди FIFO не должно быть более 3 пакетов. Если больше, значит что-то не так
	while (((status & (7 << RX_P_NO)) != (7 << RX_P_NO)) && protect--) { // Пока в очереди есть принятый пакет
		radio_cmd(FLUSH_RX); // во всех случаях выкидываем пришедший пакет.
		status = radio_cmd(NOP);
	}
}

// Основной цикл
int main(void) {
	uint8_t buf = 1;
	uint8_t size = 1;
	radio_init();
	while (!radio_start()) {
		_delay_ms(1000);
	}
	// Перед включением питания чипа и сигналом CE должно пройти время достаточное для начала работы осциллятора
	// Для типичных резонаторов с эквивалентной индуктивностью не более 30мГн достаточно 1.5 мс
	_delay_ms(2);

	radio_assert_ce();
	for (;;) {
		check_radio();
		send_data(buf, size);
		_delay_ms(100);

		// TODO здесь основной код программы
	}
}
