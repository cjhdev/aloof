/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#ifndef ALOOF_DATA_H
#define ALOOF_DATA_H

#include <stdint.h>
#include <stdbool.h>

struct aloof_stream;

enum aloof_data_type {
        
    ALOOF_DATA_TYPE_NIL,
    
    ALOOF_DATA_TYPE_U8,
    ALOOF_DATA_TYPE_U16,
    ALOOF_DATA_TYPE_U32,
    ALOOF_DATA_TYPE_U64,
    
    ALOOF_DATA_TYPE_I8,
    ALOOF_DATA_TYPE_I16,
    ALOOF_DATA_TYPE_I32,
    ALOOF_DATA_TYPE_I64,
    
    ALOOF_DATA_TYPE_BOOL,
    
    ALOOF_DATA_TYPE_FLOAT,
    ALOOF_DATA_TYPE_DOUBLE,
    
    ALOOF_DATA_TYPE_STRING,
    ALOOF_DATA_TYPE_BLOB
    
};

struct aloof_data_rx {
    
    enum aloof_data_type type;
    
    union {

        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        
        bool bl;
        
        float fl;
        double db;
        
        struct {        
            uint32_t size;        
            uint32_t start;
            struct aloof_stream *s;
        } string;
        
    } value;
};

bool aloof_data_put_nil(struct aloof_stream *out);
bool aloof_data_put_u8(struct aloof_stream *out, uint8_t value);
bool aloof_data_put_u16(struct aloof_stream *out, uint16_t value);
bool aloof_data_put_u32(struct aloof_stream *out, uint32_t value);
bool aloof_data_put_u64(struct aloof_stream *out, uint64_t value);
bool aloof_data_put_i8(struct aloof_stream *out, int8_t value);
bool aloof_data_put_i16(struct aloof_stream *out, int16_t value);
bool aloof_data_put_i32(struct aloof_stream *out, int32_t value);
bool aloof_data_put_i64(struct aloof_stream *out, int64_t value);
bool aloof_data_put_bool(struct aloof_stream *out, bool value);
bool aloof_data_put_float(struct aloof_stream *out, float value);
bool aloof_data_put_double(struct aloof_stream *out, double value);
bool aloof_data_put_string(struct aloof_stream *out, const void *value, uint32_t size);
bool aloof_data_put_blob(struct aloof_stream *out, const void *value, uint32_t size);

bool aloof_data_get(struct aloof_stream *in, struct aloof_data_rx *data);
uint32_t aloof_data_get_string(struct aloof_data_rx *self, void *value, uint32_t max);
uint32_t aloof_data_get_string_size(const struct aloof_data_rx *self);

#endif
