#ifndef _RING_H
#define _RING_H
#include <stdint.h>

typedef struct ring_buffer_t {
   uint8_t  *head, *tail;
   uint8_t  *buf;
   uint32_t size;
} ring_buffer_t;

void rb_reset( ring_buffer_t *rb );
uint32_t rb_bytes_free( const ring_buffer_t *rb );
uint32_t rb_bytes_used( const ring_buffer_t *rb );
uint32_t rb_is_full( const ring_buffer_t *rb );
uint32_t rb_is_empty( const ring_buffer_t *rb );
void * rb_memcpy_into( ring_buffer_t *dst, const void *src, uint32_t count );
void * rb_memcpy_from( void *dst, ring_buffer_t *src, uint32_t count );
uint32_t rb_get_byte( ring_buffer_t *rb, uint8_t  *c );
uint32_t rb_put_byte( ring_buffer_t *rb, uint8_t  c );
uint32_t rb_peek_byte( ring_buffer_t *rb, uint8_t *c, int offset);

#endif
