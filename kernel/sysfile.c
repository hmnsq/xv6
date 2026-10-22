//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
ਸਥਿਰ ਪੂਰਨ
argfd(ਪੂਰਨ n, ਪੂਰਨ *pfd, ਬਣਤਰ file **pf)
{
  ਪੂਰਨ fd;
  ਬਣਤਰ file *f;

  argint(n, &fd);
  ਜੇ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ਵਾਪਸ -1;
  ਜੇ (pfd)
    *pfd = fd;
  ਜੇ (pf)
    *pf = f;
  ਵਾਪਸ 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
ਸਥਿਰ ਪੂਰਨ
fdalloc(ਬਣਤਰ file *f)
{
  ਪੂਰਨ fd;
  ਬਣਤਰ proc *p = myproc();

  ਲਈ (fd = 0; fd < NOFILE; fd++) {
    ਜੇ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ਵਾਪਸ fd;
    }
  }
  ਵਾਪਸ -1;
}

uint64
sys_dup(ਖਾਲੀ)
{
  ਬਣਤਰ file *f;
  ਪੂਰਨ fd;

  ਜੇ (argfd(0, 0, &f) < 0)
    ਵਾਪਸ -1;
  ਜੇ ((fd = fdalloc(f)) < 0)
    ਵਾਪਸ -1;
  filedup(f);
  ਵਾਪਸ fd;
}

uint64
sys_read(ਖਾਲੀ)
{
  ਬਣਤਰ file *f;
  ਪੂਰਨ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ਜੇ (argfd(0, 0, &f) < 0)
    ਵਾਪਸ -1;
  ਵਾਪਸ fileread(f, p, n);
}

uint64
sys_write(ਖਾਲੀ)
{
  ਬਣਤਰ file *f;
  ਪੂਰਨ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ਜੇ (argfd(0, 0, &f) < 0)
    ਵਾਪਸ -1;

  ਵਾਪਸ filewrite(f, p, n);
}

uint64
sys_close(ਖਾਲੀ)
{
  ਪੂਰਨ fd;
  ਬਣਤਰ file *f;

  ਜੇ (argfd(0, &fd, &f) < 0)
    ਵਾਪਸ -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ਵਾਪਸ 0;
}

uint64
sys_fstat(ਖਾਲੀ)
{
  ਬਣਤਰ file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ਜੇ (argfd(0, 0, &f) < 0)
    ਵਾਪਸ -1;
  ਵਾਪਸ filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(ਖਾਲੀ)
{
  ਅੱਖਰ name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  ਬਣਤਰ inode *dp, *ip;

  ਜੇ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ਵਾਪਸ -1;

  begin_op();
  ਜੇ ((ip = namei(old)) == 0) {
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }

  ilock(ip);
  ਜੇ (ip->type == T_DIR) {
    iunlockput(ip);
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }

  ਜੇ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ਜੇ ((dp = nameiparent(new, name)) == 0)
    ਜਾਓ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ਜੇ (dp->nlink == 0) {
    iunlockput(dp);
    ਜਾਓ bad;
  }
  ਜੇ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    ਜਾਓ bad;
  }
  iunlockput(dp);
  iput(ip);

  ਸਿਰਾ_op();

  ਵਾਪਸ 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  ਸਿਰਾ_op();
  ਵਾਪਸ -1;
}

// Is the directory dp empty except for "." and ".." ?
ਸਥਿਰ ਪੂਰਨ
isdirempty(ਬਣਤਰ inode *dp)
{
  ਪੂਰਨ off;
  ਬਣਤਰ dirent de;

  ਲਈ (off = 2 * ਆਕਾਰ(de); off < dp->ਮਾਪ; off += ਆਕਾਰ(de)) {
    ਜੇ (readi(dp, 0, (uint64)&de, off, ਆਕਾਰ(de)) != ਆਕਾਰ(de))
      panic("isdirempty: readi");
    ਜੇ (de.inum != 0)
      ਵਾਪਸ 0;
  }
  ਵਾਪਸ 1;
}

uint64
sys_unlink(ਖਾਲੀ)
{
  ਬਣਤਰ inode *ip, *dp;
  ਬਣਤਰ dirent de;
  ਅੱਖਰ name[DIRSIZ], ਰਸਤਾ[MAXPATH];
  uint off;

  ਜੇ (argstr(0, ਰਸਤਾ, MAXPATH) < 0)
    ਵਾਪਸ -1;

  begin_op();
  ਜੇ ((dp = nameiparent(ਰਸਤਾ, name)) == 0) {
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ਜੇ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    ਜਾਓ bad;

  ਜੇ ((ip = dirlookup(dp, name, &off)) == 0)
    ਜਾਓ bad;
  ilock(ip);

  ਜੇ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ਜੇ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    ਜਾਓ bad;
  }

  memset(&de, 0, ਆਕਾਰ(de));
  ਜੇ (writei(dp, 0, (uint64)&de, off, ਆਕਾਰ(de)) != ਆਕਾਰ(de))
    panic("unlink: writei");
  ਜੇ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  ਸਿਰਾ_op();

  ਵਾਪਸ 0;

bad:
  iunlockput(dp);
  ਸਿਰਾ_op();
  ਵਾਪਸ -1;
}

ਸਥਿਰ ਬਣਤਰ inode *
create(ਅੱਖਰ *ਰਸਤਾ, ਛੋਟਾ type, ਛੋਟਾ major, ਛੋਟਾ minor)
{
  ਬਣਤਰ inode *ip, *dp;
  ਅੱਖਰ name[DIRSIZ];

  ਜੇ ((dp = nameiparent(ਰਸਤਾ, name)) == 0)
    ਵਾਪਸ 0;

  ilock(dp);

  ਜੇ (dp->nlink == 0) {
    iunlockput(dp);
    ਵਾਪਸ 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ਜੇ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ਵਾਪਸ 0;
  }

  ਜੇ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ਜੇ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ਵਾਪਸ ip;
    iunlockput(ip);
    ਵਾਪਸ 0;
  }

  ਜੇ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ਵਾਪਸ 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ਜੇ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ਜੇ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      ਜਾਓ fail;
  }

  ਜੇ (dirlink(dp, name, ip->inum) < 0)
    ਜਾਓ fail;

  ਜੇ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ਵਾਪਸ ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ਵਾਪਸ 0;
}

uint64
sys_open(ਖਾਲੀ)
{
  ਅੱਖਰ ਰਸਤਾ[MAXPATH];
  ਪੂਰਨ fd, omode;
  ਬਣਤਰ file *f;
  ਬਣਤਰ inode *ip;
  ਪੂਰਨ n;

  argint(1, &omode);
  ਜੇ ((n = argstr(0, ਰਸਤਾ, MAXPATH)) < 0)
    ਵਾਪਸ -1;

  begin_op();

  ਜੇ (omode & O_CREATE) {
    ip = create(ਰਸਤਾ, T_FILE, 0, 0);
    ਜੇ (ip == 0) {
      ਸਿਰਾ_op();
      ਵਾਪਸ -1;
    }
  } ਨਹੀਂਤਾਂ {
    ਜੇ ((ip = namei(ਰਸਤਾ)) == 0) {
      ਸਿਰਾ_op();
      ਵਾਪਸ -1;
    }
    ilock(ip);
    ਜੇ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      ਸਿਰਾ_op();
      ਵਾਪਸ -1;
    }
  }

  ਜੇ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }

  ਜੇ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ਜੇ (f)
      fileclose(f);
    iunlockput(ip);
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }

  ਜੇ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } ਨਹੀਂਤਾਂ {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ਜੇ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  ਸਿਰਾ_op();

  ਵਾਪਸ fd;
}

uint64
sys_mkdir(ਖਾਲੀ)
{
  ਅੱਖਰ ਰਸਤਾ[MAXPATH];
  ਬਣਤਰ inode *ip;

  begin_op();
  ਜੇ (argstr(0, ਰਸਤਾ, MAXPATH) < 0 || (ip = create(ਰਸਤਾ, T_DIR, 0, 0)) == 0) {
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }
  iunlockput(ip);
  ਸਿਰਾ_op();
  ਵਾਪਸ 0;
}

uint64
sys_mknod(ਖਾਲੀ)
{
  ਬਣਤਰ inode *ip;
  ਅੱਖਰ ਰਸਤਾ[MAXPATH];
  ਪੂਰਨ major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ਜੇ ((argstr(0, ਰਸਤਾ, MAXPATH)) < 0 ||
      (ip = create(ਰਸਤਾ, T_DEVICE, major, minor)) == 0) {
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }
  iunlockput(ip);
  ਸਿਰਾ_op();
  ਵਾਪਸ 0;
}

uint64
sys_chdir(ਖਾਲੀ)
{
  ਅੱਖਰ ਰਸਤਾ[MAXPATH];
  ਬਣਤਰ inode *ip;
  ਬਣਤਰ proc *p = myproc();

  begin_op();
  ਜੇ (argstr(0, ਰਸਤਾ, MAXPATH) < 0 || (ip = namei(ਰਸਤਾ)) == 0) {
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }
  ilock(ip);
  ਜੇ (ip->type != T_DIR) {
    iunlockput(ip);
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }
  iunlock(ip);
  iput(p->cwd);
  ਸਿਰਾ_op();
  p->cwd = ip;
  ਵਾਪਸ 0;
}

uint64
sys_exec(ਖਾਲੀ)
{
  ਅੱਖਰ ਰਸਤਾ[MAXPATH], *argv[MAXARG];
  ਪੂਰਨ i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ਜੇ (argstr(0, ਰਸਤਾ, MAXPATH) < 0) {
    ਵਾਪਸ -1;
  }
  memset(argv, 0, ਆਕਾਰ(argv));
  ਲਈ (i = 0;; i++) {
    ਜੇ (i >= NELEM(argv)) {
      ਜਾਓ bad;
    }
    ਜੇ (fetchaddr(uargv + ਆਕਾਰ(uint64) * i, (uint64 *)&uarg) < 0) {
      ਜਾਓ bad;
    }
    ਜੇ (uarg == 0) {
      argv[i] = 0;
      ਰੋਕੋ;
    }
    argv[i] = kalloc();
    ਜੇ (argv[i] == 0)
      ਜਾਓ bad;
    ਜੇ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      ਜਾਓ bad;
  }

  ਪੂਰਨ ret = kexec(ਰਸਤਾ, argv);

  ਲਈ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ਵਾਪਸ ret;

bad:
  ਲਈ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ਵਾਪਸ -1;
}

uint64
sys_pipe(ਖਾਲੀ)
{
  uint64 fdarray; // user pointer to array of two integers
  ਬਣਤਰ file *rf, *wf;
  ਪੂਰਨ fd0, fd1;
  ਬਣਤਰ proc *p = myproc();

  argaddr(0, &fdarray);
  ਜੇ (pipealloc(&rf, &wf) < 0)
    ਵਾਪਸ -1;
  fd0 = -1;
  ਜੇ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ਜੇ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ਵਾਪਸ -1;
  }
  ਜੇ (copyout(p->pagetable, p->sz, fdarray, (ਅੱਖਰ *)&fd0, ਆਕਾਰ(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + ਆਕਾਰ(fd0), (ਅੱਖਰ *)&fd1,
              ਆਕਾਰ(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ਵਾਪਸ -1;
  }
  ਵਾਪਸ 0;
}
