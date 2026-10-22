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
static целое
argfd(целое n, целое *pfd, struct file **pf)
{
  целое fd;
  struct file *f;

  argint(n, &fd);
  если (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    вернуть -1;
  если (pfd)
    *pfd = fd;
  если (pf)
    *pf = f;
  вернуть 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static целое
fdalloc(struct file *f)
{
  целое fd;
  struct proc *p = myproc();

  для (fd = 0; fd < NOFILE; fd++) {
    если (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      вернуть fd;
    }
  }
  вернуть -1;
}

uint64
sys_dup(void)
{
  struct file *f;
  целое fd;

  если (argfd(0, 0, &f) < 0)
    вернуть -1;
  если ((fd = fdalloc(f)) < 0)
    вернуть -1;
  filedup(f);
  вернуть fd;
}

uint64
sys_read(void)
{
  struct file *f;
  целое n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  если (argfd(0, 0, &f) < 0)
    вернуть -1;
  вернуть fileread(f, p, n);
}

uint64
sys_write(void)
{
  struct file *f;
  целое n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  если (argfd(0, 0, &f) < 0)
    вернуть -1;

  вернуть filewrite(f, p, n);
}

uint64
sys_close(void)
{
  целое fd;
  struct file *f;

  если (argfd(0, &fd, &f) < 0)
    вернуть -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  вернуть 0;
}

uint64
sys_fstat(void)
{
  struct file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  если (argfd(0, 0, &f) < 0)
    вернуть -1;
  вернуть filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  char name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  struct inode *dp, *ip;

  если (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    вернуть -1;

  begin_op();
  если ((ip = namei(old)) == 0) {
    end_op();
    вернуть -1;
  }

  ilock(ip);
  если (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    вернуть -1;
  }

  если (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    вернуть -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  если ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  если (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  если (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  вернуть 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  вернуть -1;
}

// Is the directory dp empty except for "." and ".." ?
static целое
isdirempty(struct inode *dp)
{
  целое off;
  struct dirent de;

  для (off = 2 * sizeof(de); off < dp->size; off += sizeof(de)) {
    если (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    если (de.inum != 0)
      вернуть 0;
  }
  вернуть 1;
}

uint64
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], path[MAXPATH];
  uint off;

  если (argstr(0, path, MAXPATH) < 0)
    вернуть -1;

  begin_op();
  если ((dp = nameiparent(path, name)) == 0) {
    end_op();
    вернуть -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  если (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  если ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  если (ip->nlink < 1)
    panic("unlink: nlink < 1");
  если (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  если (writei(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  если (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  вернуть 0;

bad:
  iunlockput(dp);
  end_op();
  вернуть -1;
}

static struct inode *
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  если ((dp = nameiparent(path, name)) == 0)
    вернуть 0;

  ilock(dp);

  если (dp->nlink == 0) {
    iunlockput(dp);
    вернуть 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  если (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    вернуть 0;
  }

  если ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    если (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      вернуть ip;
    iunlockput(ip);
    вернуть 0;
  }

  если ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    вернуть 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  если (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    если (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  если (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  если (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  вернуть ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  вернуть 0;
}

uint64
sys_open(void)
{
  char path[MAXPATH];
  целое fd, omode;
  struct file *f;
  struct inode *ip;
  целое n;

  argint(1, &omode);
  если ((n = argstr(0, path, MAXPATH)) < 0)
    вернуть -1;

  begin_op();

  если (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    если (ip == 0) {
      end_op();
      вернуть -1;
    }
  } иначе {
    если ((ip = namei(path)) == 0) {
      end_op();
      вернуть -1;
    }
    ilock(ip);
    если (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      вернуть -1;
    }
  }

  если (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    вернуть -1;
  }

  если ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    если (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    вернуть -1;
  }

  если (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } иначе {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  если ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  вернуть fd;
}

uint64
sys_mkdir(void)
{
  char path[MAXPATH];
  struct inode *ip;

  begin_op();
  если (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    вернуть -1;
  }
  iunlockput(ip);
  end_op();
  вернуть 0;
}

uint64
sys_mknod(void)
{
  struct inode *ip;
  char path[MAXPATH];
  целое major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  если ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    вернуть -1;
  }
  iunlockput(ip);
  end_op();
  вернуть 0;
}

uint64
sys_chdir(void)
{
  char path[MAXPATH];
  struct inode *ip;
  struct proc *p = myproc();

  begin_op();
  если (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    вернуть -1;
  }
  ilock(ip);
  если (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    вернуть -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  вернуть 0;
}

uint64
sys_exec(void)
{
  char path[MAXPATH], *argv[MAXARG];
  целое i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  если (argstr(0, path, MAXPATH) < 0) {
    вернуть -1;
  }
  memset(argv, 0, sizeof(argv));
  для (i = 0;; i++) {
    если (i >= NELEM(argv)) {
      goto bad;
    }
    если (fetchaddr(uargv + sizeof(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    если (uarg == 0) {
      argv[i] = 0;
      break;
    }
    argv[i] = kalloc();
    если (argv[i] == 0)
      goto bad;
    если (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  целое ret = kexec(path, argv);

  для (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  вернуть ret;

bad:
  для (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  вернуть -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  struct file *rf, *wf;
  целое fd0, fd1;
  struct proc *p = myproc();

  argaddr(0, &fdarray);
  если (pipealloc(&rf, &wf) < 0)
    вернуть -1;
  fd0 = -1;
  если ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    если (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    вернуть -1;
  }
  если (copyout(p->pagetable, p->sz, fdarray, (char *)&fd0, sizeof(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + sizeof(fd0), (char *)&fd1,
              sizeof(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    вернуть -1;
  }
  вернуть 0;
}
