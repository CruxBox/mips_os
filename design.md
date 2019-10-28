# File table design
- Consists of File descriptors and File Handler

File descriptors are integers that map to the array-fileHandler

## fileHandler needs to track:
- Pointer to file object, vnode
- Some sort of refcount (see note)
- A seeker
- Lock, since threads will be sharing complete table while forked processes will make a copy of the table

### Note:
- Forked processes will make a copy of the table but the "pointers" to vnode are different. This way I won't need a refcount as it was needed to fix the issue of "closing pointer of forked process shouldnt close pointer of another process"

### Design:
- Seeker can be a char*
- reader writer lock for locks.
- In work: Pointer to vnode?

### Helper functions:
- Function to return available fd
- Fuction to do deep-copy of table (for fork)

Since we are in kernel space, copyinout is not required. Which other function to use?V


