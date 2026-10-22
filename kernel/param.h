#تعریف_کن NPROC       64                // maximum number of processes
#تعریف_کن NCPU        8                 // maximum number of CPUs
#تعریف_کن NOFILE      16                // open files per process
#تعریف_کن NFILE       100               // open files per system
#تعریف_کن NINODE      50                // maximum number of active i-nodes
#تعریف_کن NDEV        10                // maximum major device number
#تعریف_کن ROOTDEV     1                 // device number of file system root disk
#تعریف_کن MAXARG      32                // max exec arguments
#تعریف_کن MAXOPBLOCKS 10                // max # of blocks any FS op writes
#تعریف_کن LOGBLOCKS   (MAXOPBLOCKS * 3) // max data blocks in on-disk log
#تعریف_کن NBUF        (MAXOPBLOCKS * 3) // size of disk block cache
#تعریف_کن FSSIZE      2000              // size of file system in blocks
#تعریف_کن MAXPATH     128               // maximum file path name
#تعریف_کن USERSTACK   1                 // user stack pages
