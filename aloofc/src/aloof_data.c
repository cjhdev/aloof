/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#include "aloof_data.h"
#include "aloof_oer.h"
#include "aloof_stream.h"

/* functions **********************************************************/

bool aloof_data_put_nil(struct aloof_stream *out)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_NIL);
}

bool aloof_data_put_u8(struct aloof_stream *out, uint8_t value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_U8) && aloof_oer_putU8(out, value);        
}

bool aloof_data_put_u16(struct aloof_stream *out, uint16_t value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_U16) && aloof_oer_putU16(out, value);        
}

bool aloof_data_put_u32(struct aloof_stream *out, uint32_t value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_U32) && aloof_oer_putU32(out, value);        
}

bool aloof_data_put_u64(struct aloof_stream *out, uint64_t value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_U64) && aloof_oer_putU64(out, value);        
}

bool aloof_data_put_i8(struct aloof_stream *out, int8_t value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_I8) && aloof_oer_putI8(out, value);        
}

bool aloof_data_put_i16(struct aloof_stream *out, int16_t value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_I16) && aloof_oer_putI16(out, value);        
}

bool aloof_data_put_i32(struct aloof_stream *out, int32_t value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_I32) && aloof_oer_putI32(out, value);        
}

bool aloof_data_put_i64(struct aloof_stream *out, int64_t value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_I64) && aloof_oer_putI64(out, value);        
}

bool aloof_data_put_bool(struct aloof_stream *out, bool value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_BOOL) && aloof_oer_putBool(out, value);        
}

bool aloof_data_put_float(struct aloof_stream *out, float value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_FLOAT) && aloof_oer_putFloat(out, value);        
}

bool aloof_data_put_double(struct aloof_stream *out, double value)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_DOUBLE) && aloof_oer_putDouble(out, value);        
}

bool aloof_data_put_string(struct aloof_stream *out, const void *value, uint32_t size)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_STRING) && aloof_oer_putString(out, value, size);        
}

bool aloof_data_put_blob(struct aloof_stream *out, const void *value, uint32_t size)
{
    return aloof_oer_putU8(out, ALOOF_DATA_TYPE_BLOB) && aloof_oer_putString(out, value, size);        
}

bool aloof_data_get(struct aloof_stream *in, struct aloof_data_rx *data)
{
    uint8_t tag;
    bool retval = false;
    
    static const enum aloof_data_type types[] = {        
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
    
    if(aloof_oer_getU8(in, &tag)){
        
        if(tag < (sizeof(types)/sizeof(*types))){
        
            data->type = types[tag];
            
            switch(data->type){
            default:
            case ALOOF_DATA_TYPE_NIL:
                retval = true;
                break;
            case ALOOF_DATA_TYPE_U8:
                retval = aloof_oer_getU8(in, &data->value.u8);
                break;                
            case ALOOF_DATA_TYPE_U16:
                retval = aloof_oer_getU16(in, &data->value.u16);
                break;
            case ALOOF_DATA_TYPE_U32:
                retval = aloof_oer_getU32(in, &data->value.u32);
                break;
            case ALOOF_DATA_TYPE_U64:
                retval = aloof_oer_getU64(in, &data->value.u64);
                break;            
            case ALOOF_DATA_TYPE_I8:
                retval = aloof_oer_getI8(in, &data->value.i8);
                break;            
            case ALOOF_DATA_TYPE_I16:
                retval = aloof_oer_getI16(in, &data->value.i16);
                break;            
            case ALOOF_DATA_TYPE_I32:
                retval = aloof_oer_getI32(in, &data->value.i32);
                break;            
            case ALOOF_DATA_TYPE_I64:
                retval = aloof_oer_getI64(in, &data->value.i64);
                break;                        
            case ALOOF_DATA_TYPE_BOOL:
                retval = aloof_oer_getBool(in, &data->value.bl);
                break;                        
            case ALOOF_DATA_TYPE_FLOAT:
                retval = aloof_oer_getFloat(in, &data->value.fl);
                break;            
            case ALOOF_DATA_TYPE_DOUBLE:
                retval = aloof_oer_getDouble(in, &data->value.db);
                break;            
            case ALOOF_DATA_TYPE_STRING:
            case ALOOF_DATA_TYPE_BLOB:
                retval = aloof_oer_getUint(in, &data->value.string.size);
                data->value.string.start = aloof_stream_tell(in);
                data->value.string.s = in;                
                break;
            }           
        }
    }    
    
    return retval;
}

uint32_t aloof_data_get_string_size(const struct aloof_data_rx *self)
{
    uint32_t retval = 0U;
    
    switch(self->type){
    case ALOOF_DATA_TYPE_STRING:
    case ALOOF_DATA_TYPE_BLOB:
        retval = self->value.string.size;
        break;
    default:
        break;
    }    
    
    return retval;
}

uint32_t aloof_data_get_string(struct aloof_data_rx *self, void *value, uint32_t max)
{
    uint32_t retval = 0U;
    
    switch(self->type){
    case ALOOF_DATA_TYPE_STRING:
    case ALOOF_DATA_TYPE_BLOB:
    
        if(self->value.string.start < (aloof_stream_tell(self->value.string.s) + self->value.string.size)){
            
            retval = (aloof_stream_tell(self->value.string.s) + self->value.string.size) - self->value.string.start;
            
            retval = (retval > max) ? max : retval;
            
            if(aloof_stream_read(self->value.string.s, value, retval)){
            
                self->value.string.start += retval;
            }
            else{
                
                retval = 0U;
            }
        }
        break;
    
    default:
        break;
    }
    
    return retval;
}
