/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#ifndef ALOOF_CRC_H
#define ALOOF_CRC_H

#include <stdint.h>
#include <stddef.h>

uint16_t aloof_crc_ccitt16_begin(void);
uint16_t aloof_crc_ccitt16_update(uint16_t crc, uint8_t c);
uint16_t aloof_crc_ccitt16_finish(uint16_t crc);
uint16_t aloof_crc_ccitt16_block(const void *in, size_t len);

uint32_t aloof_crc_crc32_begin(void);
uint32_t aloof_crc_crc32_update(uint32_t crc, uint8_t c);
uint32_t aloof_crc_crc32_finish(uint32_t crc);
uint32_t aloof_crc_crc32_block(const void *in, size_t len);

#endif
