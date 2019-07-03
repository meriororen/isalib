#include "ring.h"
#include <string.h>

#define ASSERT( x ) do { if (!(x)) while(1); } while(0)
#define MIN( x, y ) ( x > y ? y : x )

void rb_init( ring_buffer_t *rb, uint8_t * buffer, uint32_t size)
{
	rb->size = size;
	rb->buf = &buffer[0];
	rb->head = &buffer[0];
	rb->tail = &buffer[0];
}

static inline uint32_t rb_buffer_size( const ring_buffer_t *rb )
{
	return rb->size;	
}

void rb_reset( ring_buffer_t *rb )
{
	rb->head = rb->tail = rb->buf;	
}

static inline uint32_t  rb_capacity( const ring_buffer_t *rb )
{
	return rb_buffer_size(rb) - 1;	
}

static const uint8_t * rb_end( const ring_buffer_t *rb )
{
	return rb->buf + rb_buffer_size(rb);	
}

uint32_t  rb_bytes_free( const ring_buffer_t *rb )
{
	if (rb->head >= rb->tail) {
		return rb_capacity(rb) - (rb->head - rb->tail);
	} else {
		return rb->tail - rb->head - 1;
	}
}

uint32_t  rb_bytes_used( const ring_buffer_t *rb )
{
	return rb_capacity(rb) - rb_bytes_free(rb);
}

uint32_t  rb_is_full( const ring_buffer_t *rb )
{
	return rb_bytes_free(rb) == 0;	
}

uint32_t  rb_is_empty( const ring_buffer_t *rb )
{
	return rb_bytes_free(rb) == rb_capacity(rb);	
}

static uint8_t * rb_nextp( ring_buffer_t *rb, const uint8_t *p )
{
	return rb->buf + ((++p - rb->buf) % rb_buffer_size(rb));	
}

void * rb_memcpy_into( ring_buffer_t *dst, const void *src, uint32_t  count )
{
	const uint8_t *u8src = src;
	const uint8_t *bufend = rb_end(dst);
	uint32_t  overflow = count > rb_bytes_free(dst);
	uint32_t  nread = 0;

	while (nread != count) {
		uint32_t  n = MIN(bufend - dst->head, count - nread);
		memcpy(dst->head, u8src + nread, n);
		dst->head += n;
		nread += n;

		/* wrap? */
		if (dst->head == bufend)
			dst->head = dst->buf;
	}

	if (overflow) {
		dst->tail = rb_nextp(dst, dst->head);
		ASSERT(rb_is_full(dst)); 
	}

	return dst->head;
}

void * rb_memcpy_from( void *dst, ring_buffer_t *src, uint32_t  count )
{
	uint32_t  bytes_used = rb_bytes_used(src);

	if (count > bytes_used) {
		return 0;
	}

	uint8_t *u8dst = dst;
	const uint8_t *bufend = rb_end(src);
	uint32_t  nwritten = 0;
	while (nwritten != count) {
		ASSERT(bufend > src->tail);
		uint32_t  n = MIN(bufend - src->tail, count - nwritten);
		memcpy(u8dst + nwritten, src->tail, n);
		src->tail += n;
		nwritten += n;

		/* wrap? */
		if (src->tail == bufend)
			src->tail = src->buf;
	}

	return src->head;
}

uint32_t  rb_get_byte( ring_buffer_t *rb, uint8_t *c )
{
	if (rb_is_empty(rb)) {
		return -1;
	}

	const uint8_t *bufend = rb_end(rb);
	*c = *(rb->tail);
	rb->tail++;

	if (rb->tail == bufend)
		rb->tail = rb->buf;

	return 0;
}

uint32_t  rb_put_byte( ring_buffer_t *rb, uint8_t c )
{
	if (rb_is_full(rb)) {
		return -1;
	}

	const uint8_t *bufend = rb_end(rb);
	*(rb->head) = c;
	rb->head++;

	if (rb->head == bufend) {
		rb->head = rb->buf;   
	}

	return 0;
}

uint32_t rb_peek_byte( ring_buffer_t *rb, uint8_t *c, int offset)
{
	if (rb_is_empty(rb)) return -1;

	const uint8_t *bufend = rb_end(rb);

	if (rb->tail + offset < bufend) {
		*c = *(rb->tail + offset);
	} else {
		*c = *(rb->head + offset - (bufend - rb->tail));
	}

	return 0;
}
