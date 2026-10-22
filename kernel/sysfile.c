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
static 整数
argfd(整数 n, 整数 *pfd, struct file **pf)
{
  整数 fd;
  struct file *f;

  argint(n, &fd);
  如果 (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    返回 -1;
  如果 (pfd)
    *pfd = fd;
  如果 (pf)
    *pf = f;
  返回 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static 整数
fdalloc(struct file *f)
{
  整数 fd;
  struct proc *p = myproc();

  为 (fd = 0; fd < NOFILE; fd++) {
    如果 (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      返回 fd;
    }
  }
  返回 -1;
}

uint64
sys_dup(void)
{
  struct file *f;
  整数 fd;

  如果 (argfd(0, 0, &f) < 0)
    返回 -1;
  如果 ((fd = fdalloc(f)) < 0)
    返回 -1;
  filedup(f);
  返回 fd;
}

uint64
sys_read(void)
{
  struct file *f;
  整数 n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  如果 (argfd(0, 0, &f) < 0)
    返回 -1;
  返回 fileread(f, p, n);
}

uint64
sys_write(void)
{
  struct file *f;
  整数 n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  如果 (argfd(0, 0, &f) < 0)
    返回 -1;

  返回 filewrite(f, p, n);
}

uint64
sys_close(void)
{
  整数 fd;
  struct file *f;

  如果 (argfd(0, &fd, &f) < 0)
    返回 -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  返回 0;
}

uint64
sys_fstat(void)
{
  struct file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  如果 (argfd(0, 0, &f) < 0)
    返回 -1;
  返回 filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  char name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  struct inode *dp, *ip;

  如果 (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    返回 -1;

  begin_op();
  如果 ((ip = namei(old)) == 0) {
    end_op();
    返回 -1;
  }

  ilock(ip);
  如果 (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    返回 -1;
  }

  如果 (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    返回 -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  如果 ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  如果 (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  如果 (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  返回 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  返回 -1;
}

// Is the directory dp empty except for "." and ".." ?
static 整数
isdirempty(struct inode *dp)
{
  整数 off;
  struct dirent de;

  为 (off = 2 * sizeof(de); off < dp->size; off += sizeof(de)) {
    如果 (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    如果 (de.inum != 0)
      返回 0;
  }
  返回 1;
}

uint64
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], path[MAXPATH];
  uint off;

  如果 (argstr(0, path, MAXPATH) < 0)
    返回 -1;

  begin_op();
  如果 ((dp = nameiparent(path, name)) == 0) {
    end_op();
    返回 -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  如果 (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  如果 ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  如果 (ip->nlink < 1)
    panic("unlink: nlink < 1");
  如果 (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  如果 (writei(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  如果 (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  返回 0;

bad:
  iunlockput(dp);
  end_op();
  返回 -1;
}

static struct inode *
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  如果 ((dp = nameiparent(path, name)) == 0)
    返回 0;

  ilock(dp);

  如果 (dp->nlink == 0) {
    iunlockput(dp);
    返回 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  如果 (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    返回 0;
  }

  如果 ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    如果 (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      返回 ip;
    iunlockput(ip);
    返回 0;
  }

  如果 ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    返回 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  如果 (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    如果 (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  如果 (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  如果 (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  返回 ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  返回 0;
}

uint64
sys_open(void)
{
  char path[MAXPATH];
  整数 fd, omode;
  struct file *f;
  struct inode *ip;
  整数 n;

  argint(1, &omode);
  如果 ((n = argstr(0, path, MAXPATH)) < 0)
    返回 -1;

  begin_op();

  如果 (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    如果 (ip == 0) {
      end_op();
      返回 -1;
    }
  } 否则 {
    如果 ((ip = namei(path)) == 0) {
      end_op();
      返回 -1;
    }
    ilock(ip);
    如果 (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      返回 -1;
    }
  }

  如果 (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    返回 -1;
  }

  如果 ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    如果 (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    返回 -1;
  }

  如果 (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } 否则 {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  如果 ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  返回 fd;
}

uint64
sys_mkdir(void)
{
  char path[MAXPATH];
  struct inode *ip;

  begin_op();
  如果 (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    返回 -1;
  }
  iunlockput(ip);
  end_op();
  返回 0;
}

uint64
sys_mknod(void)
{
  struct inode *ip;
  char path[MAXPATH];
  整数 major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  如果 ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    返回 -1;
  }
  iunlockput(ip);
  end_op();
  返回 0;
}

uint64
sys_chdir(void)
{
  char path[MAXPATH];
  struct inode *ip;
  struct proc *p = myproc();

  begin_op();
  如果 (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    返回 -1;
  }
  ilock(ip);
  如果 (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    返回 -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  返回 0;
}

uint64
sys_exec(void)
{
  char path[MAXPATH], *argv[MAXARG];
  整数 i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  如果 (argstr(0, path, MAXPATH) < 0) {
    返回 -1;
  }
  memset(argv, 0, sizeof(argv));
  为 (i = 0;; i++) {
    如果 (i >= NELEM(argv)) {
      goto bad;
    }
    如果 (fetchaddr(uargv + sizeof(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    如果 (uarg == 0) {
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    如果 (argv[i] == 0)
      goto bad;
    如果 (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  整数 ret = kexec(path, argv);

  为 (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  返回 ret;

bad:
  为 (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  返回 -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  struct file *rf, *wf;
  整数 fd0, fd1;
  struct proc *p = myproc();

  argaddr(0, &fdarray);
  如果 (pipealloc(&rf, &wf) < 0)
    返回 -1;
  fd0 = -1;
  如果 ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    如果 (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    返回 -1;
  }
  如果 (copyout(p->pagetable, p->sz, fdarray, (char *)&fd0, sizeof(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + sizeof(fd0), (char *)&fd1,
              sizeof(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    返回 -1;
  }
  返回 0;
}
