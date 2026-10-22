//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "stat.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "fs.h"
#شامل "sleeplock.h"
#شامل "file.h"
#شامل "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
ایستا صحیح
argfd(صحیح n, صحیح *pfd, ساختار file **pf)
{
  صحیح fd;
  ساختار file *f;

  argint(n, &fd);
  اگر (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    بازگشت -1;
  اگر (pfd)
    *pfd = fd;
  اگر (pf)
    *pf = f;
  بازگشت 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
ایستا صحیح
fdalloc(ساختار file *f)
{
  صحیح fd;
  ساختار proc *p = myproc();

  برای (fd = 0; fd < NOFILE; fd++) {
    اگر (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      بازگشت fd;
    }
  }
  بازگشت -1;
}

uint64
sys_dup(تهی)
{
  ساختار file *f;
  صحیح fd;

  اگر (argfd(0, 0, &f) < 0)
    بازگشت -1;
  اگر ((fd = fdalloc(f)) < 0)
    بازگشت -1;
  filedup(f);
  بازگشت fd;
}

uint64
sys_read(تهی)
{
  ساختار file *f;
  صحیح n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  اگر (argfd(0, 0, &f) < 0)
    بازگشت -1;
  بازگشت fileread(f, p, n);
}

uint64
sys_write(تهی)
{
  ساختار file *f;
  صحیح n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  اگر (argfd(0, 0, &f) < 0)
    بازگشت -1;

  بازگشت filewrite(f, p, n);
}

uint64
sys_close(تهی)
{
  صحیح fd;
  ساختار file *f;

  اگر (argfd(0, &fd, &f) < 0)
    بازگشت -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  بازگشت 0;
}

uint64
sys_fstat(تهی)
{
  ساختار file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  اگر (argfd(0, 0, &f) < 0)
    بازگشت -1;
  بازگشت filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(تهی)
{
  نویسه name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  ساختار inode *dp, *ip;

  اگر (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    بازگشت -1;

  begin_op();
  اگر ((ip = namei(old)) == 0) {
    end_op();
    بازگشت -1;
  }

  ilock(ip);
  اگر (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    بازگشت -1;
  }

  اگر (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    بازگشت -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  اگر ((dp = nameiparent(new, name)) == 0)
    برو bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  اگر (dp->nlink == 0) {
    iunlockput(dp);
    برو bad;
  }
  اگر (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    برو bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  بازگشت 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  بازگشت -1;
}

// Is the directory dp empty except for "." and ".." ?
ایستا صحیح
isdirempty(ساختار inode *dp)
{
  صحیح off;
  ساختار dirent de;

  برای (off = 2 * اندازه(de); off < dp->size; off += اندازه(de)) {
    اگر (readi(dp, 0, (uint64)&de, off, اندازه(de)) != اندازه(de))
      panic("isdirempty: readi");
    اگر (de.inum != 0)
      بازگشت 0;
  }
  بازگشت 1;
}

uint64
sys_unlink(تهی)
{
  ساختار inode *ip, *dp;
  ساختار dirent de;
  نویسه name[DIRSIZ], path[MAXPATH];
  uint off;

  اگر (argstr(0, path, MAXPATH) < 0)
    بازگشت -1;

  begin_op();
  اگر ((dp = nameiparent(path, name)) == 0) {
    end_op();
    بازگشت -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  اگر (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    برو bad;

  اگر ((ip = dirlookup(dp, name, &off)) == 0)
    برو bad;
  ilock(ip);

  اگر (ip->nlink < 1)
    panic("unlink: nlink < 1");
  اگر (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    برو bad;
  }

  memset(&de, 0, اندازه(de));
  اگر (writei(dp, 0, (uint64)&de, off, اندازه(de)) != اندازه(de))
    panic("unlink: writei");
  اگر (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  بازگشت 0;

bad:
  iunlockput(dp);
  end_op();
  بازگشت -1;
}

ایستا ساختار inode *
create(نویسه *path, short type, short major, short minor)
{
  ساختار inode *ip, *dp;
  نویسه name[DIRSIZ];

  اگر ((dp = nameiparent(path, name)) == 0)
    بازگشت 0;

  ilock(dp);

  اگر (dp->nlink == 0) {
    iunlockput(dp);
    بازگشت 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  اگر (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    بازگشت 0;
  }

  اگر ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    اگر (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      بازگشت ip;
    iunlockput(ip);
    بازگشت 0;
  }

  اگر ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    بازگشت 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  اگر (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    اگر (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      برو fail;
  }

  اگر (dirlink(dp, name, ip->inum) < 0)
    برو fail;

  اگر (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  بازگشت ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  بازگشت 0;
}

uint64
sys_open(تهی)
{
  نویسه path[MAXPATH];
  صحیح fd, omode;
  ساختار file *f;
  ساختار inode *ip;
  صحیح n;

  argint(1, &omode);
  اگر ((n = argstr(0, path, MAXPATH)) < 0)
    بازگشت -1;

  begin_op();

  اگر (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    اگر (ip == 0) {
      end_op();
      بازگشت -1;
    }
  } وگرنه {
    اگر ((ip = namei(path)) == 0) {
      end_op();
      بازگشت -1;
    }
    ilock(ip);
    اگر (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      بازگشت -1;
    }
  }

  اگر (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    بازگشت -1;
  }

  اگر ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    اگر (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    بازگشت -1;
  }

  اگر (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } وگرنه {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  اگر ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  بازگشت fd;
}

uint64
sys_mkdir(تهی)
{
  نویسه path[MAXPATH];
  ساختار inode *ip;

  begin_op();
  اگر (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    بازگشت -1;
  }
  iunlockput(ip);
  end_op();
  بازگشت 0;
}

uint64
sys_mknod(تهی)
{
  ساختار inode *ip;
  نویسه path[MAXPATH];
  صحیح major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  اگر ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    بازگشت -1;
  }
  iunlockput(ip);
  end_op();
  بازگشت 0;
}

uint64
sys_chdir(تهی)
{
  نویسه path[MAXPATH];
  ساختار inode *ip;
  ساختار proc *p = myproc();

  begin_op();
  اگر (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    بازگشت -1;
  }
  ilock(ip);
  اگر (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    بازگشت -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  بازگشت 0;
}

uint64
sys_exec(تهی)
{
  نویسه path[MAXPATH], *argv[MAXARG];
  صحیح i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  اگر (argstr(0, path, MAXPATH) < 0) {
    بازگشت -1;
  }
  memset(argv, 0, اندازه(argv));
  برای (i = 0;; i++) {
    اگر (i >= NELEM(argv)) {
      برو bad;
    }
    اگر (fetchaddr(uargv + اندازه(uint64) * i, (uint64 *)&uarg) < 0) {
      برو bad;
    }
    اگر (uarg == 0) {
      argv[i] = 0;
      بشکن;
    }
    argv[i] = kalloc();
    اگر (argv[i] == 0)
      برو bad;
    اگر (fetchstr(uarg, argv[i], PGSIZE) < 0)
      برو bad;
  }

  صحیح ret = kexec(path, argv);

  برای (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  بازگشت ret;

bad:
  برای (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  بازگشت -1;
}

uint64
sys_pipe(تهی)
{
  uint64 fdarray; // user pointer to array of two integers
  ساختار file *rf, *wf;
  صحیح fd0, fd1;
  ساختار proc *p = myproc();

  argaddr(0, &fdarray);
  اگر (pipealloc(&rf, &wf) < 0)
    بازگشت -1;
  fd0 = -1;
  اگر ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    اگر (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    بازگشت -1;
  }
  اگر (copyout(p->pagetable, p->sz, fdarray, (نویسه *)&fd0, اندازه(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + اندازه(fd0), (نویسه *)&fd1,
              اندازه(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    بازگشت -1;
  }
  بازگشت 0;
}
