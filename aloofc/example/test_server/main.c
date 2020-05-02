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

enum test_objects {
    
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

static struct aloof_server server;

static bool app_reader(struct aloof_server *self, uint32_t oid, enum aloof_access_error *error, struct aloof_stream *out);
static bool app_writer(struct aloof_server *self, uint32_t oid, struct aloof_data_rx *data, enum aloof_access_error *error);

#define LOG(...) do{printf(__VA_ARGS__);printf("\n");}while(0)

int main(int argc, char **argv)
{
    int listener, client, n;
    struct sockaddr_in serv_addr, cli_addr;
    
    uint8_t rx_buffer[100U];
    uint8_t tx_buffer[100U];
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

static bool app_reader(struct aloof_server *self, uint32_t oid, enum aloof_access_error *error, struct aloof_stream *out)
{    
    bool retval = true;
    
    switch(oid){
    case TEST_NIL:
        aloof_data_put_nil(out);
        break;    
    case TEST_U8:
        aloof_data_put_u8(out, 0x80);
        break;    
    case TEST_U16:
        aloof_data_put_u16(out, 0x100);
        break;    
    case TEST_U32:
        aloof_data_put_u32(out, 0x10000);
        break;    
    case TEST_U64:
        aloof_data_put_u64(out, 0x100000000);
        break;    
    case TEST_I8:
        aloof_data_put_i8(out, -42);
        break;
    case TEST_I16:
        aloof_data_put_i16(out, -42);
        break;
    case TEST_I32:
        aloof_data_put_i32(out, -42);
        break;
    case TEST_I64:
        aloof_data_put_i64(out, -42);
        break;
    case TEST_BOOL:
        aloof_data_put_bool(out, true);
        break;
    case TEST_FLOAT:
        aloof_data_put_float(out, 42);
        break;
    case TEST_DOUBLE:
        aloof_data_put_double(out, 42);
        break;
    case TEST_STRING:
    {
        const char value[] = "hello";
        aloof_data_put_string(out, value, sizeof(value)-1U);
    }
        break;
    case TEST_BLOB:
    {
        const char value[] = "hello";
        aloof_data_put_blob(out, value, sizeof(value)-1U);
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
    
    return retval; 
}

static bool app_writer(struct aloof_server *self, uint32_t oid, struct aloof_data_rx *data, enum aloof_access_error *error)
{
    bool retval = true;
    
    switch(oid){
    case TEST_NIL:
        break;    
    case TEST_U8:
        break;    
    case TEST_U16:
        break;    
    case TEST_U32:
        break;    
    case TEST_U64:
        break;    
    case TEST_I8:
        break;
    case TEST_I16:
        break;
    case TEST_I32:
        break;
    case TEST_I64:
        break;
    case TEST_BOOL:
        break;
    case TEST_FLOAT:
        break;
    case TEST_DOUBLE:
        break;
    case TEST_STRING:    
        break;
    case TEST_BLOB:    
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
    
    return retval; 
}
