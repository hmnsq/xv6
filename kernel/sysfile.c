//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#অন্তর্ভাব "types.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "stat.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "proc.h"
#অন্তর্ভাব "fs.h"
#অন্তর্ভাব "sleeplock.h"
#অন্তর্ভাব "file.h"
#অন্তর্ভাব "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
স্থির পূর্ণাঙ্ক
argfd(পূর্ণাঙ্ক n, পূর্ণাঙ্ক *pfd, সংরচনা file **pf)
{
  পূর্ণাঙ্ক fd;
  সংরচনা file *f;

  argint(n, &fd);
  যদি (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    প্রত্যাগম -1;
  যদি (pfd)
    *pfd = fd;
  যদি (pf)
    *pf = f;
  প্রত্যাগম 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
স্থির পূর্ণাঙ্ক
fdalloc(সংরচনা file *f)
{
  পূর্ণাঙ্ক fd;
  সংরচনা proc *p = myproc();

  প্রতি (fd = 0; fd < NOFILE; fd++) {
    যদি (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      প্রত্যাগম fd;
    }
  }
  প্রত্যাগম -1;
}

uint64
sys_dup(শূন্য)
{
  সংরচনা file *f;
  পূর্ণাঙ্ক fd;

  যদি (argfd(0, 0, &f) < 0)
    প্রত্যাগম -1;
  যদি ((fd = fdalloc(f)) < 0)
    প্রত্যাগম -1;
  filedup(f);
  প্রত্যাগম fd;
}

uint64
sys_read(শূন্য)
{
  সংরচনা file *f;
  পূর্ণাঙ্ক n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  যদি (argfd(0, 0, &f) < 0)
    প্রত্যাগম -1;
  প্রত্যাগম fileread(f, p, n);
}

uint64
sys_write(শূন্য)
{
  সংরচনা file *f;
  পূর্ণাঙ্ক n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  যদি (argfd(0, 0, &f) < 0)
    প্রত্যাগম -1;

  প্রত্যাগম filewrite(f, p, n);
}

uint64
sys_close(শূন্য)
{
  পূর্ণাঙ্ক fd;
  সংরচনা file *f;

  যদি (argfd(0, &fd, &f) < 0)
    প্রত্যাগম -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  প্রত্যাগম 0;
}

uint64
sys_fstat(শূন্য)
{
  সংরচনা file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  যদি (argfd(0, 0, &f) < 0)
    প্রত্যাগম -1;
  প্রত্যাগম filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(শূন্য)
{
  বর্ণ name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  সংরচনা inode *dp, *ip;

  যদি (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    প্রত্যাগম -1;

  begin_op();
  যদি ((ip = namei(old)) == 0) {
    end_op();
    প্রত্যাগম -1;
  }

  ilock(ip);
  যদি (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    প্রত্যাগম -1;
  }

  যদি (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    প্রত্যাগম -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  যদি ((dp = nameiparent(new, name)) == 0)
    গচ্ছ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  যদি (dp->nlink == 0) {
    iunlockput(dp);
    গচ্ছ bad;
  }
  যদি (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    গচ্ছ bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  প্রত্যাগম 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  প্রত্যাগম -1;
}

// Is the directory dp empty except for "." and ".." ?
স্থির পূর্ণাঙ্ক
isdirempty(সংরচনা inode *dp)
{
  পূর্ণাঙ্ক off;
  সংরচনা dirent de;

  প্রতি (off = 2 * পরিমাণ(de); off < dp->size; off += পরিমাণ(de)) {
    যদি (readi(dp, 0, (uint64)&de, off, পরিমাণ(de)) != পরিমাণ(de))
      panic("isdirempty: readi");
    যদি (de.inum != 0)
      প্রত্যাগম 0;
  }
  প্রত্যাগম 1;
}

uint64
sys_unlink(শূন্য)
{
  সংরচনা inode *ip, *dp;
  সংরচনা dirent de;
  বর্ণ name[DIRSIZ], path[MAXPATH];
  uint off;

  যদি (argstr(0, path, MAXPATH) < 0)
    প্রত্যাগম -1;

  begin_op();
  যদি ((dp = nameiparent(path, name)) == 0) {
    end_op();
    প্রত্যাগম -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  যদি (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    গচ্ছ bad;

  যদি ((ip = dirlookup(dp, name, &off)) == 0)
    গচ্ছ bad;
  ilock(ip);

  যদি (ip->nlink < 1)
    panic("unlink: nlink < 1");
  যদি (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    গচ্ছ bad;
  }

  memset(&de, 0, পরিমাণ(de));
  যদি (writei(dp, 0, (uint64)&de, off, পরিমাণ(de)) != পরিমাণ(de))
    panic("unlink: writei");
  যদি (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  প্রত্যাগম 0;

bad:
  iunlockput(dp);
  end_op();
  প্রত্যাগম -1;
}

স্থির সংরচনা inode *
create(বর্ণ *path, short type, short major, short minor)
{
  সংরচনা inode *ip, *dp;
  বর্ণ name[DIRSIZ];

  যদি ((dp = nameiparent(path, name)) == 0)
    প্রত্যাগম 0;

  ilock(dp);

  যদি (dp->nlink == 0) {
    iunlockput(dp);
    প্রত্যাগম 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  যদি (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    প্রত্যাগম 0;
  }

  যদি ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    যদি (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      প্রত্যাগম ip;
    iunlockput(ip);
    প্রত্যাগম 0;
  }

  যদি ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    প্রত্যাগম 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  যদি (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    যদি (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      গচ্ছ fail;
  }

  যদি (dirlink(dp, name, ip->inum) < 0)
    গচ্ছ fail;

  যদি (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  প্রত্যাগম ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  প্রত্যাগম 0;
}

uint64
sys_open(শূন্য)
{
  বর্ণ path[MAXPATH];
  পূর্ণাঙ্ক fd, omode;
  সংরচনা file *f;
  সংরচনা inode *ip;
  পূর্ণাঙ্ক n;

  argint(1, &omode);
  যদি ((n = argstr(0, path, MAXPATH)) < 0)
    প্রত্যাগম -1;

  begin_op();

  যদি (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    যদি (ip == 0) {
      end_op();
      প্রত্যাগম -1;
    }
  } অন্যথা {
    যদি ((ip = namei(path)) == 0) {
      end_op();
      প্রত্যাগম -1;
    }
    ilock(ip);
    যদি (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      প্রত্যাগম -1;
    }
  }

  যদি (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    প্রত্যাগম -1;
  }

  যদি ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    যদি (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    প্রত্যাগম -1;
  }

  যদি (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } অন্যথা {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  যদি ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  প্রত্যাগম fd;
}

uint64
sys_mkdir(শূন্য)
{
  বর্ণ path[MAXPATH];
  সংরচনা inode *ip;

  begin_op();
  যদি (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    প্রত্যাগম -1;
  }
  iunlockput(ip);
  end_op();
  প্রত্যাগম 0;
}

uint64
sys_mknod(শূন্য)
{
  সংরচনা inode *ip;
  বর্ণ path[MAXPATH];
  পূর্ণাঙ্ক major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  যদি ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    প্রত্যাগম -1;
  }
  iunlockput(ip);
  end_op();
  প্রত্যাগম 0;
}

uint64
sys_chdir(শূন্য)
{
  বর্ণ path[MAXPATH];
  সংরচনা inode *ip;
  সংরচনা proc *p = myproc();

  begin_op();
  যদি (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    প্রত্যাগম -1;
  }
  ilock(ip);
  যদি (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    প্রত্যাগম -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  প্রত্যাগম 0;
}

uint64
sys_exec(শূন্য)
{
  বর্ণ path[MAXPATH], *argv[MAXARG];
  পূর্ণাঙ্ক i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  যদি (argstr(0, path, MAXPATH) < 0) {
    প্রত্যাগম -1;
  }
  memset(argv, 0, পরিমাণ(argv));
  প্রতি (i = 0;; i++) {
    যদি (i >= NELEM(argv)) {
      গচ্ছ bad;
    }
    যদি (fetchaddr(uargv + পরিমাণ(uint64) * i, (uint64 *)&uarg) < 0) {
      গচ্ছ bad;
    }
    যদি (uarg == 0) {
      argv[i] = 0;
      ভঙ্গ;
    }
    argv[i] = kalloc();
    যদি (argv[i] == 0)
      গচ্ছ bad;
    যদি (fetchstr(uarg, argv[i], PGSIZE) < 0)
      গচ্ছ bad;
  }

  পূর্ণাঙ্ক ret = kexec(path, argv);

  প্রতি (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  প্রত্যাগম ret;

bad:
  প্রতি (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  প্রত্যাগম -1;
}

uint64
sys_pipe(শূন্য)
{
  uint64 fdarray; // user pointer to array of two integers
  সংরচনা file *rf, *wf;
  পূর্ণাঙ্ক fd0, fd1;
  সংরচনা proc *p = myproc();

  argaddr(0, &fdarray);
  যদি (pipealloc(&rf, &wf) < 0)
    প্রত্যাগম -1;
  fd0 = -1;
  যদি ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    যদি (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    প্রত্যাগম -1;
  }
  যদি (copyout(p->pagetable, p->sz, fdarray, (বর্ণ *)&fd0, পরিমাণ(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + পরিমাণ(fd0), (বর্ণ *)&fd1,
              পরিমাণ(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    প্রত্যাগম -1;
  }
  প্রত্যাগম 0;
}
