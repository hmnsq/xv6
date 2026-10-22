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
static ចំនួនគត់
argfd(ចំនួនគត់ n, ចំនួនគត់ *pfd, sa_nracanaa file **pf)
{
  ចំនួនគត់ fd;
  sa_nracanaa file *f;

  argint(n, &fd);
  ប្រសិនបើ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ត្រឡប់ -1;
  ប្រសិនបើ (pfd)
    *pfd = fd;
  ប្រសិនបើ (pf)
    *pf = f;
  ត្រឡប់ 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static ចំនួនគត់
fdalloc(sa_nracanaa file *f)
{
  ចំនួនគត់ fd;
  sa_nracanaa proc *p = myproc();

  សម្រាប់ (fd = 0; fd < NOFILE; fd++) {
    ប្រសិនបើ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ត្រឡប់ fd;
    }
  }
  ត្រឡប់ -1;
}

uint64
sys_dup(void)
{
  sa_nracanaa file *f;
  ចំនួនគត់ fd;

  ប្រសិនបើ (argfd(0, 0, &f) < 0)
    ត្រឡប់ -1;
  ប្រសិនបើ ((fd = fdalloc(f)) < 0)
    ត្រឡប់ -1;
  filedup(f);
  ត្រឡប់ fd;
}

uint64
sys_read(void)
{
  sa_nracanaa file *f;
  ចំនួនគត់ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ប្រសិនបើ (argfd(0, 0, &f) < 0)
    ត្រឡប់ -1;
  ត្រឡប់ fileread(f, p, n);
}

uint64
sys_write(void)
{
  sa_nracanaa file *f;
  ចំនួនគត់ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ប្រសិនបើ (argfd(0, 0, &f) < 0)
    ត្រឡប់ -1;

  ត្រឡប់ filewrite(f, p, n);
}

uint64
sys_close(void)
{
  ចំនួនគត់ fd;
  sa_nracanaa file *f;

  ប្រសិនបើ (argfd(0, &fd, &f) < 0)
    ត្រឡប់ -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ត្រឡប់ 0;
}

uint64
sys_fstat(void)
{
  sa_nracanaa file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ប្រសិនបើ (argfd(0, 0, &f) < 0)
    ត្រឡប់ -1;
  ត្រឡប់ filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  var_na name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  sa_nracanaa inode *dp, *ip;

  ប្រសិនបើ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ត្រឡប់ -1;

  begin_op();
  ប្រសិនបើ ((ip = namei(old)) == 0) {
    end_op();
    ត្រឡប់ -1;
  }

  ilock(ip);
  ប្រសិនបើ (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    ត្រឡប់ -1;
  }

  ប្រសិនបើ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    ត្រឡប់ -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ប្រសិនបើ ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ប្រសិនបើ (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  ប្រសិនបើ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  ត្រឡប់ 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  ត្រឡប់ -1;
}

// Is the directory dp empty except for "." and ".." ?
static ចំនួនគត់
isdirempty(sa_nracanaa inode *dp)
{
  ចំនួនគត់ off;
  sa_nracanaa dirent de;

  សម្រាប់ (off = 2 * parimaa_na(de); off < dp->size; off += parimaa_na(de)) {
    ប្រសិនបើ (readi(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
      panic("isdirempty: readi");
    ប្រសិនបើ (de.inum != 0)
      ត្រឡប់ 0;
  }
  ត្រឡប់ 1;
}

uint64
sys_unlink(void)
{
  sa_nracanaa inode *ip, *dp;
  sa_nracanaa dirent de;
  var_na name[DIRSIZ], path[MAXPATH];
  uint off;

  ប្រសិនបើ (argstr(0, path, MAXPATH) < 0)
    ត្រឡប់ -1;

  begin_op();
  ប្រសិនបើ ((dp = nameiparent(path, name)) == 0) {
    end_op();
    ត្រឡប់ -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ប្រសិនបើ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  ប្រសិនបើ ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  ប្រសិនបើ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ប្រសិនបើ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, parimaa_na(de));
  ប្រសិនបើ (writei(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
    panic("unlink: writei");
  ប្រសិនបើ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  ត្រឡប់ 0;

bad:
  iunlockput(dp);
  end_op();
  ត្រឡប់ -1;
}

static sa_nracanaa inode *
create(var_na *path, short type, short major, short minor)
{
  sa_nracanaa inode *ip, *dp;
  var_na name[DIRSIZ];

  ប្រសិនបើ ((dp = nameiparent(path, name)) == 0)
    ត្រឡប់ 0;

  ilock(dp);

  ប្រសិនបើ (dp->nlink == 0) {
    iunlockput(dp);
    ត្រឡប់ 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ប្រសិនបើ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ត្រឡប់ 0;
  }

  ប្រសិនបើ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ប្រសិនបើ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ត្រឡប់ ip;
    iunlockput(ip);
    ត្រឡប់ 0;
  }

  ប្រសិនបើ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ត្រឡប់ 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ប្រសិនបើ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ប្រសិនបើ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  ប្រសិនបើ (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  ប្រសិនបើ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ត្រឡប់ ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ត្រឡប់ 0;
}

uint64
sys_open(void)
{
  var_na path[MAXPATH];
  ចំនួនគត់ fd, omode;
  sa_nracanaa file *f;
  sa_nracanaa inode *ip;
  ចំនួនគត់ n;

  argint(1, &omode);
  ប្រសិនបើ ((n = argstr(0, path, MAXPATH)) < 0)
    ត្រឡប់ -1;

  begin_op();

  ប្រសិនបើ (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    ប្រសិនបើ (ip == 0) {
      end_op();
      ត្រឡប់ -1;
    }
  } បើមិនដូច្នេះ {
    ប្រសិនបើ ((ip = namei(path)) == 0) {
      end_op();
      ត្រឡប់ -1;
    }
    ilock(ip);
    ប្រសិនបើ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      ត្រឡប់ -1;
    }
  }

  ប្រសិនបើ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    ត្រឡប់ -1;
  }

  ប្រសិនបើ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ប្រសិនបើ (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    ត្រឡប់ -1;
  }

  ប្រសិនបើ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } បើមិនដូច្នេះ {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ប្រសិនបើ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  ត្រឡប់ fd;
}

uint64
sys_mkdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;

  begin_op();
  ប្រសិនបើ (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    ត្រឡប់ -1;
  }
  iunlockput(ip);
  end_op();
  ត្រឡប់ 0;
}

uint64
sys_mknod(void)
{
  sa_nracanaa inode *ip;
  var_na path[MAXPATH];
  ចំនួនគត់ major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ប្រសិនបើ ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    ត្រឡប់ -1;
  }
  iunlockput(ip);
  end_op();
  ត្រឡប់ 0;
}

uint64
sys_chdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;
  sa_nracanaa proc *p = myproc();

  begin_op();
  ប្រសិនបើ (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    ត្រឡប់ -1;
  }
  ilock(ip);
  ប្រសិនបើ (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    ត្រឡប់ -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  ត្រឡប់ 0;
}

uint64
sys_exec(void)
{
  var_na path[MAXPATH], *argv[MAXARG];
  ចំនួនគត់ i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ប្រសិនបើ (argstr(0, path, MAXPATH) < 0) {
    ត្រឡប់ -1;
  }
  memset(argv, 0, parimaa_na(argv));
  សម្រាប់ (i = 0;; i++) {
    ប្រសិនបើ (i >= NELEM(argv)) {
      goto bad;
    }
    ប្រសិនបើ (fetchaddr(uargv + parimaa_na(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    ប្រសិនបើ (uarg == 0) {
      argv[i] = 0;
      bha_nga;
    }
    argv[i] = kalloc();
    ប្រសិនបើ (argv[i] == 0)
      goto bad;
    ប្រសិនបើ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  ចំនួនគត់ ret = kexec(path, argv);

  សម្រាប់ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ត្រឡប់ ret;

bad:
  សម្រាប់ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ត្រឡប់ -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  sa_nracanaa file *rf, *wf;
  ចំនួនគត់ fd0, fd1;
  sa_nracanaa proc *p = myproc();

  argaddr(0, &fdarray);
  ប្រសិនបើ (pipealloc(&rf, &wf) < 0)
    ត្រឡប់ -1;
  fd0 = -1;
  ប្រសិនបើ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ប្រសិនបើ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ត្រឡប់ -1;
  }
  ប្រសិនបើ (copyout(p->pagetable, p->sz, fdarray, (var_na *)&fd0, parimaa_na(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + parimaa_na(fd0), (var_na *)&fd1,
              parimaa_na(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ត្រឡប់ -1;
  }
  ត្រឡប់ 0;
}
