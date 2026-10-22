//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#ضمن "types.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "param.h"
#ضمن "stat.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "fs.h"
#ضمن "sleeplock.h"
#ضمن "file.h"
#ضمن "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
ساكن صحيح
argfd(صحيح n, صحيح *pfd, بنية file **pf)
{
  صحيح fd;
  بنية file *f;

  argint(n, &fd);
  אם (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    החזר -1;
  אם (pfd)
    *pfd = fd;
  אם (pf)
    *pf = f;
  החזר 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
ساكن صحيح
fdalloc(بنية file *f)
{
  صحيح fd;
  بنية proc *p = myproc();

  עבור (fd = 0; fd < NOFILE; fd++) {
    אם (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      החזר fd;
    }
  }
  החזר -1;
}

uint64
sys_dup(فراغ)
{
  بنية file *f;
  صحيح fd;

  אם (argfd(0, 0, &f) < 0)
    החזר -1;
  אם ((fd = fdalloc(f)) < 0)
    החזר -1;
  filedup(f);
  החזר fd;
}

uint64
sys_read(فراغ)
{
  بنية file *f;
  صحيح n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  אם (argfd(0, 0, &f) < 0)
    החזר -1;
  החזר fileread(f, p, n);
}

uint64
sys_write(فراغ)
{
  بنية file *f;
  صحيح n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  אם (argfd(0, 0, &f) < 0)
    החזר -1;

  החזר filewrite(f, p, n);
}

uint64
sys_close(فراغ)
{
  صحيح fd;
  بنية file *f;

  אם (argfd(0, &fd, &f) < 0)
    החזר -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  החזר 0;
}

uint64
sys_fstat(فراغ)
{
  بنية file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  אם (argfd(0, 0, &f) < 0)
    החזר -1;
  החזר filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(فراغ)
{
  حرف name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  بنية inode *dp, *ip;

  אם (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    החזר -1;

  begin_op();
  אם ((ip = namei(old)) == 0) {
    end_op();
    החזר -1;
  }

  ilock(ip);
  אם (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    החזר -1;
  }

  אם (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    החזר -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  אם ((dp = nameiparent(new, name)) == 0)
    اذهب bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  אם (dp->nlink == 0) {
    iunlockput(dp);
    اذهب bad;
  }
  אם (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    اذهب bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  החזר 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  החזר -1;
}

// Is the directory dp empty except for "." and ".." ?
ساكن صحيح
isdirempty(بنية inode *dp)
{
  صحيح off;
  بنية dirent de;

  עבור (off = 2 * حجم(de); off < dp->size; off += حجم(de)) {
    אם (readi(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
      panic("isdirempty: readi");
    אם (de.inum != 0)
      החזר 0;
  }
  החזר 1;
}

uint64
sys_unlink(فراغ)
{
  بنية inode *ip, *dp;
  بنية dirent de;
  حرف name[DIRSIZ], path[MAXPATH];
  uint off;

  אם (argstr(0, path, MAXPATH) < 0)
    החזר -1;

  begin_op();
  אם ((dp = nameiparent(path, name)) == 0) {
    end_op();
    החזר -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  אם (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    اذهب bad;

  אם ((ip = dirlookup(dp, name, &off)) == 0)
    اذهب bad;
  ilock(ip);

  אם (ip->nlink < 1)
    panic("unlink: nlink < 1");
  אם (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    اذهب bad;
  }

  memset(&de, 0, حجم(de));
  אם (writei(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
    panic("unlink: writei");
  אם (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  החזר 0;

bad:
  iunlockput(dp);
  end_op();
  החזר -1;
}

ساكن بنية inode *
create(حرف *path, short type, short major, short minor)
{
  بنية inode *ip, *dp;
  حرف name[DIRSIZ];

  אם ((dp = nameiparent(path, name)) == 0)
    החזר 0;

  ilock(dp);

  אם (dp->nlink == 0) {
    iunlockput(dp);
    החזר 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  אם (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    החזר 0;
  }

  אם ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    אם (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      החזר ip;
    iunlockput(ip);
    החזר 0;
  }

  אם ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    החזר 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  אם (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    אם (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      اذهب fail;
  }

  אם (dirlink(dp, name, ip->inum) < 0)
    اذهب fail;

  אם (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  החזר ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  החזר 0;
}

uint64
sys_open(فراغ)
{
  حرف path[MAXPATH];
  صحيح fd, omode;
  بنية file *f;
  بنية inode *ip;
  صحيح n;

  argint(1, &omode);
  אם ((n = argstr(0, path, MAXPATH)) < 0)
    החזר -1;

  begin_op();

  אם (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    אם (ip == 0) {
      end_op();
      החזר -1;
    }
  } אחרת {
    אם ((ip = namei(path)) == 0) {
      end_op();
      החזר -1;
    }
    ilock(ip);
    אם (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      החזר -1;
    }
  }

  אם (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    החזר -1;
  }

  אם ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    אם (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    החזר -1;
  }

  אם (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } אחרת {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  אם ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  החזר fd;
}

uint64
sys_mkdir(فراغ)
{
  حرف path[MAXPATH];
  بنية inode *ip;

  begin_op();
  אם (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    החזר -1;
  }
  iunlockput(ip);
  end_op();
  החזר 0;
}

uint64
sys_mknod(فراغ)
{
  بنية inode *ip;
  حرف path[MAXPATH];
  صحيح major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  אם ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    החזר -1;
  }
  iunlockput(ip);
  end_op();
  החזר 0;
}

uint64
sys_chdir(فراغ)
{
  حرف path[MAXPATH];
  بنية inode *ip;
  بنية proc *p = myproc();

  begin_op();
  אם (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    החזר -1;
  }
  ilock(ip);
  אם (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    החזר -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  החזר 0;
}

uint64
sys_exec(فراغ)
{
  حرف path[MAXPATH], *argv[MAXARG];
  صحيح i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  אם (argstr(0, path, MAXPATH) < 0) {
    החזר -1;
  }
  memset(argv, 0, حجم(argv));
  עבור (i = 0;; i++) {
    אם (i >= NELEM(argv)) {
      اذهب bad;
    }
    אם (fetchaddr(uargv + حجم(uint64) * i, (uint64 *)&uarg) < 0) {
      اذهب bad;
    }
    אם (uarg == 0) {
      argv[i] = 0;
      اكسر;
    }
    argv[i] = kalloc();
    אם (argv[i] == 0)
      اذهب bad;
    אם (fetchstr(uarg, argv[i], PGSIZE) < 0)
      اذهب bad;
  }

  صحيح ret = kexec(path, argv);

  עבור (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  החזר ret;

bad:
  עבור (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  החזר -1;
}

uint64
sys_pipe(فراغ)
{
  uint64 fdarray; // user pointer to array of two integers
  بنية file *rf, *wf;
  صحيح fd0, fd1;
  بنية proc *p = myproc();

  argaddr(0, &fdarray);
  אם (pipealloc(&rf, &wf) < 0)
    החזר -1;
  fd0 = -1;
  אם ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    אם (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    החזר -1;
  }
  אם (copyout(p->pagetable, p->sz, fdarray, (حرف *)&fd0, حجم(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + حجم(fd0), (حرف *)&fd1,
              حجم(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    החזר -1;
  }
  החזר 0;
}
