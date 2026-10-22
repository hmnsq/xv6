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
static số_nguyên
argfd(số_nguyên n, số_nguyên *pfd, struct file **pf)
{
  số_nguyên fd;
  struct file *f;

  argint(n, &fd);
  nếu (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    trả_về -1;
  nếu (pfd)
    *pfd = fd;
  nếu (pf)
    *pf = f;
  trả_về 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static số_nguyên
fdalloc(struct file *f)
{
  số_nguyên fd;
  struct proc *p = myproc();

  cho (fd = 0; fd < NOFILE; fd++) {
    nếu (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      trả_về fd;
    }
  }
  trả_về -1;
}

uint64
sys_dup(void)
{
  struct file *f;
  số_nguyên fd;

  nếu (argfd(0, 0, &f) < 0)
    trả_về -1;
  nếu ((fd = fdalloc(f)) < 0)
    trả_về -1;
  filedup(f);
  trả_về fd;
}

uint64
sys_read(void)
{
  struct file *f;
  số_nguyên n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  nếu (argfd(0, 0, &f) < 0)
    trả_về -1;
  trả_về fileread(f, p, n);
}

uint64
sys_write(void)
{
  struct file *f;
  số_nguyên n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  nếu (argfd(0, 0, &f) < 0)
    trả_về -1;

  trả_về filewrite(f, p, n);
}

uint64
sys_close(void)
{
  số_nguyên fd;
  struct file *f;

  nếu (argfd(0, &fd, &f) < 0)
    trả_về -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  trả_về 0;
}

uint64
sys_fstat(void)
{
  struct file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  nếu (argfd(0, 0, &f) < 0)
    trả_về -1;
  trả_về filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  char name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  struct inode *dp, *ip;

  nếu (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    trả_về -1;

  begin_op();
  nếu ((ip = namei(old)) == 0) {
    end_op();
    trả_về -1;
  }

  ilock(ip);
  nếu (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    trả_về -1;
  }

  nếu (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    trả_về -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  nếu ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  nếu (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  nếu (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  trả_về 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  trả_về -1;
}

// Is the directory dp empty except for "." and ".." ?
static số_nguyên
isdirempty(struct inode *dp)
{
  số_nguyên off;
  struct dirent de;

  cho (off = 2 * sizeof(de); off < dp->size; off += sizeof(de)) {
    nếu (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    nếu (de.inum != 0)
      trả_về 0;
  }
  trả_về 1;
}

uint64
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], path[MAXPATH];
  uint off;

  nếu (argstr(0, path, MAXPATH) < 0)
    trả_về -1;

  begin_op();
  nếu ((dp = nameiparent(path, name)) == 0) {
    end_op();
    trả_về -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  nếu (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  nếu ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  nếu (ip->nlink < 1)
    panic("unlink: nlink < 1");
  nếu (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  nếu (writei(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  nếu (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  trả_về 0;

bad:
  iunlockput(dp);
  end_op();
  trả_về -1;
}

static struct inode *
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  nếu ((dp = nameiparent(path, name)) == 0)
    trả_về 0;

  ilock(dp);

  nếu (dp->nlink == 0) {
    iunlockput(dp);
    trả_về 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  nếu (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    trả_về 0;
  }

  nếu ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    nếu (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      trả_về ip;
    iunlockput(ip);
    trả_về 0;
  }

  nếu ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    trả_về 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  nếu (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    nếu (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  nếu (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  nếu (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  trả_về ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  trả_về 0;
}

uint64
sys_open(void)
{
  char path[MAXPATH];
  số_nguyên fd, omode;
  struct file *f;
  struct inode *ip;
  số_nguyên n;

  argint(1, &omode);
  nếu ((n = argstr(0, path, MAXPATH)) < 0)
    trả_về -1;

  begin_op();

  nếu (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    nếu (ip == 0) {
      end_op();
      trả_về -1;
    }
  } không_thì {
    nếu ((ip = namei(path)) == 0) {
      end_op();
      trả_về -1;
    }
    ilock(ip);
    nếu (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      trả_về -1;
    }
  }

  nếu (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    trả_về -1;
  }

  nếu ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    nếu (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    trả_về -1;
  }

  nếu (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } không_thì {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  nếu ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  trả_về fd;
}

uint64
sys_mkdir(void)
{
  char path[MAXPATH];
  struct inode *ip;

  begin_op();
  nếu (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    trả_về -1;
  }
  iunlockput(ip);
  end_op();
  trả_về 0;
}

uint64
sys_mknod(void)
{
  struct inode *ip;
  char path[MAXPATH];
  số_nguyên major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  nếu ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    trả_về -1;
  }
  iunlockput(ip);
  end_op();
  trả_về 0;
}

uint64
sys_chdir(void)
{
  char path[MAXPATH];
  struct inode *ip;
  struct proc *p = myproc();

  begin_op();
  nếu (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    trả_về -1;
  }
  ilock(ip);
  nếu (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    trả_về -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  trả_về 0;
}

uint64
sys_exec(void)
{
  char path[MAXPATH], *argv[MAXARG];
  số_nguyên i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  nếu (argstr(0, path, MAXPATH) < 0) {
    trả_về -1;
  }
  memset(argv, 0, sizeof(argv));
  cho (i = 0;; i++) {
    nếu (i >= NELEM(argv)) {
      goto bad;
    }
    nếu (fetchaddr(uargv + sizeof(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    nếu (uarg == 0) {
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    nếu (argv[i] == 0)
      goto bad;
    nếu (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  số_nguyên ret = kexec(path, argv);

  cho (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  trả_về ret;

bad:
  cho (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  trả_về -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  struct file *rf, *wf;
  số_nguyên fd0, fd1;
  struct proc *p = myproc();

  argaddr(0, &fdarray);
  nếu (pipealloc(&rf, &wf) < 0)
    trả_về -1;
  fd0 = -1;
  nếu ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    nếu (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    trả_về -1;
  }
  nếu (copyout(p->pagetable, p->sz, fdarray, (char *)&fd0, sizeof(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + sizeof(fd0), (char *)&fd1,
              sizeof(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    trả_về -1;
  }
  trả_về 0;
}
