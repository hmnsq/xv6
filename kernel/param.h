#घोषणा NPROC       64                // maximum number of processes
#घोषणा NCPU        8                 // maximum number of CPUs
#घोषणा NOFILE      16                // open files per process
#घोषणा NFILE       100               // open files per system
#घोषणा NINODE      50                // maximum number of active i-nodes
#घोषणा NDEV        10                // maximum major device number
#घोषणा ROOTDEV     1                 // device number of file system root disk
#घोषणा MAXARG      32                // max exec arguments
#घोषणा MAXOPBLOCKS 10                // max # of blocks any FS op writes
#घोषणा LOGBLOCKS   (MAXOPBLOCKS * 3) // max data blocks in on-disk log
#घोषणा NBUF        (MAXOPBLOCKS * 3) // size of disk block cache
#घोषणा FSSIZE      2000              // size of file system in blocks
#घोषणा MAXPATH     128               // maximum file path name
#घोषणा USERSTACK   1                 // user stack pages
