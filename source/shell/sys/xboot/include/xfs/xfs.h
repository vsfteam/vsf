#ifndef __XBOOT_XFS_H__
#define __XBOOT_XFS_H__

#include "utilities/ooc_class.h"
#include "component/vsf_component.h"
#include <xboot.h>

struct xfs_context_t {
	int dummy;
};

struct xfs_file_t {
	implement(vk_file_t)
};

struct xfs_file_t * xfs_open_read(struct xfs_context_t * ctx, const char * name);
s64_t xfs_length(struct xfs_file_t * file);
s64_t xfs_read(struct xfs_file_t * file, void * buf, s64_t size);
void xfs_close(struct xfs_file_t * file);
s64_t xfs_seek(struct xfs_file_t * file, s64_t offset);

#endif
