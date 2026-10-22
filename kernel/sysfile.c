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
static ціле
argfd(ціле n, ціле *pfd, struct file **pf)
{
  ціле fd;
  struct file *f;

  argint(n, &fd);
  якщо (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    повернути -1;
  якщо (pfd)
    *pfd = fd;
  якщо (pf)
    *pf = f;
  повернути 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static ціле
fdalloc(struct file *f)
{
  ціле fd;
  struct proc *p = myproc();

  для (fd = 0; fd < NOFILE; fd++) {
    якщо (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      повернути fd;
    }
  }
  повернути -1;
}

uint64
sys_dup(void)
{
  struct file *f;
  ціле fd;

  якщо (argfd(0, 0, &f) < 0)
    повернути -1;
  якщо ((fd = fdalloc(f)) < 0)
    повернути -1;
  filedup(f);
  повернути fd;
}

uint64
sys_read(void)
{
  struct file *f;
  ціле n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  якщо (argfd(0, 0, &f) < 0)
    повернути -1;
  повернути fileread(f, p, n);
}

uint64
sys_write(void)
{
  struct file *f;
  ціле n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  якщо (argfd(0, 0, &f) < 0)
    повернути -1;

  повернути filewrite(f, p, n);
}

uint64
sys_close(void)
{
  ціле fd;
  struct file *f;

  якщо (argfd(0, &fd, &f) < 0)
    повернути -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  повернути 0;
}

uint64
sys_fstat(void)
{
  struct file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  якщо (argfd(0, 0, &f) < 0)
    повернути -1;
  повернути filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  char name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  struct inode *dp, *ip;

  якщо (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    повернути -1;

  begin_op();
  якщо ((ip = namei(old)) == 0) {
    end_op();
    повернути -1;
  }

  ilock(ip);
  якщо (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    повернути -1;
  }

  якщо (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    повернути -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  якщо ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  якщо (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  якщо (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  повернути 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  повернути -1;
}

// Is the directory dp empty except for "." and ".." ?
static ціле
isdirempty(struct inode *dp)
{
  ціле off;
  struct dirent de;

  для (off = 2 * sizeof(de); off < dp->size; off += sizeof(de)) {
    якщо (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    якщо (de.inum != 0)
      повернути 0;
  }
  повернути 1;
}

uint64
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], path[MAXPATH];
  uint off;

  якщо (argstr(0, path, MAXPATH) < 0)
    повернути -1;

  begin_op();
  якщо ((dp = nameiparent(path, name)) == 0) {
    end_op();
    повернути -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  якщо (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  якщо ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  якщо (ip->nlink < 1)
    panic("unlink: nlink < 1");
  якщо (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  якщо (writei(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  якщо (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  повернути 0;

bad:
  iunlockput(dp);
  end_op();
  повернути -1;
}

static struct inode *
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  якщо ((dp = nameiparent(path, name)) == 0)
    повернути 0;

  ilock(dp);

  якщо (dp->nlink == 0) {
    iunlockput(dp);
    повернути 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  якщо (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    повернути 0;
  }

  якщо ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    якщо (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      повернути ip;
    iunlockput(ip);
    повернути 0;
  }

  якщо ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    повернути 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  якщо (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    якщо (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  якщо (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  якщо (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  повернути ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  повернути 0;
}

uint64
sys_open(void)
{
  char path[MAXPATH];
  ціле fd, omode;
  struct file *f;
  struct inode *ip;
  ціле n;

  argint(1, &omode);
  якщо ((n = argstr(0, path, MAXPATH)) < 0)
    повернути -1;

  begin_op();

  якщо (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    якщо (ip == 0) {
      end_op();
      повернути -1;
    }
  } інакше {
    якщо ((ip = namei(path)) == 0) {
      end_op();
      повернути -1;
    }
    ilock(ip);
    якщо (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      повернути -1;
    }
  }

  якщо (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    повернути -1;
  }

  якщо ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    якщо (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    повернути -1;
  }

  якщо (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } інакше {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  якщо ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  повернути fd;
}

uint64
sys_mkdir(void)
{
  char path[MAXPATH];
  struct inode *ip;

  begin_op();
  якщо (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    повернути -1;
  }
  iunlockput(ip);
  end_op();
  повернути 0;
}

uint64
sys_mknod(void)
{
  struct inode *ip;
  char path[MAXPATH];
  ціле major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  якщо ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    повернути -1;
  }
  iunlockput(ip);
  end_op();
  повернути 0;
}

uint64
sys_chdir(void)
{
  char path[MAXPATH];
  struct inode *ip;
  struct proc *p = myproc();

  begin_op();
  якщо (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    повернути -1;
  }
  ilock(ip);
  якщо (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    повернути -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  повернути 0;
}

uint64
sys_exec(void)
{
  char path[MAXPATH], *argv[MAXARG];
  ціле i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  якщо (argstr(0, path, MAXPATH) < 0) {
    повернути -1;
  }
  memset(argv, 0, sizeof(argv));
  для (i = 0;; i++) {
    якщо (i >= NELEM(argv)) {
      goto bad;
    }
    якщо (fetchaddr(uargv + sizeof(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    якщо (uarg == 0) {
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    якщо (argv[i] == 0)
      goto bad;
    якщо (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  ціле ret = kexec(path, argv);

  для (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  повернути ret;

bad:
  для (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  повернути -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  struct file *rf, *wf;
  ціле fd0, fd1;
  struct proc *p = myproc();

  argaddr(0, &fdarray);
  якщо (pipealloc(&rf, &wf) < 0)
    повернути -1;
  fd0 = -1;
  якщо ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    якщо (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    повернути -1;
  }
  якщо (copyout(p->pagetable, p->sz, fdarray, (char *)&fd0, sizeof(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + sizeof(fd0), (char *)&fd1,
              sizeof(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    повернути -1;
  }
  повернути 0;
}
