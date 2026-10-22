//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#అన్తర్భావ "types.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "stat.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "file.h"
#అన్తర్భావ "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
స్థిర పూర్ణాంకం
argfd(పూర్ణాంకం n, పూర్ణాంకం *pfd, నిర్మాణం file **pf)
{
  పూర్ణాంకం fd;
  నిర్మాణం file *f;

  argint(n, &fd);
  ఒకవేళ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    తిరిగి -1;
  ఒకవేళ (pfd)
    *pfd = fd;
  ఒకవేళ (pf)
    *pf = f;
  తిరిగి 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
స్థిర పూర్ణాంకం
fdalloc(నిర్మాణం file *f)
{
  పూర్ణాంకం fd;
  నిర్మాణం proc *p = myproc();

  కొరకు (fd = 0; fd < NOFILE; fd++) {
    ఒకవేళ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      తిరిగి fd;
    }
  }
  తిరిగి -1;
}

uint64
sys_dup(శూన్య)
{
  నిర్మాణం file *f;
  పూర్ణాంకం fd;

  ఒకవేళ (argfd(0, 0, &f) < 0)
    తిరిగి -1;
  ఒకవేళ ((fd = fdalloc(f)) < 0)
    తిరిగి -1;
  filedup(f);
  తిరిగి fd;
}

uint64
sys_read(శూన్య)
{
  నిర్మాణం file *f;
  పూర్ణాంకం n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ఒకవేళ (argfd(0, 0, &f) < 0)
    తిరిగి -1;
  తిరిగి fileread(f, p, n);
}

uint64
sys_write(శూన్య)
{
  నిర్మాణం file *f;
  పూర్ణాంకం n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ఒకవేళ (argfd(0, 0, &f) < 0)
    తిరిగి -1;

  తిరిగి filewrite(f, p, n);
}

uint64
sys_close(శూన్య)
{
  పూర్ణాంకం fd;
  నిర్మాణం file *f;

  ఒకవేళ (argfd(0, &fd, &f) < 0)
    తిరిగి -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  తిరిగి 0;
}

uint64
sys_fstat(శూన్య)
{
  నిర్మాణం file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ఒకవేళ (argfd(0, 0, &f) < 0)
    తిరిగి -1;
  తిరిగి filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(శూన్య)
{
  అక్షరం name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  నిర్మాణం inode *dp, *ip;

  ఒకవేళ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    తిరిగి -1;

  begin_op();
  ఒకవేళ ((ip = namei(old)) == 0) {
    చివర_op();
    తిరిగి -1;
  }

  ilock(ip);
  ఒకవేళ (ip->type == T_DIR) {
    iunlockput(ip);
    చివర_op();
    తిరిగి -1;
  }

  ఒకవేళ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    చివర_op();
    తిరిగి -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ఒకవేళ ((dp = nameiparent(new, name)) == 0)
    వెళ్ళు bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ఒకవేళ (dp->nlink == 0) {
    iunlockput(dp);
    వెళ్ళు bad;
  }
  ఒకవేళ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    వెళ్ళు bad;
  }
  iunlockput(dp);
  iput(ip);

  చివర_op();

  తిరిగి 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  చివర_op();
  తిరిగి -1;
}

// Is the directory dp empty except for "." and ".." ?
స్థిర పూర్ణాంకం
isdirempty(నిర్మాణం inode *dp)
{
  పూర్ణాంకం off;
  నిర్మాణం dirent de;

  కొరకు (off = 2 * పరిమాణం(de); off < dp->కొలత; off += పరిమాణం(de)) {
    ఒకవేళ (readi(dp, 0, (uint64)&de, off, పరిమాణం(de)) != పరిమాణం(de))
      panic("isdirempty: readi");
    ఒకవేళ (de.inum != 0)
      తిరిగి 0;
  }
  తిరిగి 1;
}

uint64
sys_unlink(శూన్య)
{
  నిర్మాణం inode *ip, *dp;
  నిర్మాణం dirent de;
  అక్షరం name[DIRSIZ], మార్గం[MAXPATH];
  uint off;

  ఒకవేళ (argstr(0, మార్గం, MAXPATH) < 0)
    తిరిగి -1;

  begin_op();
  ఒకవేళ ((dp = nameiparent(మార్గం, name)) == 0) {
    చివర_op();
    తిరిగి -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ఒకవేళ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    వెళ్ళు bad;

  ఒకవేళ ((ip = dirlookup(dp, name, &off)) == 0)
    వెళ్ళు bad;
  ilock(ip);

  ఒకవేళ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ఒకవేళ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    వెళ్ళు bad;
  }

  memset(&de, 0, పరిమాణం(de));
  ఒకవేళ (writei(dp, 0, (uint64)&de, off, పరిమాణం(de)) != పరిమాణం(de))
    panic("unlink: writei");
  ఒకవేళ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  చివర_op();

  తిరిగి 0;

bad:
  iunlockput(dp);
  చివర_op();
  తిరిగి -1;
}

స్థిర నిర్మాణం inode *
create(అక్షరం *మార్గం, పొట్టి type, పొట్టి major, పొట్టి minor)
{
  నిర్మాణం inode *ip, *dp;
  అక్షరం name[DIRSIZ];

  ఒకవేళ ((dp = nameiparent(మార్గం, name)) == 0)
    తిరిగి 0;

  ilock(dp);

  ఒకవేళ (dp->nlink == 0) {
    iunlockput(dp);
    తిరిగి 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ఒకవేళ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    తిరిగి 0;
  }

  ఒకవేళ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ఒకవేళ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      తిరిగి ip;
    iunlockput(ip);
    తిరిగి 0;
  }

  ఒకవేళ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    తిరిగి 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ఒకవేళ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ఒకవేళ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      వెళ్ళు fail;
  }

  ఒకవేళ (dirlink(dp, name, ip->inum) < 0)
    వెళ్ళు fail;

  ఒకవేళ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  తిరిగి ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  తిరిగి 0;
}

uint64
sys_open(శూన్య)
{
  అక్షరం మార్గం[MAXPATH];
  పూర్ణాంకం fd, omode;
  నిర్మాణం file *f;
  నిర్మాణం inode *ip;
  పూర్ణాంకం n;

  argint(1, &omode);
  ఒకవేళ ((n = argstr(0, మార్గం, MAXPATH)) < 0)
    తిరిగి -1;

  begin_op();

  ఒకవేళ (omode & O_CREATE) {
    ip = create(మార్గం, T_FILE, 0, 0);
    ఒకవేళ (ip == 0) {
      చివర_op();
      తిరిగి -1;
    }
  } లేకపోతే {
    ఒకవేళ ((ip = namei(మార్గం)) == 0) {
      చివర_op();
      తిరిగి -1;
    }
    ilock(ip);
    ఒకవేళ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      చివర_op();
      తిరిగి -1;
    }
  }

  ఒకవేళ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    చివర_op();
    తిరిగి -1;
  }

  ఒకవేళ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ఒకవేళ (f)
      fileclose(f);
    iunlockput(ip);
    చివర_op();
    తిరిగి -1;
  }

  ఒకవేళ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } లేకపోతే {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ఒకవేళ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  చివర_op();

  తిరిగి fd;
}

uint64
sys_mkdir(శూన్య)
{
  అక్షరం మార్గం[MAXPATH];
  నిర్మాణం inode *ip;

  begin_op();
  ఒకవేళ (argstr(0, మార్గం, MAXPATH) < 0 || (ip = create(మార్గం, T_DIR, 0, 0)) == 0) {
    చివర_op();
    తిరిగి -1;
  }
  iunlockput(ip);
  చివర_op();
  తిరిగి 0;
}

uint64
sys_mknod(శూన్య)
{
  నిర్మాణం inode *ip;
  అక్షరం మార్గం[MAXPATH];
  పూర్ణాంకం major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ఒకవేళ ((argstr(0, మార్గం, MAXPATH)) < 0 ||
      (ip = create(మార్గం, T_DEVICE, major, minor)) == 0) {
    చివర_op();
    తిరిగి -1;
  }
  iunlockput(ip);
  చివర_op();
  తిరిగి 0;
}

uint64
sys_chdir(శూన్య)
{
  అక్షరం మార్గం[MAXPATH];
  నిర్మాణం inode *ip;
  నిర్మాణం proc *p = myproc();

  begin_op();
  ఒకవేళ (argstr(0, మార్గం, MAXPATH) < 0 || (ip = namei(మార్గం)) == 0) {
    చివర_op();
    తిరిగి -1;
  }
  ilock(ip);
  ఒకవేళ (ip->type != T_DIR) {
    iunlockput(ip);
    చివర_op();
    తిరిగి -1;
  }
  iunlock(ip);
  iput(p->cwd);
  చివర_op();
  p->cwd = ip;
  తిరిగి 0;
}

uint64
sys_exec(శూన్య)
{
  అక్షరం మార్గం[MAXPATH], *argv[MAXARG];
  పూర్ణాంకం i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ఒకవేళ (argstr(0, మార్గం, MAXPATH) < 0) {
    తిరిగి -1;
  }
  memset(argv, 0, పరిమాణం(argv));
  కొరకు (i = 0;; i++) {
    ఒకవేళ (i >= NELEM(argv)) {
      వెళ్ళు bad;
    }
    ఒకవేళ (fetchaddr(uargv + పరిమాణం(uint64) * i, (uint64 *)&uarg) < 0) {
      వెళ్ళు bad;
    }
    ఒకవేళ (uarg == 0) {
      argv[i] = 0;
      ఆపు;
    }
    argv[i] = kalloc();
    ఒకవేళ (argv[i] == 0)
      వెళ్ళు bad;
    ఒకవేళ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      వెళ్ళు bad;
  }

  పూర్ణాంకం ret = kexec(మార్గం, argv);

  కొరకు (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  తిరిగి ret;

bad:
  కొరకు (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  తిరిగి -1;
}

uint64
sys_pipe(శూన్య)
{
  uint64 fdarray; // user pointer to array of two integers
  నిర్మాణం file *rf, *wf;
  పూర్ణాంకం fd0, fd1;
  నిర్మాణం proc *p = myproc();

  argaddr(0, &fdarray);
  ఒకవేళ (pipealloc(&rf, &wf) < 0)
    తిరిగి -1;
  fd0 = -1;
  ఒకవేళ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ఒకవేళ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    తిరిగి -1;
  }
  ఒకవేళ (copyout(p->pagetable, p->sz, fdarray, (అక్షరం *)&fd0, పరిమాణం(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + పరిమాణం(fd0), (అక్షరం *)&fd1,
              పరిమాణం(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    తిరిగి -1;
  }
  తిరిగి 0;
}
