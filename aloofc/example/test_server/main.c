#include "aloof.h"
#include "aloof_data.h"
#include "aloof_stream.h"

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdlib.h>

enum test_object_id {
    
    TEST_NIL,
    
    TEST_U8,
    TEST_U16,
    TEST_U32,
    TEST_U64,
    
    TEST_I8,
    TEST_I16,
    TEST_I32,
    TEST_I64,
    
    TEST_BOOL,
    
    TEST_FLOAT,
    TEST_DOUBLE,
    
    TEST_STRING,
    TEST_BLOB,
    
    TEST_UNKNOWN,
    TEST_NO_ACCESS,
    TEST_AUTH,
    TEST_ARG,
    TEST_TEMP,
    TEST_APP
    
};

struct object_def {
    
    enum test_object_id oid;
    enum aloof_data_type type;
};

static struct aloof_server server;

static bool app_reader(struct aloof_server *self, uint32_t oid, enum aloof_access_error *error, struct aloof_stream *out);
static bool app_writer(struct aloof_server *self, uint32_t oid, struct aloof_data_rx *data, enum aloof_access_error *error);

#define LOG(...) do{printf(__VA_ARGS__);printf("\n");}while(0)

int main(int argc, char **argv)
{
    int listener, client, n;
    struct sockaddr_in serv_addr, cli_addr;
    
    static uint8_t rx_buffer[1024U];
    static uint8_t tx_buffer[1024U];
    struct aloof_stream rx, tx;
    
    uint16_t header;
    uint16_t portno = 4567;
    
    {
        struct aloof_server_init_arg arg = {0};
        
        arg.read = app_reader;
        arg.write = app_writer;
        
        aloof_server_init(&server, &arg);
    }
    
    listener = socket(AF_INET, SOCK_STREAM, 0);
    
    assert(listener != -1);
    
    (void)memset(&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if(bind(listener, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        
        LOG("cannot bind to port %d", portno);
    }
    else{
    
        if(listen(listener, 5) == -1){
        
            LOG("listen()");
        }
        else{
    
            for(;;){
            
                socklen_t clilen = sizeof(cli_addr);
            
                LOG("waiting for connection on %u", portno);
            
                client = accept(listener, (struct sockaddr *)&cli_addr, &clilen);
                
                if(client == -1){
                    
                    break;
                }
            
                LOG("accepted connection");
         
                for(;;){
              
                    LOG("reading header");
              
                    n = read(client, &header, sizeof(header));

                    if(n == -1){
                        
                        break;
                    }
                    
                    if(n != sizeof(header)){
                        
                        break;
                    }
                    
                    if(header > sizeof(rx_buffer)){
                        
                        LOG("message too large for buffer");
                        break;
                    }
                          
                    LOG("reading %d bytes", header);

                    n = read(client, rx_buffer, header);
                    
                    if(n == -1){
                        
                        break;
                    }
                    
                    if(n != header){
                        
                        break;
                    }
                    
                    aloof_stream_init_ro(&rx, rx_buffer, header);
                    aloof_stream_init(&tx, tx_buffer, sizeof(tx_buffer));
                  
                    LOG("processing input");
                  
                    aloof_server_process(&server, &rx, &tx);
                  
                    if(aloof_stream_tell(&tx) > 0){
                      
                        header = aloof_stream_tell(&tx);
                        
                        LOG("writing %d bytes of output", header);
                      
                        write(client, &header, sizeof(header));
                        write(client, tx_buffer, header);
                    }
                    else{
                      
                        LOG("no output to write");
                    }                    
                }
                
                LOG("closing connection");
                close(client);
            }
        }
    }
    
    close(listener);    
    
    exit(EXIT_SUCCESS);
}

struct state_holder {
    
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
        char value[512];
        uint32_t size;
    } string;
    struct {
        uint8_t value[512];
        uint32_t size;
    } blob;
};

static struct state_holder state;

static const struct object_def *lookup_oid(uint32_t oid)
{
    const struct object_def *retval = NULL;
    size_t i;
    
    static const struct object_def objects[] = {
        {TEST_NIL, ALOOF_DATA_TYPE_NIL},
        
        {TEST_U8, ALOOF_DATA_TYPE_U8},
        {TEST_U16, ALOOF_DATA_TYPE_U16},
        {TEST_U32, ALOOF_DATA_TYPE_U32},
        {TEST_U64, ALOOF_DATA_TYPE_U64},
        
        {TEST_I8, ALOOF_DATA_TYPE_I8},
        {TEST_I16, ALOOF_DATA_TYPE_I16},
        {TEST_I32, ALOOF_DATA_TYPE_I32},
        {TEST_I64, ALOOF_DATA_TYPE_I64},
        
        {TEST_BOOL, ALOOF_DATA_TYPE_BOOL},
        
        {TEST_FLOAT, ALOOF_DATA_TYPE_FLOAT},
        {TEST_DOUBLE, ALOOF_DATA_TYPE_DOUBLE},
        
        {TEST_STRING, ALOOF_DATA_TYPE_STRING},
        {TEST_BLOB, ALOOF_DATA_TYPE_BLOB},
        
        {TEST_UNKNOWN, ALOOF_DATA_TYPE_NIL},
        {TEST_NO_ACCESS, ALOOF_DATA_TYPE_NIL},
        {TEST_AUTH, ALOOF_DATA_TYPE_NIL},
        {TEST_ARG, ALOOF_DATA_TYPE_NIL},
        {TEST_TEMP, ALOOF_DATA_TYPE_NIL},
        {TEST_APP, ALOOF_DATA_TYPE_NIL}
    };
    
    for(i=0U;i<sizeof(objects)/sizeof(*objects);i++){
        
        if(objects[i].oid == oid){
            
            retval = &objects[i];
            break;
        }
    }
    
    return retval;
}

static bool app_reader(struct aloof_server *self, uint32_t oid, enum aloof_access_error *error, struct aloof_stream *out)
{    
    bool retval = true;
    
    const struct object_def *obj = lookup_oid(oid);
    
    if(obj == NULL){
        
        retval = false;
        *error = ALOOF_ACCESS_ERROR_UNKNOWN;
    }
    else{
        
        switch(oid){
        case TEST_NIL:
            aloof_data_put_nil(out);
            break;    
        case TEST_U8:
            aloof_data_put_u8(out, state.u8);
            break;    
        case TEST_U16:
            aloof_data_put_u16(out, state.u16);
            break;    
        case TEST_U32:
            aloof_data_put_u32(out, state.u32);
            break;    
        case TEST_U64:
            aloof_data_put_u64(out, state.u64);
            break;    
        case TEST_I8:
            aloof_data_put_i8(out, state.i8);
            break;
        case TEST_I16:
            aloof_data_put_i16(out, state.i16);
            break;
        case TEST_I32:
            aloof_data_put_i32(out, state.i32);
            break;
        case TEST_I64:
            aloof_data_put_i64(out, state.i64);
            break;
        case TEST_BOOL:
            aloof_data_put_bool(out, state.bl);
            break;
        case TEST_FLOAT:
            aloof_data_put_float(out, state.fl);
            break;
        case TEST_DOUBLE:
            aloof_data_put_double(out, state.db);
            break;
        case TEST_STRING:
            aloof_data_put_string(out, state.string.value, state.string.size);
            break;
        case TEST_BLOB:
            aloof_data_put_string(out, state.blob.value, state.blob.size);
            break;
        case TEST_NO_ACCESS:
            *error = ALOOF_ACCESS_ERROR_ACCESS;
            retval = false;
            break;
        case TEST_AUTH:
            *error = ALOOF_ACCESS_ERROR_AUTHENTICATION;
            retval = false;
            break;
        case TEST_ARG:
            *error = ALOOF_ACCESS_ERROR_ARGUMENT;
            retval = false;
            break;
        case TEST_TEMP:
            *error = ALOOF_ACCESS_ERROR_TEMPORARY;
            retval = false;
            break;
        case TEST_APP:
            *error = ALOOF_ACCESS_ERROR_APPLICATION;
            retval = false;
            break;
            
        case TEST_UNKNOWN:
        default:  
            *error = ALOOF_ACCESS_ERROR_UNKNOWN;
            retval = false;
        }   
    }
    
    return retval; 
}

static bool app_writer(struct aloof_server *self, uint32_t oid, struct aloof_data_rx *data, enum aloof_access_error *error)
{
    bool retval = true;
    
    const struct object_def *obj = lookup_oid(oid);
    
    if(obj == NULL){
        
        retval = false;
        *error = ALOOF_ACCESS_ERROR_UNKNOWN;
    }
    else{
    
        if(obj->type != data->type){
            
            retval = false;
            *error = ALOOF_ACCESS_ERROR_ARGUMENT;
        }
        else{
    
            switch(oid){
            case TEST_NIL:
                break;    
            case TEST_U8:
                state.u8 = data->value.u8;
                break;    
            case TEST_U16:
                state.u16 = data->value.u16;
                break;    
            case TEST_U32:
                state.u32 = data->value.u32;
                break;     
            case TEST_U64:
                state.u64 = data->value.u64;
                break;    
            case TEST_I8:
                state.i8 = data->value.i8;
                break;    
            case TEST_I16:
                state.i16 = data->value.i16;
                break;    
            case TEST_I32:
                state.i32 = data->value.i32;
                break;    
            case TEST_I64:
                state.i64 = data->value.i64;
                break;    
            case TEST_BOOL:
                state.bl = data->value.bl;
                break;    
            case TEST_FLOAT:
                state.fl = data->value.fl; 
                break;
            case TEST_DOUBLE:
                state.db = data->value.db; 
                break;
            case TEST_STRING:    
                if(aloof_data_get_string_size(data) <= sizeof(state.string.value)){
                    
                    if(aloof_data_get_string(data, state.string.value, sizeof(state.string.value)) == aloof_data_get_string_size(data)){
                        
                        state.string.size = aloof_data_get_string_size(data);
                    }            
                    else{
                        
                        *error = ALOOF_ACCESS_ERROR_APPLICATION;
                        retval = false;
                    }
                }
                else{
                
                    *error = ALOOF_ACCESS_ERROR_APPLICATION;
                    retval = false;
                }        
                break;
            case TEST_BLOB:    
                if(aloof_data_get_string_size(data) <= sizeof(state.blob.value)){
                    
                    if(aloof_data_get_string(data, state.blob.value, sizeof(state.blob.value)) == aloof_data_get_string_size(data)){
                    
                        state.blob.size = aloof_data_get_string_size(data);
                    }            
                    else{
                        
                        *error = ALOOF_ACCESS_ERROR_APPLICATION;
                        retval = false;
                    }
                }
                else{
                
                    *error = ALOOF_ACCESS_ERROR_APPLICATION;
                    retval = false;
                }            
                break;
            case TEST_NO_ACCESS:
            {
                *error = ALOOF_ACCESS_ERROR_ACCESS;
                retval = false;
            }
                break;
            case TEST_AUTH:
            {
                *error = ALOOF_ACCESS_ERROR_AUTHENTICATION;
                retval = false;
            }
                break;
            case TEST_ARG:
            {
                *error = ALOOF_ACCESS_ERROR_ARGUMENT;
                retval = false;
            }
                break;
            case TEST_TEMP:
            {
                *error = ALOOF_ACCESS_ERROR_TEMPORARY;
                retval = false;
            }
                break;
            case TEST_APP:
            {
                *error = ALOOF_ACCESS_ERROR_APPLICATION;
                retval = false;
            }
                break;
            case TEST_UNKNOWN:
            default:  
                *error = ALOOF_ACCESS_ERROR_UNKNOWN;
                retval = false;
            }   
        }
    }
    
    return retval; 
}
