/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#include "aloof_stream.h"
#include <string.h>

/* current position, maximum position, adjustment (+/-) */
static bool adjustOffset(uint32_t *pos, uint32_t max, int32_t adjust);

/* functions **********************************************************/

bool aloof_stream_write(struct aloof_stream *self, const void *ptr, uint32_t size)
{       
    bool retval = false;
    
    if(self->out != NULL){
        
        if((self->max - self->pos) >= size){
            
            (void)memcpy(&self->out[self->pos], ptr, size);
            self->pos += size;
            retval = true;
        }
        else{

            /* buffer too short */
        }
    }
        

    return retval;
}

bool aloof_stream_read(struct aloof_stream *self, void *ptr, uint32_t size)
{    
    bool retval = false;
    
    if(self->in != NULL){
        if((self->max - self->pos) >= size){
            
            (void)memcpy(ptr, &self->in[self->pos], size);
            self->pos += size;
            retval = true;
        }
        else{

            /* buffer too short */
        }
    }
        

    return retval;
}

struct aloof_stream *aloof_stream_init_ro(struct aloof_stream *self, const void *buf, uint32_t max)
{
    (void)memset(self, 0, sizeof(*self));
    
    self->in = buf;        
    self->max = max;    
    
    return self;
}

struct aloof_stream *aloof_stream_init(struct aloof_stream *self, void *buf, uint32_t max)
{    
    (void)memset(self, 0, sizeof(*self));
    
    self->out = buf;
    self->in = buf;
    self->max = max;

    return self;
}

uint32_t aloof_stream_tell(const struct aloof_stream *self)
{
    return self->pos;
}

bool aloof_stream_seek_set(struct aloof_stream *self, uint32_t offset)
{
    bool retval = false;

    if(self->max >= offset){

        self->pos = offset;
        retval = true;
    }    
    
    return retval;
}

bool aloof_stream_seek_cur(struct aloof_stream *self, int32_t offset)
{
    return adjustOffset(&self->pos, self->max, offset);
}

/* static functions ***************************************************/

static bool adjustOffset(uint32_t *pos, uint32_t max, int32_t offset)
{
    bool retval = false;

    if(offset > 0){

        if(((*pos + (uint32_t)offset) > *pos) && ((*pos + (uint32_t)offset) <= max)){

            *pos += (uint32_t)offset;
            retval = true;
        }
    }
    else if(offset < 0){
    
        if((*pos + (uint32_t)offset) < *pos){

            *pos += (uint32_t)offset;
            retval = true;
        }
    }
    else{

        retval = true;
    }

    return retval;
}
