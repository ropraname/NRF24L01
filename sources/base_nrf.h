/*
 * nrf.h
 *
 *  Created on: 25 θών. 2020 γ.
 *      Author: ysatt
 */

#include <stdint.h>

#ifndef BASE_NRF_H_
#define BASE_NRF_H_

inline void radio_assert_ce();
inline void radio_deassert_ce();
inline static void csn_assert();
inline static void csn_deassert();
void radio_init();
uint8_t radio_read_buf(uint8_t cmd, uint8_t *buf, uint8_t count);
uint8_t radio_write_buf(uint8_t cmd, uint8_t *buf, uint8_t count);
uint8_t radio_readreg(uint8_t reg);
uint8_t radio_writereg(uint8_t reg, uint8_t val);
uint8_t radio_readreg_buf(uint8_t reg, uint8_t *buf, uint8_t count);
uint8_t radio_writereg_buf(uint8_t reg, uint8_t *buf, uint8_t count);
uint8_t radio_read_rx_payload_width();
uint8_t radio_cmd(uint8_t cmd);
uint8_t radio_is_interrupt();
uint8_t radio_start();

#endif /* BASE_NRF_H_ */
