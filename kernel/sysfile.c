//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "stat.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "fs.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "file.h"
#അന്തര്ഭാവ "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
സ്ഥിതം പൂർണ്ണസംഖ്യ
argfd(പൂർണ്ണസംഖ്യ n, പൂർണ്ണസംഖ്യ *pfd, ഘടന file **pf)
{
  പൂർണ്ണസംഖ്യ fd;
  ഘടന file *f;

  argint(n, &fd);
  എങ്കിൽ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    തിരികെ -1;
  എങ്കിൽ (pfd)
    *pfd = fd;
  എങ്കിൽ (pf)
    *pf = f;
  തിരികെ 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
സ്ഥിതം പൂർണ്ണസംഖ്യ
fdalloc(ഘടന file *f)
{
  പൂർണ്ണസംഖ്യ fd;
  ഘടന proc *p = myproc();

  വേണ്ടി (fd = 0; fd < NOFILE; fd++) {
    എങ്കിൽ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      തിരികെ fd;
    }
  }
  തിരികെ -1;
}

uint64
sys_dup(ശൂന്യം)
{
  ഘടന file *f;
  പൂർണ്ണസംഖ്യ fd;

  എങ്കിൽ (argfd(0, 0, &f) < 0)
    തിരികെ -1;
  എങ്കിൽ ((fd = fdalloc(f)) < 0)
    തിരികെ -1;
  filedup(f);
  തിരികെ fd;
}

uint64
sys_read(ശൂന്യം)
{
  ഘടന file *f;
  പൂർണ്ണസംഖ്യ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  എങ്കിൽ (argfd(0, 0, &f) < 0)
    തിരികെ -1;
  തിരികെ fileread(f, p, n);
}

uint64
sys_write(ശൂന്യം)
{
  ഘടന file *f;
  പൂർണ്ണസംഖ്യ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  എങ്കിൽ (argfd(0, 0, &f) < 0)
    തിരികെ -1;

  തിരികെ filewrite(f, p, n);
}

uint64
sys_close(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ fd;
  ഘടന file *f;

  എങ്കിൽ (argfd(0, &fd, &f) < 0)
    തിരികെ -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  തിരികെ 0;
}

uint64
sys_fstat(ശൂന്യം)
{
  ഘടന file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  എങ്കിൽ (argfd(0, 0, &f) < 0)
    തിരികെ -1;
  തിരികെ filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(ശൂന്യം)
{
  അക്ഷരം name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  ഘടന inode *dp, *ip;

  എങ്കിൽ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    തിരികെ -1;

  begin_op();
  എങ്കിൽ ((ip = namei(old)) == 0) {
    end_op();
    തിരികെ -1;
  }

  ilock(ip);
  എങ്കിൽ (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    തിരികെ -1;
  }

  എങ്കിൽ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    തിരികെ -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  എങ്കിൽ ((dp = nameiparent(new, name)) == 0)
    പോകുക bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  എങ്കിൽ (dp->nlink == 0) {
    iunlockput(dp);
    പോകുക bad;
  }
  എങ്കിൽ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    പോകുക bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  തിരികെ 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  തിരികെ -1;
}

// Is the directory dp empty except for "." and ".." ?
സ്ഥിതം പൂർണ്ണസംഖ്യ
isdirempty(ഘടന inode *dp)
{
  പൂർണ്ണസംഖ്യ off;
  ഘടന dirent de;

  വേണ്ടി (off = 2 * വലുപ്പം(de); off < dp->size; off += വലുപ്പം(de)) {
    എങ്കിൽ (readi(dp, 0, (uint64)&de, off, വലുപ്പം(de)) != വലുപ്പം(de))
      panic("isdirempty: readi");
    എങ്കിൽ (de.inum != 0)
      തിരികെ 0;
  }
  തിരികെ 1;
}

uint64
sys_unlink(ശൂന്യം)
{
  ഘടന inode *ip, *dp;
  ഘടന dirent de;
  അക്ഷരം name[DIRSIZ], path[MAXPATH];
  uint off;

  എങ്കിൽ (argstr(0, path, MAXPATH) < 0)
    തിരികെ -1;

  begin_op();
  എങ്കിൽ ((dp = nameiparent(path, name)) == 0) {
    end_op();
    തിരികെ -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  എങ്കിൽ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    പോകുക bad;

  എങ്കിൽ ((ip = dirlookup(dp, name, &off)) == 0)
    പോകുക bad;
  ilock(ip);

  എങ്കിൽ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  എങ്കിൽ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    പോകുക bad;
  }

  memset(&de, 0, വലുപ്പം(de));
  എങ്കിൽ (writei(dp, 0, (uint64)&de, off, വലുപ്പം(de)) != വലുപ്പം(de))
    panic("unlink: writei");
  എങ്കിൽ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  തിരികെ 0;

bad:
  iunlockput(dp);
  end_op();
  തിരികെ -1;
}

സ്ഥിതം ഘടന inode *
create(അക്ഷരം *path, short type, short major, short minor)
{
  ഘടന inode *ip, *dp;
  അക്ഷരം name[DIRSIZ];

  എങ്കിൽ ((dp = nameiparent(path, name)) == 0)
    തിരികെ 0;

  ilock(dp);

  എങ്കിൽ (dp->nlink == 0) {
    iunlockput(dp);
    തിരികെ 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  എങ്കിൽ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    തിരികെ 0;
  }

  എങ്കിൽ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    എങ്കിൽ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      തിരികെ ip;
    iunlockput(ip);
    തിരികെ 0;
  }

  എങ്കിൽ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    തിരികെ 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  എങ്കിൽ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    എങ്കിൽ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      പോകുക fail;
  }

  എങ്കിൽ (dirlink(dp, name, ip->inum) < 0)
    പോകുക fail;

  എങ്കിൽ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  തിരികെ ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  തിരികെ 0;
}

uint64
sys_open(ശൂന്യം)
{
  അക്ഷരം path[MAXPATH];
  പൂർണ്ണസംഖ്യ fd, omode;
  ഘടന file *f;
  ഘടന inode *ip;
  പൂർണ്ണസംഖ്യ n;

  argint(1, &omode);
  എങ്കിൽ ((n = argstr(0, path, MAXPATH)) < 0)
    തിരികെ -1;

  begin_op();

  എങ്കിൽ (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    എങ്കിൽ (ip == 0) {
      end_op();
      തിരികെ -1;
    }
  } അല്ലെങ്കിൽ {
    എങ്കിൽ ((ip = namei(path)) == 0) {
      end_op();
      തിരികെ -1;
    }
    ilock(ip);
    എങ്കിൽ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      തിരികെ -1;
    }
  }

  എങ്കിൽ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    തിരികെ -1;
  }

  എങ്കിൽ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    എങ്കിൽ (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    തിരികെ -1;
  }

  എങ്കിൽ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } അല്ലെങ്കിൽ {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  എങ്കിൽ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  തിരികെ fd;
}

uint64
sys_mkdir(ശൂന്യം)
{
  അക്ഷരം path[MAXPATH];
  ഘടന inode *ip;

  begin_op();
  എങ്കിൽ (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    തിരികെ -1;
  }
  iunlockput(ip);
  end_op();
  തിരികെ 0;
}

uint64
sys_mknod(ശൂന്യം)
{
  ഘടന inode *ip;
  അക്ഷരം path[MAXPATH];
  പൂർണ്ണസംഖ്യ major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  എങ്കിൽ ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    തിരികെ -1;
  }
  iunlockput(ip);
  end_op();
  തിരികെ 0;
}

uint64
sys_chdir(ശൂന്യം)
{
  അക്ഷരം path[MAXPATH];
  ഘടന inode *ip;
  ഘടന proc *p = myproc();

  begin_op();
  എങ്കിൽ (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    തിരികെ -1;
  }
  ilock(ip);
  എങ്കിൽ (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    തിരികെ -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  തിരികെ 0;
}

uint64
sys_exec(ശൂന്യം)
{
  അക്ഷരം path[MAXPATH], *argv[MAXARG];
  പൂർണ്ണസംഖ്യ i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  എങ്കിൽ (argstr(0, path, MAXPATH) < 0) {
    തിരികെ -1;
  }
  memset(argv, 0, വലുപ്പം(argv));
  വേണ്ടി (i = 0;; i++) {
    എങ്കിൽ (i >= NELEM(argv)) {
      പോകുക bad;
    }
    എങ്കിൽ (fetchaddr(uargv + വലുപ്പം(uint64) * i, (uint64 *)&uarg) < 0) {
      പോകുക bad;
    }
    എങ്കിൽ (uarg == 0) {
      argv[i] = 0;
      നിർത്തുക;
    }
    argv[i] = kalloc();
    എങ്കിൽ (argv[i] == 0)
      പോകുക bad;
    എങ്കിൽ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      പോകുക bad;
  }

  പൂർണ്ണസംഖ്യ ret = kexec(path, argv);

  വേണ്ടി (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  തിരികെ ret;

bad:
  വേണ്ടി (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  തിരികെ -1;
}

uint64
sys_pipe(ശൂന്യം)
{
  uint64 fdarray; // user pointer to array of two integers
  ഘടന file *rf, *wf;
  പൂർണ്ണസംഖ്യ fd0, fd1;
  ഘടന proc *p = myproc();

  argaddr(0, &fdarray);
  എങ്കിൽ (pipealloc(&rf, &wf) < 0)
    തിരികെ -1;
  fd0 = -1;
  എങ്കിൽ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    എങ്കിൽ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    തിരികെ -1;
  }
  എങ്കിൽ (copyout(p->pagetable, p->sz, fdarray, (അക്ഷരം *)&fd0, വലുപ്പം(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + വലുപ്പം(fd0), (അക്ഷരം *)&fd1,
              വലുപ്പം(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    തിരികെ -1;
  }
  തിരികെ 0;
}
