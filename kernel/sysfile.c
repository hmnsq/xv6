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
  ᱡᱩᱫᱤ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ᱨᱩᱣᱟᱹᱲ -1;
  ᱡᱩᱫᱤ (pfd)
    *pfd = fd;
  ᱡᱩᱫᱤ (pf)
    *pf = f;
  ᱨᱩᱣᱟᱹᱲ 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static puur_naa_nka
fdalloc(sa_nracanaa file *f)
{
  puur_naa_nka fd;
  sa_nracanaa proc *p = myproc();

  for (fd = 0; fd < NOFILE; fd++) {
    ᱡᱩᱫᱤ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ᱨᱩᱣᱟᱹᱲ fd;
    }
  }
  ᱨᱩᱣᱟᱹᱲ -1;
}

uint64
sys_dup(void)
{
  sa_nracanaa file *f;
  puur_naa_nka fd;

  ᱡᱩᱫᱤ (argfd(0, 0, &f) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;
  ᱡᱩᱫᱤ ((fd = fdalloc(f)) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;
  filedup(f);
  ᱨᱩᱣᱟᱹᱲ fd;
}

uint64
sys_read(void)
{
  sa_nracanaa file *f;
  puur_naa_nka n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ᱡᱩᱫᱤ (argfd(0, 0, &f) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;
  ᱨᱩᱣᱟᱹᱲ fileread(f, p, n);
}

uint64
sys_write(void)
{
  sa_nracanaa file *f;
  puur_naa_nka n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ᱡᱩᱫᱤ (argfd(0, 0, &f) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;

  ᱨᱩᱣᱟᱹᱲ filewrite(f, p, n);
}

uint64
sys_close(void)
{
  puur_naa_nka fd;
  sa_nracanaa file *f;

  ᱡᱩᱫᱤ (argfd(0, &fd, &f) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ᱨᱩᱣᱟᱹᱲ 0;
}

uint64
sys_fstat(void)
{
  sa_nracanaa file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ᱡᱩᱫᱤ (argfd(0, 0, &f) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;
  ᱨᱩᱣᱟᱹᱲ filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  var_na name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  sa_nracanaa inode *dp, *ip;

  ᱡᱩᱫᱤ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;

  begin_op();
  ᱡᱩᱫᱤ ((ip = namei(old)) == 0) {
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }

  ilock(ip);
  ᱡᱩᱫᱤ (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }

  ᱡᱩᱫᱤ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ᱡᱩᱫᱤ ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ᱡᱩᱫᱤ (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  ᱡᱩᱫᱤ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  ᱨᱩᱣᱟᱹᱲ 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  ᱨᱩᱣᱟᱹᱲ -1;
}

// Is the directory dp empty except for "." and ".." ?
static puur_naa_nka
isdirempty(sa_nracanaa inode *dp)
{
  puur_naa_nka off;
  sa_nracanaa dirent de;

  for (off = 2 * parimaa_na(de); off < dp->size; off += parimaa_na(de)) {
    ᱡᱩᱫᱤ (readi(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
      panic("isdirempty: readi");
    ᱡᱩᱫᱤ (de.inum != 0)
      ᱨᱩᱣᱟᱹᱲ 0;
  }
  ᱨᱩᱣᱟᱹᱲ 1;
}

uint64
sys_unlink(void)
{
  sa_nracanaa inode *ip, *dp;
  sa_nracanaa dirent de;
  var_na name[DIRSIZ], path[MAXPATH];
  uint off;

  ᱡᱩᱫᱤ (argstr(0, path, MAXPATH) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;

  begin_op();
  ᱡᱩᱫᱤ ((dp = nameiparent(path, name)) == 0) {
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ᱡᱩᱫᱤ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  ᱡᱩᱫᱤ ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  ᱡᱩᱫᱤ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ᱡᱩᱫᱤ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, parimaa_na(de));
  ᱡᱩᱫᱤ (writei(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
    panic("unlink: writei");
  ᱡᱩᱫᱤ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  ᱨᱩᱣᱟᱹᱲ 0;

bad:
  iunlockput(dp);
  end_op();
  ᱨᱩᱣᱟᱹᱲ -1;
}

static sa_nracanaa inode *
create(var_na *path, short type, short major, short minor)
{
  sa_nracanaa inode *ip, *dp;
  var_na name[DIRSIZ];

  ᱡᱩᱫᱤ ((dp = nameiparent(path, name)) == 0)
    ᱨᱩᱣᱟᱹᱲ 0;

  ilock(dp);

  ᱡᱩᱫᱤ (dp->nlink == 0) {
    iunlockput(dp);
    ᱨᱩᱣᱟᱹᱲ 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ᱡᱩᱫᱤ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ᱨᱩᱣᱟᱹᱲ 0;
  }

  ᱡᱩᱫᱤ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ᱡᱩᱫᱤ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ᱨᱩᱣᱟᱹᱲ ip;
    iunlockput(ip);
    ᱨᱩᱣᱟᱹᱲ 0;
  }

  ᱡᱩᱫᱤ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ᱨᱩᱣᱟᱹᱲ 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ᱡᱩᱫᱤ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ᱡᱩᱫᱤ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  ᱡᱩᱫᱤ (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  ᱡᱩᱫᱤ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ᱨᱩᱣᱟᱹᱲ ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ᱨᱩᱣᱟᱹᱲ 0;
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
  ᱡᱩᱫᱤ ((n = argstr(0, path, MAXPATH)) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;

  begin_op();

  ᱡᱩᱫᱤ (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    ᱡᱩᱫᱤ (ip == 0) {
      end_op();
      ᱨᱩᱣᱟᱹᱲ -1;
    }
  } ᱵᱟᱝ {
    ᱡᱩᱫᱤ ((ip = namei(path)) == 0) {
      end_op();
      ᱨᱩᱣᱟᱹᱲ -1;
    }
    ilock(ip);
    ᱡᱩᱫᱤ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      ᱨᱩᱣᱟᱹᱲ -1;
    }
  }

  ᱡᱩᱫᱤ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }

  ᱡᱩᱫᱤ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ᱡᱩᱫᱤ (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }

  ᱡᱩᱫᱤ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } ᱵᱟᱝ {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ᱡᱩᱫᱤ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  ᱨᱩᱣᱟᱹᱲ fd;
}

uint64
sys_mkdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;

  begin_op();
  ᱡᱩᱫᱤ (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }
  iunlockput(ip);
  end_op();
  ᱨᱩᱣᱟᱹᱲ 0;
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
  ᱡᱩᱫᱤ ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }
  iunlockput(ip);
  end_op();
  ᱨᱩᱣᱟᱹᱲ 0;
}

uint64
sys_chdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;
  sa_nracanaa proc *p = myproc();

  begin_op();
  ᱡᱩᱫᱤ (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }
  ilock(ip);
  ᱡᱩᱫᱤ (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    ᱨᱩᱣᱟᱹᱲ -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  ᱨᱩᱣᱟᱹᱲ 0;
}

uint64
sys_exec(void)
{
  var_na path[MAXPATH], *argv[MAXARG];
  puur_naa_nka i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ᱡᱩᱫᱤ (argstr(0, path, MAXPATH) < 0) {
    ᱨᱩᱣᱟᱹᱲ -1;
  }
  memset(argv, 0, parimaa_na(argv));
  for (i = 0;; i++) {
    ᱡᱩᱫᱤ (i >= NELEM(argv)) {
      goto bad;
    }
    ᱡᱩᱫᱤ (fetchaddr(uargv + parimaa_na(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    ᱡᱩᱫᱤ (uarg == 0) {
      argv[i] = 0;
      bha_nga;
    }
    argv[i] = kalloc();
    ᱡᱩᱫᱤ (argv[i] == 0)
      goto bad;
    ᱡᱩᱫᱤ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  puur_naa_nka ret = kexec(path, argv);

  for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ᱨᱩᱣᱟᱹᱲ ret;

bad:
  for (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ᱨᱩᱣᱟᱹᱲ -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  sa_nracanaa file *rf, *wf;
  puur_naa_nka fd0, fd1;
  sa_nracanaa proc *p = myproc();

  argaddr(0, &fdarray);
  ᱡᱩᱫᱤ (pipealloc(&rf, &wf) < 0)
    ᱨᱩᱣᱟᱹᱲ -1;
  fd0 = -1;
  ᱡᱩᱫᱤ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ᱡᱩᱫᱤ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ᱨᱩᱣᱟᱹᱲ -1;
  }
  ᱡᱩᱫᱤ (copyout(p->pagetable, p->sz, fdarray, (var_na *)&fd0, parimaa_na(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + parimaa_na(fd0), (var_na *)&fd1,
              parimaa_na(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ᱨᱩᱣᱟᱹᱲ -1;
  }
  ᱨᱩᱣᱟᱹᱲ 0;
}
