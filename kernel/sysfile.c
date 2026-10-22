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
স্থির পূর্ণসংখ্যা
argfd(পূর্ণসংখ্যা n, পূর্ণসংখ্যা *pfd, কাঠামো file **pf)
{
  পূর্ণসংখ্যা fd;
  কাঠামো file *f;

  argint(n, &fd);
  যদি (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ফেরত -1;
  যদি (pfd)
    *pfd = fd;
  যদি (pf)
    *pf = f;
  ফেরত 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
স্থির পূর্ণসংখ্যা
fdalloc(কাঠামো file *f)
{
  পূর্ণসংখ্যা fd;
  কাঠামো proc *p = myproc();

  জন্য (fd = 0; fd < NOFILE; fd++) {
    যদি (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ফেরত fd;
    }
  }
  ফেরত -1;
}

uint64
sys_dup(শূন্য)
{
  কাঠামো file *f;
  পূর্ণসংখ্যা fd;

  যদি (argfd(0, 0, &f) < 0)
    ফেরত -1;
  যদি ((fd = fdalloc(f)) < 0)
    ফেরত -1;
  filedup(f);
  ফেরত fd;
}

uint64
sys_read(শূন্য)
{
  কাঠামো file *f;
  পূর্ণসংখ্যা n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  যদি (argfd(0, 0, &f) < 0)
    ফেরত -1;
  ফেরত fileread(f, p, n);
}

uint64
sys_write(শূন্য)
{
  কাঠামো file *f;
  পূর্ণসংখ্যা n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  যদি (argfd(0, 0, &f) < 0)
    ফেরত -1;

  ফেরত filewrite(f, p, n);
}

uint64
sys_close(শূন্য)
{
  পূর্ণসংখ্যা fd;
  কাঠামো file *f;

  যদি (argfd(0, &fd, &f) < 0)
    ফেরত -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ফেরত 0;
}

uint64
sys_fstat(শূন্য)
{
  কাঠামো file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  যদি (argfd(0, 0, &f) < 0)
    ফেরত -1;
  ফেরত filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(শূন্য)
{
  অক্ষর name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  কাঠামো inode *dp, *ip;

  যদি (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ফেরত -1;

  begin_op();
  যদি ((ip = namei(old)) == 0) {
    end_op();
    ফেরত -1;
  }

  ilock(ip);
  যদি (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    ফেরত -1;
  }

  যদি (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    ফেরত -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  যদি ((dp = nameiparent(new, name)) == 0)
    যাও bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  যদি (dp->nlink == 0) {
    iunlockput(dp);
    যাও bad;
  }
  যদি (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    যাও bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  ফেরত 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  ফেরত -1;
}

// Is the directory dp empty except for "." and ".." ?
স্থির পূর্ণসংখ্যা
isdirempty(কাঠামো inode *dp)
{
  পূর্ণসংখ্যা off;
  কাঠামো dirent de;

  জন্য (off = 2 * আকার(de); off < dp->আকার_2; off += আকার(de)) {
    যদি (readi(dp, 0, (uint64)&de, off, আকার(de)) != আকার(de))
      panic("isdirempty: readi");
    যদি (de.inum != 0)
      ফেরত 0;
  }
  ফেরত 1;
}

uint64
sys_unlink(শূন্য)
{
  কাঠামো inode *ip, *dp;
  কাঠামো dirent de;
  অক্ষর name[DIRSIZ], path[MAXPATH];
  uint off;

  যদি (argstr(0, path, MAXPATH) < 0)
    ফেরত -1;

  begin_op();
  যদি ((dp = nameiparent(path, name)) == 0) {
    end_op();
    ফেরত -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  যদি (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    যাও bad;

  যদি ((ip = dirlookup(dp, name, &off)) == 0)
    যাও bad;
  ilock(ip);

  যদি (ip->nlink < 1)
    panic("unlink: nlink < 1");
  যদি (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    যাও bad;
  }

  memset(&de, 0, আকার(de));
  যদি (writei(dp, 0, (uint64)&de, off, আকার(de)) != আকার(de))
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

  ফেরত 0;

bad:
  iunlockput(dp);
  end_op();
  ফেরত -1;
}

স্থির কাঠামো inode *
create(অক্ষর *path, short type, short major, short minor)
{
  কাঠামো inode *ip, *dp;
  অক্ষর name[DIRSIZ];

  যদি ((dp = nameiparent(path, name)) == 0)
    ফেরত 0;

  ilock(dp);

  যদি (dp->nlink == 0) {
    iunlockput(dp);
    ফেরত 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  যদি (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ফেরত 0;
  }

  যদি ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    যদি (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ফেরত ip;
    iunlockput(ip);
    ফেরত 0;
  }

  যদি ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ফেরত 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  যদি (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    যদি (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      যাও fail;
  }

  যদি (dirlink(dp, name, ip->inum) < 0)
    যাও fail;

  যদি (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ফেরত ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ফেরত 0;
}

uint64
sys_open(শূন্য)
{
  অক্ষর path[MAXPATH];
  পূর্ণসংখ্যা fd, omode;
  কাঠামো file *f;
  কাঠামো inode *ip;
  পূর্ণসংখ্যা n;

  argint(1, &omode);
  যদি ((n = argstr(0, path, MAXPATH)) < 0)
    ফেরত -1;

  begin_op();

  যদি (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    যদি (ip == 0) {
      end_op();
      ফেরত -1;
    }
  } অন্যথা {
    যদি ((ip = namei(path)) == 0) {
      end_op();
      ফেরত -1;
    }
    ilock(ip);
    যদি (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      ফেরত -1;
    }
  }

  যদি (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    ফেরত -1;
  }

  যদি ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    যদি (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    ফেরত -1;
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

  ফেরত fd;
}

uint64
sys_mkdir(শূন্য)
{
  অক্ষর path[MAXPATH];
  কাঠামো inode *ip;

  begin_op();
  যদি (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    ফেরত -1;
  }
  iunlockput(ip);
  end_op();
  ফেরত 0;
}

uint64
sys_mknod(শূন্য)
{
  কাঠামো inode *ip;
  অক্ষর path[MAXPATH];
  পূর্ণসংখ্যা major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  যদি ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    ফেরত -1;
  }
  iunlockput(ip);
  end_op();
  ফেরত 0;
}

uint64
sys_chdir(শূন্য)
{
  অক্ষর path[MAXPATH];
  কাঠামো inode *ip;
  কাঠামো proc *p = myproc();

  begin_op();
  যদি (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    ফেরত -1;
  }
  ilock(ip);
  যদি (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    ফেরত -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  ফেরত 0;
}

uint64
sys_exec(শূন্য)
{
  অক্ষর path[MAXPATH], *argv[MAXARG];
  পূর্ণসংখ্যা i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  যদি (argstr(0, path, MAXPATH) < 0) {
    ফেরত -1;
  }
  memset(argv, 0, আকার(argv));
  জন্য (i = 0;; i++) {
    যদি (i >= NELEM(argv)) {
      যাও bad;
    }
    যদি (fetchaddr(uargv + আকার(uint64) * i, (uint64 *)&uarg) < 0) {
      যাও bad;
    }
    যদি (uarg == 0) {
      argv[i] = 0;
      বিরতি;
    }
    argv[i] = kalloc();
    যদি (argv[i] == 0)
      যাও bad;
    যদি (fetchstr(uarg, argv[i], PGSIZE) < 0)
      যাও bad;
  }

  পূর্ণসংখ্যা ret = kexec(path, argv);

  জন্য (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ফেরত ret;

bad:
  জন্য (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ফেরত -1;
}

uint64
sys_pipe(শূন্য)
{
  uint64 fdarray; // user pointer to array of two integers
  কাঠামো file *rf, *wf;
  পূর্ণসংখ্যা fd0, fd1;
  কাঠামো proc *p = myproc();

  argaddr(0, &fdarray);
  যদি (pipealloc(&rf, &wf) < 0)
    ফেরত -1;
  fd0 = -1;
  যদি ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    যদি (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ফেরত -1;
  }
  যদি (copyout(p->pagetable, p->sz, fdarray, (অক্ষর *)&fd0, আকার(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + আকার(fd0), (অক্ষর *)&fd1,
              আকার(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ফেরত -1;
  }
  ফেরত 0;
}
