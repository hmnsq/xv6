//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "stat.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "file.h"
#ಅನ್ತರ್ಭಾವ "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
argfd(ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ *pfd, ರಚನೆ file **pf)
{
  ಪೂರ್ಣಾಂಕ fd;
  ರಚನೆ file *f;

  argint(n, &fd);
  ಆದರೆ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ (pfd)
    *pfd = fd;
  ಆದರೆ (pf)
    *pf = f;
  ಹಿಂತಿರುಗಿ 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fdalloc(ರಚನೆ file *f)
{
  ಪೂರ್ಣಾಂಕ fd;
  ರಚನೆ proc *p = myproc();

  ಗಾಗಿ (fd = 0; fd < NOFILE; fd++) {
    ಆದರೆ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ಹಿಂತಿರುಗಿ fd;
    }
  }
  ಹಿಂತಿರುಗಿ -1;
}

uint64
sys_dup(ಶೂನ್ಯ)
{
  ರಚನೆ file *f;
  ಪೂರ್ಣಾಂಕ fd;

  ಆದರೆ (argfd(0, 0, &f) < 0)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ ((fd = fdalloc(f)) < 0)
    ಹಿಂತಿರುಗಿ -1;
  filedup(f);
  ಹಿಂತಿರುಗಿ fd;
}

uint64
sys_read(ಶೂನ್ಯ)
{
  ರಚನೆ file *f;
  ಪೂರ್ಣಾಂಕ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ಆದರೆ (argfd(0, 0, &f) < 0)
    ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ fileread(f, p, n);
}

uint64
sys_write(ಶೂನ್ಯ)
{
  ರಚನೆ file *f;
  ಪೂರ್ಣಾಂಕ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ಆದರೆ (argfd(0, 0, &f) < 0)
    ಹಿಂತಿರುಗಿ -1;

  ಹಿಂತಿರುಗಿ filewrite(f, p, n);
}

uint64
sys_close(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ fd;
  ರಚನೆ file *f;

  ಆದರೆ (argfd(0, &fd, &f) < 0)
    ಹಿಂತಿರುಗಿ -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ಹಿಂತಿರುಗಿ 0;
}

uint64
sys_fstat(ಶೂನ್ಯ)
{
  ರಚನೆ file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ಆದರೆ (argfd(0, 0, &f) < 0)
    ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(ಶೂನ್ಯ)
{
  ಅಕ್ಷರ name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  ರಚನೆ inode *dp, *ip;

  ಆದರೆ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ಹಿಂತಿರುಗಿ -1;

  begin_op();
  ಆದರೆ ((ip = namei(old)) == 0) {
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }

  ilock(ip);
  ಆದರೆ (ip->type == T_DIR) {
    iunlockput(ip);
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }

  ಆದರೆ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ಆದರೆ ((dp = nameiparent(new, name)) == 0)
    ಹೋಗು bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ಆದರೆ (dp->nlink == 0) {
    iunlockput(dp);
    ಹೋಗು bad;
  }
  ಆದರೆ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    ಹೋಗು bad;
  }
  iunlockput(dp);
  iput(ip);

  ಕೊನೆ_op();

  ಹಿಂತಿರುಗಿ 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  ಕೊನೆ_op();
  ಹಿಂತಿರುಗಿ -1;
}

// Is the directory dp empty except for "." and ".." ?
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
isdirempty(ರಚನೆ inode *dp)
{
  ಪೂರ್ಣಾಂಕ off;
  ರಚನೆ dirent de;

  ಗಾಗಿ (off = 2 * ಗಾತ್ರ(de); off < dp->ಗಾತ್ರ_2; off += ಗಾತ್ರ(de)) {
    ಆದರೆ (readi(dp, 0, (uint64)&de, off, ಗಾತ್ರ(de)) != ಗಾತ್ರ(de))
      panic("isdirempty: readi");
    ಆದರೆ (de.inum != 0)
      ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ 1;
}

uint64
sys_unlink(ಶೂನ್ಯ)
{
  ರಚನೆ inode *ip, *dp;
  ರಚನೆ dirent de;
  ಅಕ್ಷರ name[DIRSIZ], ಮಾರ್ಗ[MAXPATH];
  uint off;

  ಆದರೆ (argstr(0, ಮಾರ್ಗ, MAXPATH) < 0)
    ಹಿಂತಿರುಗಿ -1;

  begin_op();
  ಆದರೆ ((dp = nameiparent(ಮಾರ್ಗ, name)) == 0) {
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ಆದರೆ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    ಹೋಗು bad;

  ಆದರೆ ((ip = dirlookup(dp, name, &off)) == 0)
    ಹೋಗು bad;
  ilock(ip);

  ಆದರೆ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ಆದರೆ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    ಹೋಗು bad;
  }

  memset(&de, 0, ಗಾತ್ರ(de));
  ಆದರೆ (writei(dp, 0, (uint64)&de, off, ಗಾತ್ರ(de)) != ಗಾತ್ರ(de))
    panic("unlink: writei");
  ಆದರೆ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  ಕೊನೆ_op();

  ಹಿಂತಿರುಗಿ 0;

bad:
  iunlockput(dp);
  ಕೊನೆ_op();
  ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ರಚನೆ inode *
create(ಅಕ್ಷರ *ಮಾರ್ಗ, ಗಿಡ್ಡ type, ಗಿಡ್ಡ major, ಗಿಡ್ಡ minor)
{
  ರಚನೆ inode *ip, *dp;
  ಅಕ್ಷರ name[DIRSIZ];

  ಆದರೆ ((dp = nameiparent(ಮಾರ್ಗ, name)) == 0)
    ಹಿಂತಿರುಗಿ 0;

  ilock(dp);

  ಆದರೆ (dp->nlink == 0) {
    iunlockput(dp);
    ಹಿಂತಿರುಗಿ 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ಆದರೆ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ಆದರೆ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ಹಿಂತಿರುಗಿ ip;
    iunlockput(ip);
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ಹಿಂತಿರುಗಿ 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ಆದರೆ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ಆದರೆ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      ಹೋಗು fail;
  }

  ಆದರೆ (dirlink(dp, name, ip->inum) < 0)
    ಹೋಗು fail;

  ಆದರೆ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ಹಿಂತಿರುಗಿ ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ಹಿಂತಿರುಗಿ 0;
}

uint64
sys_open(ಶೂನ್ಯ)
{
  ಅಕ್ಷರ ಮಾರ್ಗ[MAXPATH];
  ಪೂರ್ಣಾಂಕ fd, omode;
  ರಚನೆ file *f;
  ರಚನೆ inode *ip;
  ಪೂರ್ಣಾಂಕ n;

  argint(1, &omode);
  ಆದರೆ ((n = argstr(0, ಮಾರ್ಗ, MAXPATH)) < 0)
    ಹಿಂತಿರುಗಿ -1;

  begin_op();

  ಆದರೆ (omode & O_CREATE) {
    ip = create(ಮಾರ್ಗ, T_FILE, 0, 0);
    ಆದರೆ (ip == 0) {
      ಕೊನೆ_op();
      ಹಿಂತಿರುಗಿ -1;
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ ((ip = namei(ಮಾರ್ಗ)) == 0) {
      ಕೊನೆ_op();
      ಹಿಂತಿರುಗಿ -1;
    }
    ilock(ip);
    ಆದರೆ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      ಕೊನೆ_op();
      ಹಿಂತಿರುಗಿ -1;
    }
  }

  ಆದರೆ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }

  ಆದರೆ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ಆದರೆ (f)
      fileclose(f);
    iunlockput(ip);
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }

  ಆದರೆ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } ಇಲ್ಲದಿದ್ದರೆ {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ಆದರೆ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  ಕೊನೆ_op();

  ಹಿಂತಿರುಗಿ fd;
}

uint64
sys_mkdir(ಶೂನ್ಯ)
{
  ಅಕ್ಷರ ಮಾರ್ಗ[MAXPATH];
  ರಚನೆ inode *ip;

  begin_op();
  ಆದರೆ (argstr(0, ಮಾರ್ಗ, MAXPATH) < 0 || (ip = create(ಮಾರ್ಗ, T_DIR, 0, 0)) == 0) {
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }
  iunlockput(ip);
  ಕೊನೆ_op();
  ಹಿಂತಿರುಗಿ 0;
}

uint64
sys_mknod(ಶೂನ್ಯ)
{
  ರಚನೆ inode *ip;
  ಅಕ್ಷರ ಮಾರ್ಗ[MAXPATH];
  ಪೂರ್ಣಾಂಕ major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ಆದರೆ ((argstr(0, ಮಾರ್ಗ, MAXPATH)) < 0 ||
      (ip = create(ಮಾರ್ಗ, T_DEVICE, major, minor)) == 0) {
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }
  iunlockput(ip);
  ಕೊನೆ_op();
  ಹಿಂತಿರುಗಿ 0;
}

uint64
sys_chdir(ಶೂನ್ಯ)
{
  ಅಕ್ಷರ ಮಾರ್ಗ[MAXPATH];
  ರಚನೆ inode *ip;
  ರಚನೆ proc *p = myproc();

  begin_op();
  ಆದರೆ (argstr(0, ಮಾರ್ಗ, MAXPATH) < 0 || (ip = namei(ಮಾರ್ಗ)) == 0) {
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }
  ilock(ip);
  ಆದರೆ (ip->type != T_DIR) {
    iunlockput(ip);
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }
  iunlock(ip);
  iput(p->cwd);
  ಕೊನೆ_op();
  p->cwd = ip;
  ಹಿಂತಿರುಗಿ 0;
}

uint64
sys_exec(ಶೂನ್ಯ)
{
  ಅಕ್ಷರ ಮಾರ್ಗ[MAXPATH], *argv[MAXARG];
  ಪೂರ್ಣಾಂಕ i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ಆದರೆ (argstr(0, ಮಾರ್ಗ, MAXPATH) < 0) {
    ಹಿಂತಿರುಗಿ -1;
  }
  memset(argv, 0, ಗಾತ್ರ(argv));
  ಗಾಗಿ (i = 0;; i++) {
    ಆದರೆ (i >= NELEM(argv)) {
      ಹೋಗು bad;
    }
    ಆದರೆ (fetchaddr(uargv + ಗಾತ್ರ(uint64) * i, (uint64 *)&uarg) < 0) {
      ಹೋಗು bad;
    }
    ಆದರೆ (uarg == 0) {
      argv[i] = 0;
      ನಿಲ್ಲಿಸು;
    }
    argv[i] = kalloc();
    ಆದರೆ (argv[i] == 0)
      ಹೋಗು bad;
    ಆದರೆ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      ಹೋಗು bad;
  }

  ಪೂರ್ಣಾಂಕ ret = kexec(ಮಾರ್ಗ, argv);

  ಗಾಗಿ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ಹಿಂತಿರುಗಿ ret;

bad:
  ಗಾಗಿ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ಹಿಂತಿರುಗಿ -1;
}

uint64
sys_pipe(ಶೂನ್ಯ)
{
  uint64 fdarray; // user pointer to array of two integers
  ರಚನೆ file *rf, *wf;
  ಪೂರ್ಣಾಂಕ fd0, fd1;
  ರಚನೆ proc *p = myproc();

  argaddr(0, &fdarray);
  ಆದರೆ (pipealloc(&rf, &wf) < 0)
    ಹಿಂತಿರುಗಿ -1;
  fd0 = -1;
  ಆದರೆ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ಆದರೆ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ಹಿಂತಿರುಗಿ -1;
  }
  ಆದರೆ (copyout(p->pagetable, p->sz, fdarray, (ಅಕ್ಷರ *)&fd0, ಗಾತ್ರ(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + ಗಾತ್ರ(fd0), (ಅಕ್ಷರ *)&fd1,
              ಗಾತ್ರ(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ಹಿಂತಿರುಗಿ -1;
  }
  ಹಿಂತಿರುಗಿ 0;
}
