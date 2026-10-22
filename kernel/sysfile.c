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
static ကိန်းပြည့်
argfd(ကိန်းပြည့် n, ကိန်းပြည့် *pfd, sa_nracanaa file **pf)
{
  ကိန်းပြည့် fd;
  sa_nracanaa file *f;

  argint(n, &fd);
  အကယ်၍ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ပြန်ပေး -1;
  အကယ်၍ (pfd)
    *pfd = fd;
  အကယ်၍ (pf)
    *pf = f;
  ပြန်ပေး 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static ကိန်းပြည့်
fdalloc(sa_nracanaa file *f)
{
  ကိန်းပြည့် fd;
  sa_nracanaa proc *p = myproc();

  အတွက် (fd = 0; fd < NOFILE; fd++) {
    အကယ်၍ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ပြန်ပေး fd;
    }
  }
  ပြန်ပေး -1;
}

uint64
sys_dup(void)
{
  sa_nracanaa file *f;
  ကိန်းပြည့် fd;

  အကယ်၍ (argfd(0, 0, &f) < 0)
    ပြန်ပေး -1;
  အကယ်၍ ((fd = fdalloc(f)) < 0)
    ပြန်ပေး -1;
  filedup(f);
  ပြန်ပေး fd;
}

uint64
sys_read(void)
{
  sa_nracanaa file *f;
  ကိန်းပြည့် n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  အကယ်၍ (argfd(0, 0, &f) < 0)
    ပြန်ပေး -1;
  ပြန်ပေး fileread(f, p, n);
}

uint64
sys_write(void)
{
  sa_nracanaa file *f;
  ကိန်းပြည့် n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  အကယ်၍ (argfd(0, 0, &f) < 0)
    ပြန်ပေး -1;

  ပြန်ပေး filewrite(f, p, n);
}

uint64
sys_close(void)
{
  ကိန်းပြည့် fd;
  sa_nracanaa file *f;

  အကယ်၍ (argfd(0, &fd, &f) < 0)
    ပြန်ပေး -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ပြန်ပေး 0;
}

uint64
sys_fstat(void)
{
  sa_nracanaa file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  အကယ်၍ (argfd(0, 0, &f) < 0)
    ပြန်ပေး -1;
  ပြန်ပေး filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  var_na name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  sa_nracanaa inode *dp, *ip;

  အကယ်၍ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ပြန်ပေး -1;

  begin_op();
  အကယ်၍ ((ip = namei(old)) == 0) {
    end_op();
    ပြန်ပေး -1;
  }

  ilock(ip);
  အကယ်၍ (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    ပြန်ပေး -1;
  }

  အကယ်၍ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    ပြန်ပေး -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  အကယ်၍ ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  အကယ်၍ (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  အကယ်၍ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  ပြန်ပေး 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  ပြန်ပေး -1;
}

// Is the directory dp empty except for "." and ".." ?
static ကိန်းပြည့်
isdirempty(sa_nracanaa inode *dp)
{
  ကိန်းပြည့် off;
  sa_nracanaa dirent de;

  အတွက် (off = 2 * parimaa_na(de); off < dp->size; off += parimaa_na(de)) {
    အကယ်၍ (readi(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
      panic("isdirempty: readi");
    အကယ်၍ (de.inum != 0)
      ပြန်ပေး 0;
  }
  ပြန်ပေး 1;
}

uint64
sys_unlink(void)
{
  sa_nracanaa inode *ip, *dp;
  sa_nracanaa dirent de;
  var_na name[DIRSIZ], path[MAXPATH];
  uint off;

  အကယ်၍ (argstr(0, path, MAXPATH) < 0)
    ပြန်ပေး -1;

  begin_op();
  အကယ်၍ ((dp = nameiparent(path, name)) == 0) {
    end_op();
    ပြန်ပေး -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  အကယ်၍ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  အကယ်၍ ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  အကယ်၍ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  အကယ်၍ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, parimaa_na(de));
  အကယ်၍ (writei(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
    panic("unlink: writei");
  အကယ်၍ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  ပြန်ပေး 0;

bad:
  iunlockput(dp);
  end_op();
  ပြန်ပေး -1;
}

static sa_nracanaa inode *
create(var_na *path, short type, short major, short minor)
{
  sa_nracanaa inode *ip, *dp;
  var_na name[DIRSIZ];

  အကယ်၍ ((dp = nameiparent(path, name)) == 0)
    ပြန်ပေး 0;

  ilock(dp);

  အကယ်၍ (dp->nlink == 0) {
    iunlockput(dp);
    ပြန်ပေး 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  အကယ်၍ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ပြန်ပေး 0;
  }

  အကယ်၍ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    အကယ်၍ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ပြန်ပေး ip;
    iunlockput(ip);
    ပြန်ပေး 0;
  }

  အကယ်၍ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ပြန်ပေး 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  အကယ်၍ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    အကယ်၍ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  အကယ်၍ (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  အကယ်၍ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ပြန်ပေး ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ပြန်ပေး 0;
}

uint64
sys_open(void)
{
  var_na path[MAXPATH];
  ကိန်းပြည့် fd, omode;
  sa_nracanaa file *f;
  sa_nracanaa inode *ip;
  ကိန်းပြည့် n;

  argint(1, &omode);
  အကယ်၍ ((n = argstr(0, path, MAXPATH)) < 0)
    ပြန်ပေး -1;

  begin_op();

  အကယ်၍ (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    အကယ်၍ (ip == 0) {
      end_op();
      ပြန်ပေး -1;
    }
  } မဟုတ်ပါက {
    အကယ်၍ ((ip = namei(path)) == 0) {
      end_op();
      ပြန်ပေး -1;
    }
    ilock(ip);
    အကယ်၍ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      ပြန်ပေး -1;
    }
  }

  အကယ်၍ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    ပြန်ပေး -1;
  }

  အကယ်၍ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    အကယ်၍ (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    ပြန်ပေး -1;
  }

  အကယ်၍ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } မဟုတ်ပါက {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  အကယ်၍ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  ပြန်ပေး fd;
}

uint64
sys_mkdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;

  begin_op();
  အကယ်၍ (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    ပြန်ပေး -1;
  }
  iunlockput(ip);
  end_op();
  ပြန်ပေး 0;
}

uint64
sys_mknod(void)
{
  sa_nracanaa inode *ip;
  var_na path[MAXPATH];
  ကိန်းပြည့် major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  အကယ်၍ ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    ပြန်ပေး -1;
  }
  iunlockput(ip);
  end_op();
  ပြန်ပေး 0;
}

uint64
sys_chdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;
  sa_nracanaa proc *p = myproc();

  begin_op();
  အကယ်၍ (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    ပြန်ပေး -1;
  }
  ilock(ip);
  အကယ်၍ (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    ပြန်ပေး -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  ပြန်ပေး 0;
}

uint64
sys_exec(void)
{
  var_na path[MAXPATH], *argv[MAXARG];
  ကိန်းပြည့် i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  အကယ်၍ (argstr(0, path, MAXPATH) < 0) {
    ပြန်ပေး -1;
  }
  memset(argv, 0, parimaa_na(argv));
  အတွက် (i = 0;; i++) {
    အကယ်၍ (i >= NELEM(argv)) {
      goto bad;
    }
    အကယ်၍ (fetchaddr(uargv + parimaa_na(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    အကယ်၍ (uarg == 0) {
      argv[i] = 0;
      bha_nga;
    }
    argv[i] = kalloc();
    အကယ်၍ (argv[i] == 0)
      goto bad;
    အကယ်၍ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  ကိန်းပြည့် ret = kexec(path, argv);

  အတွက် (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ပြန်ပေး ret;

bad:
  အတွက် (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ပြန်ပေး -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  sa_nracanaa file *rf, *wf;
  ကိန်းပြည့် fd0, fd1;
  sa_nracanaa proc *p = myproc();

  argaddr(0, &fdarray);
  အကယ်၍ (pipealloc(&rf, &wf) < 0)
    ပြန်ပေး -1;
  fd0 = -1;
  အကယ်၍ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    အကယ်၍ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ပြန်ပေး -1;
  }
  အကယ်၍ (copyout(p->pagetable, p->sz, fdarray, (var_na *)&fd0, parimaa_na(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + parimaa_na(fd0), (var_na *)&fd1,
              parimaa_na(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ပြန်ပေး -1;
  }
  ပြန်ပေး 0;
}
