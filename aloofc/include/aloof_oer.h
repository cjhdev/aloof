/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#ifndef ALOOF_OER_H
#define ALOOF_OER_H

#include <stdint.h>
#include <stdbool.h>

struct aloof_stream;

bool aloof_oer_putBool(struct aloof_stream *s, bool in);

bool aloof_oer_putUint(struct aloof_stream *s, uint32_t in);
bool aloof_oer_putInt(struct aloof_stream *s, int32_t in);

bool aloof_oer_putU64(struct aloof_stream *s, uint64_t in);
bool aloof_oer_putU32(struct aloof_stream *s, uint32_t in);
bool aloof_oer_putU16(struct aloof_stream *s, uint16_t in);
bool aloof_oer_putU8(struct aloof_stream *s, uint8_t in);

bool aloof_oer_putI64(struct aloof_stream *s, int64_t in);
bool aloof_oer_putI32(struct aloof_stream *s, int32_t in);
bool aloof_oer_putI16(struct aloof_stream *s, int16_t in);
bool aloof_oer_putI8(struct aloof_stream *s, int8_t in);

bool aloof_oer_putFloat(struct aloof_stream *s, float in);
bool aloof_oer_putDouble(struct aloof_stream *s, double in);

bool aloof_oer_putString(struct aloof_stream *s, const void *in, uint32_t len);

bool aloof_oer_getBool(struct aloof_stream *s, bool *out);

bool aloof_oer_getUint(struct aloof_stream *s, uint32_t *out);
bool aloof_oer_getInt(struct aloof_stream *s, int32_t *out);

bool aloof_oer_getU64(struct aloof_stream *s, uint64_t *out);
bool aloof_oer_getU32(struct aloof_stream *s, uint32_t *out);
bool aloof_oer_getU16(struct aloof_stream *s, uint16_t *out);
bool aloof_oer_getU8(struct aloof_stream *s, uint8_t *out);

bool aloof_oer_getI64(struct aloof_stream *s, int64_t *out);
bool aloof_oer_getI32(struct aloof_stream *s, int32_t *out);
bool aloof_oer_getI16(struct aloof_stream *s, int16_t *out);
bool aloof_oer_getI8(struct aloof_stream *s, int8_t *out);

bool aloof_oer_getFloat(struct aloof_stream *s, float *out);
bool aloof_oer_getDouble(struct aloof_stream *s, double *out);

bool aloof_oer_getString(struct aloof_stream *s, void *out, uint32_t max, uint32_t *len);

uint8_t aloof_oer_sizeofUint(uint32_t value);
uint8_t aloof_oer_sizeofInt(int32_t value);

#endif
