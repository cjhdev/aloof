/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#include "aloof.h"
#include "aloof_oer.h"
#include "aloof_stream.h"
#include "aloof_data.h"

#include <string.h>
#include <stddef.h>

enum aloof_message_type {
    
    ALOOF_MESSAGE_TYPE_HELLO,
    ALOOF_MESSAGE_TYPE_HELLOHELLO,
    ALOOF_MESSAGE_TYPE_ERROR,
    ALOOF_MESSAGE_TYPE_COMMAND,
    ALOOF_MESSAGE_TYPE_RESPONSE,
    ALOOF_MESSAGE_TYPE_ALERT
};

enum aloof_command_type {
    ALOOF_COMMAND_TYPE_WRITE,
    ALOOF_COMMAND_TYPE_READ    
};

enum aloof_response_type {
    ALOOF_RESPONSE_TYPE_WRITE,
    ALOOF_RESPONSE_TYPE_READ    
};

enum aloof_station {
    ALOOF_STATION_CLIENT,
    ALOOF_STATION_SERVER        
};

struct aloof_message_hello {
    
    enum aloof_station station;
    uint32_t version;
    uint32_t features;    
};

union aloof_values {

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
        
        const void *data;
        uint32_t size;
        
    } string;
};


static bool is_confirmed(uint8_t invoke_field);
static bool get_hello(struct aloof_server *self, struct aloof_stream *in, struct aloof_message_hello *msg);
static bool send_alert(struct aloof_server *self, struct aloof_stream *out, uint32_t id, enum aloof_data_type type, const union aloof_values *value);

/* functions **********************************************************/

void aloof_server_init(struct aloof_server *self, const struct aloof_server_init_arg *arg)
{
    (void)memset(self, 0, sizeof(*self));
    
    self->read = arg->read;
    self->write = arg->write;
}

void aloof_server_process(struct aloof_server *self, struct aloof_stream *in, struct aloof_stream *out)
{
    uint8_t message_type;
    uint8_t command_type;    
    uint8_t invoke_field;
    uint32_t oid;
    enum aloof_access_error access_error;
    struct aloof_message_hello msg;
    struct aloof_data_rx data;
    uint32_t start_offset;
    
    start_offset = aloof_stream_tell(in);
    
    if(aloof_oer_getU8(in, &message_type)){
            
        switch(message_type){
        default:
            break;
        case ALOOF_MESSAGE_TYPE_HELLOHELLO:
        case ALOOF_MESSAGE_TYPE_RESPONSE:
        case ALOOF_MESSAGE_TYPE_ERROR:
        case ALOOF_MESSAGE_TYPE_ALERT:
            break;
        case ALOOF_MESSAGE_TYPE_HELLO:
        
            if(get_hello(self, in, &msg)){
                
                (void)aloof_oer_putU8(out, ALOOF_MESSAGE_TYPE_HELLOHELLO);
                (void)aloof_oer_putU8(out, ALOOF_STATION_SERVER);
                (void)aloof_oer_putUint(out, self->version);
                (void)aloof_oer_putU32(out, self->features);                
            }
            break;
        
        case ALOOF_MESSAGE_TYPE_COMMAND:
            
            if(aloof_oer_getU8(in, &invoke_field) && aloof_oer_getU8(in, &command_type)){
                
                switch(command_type){
                default:
                    break;
                case ALOOF_COMMAND_TYPE_WRITE:
                
                    if(aloof_oer_getUint(in, &oid) && aloof_data_get(in, &data)){
                        
                        (void)aloof_oer_putU8(out, ALOOF_MESSAGE_TYPE_RESPONSE);    
                        (void)aloof_oer_putU8(out, invoke_field);
                        (void)aloof_oer_putU8(out, ALOOF_RESPONSE_TYPE_WRITE);
                    
                        uint32_t offset = aloof_stream_tell(in);
                    
                        if(self->write == NULL){
                            
                            (void)aloof_oer_putU8(out, 1U);
                            (void)aloof_oer_putU8(out, ALOOF_ACCESS_ERROR_UNKNOWN);
                        }
                        else{
                    
                            if(self->write(self, oid, &data, &access_error)){
                                            
                                (void)aloof_oer_putU8(out, 0U);
                            }
                            else{
                                
                                (void)aloof_oer_putU8(out, 1U);
                                (void)aloof_oer_putU8(out, access_error);
                            }               
                        }
                        
                        switch(data.type){
                        case ALOOF_DATA_TYPE_STRING:
                        case ALOOF_DATA_TYPE_BLOB:
                        
                            if(aloof_stream_tell(in) != (offset + data.value.string.size)){
                                
                                (void)aloof_stream_seek_set(in, offset + data.value.string.size);
                            }
                            break;
                            
                        default:
                            break;
                        }
                    }
                    break;
                
                case ALOOF_COMMAND_TYPE_READ:
                
                    if(aloof_oer_getUint(in, &oid)){
                    
                        (void)aloof_oer_putU8(out, ALOOF_MESSAGE_TYPE_RESPONSE);    
                        (void)aloof_oer_putU8(out, invoke_field);
                        (void)aloof_oer_putU8(out, ALOOF_RESPONSE_TYPE_READ);                                                                
                        
                        uint32_t offset = aloof_stream_tell(out);
                        
                        (void)aloof_oer_putU8(out, 0U);
                    
                        if(self->read == NULL){
                         
                            (void)aloof_oer_putU8(out, 1U);
                            (void)aloof_oer_putU8(out, ALOOF_ACCESS_ERROR_UNKNOWN);
                        }
                        else{
                            
                            if(!self->read(self, oid, &access_error, out)){
                                
                                (void)aloof_stream_seek_set(out, offset); 
                                
                                (void)aloof_oer_putU8(out, 1U);
                                (void)aloof_oer_putU8(out, access_error);
                            }  
                        }
                    }
                    break;
                }
            }
                        
            if(!is_confirmed(invoke_field)){
                
                aloof_stream_seek_set(out, start_offset);
            }                                                        
        }
    }
}

bool aloof_server_alert_nil(struct aloof_server *self, struct aloof_stream *out, uint32_t id)
{
    union aloof_values data = {0};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_NIL, &data);
}

bool aloof_server_alert_u8(struct aloof_server *self, struct aloof_stream *out, uint32_t id, uint8_t value)
{
    union aloof_values data = {.u8=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_U8, &data);
}

bool aloof_server_alert_u16(struct aloof_server *self, struct aloof_stream *out, uint32_t id, uint16_t value)
{
    union aloof_values data = {.u16=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_U16, &data);
}

bool aloof_server_alert_u32(struct aloof_server *self, struct aloof_stream *out, uint32_t id, uint32_t value)
{
    union aloof_values data = {.u32=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_U32, &data);
}

bool aloof_server_alert_u64(struct aloof_server *self, struct aloof_stream *out, uint32_t id, uint64_t value)
{
    union aloof_values data = {.u64=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_U64, &data);
}

bool aloof_server_alert_i8(struct aloof_server *self, struct aloof_stream *out, uint32_t id, int8_t value)
{
    union aloof_values data = {.i8=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_I8, &data);
}

bool aloof_server_alert_i16(struct aloof_server *self, struct aloof_stream *out, uint32_t id, int16_t value)
{
    union aloof_values data = {.i16=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_I16, &data);
}

bool aloof_server_alert_i32(struct aloof_server *self, struct aloof_stream *out, uint32_t id, int32_t value)
{
    union aloof_values data = {.i32=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_I32, &data);
}

bool aloof_server_alert_i64(struct aloof_server *self, struct aloof_stream *out, uint32_t id, int64_t value)
{
    union aloof_values data = {.i64=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_I64, &data);
}

bool aloof_server_alert_bool(struct aloof_server *self, struct aloof_stream *out, uint32_t id, bool value)
{
    union aloof_values data = {.bl=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_BOOL, &data);
}

bool aloof_server_alert_float(struct aloof_server *self, struct aloof_stream *out, uint32_t id, float value)
{
    union aloof_values data = {.fl=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_FLOAT, &data);
}

bool aloof_server_alert_double(struct aloof_server *self, struct aloof_stream *out, uint32_t id, double value)
{
    union aloof_values data = {.db=value};    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_DOUBLE, &data);
}

bool aloof_server_alert_string(struct aloof_server *self, struct aloof_stream *out, uint32_t id, const void *value, uint32_t size)
{
    union aloof_values data = {
        .string.data=value,
        .string.size=size
    };    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_STRING, &data);
}

bool aloof_server_alert_blob(struct aloof_server *self, struct aloof_stream *out, uint32_t id, const void *value, uint32_t size)
{
    union aloof_values data = {
        .string.data=value,
        .string.size=size
    };    
    return send_alert(self, out, id, ALOOF_DATA_TYPE_BLOB, &data);
}

/* static function ****************************************************/

static bool is_confirmed(uint8_t invoke_field)
{
    return (invoke_field & 0x80U) > 0U;
}

static bool get_hello(struct aloof_server *self, struct aloof_stream *in, struct aloof_message_hello *msg)
{
    bool retval = false;    
    uint8_t station;
    
    const enum aloof_station stations[] = {
        ALOOF_STATION_CLIENT,            
        ALOOF_STATION_SERVER        
    };
      
    if(aloof_oer_getU8(in, &station)){
      
        if(station < sizeof(stations)/sizeof(*stations)){
            
            msg->station = stations[station];
            
            if(aloof_oer_getUint(in, &msg->version)){
            
                if(aloof_oer_getU32(in, &msg->features)){
                
                    retval = true;
                }                
            }                        
        }
    }
    
    return retval;
}

static bool send_alert(struct aloof_server *self, struct aloof_stream *out, uint32_t id, enum aloof_data_type type, const union aloof_values *value)
{
    (void)self;
    
    bool retval = false;
    
    if(aloof_oer_putU8(out, ALOOF_MESSAGE_TYPE_ALERT)){
        
        if(aloof_oer_putUint(out, id)){
            
            switch(type){
            default:
                break;
            case ALOOF_DATA_TYPE_NIL:
                retval = aloof_data_put_nil(out);
                break;
            case ALOOF_DATA_TYPE_U8:
                retval = aloof_data_put_u8(out, value->u8);
                break;                
            case ALOOF_DATA_TYPE_U16:
                retval = aloof_data_put_u16(out, value->u16);
                break;
            case ALOOF_DATA_TYPE_U32:
                retval = aloof_data_put_u32(out, value->u32);
                break;
            case ALOOF_DATA_TYPE_U64:
                retval = aloof_data_put_u64(out, value->u64);
                break;            
            case ALOOF_DATA_TYPE_I8:
                retval = aloof_data_put_i8(out, value->i8);
                break;            
            case ALOOF_DATA_TYPE_I16:
                retval = aloof_data_put_i16(out, value->i16);
                break;            
            case ALOOF_DATA_TYPE_I32:
                retval = aloof_data_put_i32(out, value->i32);
                break;            
            case ALOOF_DATA_TYPE_I64:
                retval = aloof_data_put_i64(out, value->i64);
                break;                        
            case ALOOF_DATA_TYPE_BOOL:
                retval = aloof_data_put_bool(out, value->bl);
                break;                        
            case ALOOF_DATA_TYPE_FLOAT:
                retval = aloof_data_put_float(out, value->fl);
                break;            
            case ALOOF_DATA_TYPE_DOUBLE:
                retval = aloof_data_put_i8(out, value->db);
                break;            
            case ALOOF_DATA_TYPE_STRING:
            case ALOOF_DATA_TYPE_BLOB:
                retval = aloof_data_put_string(out, value->string.data, value->string.size);
                break;
            }
        }
    }
    
    return retval;        
}
