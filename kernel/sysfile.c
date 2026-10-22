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
स्थिर पूर्णांक
argfd(पूर्णांक n, पूर्णांक *pfd, संरचना file **pf)
{
  पूर्णांक fd;
  संरचना file *f;

  argint(n, &fd);
  जर (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    परत -1;
  जर (pfd)
    *pfd = fd;
  जर (pf)
    *pf = f;
  परत 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
स्थिर पूर्णांक
fdalloc(संरचना file *f)
{
  पूर्णांक fd;
  संरचना proc *p = myproc();

  साठी (fd = 0; fd < NOFILE; fd++) {
    जर (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      परत fd;
    }
  }
  परत -1;
}

uint64
sys_dup(शून्य)
{
  संरचना file *f;
  पूर्णांक fd;

  जर (argfd(0, 0, &f) < 0)
    परत -1;
  जर ((fd = fdalloc(f)) < 0)
    परत -1;
  filedup(f);
  परत fd;
}

uint64
sys_read(शून्य)
{
  संरचना file *f;
  पूर्णांक n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  जर (argfd(0, 0, &f) < 0)
    परत -1;
  परत fileread(f, p, n);
}

uint64
sys_write(शून्य)
{
  संरचना file *f;
  पूर्णांक n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  जर (argfd(0, 0, &f) < 0)
    परत -1;

  परत filewrite(f, p, n);
}

uint64
sys_close(शून्य)
{
  पूर्णांक fd;
  संरचना file *f;

  जर (argfd(0, &fd, &f) < 0)
    परत -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  परत 0;
}

uint64
sys_fstat(शून्य)
{
  संरचना file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  जर (argfd(0, 0, &f) < 0)
    परत -1;
  परत filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(शून्य)
{
  अक्षर name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  संरचना inode *dp, *ip;

  जर (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    परत -1;

  begin_op();
  जर ((ip = namei(old)) == 0) {
    end_op();
    परत -1;
  }

  ilock(ip);
  जर (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    परत -1;
  }

  जर (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    परत -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  जर ((dp = nameiparent(new, name)) == 0)
    जा bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  जर (dp->nlink == 0) {
    iunlockput(dp);
    जा bad;
  }
  जर (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    जा bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  परत 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  परत -1;
}

// Is the directory dp empty except for "." and ".." ?
स्थिर पूर्णांक
isdirempty(संरचना inode *dp)
{
  पूर्णांक off;
  संरचना dirent de;

  साठी (off = 2 * आकार(de); off < dp->size; off += आकार(de)) {
    जर (readi(dp, 0, (uint64)&de, off, आकार(de)) != आकार(de))
      panic("isdirempty: readi");
    जर (de.inum != 0)
      परत 0;
  }
  परत 1;
}

uint64
sys_unlink(शून्य)
{
  संरचना inode *ip, *dp;
  संरचना dirent de;
  अक्षर name[DIRSIZ], path[MAXPATH];
  uint off;

  जर (argstr(0, path, MAXPATH) < 0)
    परत -1;

  begin_op();
  जर ((dp = nameiparent(path, name)) == 0) {
    end_op();
    परत -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  जर (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    जा bad;

  जर ((ip = dirlookup(dp, name, &off)) == 0)
    जा bad;
  ilock(ip);

  जर (ip->nlink < 1)
    panic("unlink: nlink < 1");
  जर (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    जा bad;
  }

  स_रखो(&de, 0, आकार(de));
  जर (writei(dp, 0, (uint64)&de, off, आकार(de)) != आकार(de))
    panic("unlink: writei");
  जर (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  परत 0;

bad:
  iunlockput(dp);
  end_op();
  परत -1;
}

स्थिर संरचना inode *
create(अक्षर *path, लघु type, लघु major, लघु minor)
{
  संरचना inode *ip, *dp;
  अक्षर name[DIRSIZ];

  जर ((dp = nameiparent(path, name)) == 0)
    परत 0;

  ilock(dp);

  जर (dp->nlink == 0) {
    iunlockput(dp);
    परत 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  जर (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    परत 0;
  }

  जर ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    जर (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      परत ip;
    iunlockput(ip);
    परत 0;
  }

  जर ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    परत 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  जर (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    जर (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      जा fail;
  }

  जर (dirlink(dp, name, ip->inum) < 0)
    जा fail;

  जर (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  परत ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  परत 0;
}

uint64
sys_open(शून्य)
{
  अक्षर path[MAXPATH];
  पूर्णांक fd, omode;
  संरचना file *f;
  संरचना inode *ip;
  पूर्णांक n;

  argint(1, &omode);
  जर ((n = argstr(0, path, MAXPATH)) < 0)
    परत -1;

  begin_op();

  जर (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    जर (ip == 0) {
      end_op();
      परत -1;
    }
  } अन्यथा {
    जर ((ip = namei(path)) == 0) {
      end_op();
      परत -1;
    }
    ilock(ip);
    जर (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      परत -1;
    }
  }

  जर (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    परत -1;
  }

  जर ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    जर (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    परत -1;
  }

  जर (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } अन्यथा {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  जर ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  परत fd;
}

uint64
sys_mkdir(शून्य)
{
  अक्षर path[MAXPATH];
  संरचना inode *ip;

  begin_op();
  जर (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    परत -1;
  }
  iunlockput(ip);
  end_op();
  परत 0;
}

uint64
sys_mknod(शून्य)
{
  संरचना inode *ip;
  अक्षर path[MAXPATH];
  पूर्णांक major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  जर ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    परत -1;
  }
  iunlockput(ip);
  end_op();
  परत 0;
}

uint64
sys_chdir(शून्य)
{
  अक्षर path[MAXPATH];
  संरचना inode *ip;
  संरचना proc *p = myproc();

  begin_op();
  जर (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    परत -1;
  }
  ilock(ip);
  जर (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    परत -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  परत 0;
}

uint64
sys_exec(शून्य)
{
  अक्षर path[MAXPATH], *argv[MAXARG];
  पूर्णांक i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  जर (argstr(0, path, MAXPATH) < 0) {
    परत -1;
  }
  स_रखो(argv, 0, आकार(argv));
  साठी (i = 0;; i++) {
    जर (i >= NELEM(argv)) {
      जा bad;
    }
    जर (fetchaddr(uargv + आकार(uint64) * i, (uint64 *)&uarg) < 0) {
      जा bad;
    }
    जर (uarg == 0) {
      argv[i] = 0;
      थांबा;
    }
    argv[i] = kalloc();
    जर (argv[i] == 0)
      जा bad;
    जर (fetchstr(uarg, argv[i], PGSIZE) < 0)
      जा bad;
  }

  पूर्णांक ret = kexec(path, argv);

  साठी (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  परत ret;

bad:
  साठी (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  परत -1;
}

uint64
sys_pipe(शून्य)
{
  uint64 fdarray; // user pointer to array of two integers
  संरचना file *rf, *wf;
  पूर्णांक fd0, fd1;
  संरचना proc *p = myproc();

  argaddr(0, &fdarray);
  जर (pipealloc(&rf, &wf) < 0)
    परत -1;
  fd0 = -1;
  जर ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    जर (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    परत -1;
  }
  जर (copyout(p->pagetable, p->sz, fdarray, (अक्षर *)&fd0, आकार(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + आकार(fd0), (अक्षर *)&fd1,
              आकार(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    परत -1;
  }
  परत 0;
}
