/*
 * VFS operations involving the current directory.
 */

#include <types.h>
#include <kern/errno.h>
#include <stat.h>
#include <lib.h>
#include <uio.h>
#include <proc.h>
#include <current.h>
#include <vfs.h>
#include <fs.h>
#include <vnode.h>

/*
 * Get current directory as a vnode.
 */
int
vfs_getcurdir(struct vnode **ret)
{
	int rv = 0;

	spinlock_acquire(&curproc->p_lock);
	if (curproc->p_cwd!=NULL) {
		VOP_INCREF(curproc->p_cwd);
		*ret = curproc->p_cwd;
	}
	else {
		rv = ENOENT;
	}
	spinlock_release(&curproc->p_lock);

	return rv;
}

/*
 * Set current directory as a vnode.
 * The passed vnode must in fact be a directory.
 */
int
vfs_setcurdir(struct vnode *dir)
{
	struct vnode *old;
	mode_t vtype;
	int result;

	result = VOP_GETTYPE(dir, &vtype);
	if (result) {
		return result;
	}
	if (vtype != S_IFDIR) {
		return ENOTDIR;
	}

	VOP_INCREF(dir);

	spinlock_acquire(&curproc->p_lock);
	old = curproc->p_cwd;
	curproc->p_cwd = dir;
	spinlock_release(&curproc->p_lock);

	if (old!=NULL) {
		VOP_DECREF(old);
	}

	return 0;
}

/*
 * Set current directory to "none".
 */
int
vfs_clearcurdir(void)
{
	struct vnode *old;

	spinlock_acquire(&curproc->p_lock);
	old = curproc->p_cwd;
	curproc->p_cwd = NULL;
	spinlock_release(&curproc->p_lock);

	if (old!=NULL) {
		VOP_DECREF(old);
	}

	return 0;
}

/*
 * Set current directory, as a pathname. Use vfs_lookup to translate
 * it to a vnode.
 */
int
vfs_chdir(char *path)
{
	struct vnode *vn;
	int result;

	result = vfs_lookup(path, &vn);
	if (result) {
		return result;
	}
	result = vfs_setcurdir(vn);
	VOP_DECREF(vn);
	return result;
}

/*
 * Get current directory, as a pathname.
 * Use VOP_NAMEFILE to get the pathname and FSOP_GETVOLNAME to get the
 * volume name.
 */
int
vfs_getcwd(struct uio *uio)
{
	struct vnode *cwd;
	int result;
	const char *name;
	char colon=':';

	KASSERT(uio->uio_rw==UIO_READ);

	result = vfs_getcurdir(&cwd);
	if (result) {
		return result;
	}

	/* The current dir must be a directory, and thus it is not a device. */
	KASSERT(cwd->vn_fs != NULL);

	name = FSOP_GETVOLNAME(cwd->vn_fs);
	if (name==NULL) {
		vfs_biglock_acquire();
		name = vfs_getdevname(cwd->vn_fs);
		vfs_biglock_release();
	}
	KASSERT(name != NULL);

	result = uiomove((char *)name, strlen(name), uio);
	if (result) {
		goto out;
	}
	result = uiomove(&colon, 1, uio);
	if (result) {
		goto out;
	}

	result = VOP_NAMEFILE(cwd, uio);

 out:

	VOP_DECREF(cwd);
	return result;
}
