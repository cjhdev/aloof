/* Copyright (c) 2020 Cameron Harper
 * 
 * */

#ifndef ALOOF_STREAM_H
#define ALOOF_STREAM_H

#include <stdint.h>
#include <stdbool.h>

/** stream state */
struct aloof_stream {
    
    const uint8_t *in; /**< readable buffer */
    uint8_t *out;   /**< writeable buffer */
    uint32_t max;     /**< maximum size of buffer */
    uint32_t pos;     /**< current position */        
};

/** Write to a stream
 *
 * @param[in] self
 * @param[in] in buffer to write to stream
 * @param[in] size byte length of buffer to write to stream
 *
 * @return true if `size` bytes were written
 *
 * */
bool aloof_stream_write(struct aloof_stream *self, const void *ptr, uint32_t size);

/** Read from a stream
 *
 * @param[in] self
 * @param[out] out buffer of at least `bytesToRead` bytes
 * @param[in] size number of bytes to read from stream into buffer
 *
 * @return true if `size` bytes were read
 *
 * */
bool aloof_stream_read(struct aloof_stream *self, void *ptr, uint32_t size);

/** Init a read/write buffer
 *
 * @param[in] self
 * @param[in] buf buffer
 * @param[in] max length of buffer
 * 
 * */
struct aloof_stream *aloof_stream_init(struct aloof_stream *self, void *buf, uint32_t max);

/** Init a read only buffer
 *
 * @param[in] self
 * @param[in] buf buffer
 * @param[in] max length of buffer
 *
 * @return generic stream
 * 
 * */
struct aloof_stream *aloof_stream_init_ro(struct aloof_stream *self, const void *buf, uint32_t max);

/** Get current position
 *
 * @param[in] self
 * @return stream position from origin
 *
 * */
uint32_t aloof_stream_tell(const struct aloof_stream *self);

/** Set stream absolute position
 * 
 * @param[in] self
 * @param[in] offset byte offset
 *
 * @retval true  success
 * @retval false failure
 *
 * */
bool aloof_stream_seek_set(struct aloof_stream *self, uint32_t offset);

/** Set stream relative position
 * 
 * @param[in] self
 * @param[in] offset byte offset
 *
 * @retval true  success
 * @retval false failure
 *
 * */
bool aloof_stream_seek_cur(struct aloof_stream *self, int32_t offset);

#endif
