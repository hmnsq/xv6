//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "stat.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "file.h"
#अन्तर्भाव "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
स्थिर पूर्णाङ्क
argfd(पूर्णाङ्क n, पूर्णाङ्क *pfd, संरचना file **pf)
{
  पूर्णाङ्क fd;
  संरचना file *f;

  argint(n, &fd);
  सचे (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    पटिनिवत्त -1;
  सचे (pfd)
    *pfd = fd;
  सचे (pf)
    *pf = f;
  पटिनिवत्त 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
स्थिर पूर्णाङ्क
fdalloc(संरचना file *f)
{
  पूर्णाङ्क fd;
  संरचना proc *p = myproc();

  अत्थाय (fd = 0; fd < NOFILE; fd++) {
    सचे (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      पटिनिवत्त fd;
    }
  }
  पटिनिवत्त -1;
}

uint64
sys_dup(शून्य)
{
  संरचना file *f;
  पूर्णाङ्क fd;

  सचे (argfd(0, 0, &f) < 0)
    पटिनिवत्त -1;
  सचे ((fd = fdalloc(f)) < 0)
    पटिनिवत्त -1;
  filedup(f);
  पटिनिवत्त fd;
}

uint64
sys_read(शून्य)
{
  संरचना file *f;
  पूर्णाङ्क n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  सचे (argfd(0, 0, &f) < 0)
    पटिनिवत्त -1;
  पटिनिवत्त fileread(f, p, n);
}

uint64
sys_write(शून्य)
{
  संरचना file *f;
  पूर्णाङ्क n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  सचे (argfd(0, 0, &f) < 0)
    पटिनिवत्त -1;

  पटिनिवत्त filewrite(f, p, n);
}

uint64
sys_close(शून्य)
{
  पूर्णाङ्क fd;
  संरचना file *f;

  सचे (argfd(0, &fd, &f) < 0)
    पटिनिवत्त -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  पटिनिवत्त 0;
}

uint64
sys_fstat(शून्य)
{
  संरचना file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  सचे (argfd(0, 0, &f) < 0)
    पटिनिवत्त -1;
  पटिनिवत्त filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(शून्य)
{
  वर्ण name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  संरचना inode *dp, *ip;

  सचे (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    पटिनिवत्त -1;

  begin_op();
  सचे ((ip = namei(old)) == 0) {
    end_op();
    पटिनिवत्त -1;
  }

  ilock(ip);
  सचे (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    पटिनिवत्त -1;
  }

  सचे (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    पटिनिवत्त -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  सचे ((dp = nameiparent(new, name)) == 0)
    गच्छ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  सचे (dp->nlink == 0) {
    iunlockput(dp);
    गच्छ bad;
  }
  सचे (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    गच्छ bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  पटिनिवत्त 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  पटिनिवत्त -1;
}

// Is the directory dp empty except for "." and ".." ?
स्थिर पूर्णाङ्क
isdirempty(संरचना inode *dp)
{
  पूर्णाङ्क off;
  संरचना dirent de;

  अत्थाय (off = 2 * परिमाण(de); off < dp->size; off += परिमाण(de)) {
    सचे (readi(dp, 0, (uint64)&de, off, परिमाण(de)) != परिमाण(de))
      panic("isdirempty: readi");
    सचे (de.inum != 0)
      पटिनिवत्त 0;
  }
  पटिनिवत्त 1;
}

uint64
sys_unlink(शून्य)
{
  संरचना inode *ip, *dp;
  संरचना dirent de;
  वर्ण name[DIRSIZ], path[MAXPATH];
  uint off;

  सचे (argstr(0, path, MAXPATH) < 0)
    पटिनिवत्त -1;

  begin_op();
  सचे ((dp = nameiparent(path, name)) == 0) {
    end_op();
    पटिनिवत्त -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  सचे (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    गच्छ bad;

  सचे ((ip = dirlookup(dp, name, &off)) == 0)
    गच्छ bad;
  ilock(ip);

  सचे (ip->nlink < 1)
    panic("unlink: nlink < 1");
  सचे (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    गच्छ bad;
  }

  memset(&de, 0, परिमाण(de));
  सचे (writei(dp, 0, (uint64)&de, off, परिमाण(de)) != परिमाण(de))
    panic("unlink: writei");
  सचे (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  पटिनिवत्त 0;

bad:
  iunlockput(dp);
  end_op();
  पटिनिवत्त -1;
}

स्थिर संरचना inode *
create(वर्ण *path, short type, short major, short minor)
{
  संरचना inode *ip, *dp;
  वर्ण name[DIRSIZ];

  सचे ((dp = nameiparent(path, name)) == 0)
    पटिनिवत्त 0;

  ilock(dp);

  सचे (dp->nlink == 0) {
    iunlockput(dp);
    पटिनिवत्त 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  सचे (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    पटिनिवत्त 0;
  }

  सचे ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    सचे (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      पटिनिवत्त ip;
    iunlockput(ip);
    पटिनिवत्त 0;
  }

  सचे ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    पटिनिवत्त 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  सचे (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    सचे (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      गच्छ fail;
  }

  सचे (dirlink(dp, name, ip->inum) < 0)
    गच्छ fail;

  सचे (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  पटिनिवत्त ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  पटिनिवत्त 0;
}

uint64
sys_open(शून्य)
{
  वर्ण path[MAXPATH];
  पूर्णाङ्क fd, omode;
  संरचना file *f;
  संरचना inode *ip;
  पूर्णाङ्क n;

  argint(1, &omode);
  सचे ((n = argstr(0, path, MAXPATH)) < 0)
    पटिनिवत्त -1;

  begin_op();

  सचे (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    सचे (ip == 0) {
      end_op();
      पटिनिवत्त -1;
    }
  } अञ्ञथा {
    सचे ((ip = namei(path)) == 0) {
      end_op();
      पटिनिवत्त -1;
    }
    ilock(ip);
    सचे (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      पटिनिवत्त -1;
    }
  }

  सचे (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    पटिनिवत्त -1;
  }

  सचे ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    सचे (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    पटिनिवत्त -1;
  }

  सचे (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } अञ्ञथा {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  सचे ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  पटिनिवत्त fd;
}

uint64
sys_mkdir(शून्य)
{
  वर्ण path[MAXPATH];
  संरचना inode *ip;

  begin_op();
  सचे (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    पटिनिवत्त -1;
  }
  iunlockput(ip);
  end_op();
  पटिनिवत्त 0;
}

uint64
sys_mknod(शून्य)
{
  संरचना inode *ip;
  वर्ण path[MAXPATH];
  पूर्णाङ्क major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  सचे ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    पटिनिवत्त -1;
  }
  iunlockput(ip);
  end_op();
  पटिनिवत्त 0;
}

uint64
sys_chdir(शून्य)
{
  वर्ण path[MAXPATH];
  संरचना inode *ip;
  संरचना proc *p = myproc();

  begin_op();
  सचे (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    पटिनिवत्त -1;
  }
  ilock(ip);
  सचे (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    पटिनिवत्त -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  पटिनिवत्त 0;
}

uint64
sys_exec(शून्य)
{
  वर्ण path[MAXPATH], *argv[MAXARG];
  पूर्णाङ्क i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  सचे (argstr(0, path, MAXPATH) < 0) {
    पटिनिवत्त -1;
  }
  memset(argv, 0, परिमाण(argv));
  अत्थाय (i = 0;; i++) {
    सचे (i >= NELEM(argv)) {
      गच्छ bad;
    }
    सचे (fetchaddr(uargv + परिमाण(uint64) * i, (uint64 *)&uarg) < 0) {
      गच्छ bad;
    }
    सचे (uarg == 0) {
      argv[i] = 0;
      भङ्ग;
    }
    argv[i] = kalloc();
    सचे (argv[i] == 0)
      गच्छ bad;
    सचे (fetchstr(uarg, argv[i], PGSIZE) < 0)
      गच्छ bad;
  }

  पूर्णाङ्क ret = kexec(path, argv);

  अत्थाय (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  पटिनिवत्त ret;

bad:
  अत्थाय (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  पटिनिवत्त -1;
}

uint64
sys_pipe(शून्य)
{
  uint64 fdarray; // user pointer to array of two integers
  संरचना file *rf, *wf;
  पूर्णाङ्क fd0, fd1;
  संरचना proc *p = myproc();

  argaddr(0, &fdarray);
  सचे (pipealloc(&rf, &wf) < 0)
    पटिनिवत्त -1;
  fd0 = -1;
  सचे ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    सचे (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    पटिनिवत्त -1;
  }
  सचे (copyout(p->pagetable, p->sz, fdarray, (वर्ण *)&fd0, परिमाण(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + परिमाण(fd0), (वर्ण *)&fd1,
              परिमाण(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    पटिनिवत्त -1;
  }
  पटिनिवत्त 0;
}
