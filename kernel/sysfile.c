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
static จำนวนเต็ม
argfd(จำนวนเต็ม n, จำนวนเต็ม *pfd, sa_nracanaa file **pf)
{
  จำนวนเต็ม fd;
  sa_nracanaa file *f;

  argint(n, &fd);
  ถ้า (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    คืน -1;
  ถ้า (pfd)
    *pfd = fd;
  ถ้า (pf)
    *pf = f;
  คืน 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static จำนวนเต็ม
fdalloc(sa_nracanaa file *f)
{
  จำนวนเต็ม fd;
  sa_nracanaa proc *p = myproc();

  สำหรับ (fd = 0; fd < NOFILE; fd++) {
    ถ้า (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      คืน fd;
    }
  }
  คืน -1;
}

uint64
sys_dup(void)
{
  sa_nracanaa file *f;
  จำนวนเต็ม fd;

  ถ้า (argfd(0, 0, &f) < 0)
    คืน -1;
  ถ้า ((fd = fdalloc(f)) < 0)
    คืน -1;
  filedup(f);
  คืน fd;
}

uint64
sys_read(void)
{
  sa_nracanaa file *f;
  จำนวนเต็ม n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ถ้า (argfd(0, 0, &f) < 0)
    คืน -1;
  คืน fileread(f, p, n);
}

uint64
sys_write(void)
{
  sa_nracanaa file *f;
  จำนวนเต็ม n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ถ้า (argfd(0, 0, &f) < 0)
    คืน -1;

  คืน filewrite(f, p, n);
}

uint64
sys_close(void)
{
  จำนวนเต็ม fd;
  sa_nracanaa file *f;

  ถ้า (argfd(0, &fd, &f) < 0)
    คืน -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  คืน 0;
}

uint64
sys_fstat(void)
{
  sa_nracanaa file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ถ้า (argfd(0, 0, &f) < 0)
    คืน -1;
  คืน filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  var_na name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  sa_nracanaa inode *dp, *ip;

  ถ้า (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    คืน -1;

  begin_op();
  ถ้า ((ip = namei(old)) == 0) {
    end_op();
    คืน -1;
  }

  ilock(ip);
  ถ้า (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    คืน -1;
  }

  ถ้า (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    คืน -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ถ้า ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ถ้า (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  ถ้า (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  คืน 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  คืน -1;
}

// Is the directory dp empty except for "." and ".." ?
static จำนวนเต็ม
isdirempty(sa_nracanaa inode *dp)
{
  จำนวนเต็ม off;
  sa_nracanaa dirent de;

  สำหรับ (off = 2 * parimaa_na(de); off < dp->size; off += parimaa_na(de)) {
    ถ้า (readi(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
      panic("isdirempty: readi");
    ถ้า (de.inum != 0)
      คืน 0;
  }
  คืน 1;
}

uint64
sys_unlink(void)
{
  sa_nracanaa inode *ip, *dp;
  sa_nracanaa dirent de;
  var_na name[DIRSIZ], path[MAXPATH];
  uint off;

  ถ้า (argstr(0, path, MAXPATH) < 0)
    คืน -1;

  begin_op();
  ถ้า ((dp = nameiparent(path, name)) == 0) {
    end_op();
    คืน -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ถ้า (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  ถ้า ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  ถ้า (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ถ้า (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, parimaa_na(de));
  ถ้า (writei(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
    panic("unlink: writei");
  ถ้า (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  คืน 0;

bad:
  iunlockput(dp);
  end_op();
  คืน -1;
}

static sa_nracanaa inode *
create(var_na *path, short type, short major, short minor)
{
  sa_nracanaa inode *ip, *dp;
  var_na name[DIRSIZ];

  ถ้า ((dp = nameiparent(path, name)) == 0)
    คืน 0;

  ilock(dp);

  ถ้า (dp->nlink == 0) {
    iunlockput(dp);
    คืน 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ถ้า (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    คืน 0;
  }

  ถ้า ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ถ้า (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      คืน ip;
    iunlockput(ip);
    คืน 0;
  }

  ถ้า ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    คืน 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ถ้า (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ถ้า (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  ถ้า (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  ถ้า (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  คืน ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  คืน 0;
}

uint64
sys_open(void)
{
  var_na path[MAXPATH];
  จำนวนเต็ม fd, omode;
  sa_nracanaa file *f;
  sa_nracanaa inode *ip;
  จำนวนเต็ม n;

  argint(1, &omode);
  ถ้า ((n = argstr(0, path, MAXPATH)) < 0)
    คืน -1;

  begin_op();

  ถ้า (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    ถ้า (ip == 0) {
      end_op();
      คืน -1;
    }
  } มิฉะนั้น {
    ถ้า ((ip = namei(path)) == 0) {
      end_op();
      คืน -1;
    }
    ilock(ip);
    ถ้า (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      คืน -1;
    }
  }

  ถ้า (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    คืน -1;
  }

  ถ้า ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ถ้า (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    คืน -1;
  }

  ถ้า (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } มิฉะนั้น {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ถ้า ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  คืน fd;
}

uint64
sys_mkdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;

  begin_op();
  ถ้า (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    คืน -1;
  }
  iunlockput(ip);
  end_op();
  คืน 0;
}

uint64
sys_mknod(void)
{
  sa_nracanaa inode *ip;
  var_na path[MAXPATH];
  จำนวนเต็ม major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ถ้า ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    คืน -1;
  }
  iunlockput(ip);
  end_op();
  คืน 0;
}

uint64
sys_chdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;
  sa_nracanaa proc *p = myproc();

  begin_op();
  ถ้า (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    คืน -1;
  }
  ilock(ip);
  ถ้า (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    คืน -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  คืน 0;
}

uint64
sys_exec(void)
{
  var_na path[MAXPATH], *argv[MAXARG];
  จำนวนเต็ม i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ถ้า (argstr(0, path, MAXPATH) < 0) {
    คืน -1;
  }
  memset(argv, 0, parimaa_na(argv));
  สำหรับ (i = 0;; i++) {
    ถ้า (i >= NELEM(argv)) {
      goto bad;
    }
    ถ้า (fetchaddr(uargv + parimaa_na(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    ถ้า (uarg == 0) {
      argv[i] = 0;
      bha_nga;
    }
    argv[i] = kalloc();
    ถ้า (argv[i] == 0)
      goto bad;
    ถ้า (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  จำนวนเต็ม ret = kexec(path, argv);

  สำหรับ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  คืน ret;

bad:
  สำหรับ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  คืน -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  sa_nracanaa file *rf, *wf;
  จำนวนเต็ม fd0, fd1;
  sa_nracanaa proc *p = myproc();

  argaddr(0, &fdarray);
  ถ้า (pipealloc(&rf, &wf) < 0)
    คืน -1;
  fd0 = -1;
  ถ้า ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ถ้า (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    คืน -1;
  }
  ถ้า (copyout(p->pagetable, p->sz, fdarray, (var_na *)&fd0, parimaa_na(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + parimaa_na(fd0), (var_na *)&fd1,
              parimaa_na(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    คืน -1;
  }
  คืน 0;
}
