/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#include "aloof_slip.h"

#include <string.h>

enum {
    ALOOF_SLIP_END_CHAR = 192,
    ALOOF_SLIP_ESCAPE_CHAR = 219,
    ALOOF_SLIP_ESCAPE_END_CHAR = 220,
    ALOOF_SLIP_ESCAPE_ESCAPE_CHAR = 221
};

/* functions **********************************************************/

struct aloof_slip *aloof_slip_init(struct aloof_slip *self, const struct aloof_slip_init_arg *arg)
{
    (void)memset(self, 0, sizeof(*self));
    
    self->buf = arg->buf;
    self->max = arg->max;    
    
    self->state = arg->state;
    
    self->put_fn = arg->put_fn;    
    
    return self;
}

bool aloof_slip_decode(struct aloof_slip *self, uint8_t c, const void **out, uint32_t *size)
{
    bool retval = false;
    
    switch(c){                
    case ALOOF_SLIP_ESCAPE_CHAR:
    
        self->escape = true;
        break;
    
    case ALOOF_SLIP_END_CHAR:

        if(self->len > 0U){                

            *out = self->buf;
            *size = self->len;
            
            retval = true;
        }
        
        self->len = 0U;
        self->escape = false;                                                            
        break;
        
    default:

        if(self->escape){
            
            self->escape = false;
            
            switch(c){
            default:
            case ALOOF_SLIP_ESCAPE_END_CHAR:                    
                c = ALOOF_SLIP_END_CHAR;
                break;                    
            case ALOOF_SLIP_ESCAPE_ESCAPE_CHAR:
                c = ALOOF_SLIP_ESCAPE_CHAR;
                break;
            }                    
        }
        
        if(self->len < self->max){
            
            self->buf[self->len] = c;
            self->len++;
        }            
        break;
    }        
    
    return retval;
}

bool aloof_slip_encode(struct aloof_slip *self, const void *in, uint32_t size)
{
    uint32_t i;
    const uint8_t *ptr = (uint8_t *)in;
    
    for(i=0U;i<size;i++){
        
        aloof_slip_put(self, ptr[i]);
    }
    
    return true;
}

bool aloof_slip_encode_end(struct aloof_slip *self)
{
    while(!self->put_fn(self->state, ALOOF_SLIP_END_CHAR));
    
    return true;
}

bool aloof_slip_put(struct aloof_slip *self, uint8_t c)
{
    switch(c){
    case ALOOF_SLIP_END_CHAR:
        while(!self->put_fn(self->state, ALOOF_SLIP_ESCAPE_CHAR));
        while(!self->put_fn(self->state, ALOOF_SLIP_ESCAPE_END_CHAR));
        break;
    case ALOOF_SLIP_ESCAPE_CHAR:
        while(!self->put_fn(self->state, ALOOF_SLIP_ESCAPE_CHAR));
        while(!self->put_fn(self->state, ALOOF_SLIP_ESCAPE_ESCAPE_CHAR));
        break;
    default:
        while(!self->put_fn(self->state, c));
        break;
    }
    
    return true;
}
