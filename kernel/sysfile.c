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
static ཧྲིལ
argfd(ཧྲིལ n, ཧྲིལ *pfd, sa_nracanaa file **pf)
{
  ཧྲིལ fd;
  sa_nracanaa file *f;

  argint(n, &fd);
  གལ་སྲིད (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ལོག -1;
  གལ་སྲིད (pfd)
    *pfd = fd;
  གལ་སྲིད (pf)
    *pf = f;
  ལོག 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static ཧྲིལ
fdalloc(sa_nracanaa file *f)
{
  ཧྲིལ fd;
  sa_nracanaa proc *p = myproc();

  དོན་ལུ (fd = 0; fd < NOFILE; fd++) {
    གལ་སྲིད (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ལོག fd;
    }
  }
  ལོག -1;
}

uint64
sys_dup(void)
{
  sa_nracanaa file *f;
  ཧྲིལ fd;

  གལ་སྲིད (argfd(0, 0, &f) < 0)
    ལོག -1;
  གལ་སྲིད ((fd = fdalloc(f)) < 0)
    ལོག -1;
  filedup(f);
  ལོག fd;
}

uint64
sys_read(void)
{
  sa_nracanaa file *f;
  ཧྲིལ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  གལ་སྲིད (argfd(0, 0, &f) < 0)
    ལོག -1;
  ལོག fileread(f, p, n);
}

uint64
sys_write(void)
{
  sa_nracanaa file *f;
  ཧྲིལ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  གལ་སྲིད (argfd(0, 0, &f) < 0)
    ལོག -1;

  ལོག filewrite(f, p, n);
}

uint64
sys_close(void)
{
  ཧྲིལ fd;
  sa_nracanaa file *f;

  གལ་སྲིད (argfd(0, &fd, &f) < 0)
    ལོག -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ལོག 0;
}

uint64
sys_fstat(void)
{
  sa_nracanaa file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  གལ་སྲིད (argfd(0, 0, &f) < 0)
    ལོག -1;
  ལོག filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(void)
{
  var_na name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  sa_nracanaa inode *dp, *ip;

  གལ་སྲིད (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ལོག -1;

  begin_op();
  གལ་སྲིད ((ip = namei(old)) == 0) {
    end_op();
    ལོག -1;
  }

  ilock(ip);
  གལ་སྲིད (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    ལོག -1;
  }

  གལ་སྲིད (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    ལོག -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  གལ་སྲིད ((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  གལ་སྲིད (dp->nlink == 0) {
    iunlockput(dp);
    goto bad;
  }
  གལ་སྲིད (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  ལོག 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  ལོག -1;
}

// Is the directory dp empty except for "." and ".." ?
static ཧྲིལ
isdirempty(sa_nracanaa inode *dp)
{
  ཧྲིལ off;
  sa_nracanaa dirent de;

  དོན་ལུ (off = 2 * parimaa_na(de); off < dp->size; off += parimaa_na(de)) {
    གལ་སྲིད (readi(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
      panic("isdirempty: readi");
    གལ་སྲིད (de.inum != 0)
      ལོག 0;
  }
  ལོག 1;
}

uint64
sys_unlink(void)
{
  sa_nracanaa inode *ip, *dp;
  sa_nracanaa dirent de;
  var_na name[DIRSIZ], path[MAXPATH];
  uint off;

  གལ་སྲིད (argstr(0, path, MAXPATH) < 0)
    ལོག -1;

  begin_op();
  གལ་སྲིད ((dp = nameiparent(path, name)) == 0) {
    end_op();
    ལོག -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  གལ་སྲིད (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  གལ་སྲིད ((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  གལ་སྲིད (ip->nlink < 1)
    panic("unlink: nlink < 1");
  གལ་སྲིད (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, parimaa_na(de));
  གལ་སྲིད (writei(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
    panic("unlink: writei");
  གལ་སྲིད (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  ལོག 0;

bad:
  iunlockput(dp);
  end_op();
  ལོག -1;
}

static sa_nracanaa inode *
create(var_na *path, short type, short major, short minor)
{
  sa_nracanaa inode *ip, *dp;
  var_na name[DIRSIZ];

  གལ་སྲིད ((dp = nameiparent(path, name)) == 0)
    ལོག 0;

  ilock(dp);

  གལ་སྲིད (dp->nlink == 0) {
    iunlockput(dp);
    ལོག 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  གལ་སྲིད (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ལོག 0;
  }

  གལ་སྲིད ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    གལ་སྲིད (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ལོག ip;
    iunlockput(ip);
    ལོག 0;
  }

  གལ་སྲིད ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ལོག 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  གལ་སྲིད (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    གལ་སྲིད (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }

  གལ་སྲིད (dirlink(dp, name, ip->inum) < 0)
    goto fail;

  གལ་སྲིད (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ལོག ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ལོག 0;
}

uint64
sys_open(void)
{
  var_na path[MAXPATH];
  ཧྲིལ fd, omode;
  sa_nracanaa file *f;
  sa_nracanaa inode *ip;
  ཧྲིལ n;

  argint(1, &omode);
  གལ་སྲིད ((n = argstr(0, path, MAXPATH)) < 0)
    ལོག -1;

  begin_op();

  གལ་སྲིད (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    གལ་སྲིད (ip == 0) {
      end_op();
      ལོག -1;
    }
  } དེ་མིན {
    གལ་སྲིད ((ip = namei(path)) == 0) {
      end_op();
      ལོག -1;
    }
    ilock(ip);
    གལ་སྲིད (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      ལོག -1;
    }
  }

  གལ་སྲིད (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    ལོག -1;
  }

  གལ་སྲིད ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    གལ་སྲིད (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    ལོག -1;
  }

  གལ་སྲིད (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } དེ་མིན {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  གལ་སྲིད ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  ལོག fd;
}

uint64
sys_mkdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;

  begin_op();
  གལ་སྲིད (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    ལོག -1;
  }
  iunlockput(ip);
  end_op();
  ལོག 0;
}

uint64
sys_mknod(void)
{
  sa_nracanaa inode *ip;
  var_na path[MAXPATH];
  ཧྲིལ major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  གལ་སྲིད ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    ལོག -1;
  }
  iunlockput(ip);
  end_op();
  ལོག 0;
}

uint64
sys_chdir(void)
{
  var_na path[MAXPATH];
  sa_nracanaa inode *ip;
  sa_nracanaa proc *p = myproc();

  begin_op();
  གལ་སྲིད (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    ལོག -1;
  }
  ilock(ip);
  གལ་སྲིད (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    ལོག -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  ལོག 0;
}

uint64
sys_exec(void)
{
  var_na path[MAXPATH], *argv[MAXARG];
  ཧྲིལ i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  གལ་སྲིད (argstr(0, path, MAXPATH) < 0) {
    ལོག -1;
  }
  memset(argv, 0, parimaa_na(argv));
  དོན་ལུ (i = 0;; i++) {
    གལ་སྲིད (i >= NELEM(argv)) {
      goto bad;
    }
    གལ་སྲིད (fetchaddr(uargv + parimaa_na(uint64) * i, (uint64 *)&uarg) < 0) {
      goto bad;
    }
    གལ་སྲིད (uarg == 0) {
      argv[i] = 0;
      bha_nga;
    }
    argv[i] = kalloc();
    གལ་སྲིད (argv[i] == 0)
      goto bad;
    གལ་སྲིད (fetchstr(uarg, argv[i], PGSIZE) < 0)
      goto bad;
  }

  ཧྲིལ ret = kexec(path, argv);

  དོན་ལུ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ལོག ret;

bad:
  དོན་ལུ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ལོག -1;
}

uint64
sys_pipe(void)
{
  uint64 fdarray; // user pointer to array of two integers
  sa_nracanaa file *rf, *wf;
  ཧྲིལ fd0, fd1;
  sa_nracanaa proc *p = myproc();

  argaddr(0, &fdarray);
  གལ་སྲིད (pipealloc(&rf, &wf) < 0)
    ལོག -1;
  fd0 = -1;
  གལ་སྲིད ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    གལ་སྲིད (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ལོག -1;
  }
  གལ་སྲིད (copyout(p->pagetable, p->sz, fdarray, (var_na *)&fd0, parimaa_na(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + parimaa_na(fd0), (var_na *)&fd1,
              parimaa_na(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ལོག -1;
  }
  ལོག 0;
}
