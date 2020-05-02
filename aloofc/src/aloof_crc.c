/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#include "aloof_crc.h"

uint16_t aloof_crc_ccitt16_begin(void)
{
    return 0xffffU;
}

uint16_t aloof_crc_ccitt16_update(uint16_t crc, uint8_t c)
{
    uint8_t i;
    
    crc = crc ^ (((uint16_t)c) << 8);
    
    for(i=0U; i < 8U; i++){
        
        crc = (crc << 1U) ^ ((crc & (1U << 15U)) ? 0x1021U : 0U);        
    }
    
    return crc;
}

uint16_t aloof_crc_ccitt16_final(uint16_t crc)
{
    return crc;
}

uint16_t aloof_crc_ccitt16_block(const void *in, size_t len)
{
    size_t i;
    uint16_t crc = aloof_crc_ccitt16_begin();
    const uint8_t *ptr = (uint8_t *)in;
    
    for(i=0U; i < len; i++){
        
        crc = aloof_crc_ccitt16_update(crc, ptr[i]);
    }
    
    return aloof_crc_ccitt16_final(crc);
}

uint32_t aloof_crc_crc32_begin(void)
{
    return 0xffffffffUL;
}

uint32_t aloof_crc_crc32_update(uint32_t crc, uint8_t c)
{
    uint8_t i;
    
    crc ^= (uint32_t)c;
    
    for(i=0U; i < 8U; i++){
        
        if((crc & 1U) != 0U){
            
            crc = (crc >> 1U) ^ 0xEDB88320UL;
        }
        else{
            
            crc = crc >> 1U;
        }        
    }
    
    return crc;
}

uint32_t aloof_crc_crc32_final(uint32_t crc)
{
    return ~crc;
}

uint32_t aloof_crc_crc32_block(const void *in, size_t len)
{
    size_t i;
    uint32_t crc = aloof_crc_crc32_begin();
    const uint8_t *ptr = (uint8_t *)in;
    
    for(i=0U; i < len; i++){
        
        crc = aloof_crc_crc32_update(crc, ptr[i]);
    }
    
    return aloof_crc_crc32_final(crc);
}
