#ifndef VCS_CIRCBUFF_H
#define VCS_CIRCBUFF_H

#ifdef __cplusplus
extern "C"
{
#endif

struct vcs_circbuff_s;

struct vcs_circbuff_s* vcs_circbuff_alloc(unsigned int samplesize, unsigned int delay);

void vcs_circbuff_delete(struct vcs_circbuff_s* self);

void vcs_circbuff_put(struct vcs_circbuff_s* self, short *data);

short* vcs_circbuff_getcurrent(struct vcs_circbuff_s* self);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif