//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "stat.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "fs.h"
#ଅନ୍ତର୍ଭାଵ "sleeplock.h"
#ଅନ୍ତର୍ଭାଵ "file.h"
#ଅନ୍ତର୍ଭାଵ "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
argfd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pfd, ସଂରଚନା file **pf)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
  ସଂରଚନା file *f;

  argint(n, &fd);
  ଯଦି (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ଫେରନ୍ତୁ -1;
  ଯଦି (pfd)
    *pfd = fd;
  ଯଦି (pf)
    *pf = f;
  ଫେରନ୍ତୁ 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fdalloc(ସଂରଚନା file *f)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
  ସଂରଚନା proc *p = myproc();

  ପାଇଁ (fd = 0; fd < NOFILE; fd++) {
    ଯଦି (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ଫେରନ୍ତୁ fd;
    }
  }
  ଫେରନ୍ତୁ -1;
}

uint64
sys_dup(ଶୂନ୍ୟ)
{
  ସଂରଚନା file *f;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

  ଯଦି (argfd(0, 0, &f) < 0)
    ଫେରନ୍ତୁ -1;
  ଯଦି ((fd = fdalloc(f)) < 0)
    ଫେରନ୍ତୁ -1;
  filedup(f);
  ଫେରନ୍ତୁ fd;
}

uint64
sys_read(ଶୂନ୍ୟ)
{
  ସଂରଚନା file *f;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ଯଦି (argfd(0, 0, &f) < 0)
    ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ fileread(f, p, n);
}

uint64
sys_write(ଶୂନ୍ୟ)
{
  ସଂରଚନା file *f;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ଯଦି (argfd(0, 0, &f) < 0)
    ଫେରନ୍ତୁ -1;

  ଫେରନ୍ତୁ filewrite(f, p, n);
}

uint64
sys_close(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
  ସଂରଚନା file *f;

  ଯଦି (argfd(0, &fd, &f) < 0)
    ଫେରନ୍ତୁ -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ଫେରନ୍ତୁ 0;
}

uint64
sys_fstat(ଶୂନ୍ୟ)
{
  ସଂରଚନା file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ଯଦି (argfd(0, 0, &f) < 0)
    ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(ଶୂନ୍ୟ)
{
  ଅକ୍ଷର name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  ସଂରଚନା inode *dp, *ip;

  ଯଦି (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ଫେରନ୍ତୁ -1;

  begin_op();
  ଯଦି ((ip = namei(old)) == 0) {
    end_op();
    ଫେରନ୍ତୁ -1;
  }

  ilock(ip);
  ଯଦି (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    ଫେରନ୍ତୁ -1;
  }

  ଯଦି (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    ଫେରନ୍ତୁ -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ଯଦି ((dp = nameiparent(new, name)) == 0)
    ଯାଅ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ଯଦି (dp->nlink == 0) {
    iunlockput(dp);
    ଯାଅ bad;
  }
  ଯଦି (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    ଯାଅ bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  ଫେରନ୍ତୁ 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  ଫେରନ୍ତୁ -1;
}

// Is the directory dp empty except for "." and ".." ?
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
isdirempty(ସଂରଚନା inode *dp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା off;
  ସଂରଚନା dirent de;

  ପାଇଁ (off = 2 * ଆକାର(de); off < dp->size; off += ଆକାର(de)) {
    ଯଦି (readi(dp, 0, (uint64)&de, off, ଆକାର(de)) != ଆକାର(de))
      panic("isdirempty: readi");
    ଯଦି (de.inum != 0)
      ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ 1;
}

uint64
sys_unlink(ଶୂନ୍ୟ)
{
  ସଂରଚନା inode *ip, *dp;
  ସଂରଚନା dirent de;
  ଅକ୍ଷର name[DIRSIZ], path[MAXPATH];
  uint off;

  ଯଦି (argstr(0, path, MAXPATH) < 0)
    ଫେରନ୍ତୁ -1;

  begin_op();
  ଯଦି ((dp = nameiparent(path, name)) == 0) {
    end_op();
    ଫେରନ୍ତୁ -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ଯଦି (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    ଯାଅ bad;

  ଯଦି ((ip = dirlookup(dp, name, &off)) == 0)
    ଯାଅ bad;
  ilock(ip);

  ଯଦି (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ଯଦି (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    ଯାଅ bad;
  }

  memset(&de, 0, ଆକାର(de));
  ଯଦି (writei(dp, 0, (uint64)&de, off, ଆକାର(de)) != ଆକାର(de))
    panic("unlink: writei");
  ଯଦି (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  ଫେରନ୍ତୁ 0;

bad:
  iunlockput(dp);
  end_op();
  ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ସଂରଚନା inode *
create(ଅକ୍ଷର *path, short type, short major, short minor)
{
  ସଂରଚନା inode *ip, *dp;
  ଅକ୍ଷର name[DIRSIZ];

  ଯଦି ((dp = nameiparent(path, name)) == 0)
    ଫେରନ୍ତୁ 0;

  ilock(dp);

  ଯଦି (dp->nlink == 0) {
    iunlockput(dp);
    ଫେରନ୍ତୁ 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ଯଦି (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ଯଦି (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ଫେରନ୍ତୁ ip;
    iunlockput(ip);
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ଫେରନ୍ତୁ 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ଯଦି (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ଯଦି (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      ଯାଅ fail;
  }

  ଯଦି (dirlink(dp, name, ip->inum) < 0)
    ଯାଅ fail;

  ଯଦି (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ଫେରନ୍ତୁ ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ଫେରନ୍ତୁ 0;
}

uint64
sys_open(ଶୂନ୍ୟ)
{
  ଅକ୍ଷର path[MAXPATH];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, omode;
  ସଂରଚନା file *f;
  ସଂରଚନା inode *ip;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;

  argint(1, &omode);
  ଯଦି ((n = argstr(0, path, MAXPATH)) < 0)
    ଫେରନ୍ତୁ -1;

  begin_op();

  ଯଦି (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    ଯଦି (ip == 0) {
      end_op();
      ଫେରନ୍ତୁ -1;
    }
  } ଅନ୍ୟଥା {
    ଯଦି ((ip = namei(path)) == 0) {
      end_op();
      ଫେରନ୍ତୁ -1;
    }
    ilock(ip);
    ଯଦି (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      ଫେରନ୍ତୁ -1;
    }
  }

  ଯଦି (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    ଫେରନ୍ତୁ -1;
  }

  ଯଦି ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ଯଦି (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    ଫେରନ୍ତୁ -1;
  }

  ଯଦି (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } ଅନ୍ୟଥା {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ଯଦି ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  ଫେରନ୍ତୁ fd;
}

uint64
sys_mkdir(ଶୂନ୍ୟ)
{
  ଅକ୍ଷର path[MAXPATH];
  ସଂରଚନା inode *ip;

  begin_op();
  ଯଦି (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    ଫେରନ୍ତୁ -1;
  }
  iunlockput(ip);
  end_op();
  ଫେରନ୍ତୁ 0;
}

uint64
sys_mknod(ଶୂନ୍ୟ)
{
  ସଂରଚନା inode *ip;
  ଅକ୍ଷର path[MAXPATH];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ଯଦି ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    ଫେରନ୍ତୁ -1;
  }
  iunlockput(ip);
  end_op();
  ଫେରନ୍ତୁ 0;
}

uint64
sys_chdir(ଶୂନ୍ୟ)
{
  ଅକ୍ଷର path[MAXPATH];
  ସଂରଚନା inode *ip;
  ସଂରଚନା proc *p = myproc();

  begin_op();
  ଯଦି (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    ଫେରନ୍ତୁ -1;
  }
  ilock(ip);
  ଯଦି (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    ଫେରନ୍ତୁ -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  ଫେରନ୍ତୁ 0;
}

uint64
sys_exec(ଶୂନ୍ୟ)
{
  ଅକ୍ଷର path[MAXPATH], *argv[MAXARG];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ଯଦି (argstr(0, path, MAXPATH) < 0) {
    ଫେରନ୍ତୁ -1;
  }
  memset(argv, 0, ଆକାର(argv));
  ପାଇଁ (i = 0;; i++) {
    ଯଦି (i >= NELEM(argv)) {
      ଯାଅ bad;
    }
    ଯଦି (fetchaddr(uargv + ଆକାର(uint64) * i, (uint64 *)&uarg) < 0) {
      ଯାଅ bad;
    }
    ଯଦି (uarg == 0) {
      argv[i] = 0;
      ଭାଙ୍ଗ;
    }
    argv[i] = kalloc();
    ଯଦି (argv[i] == 0)
      ଯାଅ bad;
    ଯଦି (fetchstr(uarg, argv[i], PGSIZE) < 0)
      ଯାଅ bad;
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = kexec(path, argv);

  ପାଇଁ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ଫେରନ୍ତୁ ret;

bad:
  ପାଇଁ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ଫେରନ୍ତୁ -1;
}

uint64
sys_pipe(ଶୂନ୍ୟ)
{
  uint64 fdarray; // user pointer to array of two integers
  ସଂରଚନା file *rf, *wf;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd0, fd1;
  ସଂରଚନା proc *p = myproc();

  argaddr(0, &fdarray);
  ଯଦି (pipealloc(&rf, &wf) < 0)
    ଫେରନ୍ତୁ -1;
  fd0 = -1;
  ଯଦି ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ଯଦି (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ଫେରନ୍ତୁ -1;
  }
  ଯଦି (copyout(p->pagetable, p->sz, fdarray, (ଅକ୍ଷର *)&fd0, ଆକାର(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + ଆକାର(fd0), (ଅକ୍ଷର *)&fd1,
              ଆକାର(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ଫେରନ୍ତୁ -1;
  }
  ଫେରନ୍ତୁ 0;
}
