//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static цяло
argfd(цяло n, цяло *pfd, struct file **pf)
{
  цяло fd;
  struct file *f;

  argint(n, &fd);
  ако (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    върни -1;
  ако (pfd)
    *pfd = fd;
  ако (pf)
    *pf = f;
  върни 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static цяло
fdalloc(struct file *f)
{
  цяло fd;
  struct proc *p = myproc();

  за (fd = 0; fd < NOFILE; fd++) {
    ако (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      върни fd;
    }
  }
  върни -1;
}

uint64
sys_dup(void)
{
  struct file *f;
  цяло fd;

  ако (argfd(0, 0, &f) < 0)
    върни -1;
  ако ((fd = fdalloc(f)) < 0)
    върни -1;
  filedup(f);
  върни fd;
}

uint64
sys_read(void)
{
  struct file *f;
  цяло n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ако (argfd(0, 0, &f) < 0)
    върни -1;
  върни fileread(f, p, n);
}

uint64
sys_write(void)
{
  struct file *f;
  цяло n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ако (argfd(0, 0, &f) < 0)
    върни -1;

  върни filewrite(f, p, n);
}

uint64
sys_close(void)
{
  цяло fd;
  struct file *f;

  ако (argfd(0, &fd, &f) < 0)
    върни -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  върни 0;
}

uint64
sys_fstat(void)
{
  struct file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ако (argfd(0, 0, &f) < 0)
    върни -1;
  върни filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  char name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  struct inode *dp, *ip;

  ако (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    върни -1;

  begin_op();
  ако ((ip = namei(old)) == 0) {
    end_op();
    върни -1;
  }

  ilock(ip);
  ако (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    върни -1;
  }

  ако (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    върни -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ако ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ако (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  ако (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  върни 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  върни -1;
}

// Is the directory dp empty except for "." and ".." ?
static цяло
isdirempty(struct inode *dp)
{
  цяло off;
  struct dirent de;

  за (off = 2 * sizeof(de); off < dp->size; off += sizeof(de)) {
    ако (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    ако (de.inum != 0)
      върни 0;
  }
  върни 1;
}

uint64
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], path[MAXPATH];
  uint off;

  ако (argstr(0, path, MAXPATH) < 0)
    върни -1;

  begin_op();
  ако ((dp = nameiparent(path, name)) == 0) {
    end_op();
    върни -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ако (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  ако ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  ако (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ако (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  ако (writei(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  ако (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  върни 0;

bad:
  iunlockput(dp);
  end_op();
  върни -1;
}

static struct inode *
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  ако ((dp = nameiparent(path, name)) == 0)
    върни 0;

  ilock(dp);

  ако (dp->nlink == 0) {
    iunlockput(dp);
    върни 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ако (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    върни 0;
  }

  ако ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ако (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      върни ip;
    iunlockput(ip);
    върни 0;
  }

  ако ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    върни 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ако (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ако (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  ако (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  ако (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  върни ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  върни 0;
}

uint64
sys_open(void)
{
  char path[MAXPATH];
  цяло fd, omode;
  struct file *f;
  struct inode *ip;
  цяло n;

  argint(1, &omode);
  ако ((n = argstr(0, path, MAXPATH)) < 0)
    върни -1;

  begin_op();

  ако (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    ако (ip == 0) {
      end_op();
      върни -1;
    }
  } иначе {
    ако ((ip = namei(path)) == 0) {
      end_op();
      върни -1;
    }
    ilock(ip);
    ако (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      върни -1;
    }
  }

  ако (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    върни -1;
  }

  ако ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ако (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    върни -1;
  }

  ако (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } иначе {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ако ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  върни fd;
}

uint64
sys_mkdir(void)
{
  char path[MAXPATH];
  struct inode *ip;

  begin_op();
  ако (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    върни -1;
  }
  iunlockput(ip);
  end_op();
  върни 0;
}

uint64
sys_mknod(void)
{
  struct inode *ip;
  char path[MAXPATH];
  цяло major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ако ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    върни -1;
  }
  iunlockput(ip);
  end_op();
  върни 0;
}

uint64
sys_chdir(void)
{
  char path[MAXPATH];
  struct inode *ip;
  struct proc *p = myproc();

  begin_op();
  ако (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    върни -1;
  }
  ilock(ip);
  ако (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    върни -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  върни 0;
}

uint64
sys_exec(void)
{
  char path[MAXPATH], *argv[MAXARG];
  цяло i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ако (argstr(0, path, MAXPATH) < 0) {
    върни -1;
  }
  memset(argv, 0, sizeof(argv));
  за (i = 0;; i++) {
    ако (i >= NELEM(argv)) {
      goto bad;
    }
    ако (fetchaddr(uargv + sizeof(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    ако (uarg == 0) {
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    ако (argv[i] == 0)
      goto bad;
    ако (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  цяло ret = kexec(path, argv);

  за (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  върни ret;

bad:
  за (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  върни -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  struct file *rf, *wf;
  цяло fd0, fd1;
  struct proc *p = myproc();

  argaddr(0, &fdarray);
  ако (pipealloc(&rf, &wf) < 0)
    върни -1;
  fd0 = -1;
  ако ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ако (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    върни -1;
  }
  ако (copyout(p->pagetable, p->sz, fdarray, (char *)&fd0, sizeof(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + sizeof(fd0), (char *)&fd1,
              sizeof(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    върни -1;
  }
  върни 0;
}
