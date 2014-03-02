#include "vcs_circbuff.h"
#include <string.h>
#include <stdlib.h>

struct vcs_circbuff_s
{
	short* buff;
	unsigned int samplesize;
	unsigned int delay;
	unsigned int in;
	unsigned int size;
};


struct vcs_circbuff_s* vcs_circbuff_alloc(unsigned int samplesize, unsigned int delay)
{
	struct vcs_circbuff_s *circ = (struct vcs_circbuff_s *) calloc(1, sizeof(struct vcs_circbuff_s));
	if (circ != 0) {
		circ->size = delay*2;
		circ->buff = (short*) calloc(samplesize*circ->size, sizeof(short));
		circ->in = delay;
		circ->samplesize = samplesize;
		circ->delay = delay;
	}
	return circ;
}

void vcs_circbuff_put(struct vcs_circbuff_s* self, short *data)	
{
	self->in++;
	if (self->in >= self->size) self->in = 0;
	memcpy(&self->buff[self->samplesize*self->in], data, self->samplesize*sizeof(short));
}

short*  vcs_circbuff_getcurrent(struct vcs_circbuff_s* self)
{
	int x = self->in - self->delay;
	if (x < 0) x += self->size;
	return self->buff + x*self->samplesize;
}

void vcs_circbuff_delete(struct vcs_circbuff_s* self) 
{
	if (self != 0) {
	   if (self->buff != 0) free(self->buff);
	   free(self);
	}
}