/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#ifndef ALOOF_SLIP_H
#define ALOOF_SLIP_H

#include <stdint.h>
#include <stdbool.h>

typedef bool (*aloof_slip_put_fn)(void *user, uint8_t c);

struct aloof_slip {
        
    uint8_t *buf;
    uint32_t len;
    uint32_t max;
    bool escape;        
        
    void *state;
    
    aloof_slip_put_fn put_fn;
};

struct aloof_slip_init_arg {
    
    void *buf;
    uint32_t max;
    
    void *state;
    
    aloof_slip_put_fn put_fn;    
};

struct aloof_slip *aloof_slip_init(struct aloof_slip *self, const struct aloof_slip_init_arg *arg);
bool aloof_slip_decode(struct aloof_slip *self, uint8_t c, const void **out, uint32_t *size);
bool aloof_slip_put_start(struct aloof_slip *self);
bool aloof_slip_put(struct aloof_slip *self, uint8_t c);
bool aloof_slip_put_end(struct aloof_slip *self);

#endif
