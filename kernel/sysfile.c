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
static puur_naa_nka
argfd(puur_naa_nka n, puur_naa_nka *pfd, sa_nracanaa file **pf)
{
  puur_naa_nka fd;
  sa_nracanaa file *f;

  argint(n, &fd);
  ᨀᨘ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ᨀᨘᨒ -1;
  ᨀᨘ (pfd)
    *pfd = fd;
  ᨀᨘ (pf)
    *pf = f;
  ᨀᨘᨒ 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static puur_naa_nka
fdalloc(sa_nracanaa file *f)
{
  puur_naa_nka fd;
  sa_nracanaa proc *p = myproc();

  for (fd = 0; fd < NOFILE; fd++) {
    ᨀᨘ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ᨀᨘᨒ fd;
    }
  }
  ᨀᨘᨒ -1;
}

uint64
sys_dup(void)
{
  sa_nracanaa file *f;
  puur_naa_nka fd;

  ᨀᨘ (argfd(0, 0, &f) < 0)
    ᨀᨘᨒ -1;
  ᨀᨘ ((fd = fdalloc(f)) < 0)
    ᨀᨘᨒ -1;
  filedup(f);
  ᨀᨘᨒ fd;
}

uint64
sys_read(void)
{
  sa_nracanaa file *f;
  puur_naa_nka n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ᨀᨘ (argfd(0, 0, &f) < 0)
    ᨀᨘᨒ -1;
  ᨀᨘᨒ fileread(f, p, n);
}

uint64
sys_write(void)
{
  sa_nracanaa file *f;
  puur_naa_nka n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ᨀᨘ (argfd(0, 0, &f) < 0)
    ᨀᨘᨒ -1;

  ᨀᨘᨒ filewrite(f, p, n);
}

uint64
sys_close(void)
{
  puur_naa_nka fd;
  sa_nracanaa file *f;

  ᨀᨘ (argfd(0, &fd, &f) < 0)
    ᨀᨘᨒ -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ᨀᨘᨒ 0;
}

uint64
sys_fstat(void)
{
  sa_nracanaa file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ᨀᨘ (argfd(0, 0, &f) < 0)
    ᨀᨘᨒ -1;
  ᨀᨘᨒ filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  var_na name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  sa_nracanaa inode *dp, *ip;

  ᨀᨘ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ᨀᨘᨒ -1;

  begin_op();
  ᨀᨘ ((ip = namei(old)) == 0) {
    end_op();
    ᨀᨘᨒ -1;
  }

  ilock(ip);
  ᨀᨘ (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    ᨀᨘᨒ -1;
  }

  ᨀᨘ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    ᨀᨘᨒ -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ᨀᨘ ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ᨀᨘ (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  ᨀᨘ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  ᨀᨘᨒ 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  ᨀᨘᨒ -1;
}

// Is the directory dp empty except for "." and ".." ?
static puur_naa_nka
isdirempty(sa_nracanaa inode *dp)
{
  puur_naa_nka off;
  sa_nracanaa dirent de;

  for (off = 2 * parimaa_na(de); off < dp->size; off += parimaa_na(de)) {
    ᨀᨘ (readi(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
      panic("isdirempty: readi");
    ᨀᨘ (de.inum != 0)
      ᨀᨘᨒ 0;
  }
  ᨀᨘᨒ 1;
}

uint64
sys_unlink(void)
{
  sa_nracanaa inode *ip, *dp;
  sa_nracanaa dirent de;
  var_na name[DIRSIZ], path[MAXPATH];
  uint off;

  ᨀᨘ (argstr(0, path, MAXPATH) < 0)
    ᨀᨘᨒ -1;

  begin_op();
  ᨀᨘ ((dp = nameiparent(path, name)) == 0) {
    end_op();
    ᨀᨘᨒ -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ᨀᨘ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  ᨀᨘ ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  ᨀᨘ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ᨀᨘ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, parimaa_na(de));
  ᨀᨘ (writei(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
    panic("unlink: writei");
  ᨀᨘ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  ᨀᨘᨒ 0;

bad:
  iunlockput(dp);
  end_op();
  ᨀᨘᨒ -1;
}

static sa_nracanaa inode *
create(var_na *path, short type, short major, short minor)
{
  sa_nracanaa inode *ip, *dp;
  var_na name[DIRSIZ];

  ᨀᨘ ((dp = nameiparent(path, name)) == 0)
    ᨀᨘᨒ 0;

  ilock(dp);

  ᨀᨘ (dp->nlink == 0) {
    iunlockput(dp);
    ᨀᨘᨒ 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ᨀᨘ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ᨀᨘᨒ 0;
  }

  ᨀᨘ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ᨀᨘ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ᨀᨘᨒ ip;
    iunlockput(ip);
    ᨀᨘᨒ 0;
  }

  ᨀᨘ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ᨀᨘᨒ 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ᨀᨘ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ᨀᨘ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  ᨀᨘ (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  ᨀᨘ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ᨀᨘᨒ ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ᨀᨘᨒ 0;
}

uint64
sys_open(void)
{
  var_na path[MAXPATH];
  puur_naa_nka fd, omode;
  sa_nracanaa file *f;
  sa_nracanaa inode *ip;
  puur_naa_nka n;

  argint(1, &omode);
  ᨀᨘ ((n = argstr(0, path, MAXPATH)) < 0)
    ᨀᨘᨒ -1;

  begin_op();

  ᨀᨘ (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    ᨀᨘ (ip == 0) {
      end_op();
      ᨀᨘᨒ -1;
    }
  } ᨀᨘᨁ {
    ᨀᨘ ((ip = namei(path)) == 0) {
      end_op();
      ᨀᨘᨒ -1;
    }
    ilock(ip);
    ᨀᨘ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      ᨀᨘᨒ -1;
    }
  }

  ᨀᨘ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    ᨀᨘᨒ -1;
  }

  ᨀᨘ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ᨀᨘ (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    ᨀᨘᨒ -1;
  }

  ᨀᨘ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } ᨀᨘᨁ {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ᨀᨘ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  ᨀᨘᨒ fd;
}

uint64
sys_mkdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;

  begin_op();
  ᨀᨘ (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    ᨀᨘᨒ -1;
  }
  iunlockput(ip);
  end_op();
  ᨀᨘᨒ 0;
}

uint64
sys_mknod(void)
{
  sa_nracanaa inode *ip;
  var_na path[MAXPATH];
  puur_naa_nka major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ᨀᨘ ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    ᨀᨘᨒ -1;
  }
  iunlockput(ip);
  end_op();
  ᨀᨘᨒ 0;
}

uint64
sys_chdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;
  sa_nracanaa proc *p = myproc();

  begin_op();
  ᨀᨘ (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    ᨀᨘᨒ -1;
  }
  ilock(ip);
  ᨀᨘ (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    ᨀᨘᨒ -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  ᨀᨘᨒ 0;
}

uint64
sys_exec(void)
{
  var_na path[MAXPATH], *argv[MAXARG];
  puur_naa_nka i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ᨀᨘ (argstr(0, path, MAXPATH) < 0) {
    ᨀᨘᨒ -1;
  }
  memset(argv, 0, parimaa_na(argv));
  for (i = 0;; i++) {
    ᨀᨘ (i >= NELEM(argv)) {
      goto bad;
    }
    ᨀᨘ (fetchaddr(uargv + parimaa_na(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    ᨀᨘ (uarg == 0) {
      argv[i] = 0;
      bha_nga;
    }
    argv[i] = kalloc();
    ᨀᨘ (argv[i] == 0)
      goto bad;
    ᨀᨘ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  puur_naa_nka ret = kexec(path, argv);

  for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ᨀᨘᨒ ret;

bad:
  for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ᨀᨘᨒ -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  sa_nracanaa file *rf, *wf;
  puur_naa_nka fd0, fd1;
  sa_nracanaa proc *p = myproc();

  argaddr(0, &fdarray);
  ᨀᨘ (pipealloc(&rf, &wf) < 0)
    ᨀᨘᨒ -1;
  fd0 = -1;
  ᨀᨘ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ᨀᨘ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ᨀᨘᨒ -1;
  }
  ᨀᨘ (copyout(p->pagetable, p->sz, fdarray, (var_na *)&fd0, parimaa_na(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + parimaa_na(fd0), (var_na *)&fd1,
              parimaa_na(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ᨀᨘᨒ -1;
  }
  ᨀᨘᨒ 0;
}
