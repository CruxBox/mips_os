#ifndef _EMUFS_H_
#define _EMUFS_H_


/*
 * Get abstract structure definitions
 */
#include <fs.h>
#include <vnode.h>

/*
 * Our structures
 */

struct emufs_vnode {
	struct vnode ev_v;		/* abstract vnode structure */
	struct emu_softc *ev_emu;	/* device */
	uint32_t ev_handle;		/* file handle */
};

struct emufs_fs {
	struct fs ef_fs;		/* abstract filesystem structure */
	struct emu_softc *ef_emu;	/* device */
	struct emufs_vnode *ef_root;	/* root vnode */
	struct vnodearray *ef_vnodes;	/* table of loaded vnodes */
};


#endif /* _EMUFS_H_ */
