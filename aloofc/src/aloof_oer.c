/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#include "aloof_oer.h"
#include "aloof_stream.h"

static bool encodeVarint(struct aloof_stream *s, bool isSigned, uint32_t in);
static bool decodeVarint(struct aloof_stream *s, bool isSigned, uint32_t *out);

/* functions **********************************************************/

bool aloof_oer_putBool(struct aloof_stream *s, bool in)
{
    return aloof_oer_putU8(s, in ? 1U : 0U);
}

bool aloof_oer_putUint(struct aloof_stream *s, uint32_t in)
{
    return encodeVarint(s, false, in);
}

bool aloof_oer_putInt(struct aloof_stream *s, int32_t in)
{
    return encodeVarint(s, true, (uint32_t)in);
}

bool aloof_oer_putU64(struct aloof_stream *s, uint64_t in)
{
    uint8_t buf[8U];
    
    buf[0] = in >> 56;
    buf[1] = in >> 48;
    buf[2] = in >> 40;
    buf[3] = in >> 32;
    buf[4] = in >> 24;
    buf[5] = in >> 16;
    buf[6] = in >> 8;
    buf[7] = in;
    
    return aloof_stream_write(s, buf, sizeof(buf));
}

bool aloof_oer_putU32(struct aloof_stream *s, uint32_t in)
{
    uint8_t buf[4U];
    
    buf[0] = in >> 24;
    buf[1] = in >> 16;
    buf[2] = in >> 8;
    buf[3] = in;
    
    return aloof_stream_write(s, buf, sizeof(buf));
}

bool aloof_oer_putU16(struct aloof_stream *s, uint16_t in)
{
    uint8_t buf[2U];
    
    buf[0] = in >> 8;
    buf[1] = in;
     
    return aloof_stream_write(s, buf, sizeof(buf));
}

bool aloof_oer_putU8(struct aloof_stream *s, uint8_t in)
{
    return aloof_stream_write(s, &in, sizeof(in));
}

bool aloof_oer_putI8(struct aloof_stream *s, int8_t in)
{
    return aloof_oer_putU8(s, (uint8_t)in);
}

bool aloof_oer_putI16(struct aloof_stream *s, int16_t in)
{
    return aloof_oer_putU16(s, (uint16_t)in);
}

bool aloof_oer_putI32(struct aloof_stream *s, int32_t in)
{
    return aloof_oer_putU32(s, (uint32_t)in);
}

bool aloof_oer_putI64(struct aloof_stream *s, int64_t in)
{
    return aloof_oer_putU64(s, (uint64_t)in);
}

bool aloof_oer_putFloat(struct aloof_stream *s, float in)
{
    return aloof_oer_putU32(s, (uint32_t)in); 
}

bool aloof_oer_putDouble(struct aloof_stream *s, double in)
{
    return aloof_oer_putU64(s, (uint64_t)in); 
}

bool aloof_oer_putString(struct aloof_stream *s, const void *in, uint32_t len)
{
    bool retval = false;
    
    if(encodeVarint(s, false, len)){
        
        retval = aloof_stream_write(s, in, len);
    }
    
    return retval;
}

bool aloof_oer_getUint(struct aloof_stream *s, uint32_t *out)
{
    return decodeVarint(s, false, out);
}

bool aloof_oer_getInt(struct aloof_stream *s, int32_t *out)
{
    return decodeVarint(s, true, (uint32_t *)out);
}

bool aloof_oer_getI64(struct aloof_stream *s, int64_t *out)
{
    return aloof_oer_getU64(s, (uint64_t *)out);
}

bool aloof_oer_getI32(struct aloof_stream *s, int32_t *out)
{
    return aloof_oer_getU32(s, (uint32_t *)out);
}

bool aloof_oer_getI16(struct aloof_stream *s, int16_t *out)
{
    return aloof_oer_getU16(s, (uint16_t *)out);
}

bool aloof_oer_getI8(struct aloof_stream *s, int8_t *out)
{
    return aloof_oer_getU8(s, (uint8_t *)out);
}

bool aloof_oer_getU64(struct aloof_stream *s, uint64_t *out)
{
    uint8_t buf[8U];
    uint8_t pos = 0U;
    bool retval = false;
    
    if(aloof_stream_read(s, buf, sizeof(buf))){
    
        *out = buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        
        retval = true;
    }
    
    return retval;
}

bool aloof_oer_getU32(struct aloof_stream *s, uint32_t *out)
{
    uint8_t buf[4U];
    uint8_t pos = 0U;
    bool retval = false;
    
    if(aloof_stream_read(s, buf, sizeof(buf))){
    
        *out = buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
    
        retval = true;
    }
    
    return retval;
}

bool aloof_oer_getU16(struct aloof_stream *s, uint16_t *out)
{
    uint8_t buf[2U];
    uint8_t pos = 0U;
    bool retval = false;
    
    if(aloof_stream_read(s, buf, sizeof(buf))){
    
        *out = buf[pos];
        pos++;
        
        *out <<= 8;
        *out |= buf[pos];
        pos++;
    
        retval = true;
    }
    
    return retval;
}

bool aloof_oer_getU8(struct aloof_stream *s, uint8_t *out)
{
    return aloof_stream_read(s, out, sizeof(*out));
}

bool aloof_oer_getFloat(struct aloof_stream *s, float *out)
{
    return aloof_oer_getU32(s, (uint32_t *)out);
}

bool aloof_oer_getDouble(struct aloof_stream *s, double *out)
{
    return aloof_oer_getU64(s, (uint64_t *)out);
}

bool aloof_oer_getString(struct aloof_stream *s, void *out, uint32_t max, uint32_t *len)
{
    bool retval = false;
    
    if(decodeVarint(s, false, len)){
        
        if(*len <= max){
        
            retval = aloof_stream_read(s, out, *len);
        }
    }
    
    return retval; 
}

bool aloof_oer_getBool(struct aloof_stream *in, bool *out)
{
    bool retval = false;
    uint8_t value;
    
    if(aloof_oer_getU8(in, &value)){

        if((value == 0x00U) || (value == 0x01U)){

            *out = (value == 0x00U) ? false : true;
            retval = true;            
        }
        else{

            /* boolean can only be one of two values */
        }        
    }

    return retval;
}

uint8_t aloof_oer_sizeofUint(uint32_t value)
{
    uint8_t retval;

    if(value <= 0x7fUL){
        retval = 1U;
    }
    else if(value <= 0x3fffUL){
        retval = 2U;            
    }
    else if(value <= 0xffffUL){
        retval = 3U;
    }
    else if(value <= 0xffffffUL){
        retval = 4U;
    }
    else{        
        retval = 5U;
    }
    
    return retval;
}

uint8_t aloof_oer_sizeofInt(int32_t value)
{
    uint8_t retval;

    if(value < 0){

        if(value >= -64L){
            retval = 1U;
        }
        else if(value >= -8192L){
            retval = 2U;
        }
        else if(value >= -32768L){
            retval = 3U;
        }
        else if(value >= -8388608L){
            retval = 4U;
        }            
        else{
            retval = 5U;
        }
    }
    else{

        if(value <= 0x3fL){
            retval = 1U;
        }
        else if(value <= 0x1fffL){
            retval = 2U;            
        }
        else if(value <= 0x7fffL){
            retval = 3U;
        }
        else if(value <= 0x7fffffL){
            retval = 4U;
        }
        else{
            retval = 5U;
        }
    }

    return retval;
}

/* static functions ***************************************************/

static bool encodeVarint(struct aloof_stream *s, bool isSigned, uint32_t in)
{
    uint8_t buffer[9U];
    uint8_t bytes = (isSigned) ? aloof_oer_sizeofInt((int32_t)in) : aloof_oer_sizeofUint(in);
    uint8_t i;

    if(bytes == 1U){

        *buffer = (uint8_t)(in & 0x7fU);
    }
    else if(bytes == 2U){

        buffer[0] = 0x80U | ((uint8_t)(in >> 8) & 0x3fU);
        buffer[1] = (uint8_t)(in);   
    }
    else{
        
        buffer[0] = 0xC0U | (bytes-2U);
        for(i=1U; i < bytes; i++){

            buffer[i] = (uint8_t)(in >> ((bytes-(i+1U))*8U));
        }            
    }
    
    return aloof_stream_write(s, buffer, bytes);    
}

static bool decodeVarint(struct aloof_stream *s, bool isSigned, uint32_t *out)
{
    uint8_t buffer[9U];
    bool retval = false;
    uint8_t bytes;
    uint8_t i;

    if(aloof_stream_read(s, buffer, 1U)){

        if(buffer[0] < 0xc0U){

            if(buffer[0] < 0x80U){

                if(isSigned && ((buffer[0] & 0x40U) == 0x40U)){

                    *out = 0xffffffc0U;                           
                }
                else{
                    
                    *out = 0x0U;
                }
                *out |= (uint64_t)buffer[0] & 0x7fU;
                retval = true;  
            }
            else{

                if(aloof_stream_read(s, &buffer[1], 1U)){

                    if(isSigned && ((buffer[1] & 0x80U) == 0x80U)){

                        *out = 0xffffff00UL;                               
                    }
                    else{
                        
                        *out = 0x0UL;
                    }
                    *out |= (uint64_t)buffer[0] & 0x3fU;
                    *out <<= 6U;
                    *out |= buffer[1];
                    retval = true;
                }
            }
        }
        else{

            bytes = (buffer[0] & 0x3fU) + 1U;

            if(bytes <= 4U){

                if(aloof_stream_read(s, &buffer[1], bytes)){

                    if(isSigned && ((buffer[1] & 0x80U) == 0x80U)){
                        
                        *out = 0xffff00UL;
                    }
                    
                    for(i=1U; i <= bytes; i++){

                        *out <<= 8;
                        *out |= buffer[i];                        
                    }

                    retval = true;
                }
            }
            else{

                /* varint too large */
            }
        }
    }

    return retval;
}
