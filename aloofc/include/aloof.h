/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#ifndef ALOOF_H
#define ALOOF_H

#include <stdint.h>
#include <stdbool.h>

struct aloof_stream;
struct aloof_server;
struct aloof_data_rx;

enum aloof_access_error {
    
    ALOOF_ACCESS_ERROR_UNKNOWN,
    ALOOF_ACCESS_ERROR_ACCESS,
    ALOOF_ACCESS_ERROR_AUTHENTICATION,
    ALOOF_ACCESS_ERROR_ARGUMENT,
    ALOOF_ACCESS_ERROR_TEMPORARY,
    ALOOF_ACCESS_ERROR_APPLICATION
};

/** The same signature is used for read and write commands, hence this is teh read/write function pointer
 * 
 * While the same signature works for both, there will in practice be two functions, one for
 * reading, the other for writing.
 * 
 * */
typedef bool (*aloof_server_read_fn)(struct aloof_server *self, uint32_t oid, enum aloof_access_error *error, struct aloof_stream *out);
typedef bool (*aloof_server_write_fn)(struct aloof_server *self, uint32_t oid, struct aloof_data_rx *data, enum aloof_access_error *error);

struct aloof_server_init_arg {
    
    aloof_server_read_fn read;
    aloof_server_write_fn write;    
};

struct aloof_server {
  
    uint32_t version;
    uint32_t features;
    aloof_server_read_fn read;
    aloof_server_write_fn write;    
};

void aloof_server_init(struct aloof_server *self, const struct aloof_server_init_arg *arg);
void aloof_server_process(struct aloof_server *self, struct aloof_stream *in, struct aloof_stream *out);

/* put unsolicited data as an alert */
bool aloof_server_alert_nil(struct aloof_server *self, struct aloof_stream *out, uint32_t id);
bool aloof_server_alert_u8(struct aloof_server *self, struct aloof_stream *out, uint32_t id, uint8_t value);
bool aloof_server_alert_u16(struct aloof_server *self, struct aloof_stream *out, uint32_t id, uint16_t value);
bool aloof_server_alert_u32(struct aloof_server *self, struct aloof_stream *out, uint32_t id, uint32_t value);
bool aloof_server_alert_u64(struct aloof_server *self, struct aloof_stream *out, uint32_t id, uint64_t value);
bool aloof_server_alert_i8(struct aloof_server *self, struct aloof_stream *out, uint32_t id, int8_t value);
bool aloof_server_alert_i16(struct aloof_server *self, struct aloof_stream *out, uint32_t id, int16_t value);
bool aloof_server_alert_i32(struct aloof_server *self, struct aloof_stream *out, uint32_t id, int32_t value);
bool aloof_server_alert_i64(struct aloof_server *self, struct aloof_stream *out, uint32_t id, int64_t value);
bool aloof_server_alert_bool(struct aloof_server *self, struct aloof_stream *out, uint32_t id, bool value);
bool aloof_server_alert_float(struct aloof_server *self, struct aloof_stream *out, uint32_t id, float value);
bool aloof_server_alert_double(struct aloof_server *self, struct aloof_stream *out, uint32_t id, double value);
bool aloof_server_alert_string(struct aloof_server *self, struct aloof_stream *out, uint32_t id, const void *value, uint32_t size);
bool aloof_server_alert_blob(struct aloof_server *self, struct aloof_stream *out, uint32_t id, const void *value, uint32_t size);

#endif
