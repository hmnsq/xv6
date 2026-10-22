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
  𐤀𐤌 (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    أرجع -1;
  𐤀𐤌 (pfd)
    *pfd = fd;
  𐤀𐤌 (pf)
    *pf = f;
  أرجع 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
ساكن صحيح
fdalloc(بنية file *f)
{
  صحيح fd;
  بنية proc *p = myproc();

  لكل (fd = 0; fd < NOFILE; fd++) {
    𐤀𐤌 (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      أرجع fd;
    }
  }
  أرجع -1;
}

uint64
sys_dup(فراغ)
{
  بنية file *f;
  صحيح fd;

  𐤀𐤌 (argfd(0, 0, &f) < 0)
    أرجع -1;
  𐤀𐤌 ((fd = fdalloc(f)) < 0)
    أرجع -1;
  filedup(f);
  أرجع fd;
}

uint64
sys_read(فراغ)
{
  بنية file *f;
  صحيح n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  𐤀𐤌 (argfd(0, 0, &f) < 0)
    أرجع -1;
  أرجع fileread(f, p, n);
}

uint64
sys_write(فراغ)
{
  بنية file *f;
  صحيح n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  𐤀𐤌 (argfd(0, 0, &f) < 0)
    أرجع -1;

  أرجع filewrite(f, p, n);
}

uint64
sys_close(فراغ)
{
  صحيح fd;
  بنية file *f;

  𐤀𐤌 (argfd(0, &fd, &f) < 0)
    أرجع -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  أرجع 0;
}

uint64
sys_fstat(فراغ)
{
  بنية file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  𐤀𐤌 (argfd(0, 0, &f) < 0)
    أرجع -1;
  أرجع filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(فراغ)
{
  حرف name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  بنية inode *dp, *ip;

  𐤀𐤌 (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    أرجع -1;

  begin_op();
  𐤀𐤌 ((ip = namei(old)) == 0) {
    end_op();
    أرجع -1;
  }

  ilock(ip);
  𐤀𐤌 (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    أرجع -1;
  }

  𐤀𐤌 (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    أرجع -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  𐤀𐤌 ((dp = nameiparent(new, name)) == 0)
    اذهب bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  𐤀𐤌 (dp->nlink == 0) {
    iunlockput(dp);
    اذهب bad;
  }
  𐤀𐤌 (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    اذهب bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  أرجع 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  أرجع -1;
}

// Is the directory dp empty except for "." and ".." ?
ساكن صحيح
isdirempty(بنية inode *dp)
{
  صحيح off;
  بنية dirent de;

  لكل (off = 2 * حجم(de); off < dp->size; off += حجم(de)) {
    𐤀𐤌 (readi(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
      panic("isdirempty: readi");
    𐤀𐤌 (de.inum != 0)
      أرجع 0;
  }
  أرجع 1;
}

uint64
sys_unlink(فراغ)
{
  بنية inode *ip, *dp;
  بنية dirent de;
  حرف name[DIRSIZ], path[MAXPATH];
  uint off;

  𐤀𐤌 (argstr(0, path, MAXPATH) < 0)
    أرجع -1;

  begin_op();
  𐤀𐤌 ((dp = nameiparent(path, name)) == 0) {
    end_op();
    أرجع -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  𐤀𐤌 (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    اذهب bad;

  𐤀𐤌 ((ip = dirlookup(dp, name, &off)) == 0)
    اذهب bad;
  ilock(ip);

  𐤀𐤌 (ip->nlink < 1)
    panic("unlink: nlink < 1");
  𐤀𐤌 (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    اذهب bad;
  }

  memset(&de, 0, حجم(de));
  𐤀𐤌 (writei(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
    panic("unlink: writei");
  𐤀𐤌 (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  أرجع 0;

bad:
  iunlockput(dp);
  end_op();
  أرجع -1;
}

ساكن بنية inode *
create(حرف *path, short type, short major, short minor)
{
  بنية inode *ip, *dp;
  حرف name[DIRSIZ];

  𐤀𐤌 ((dp = nameiparent(path, name)) == 0)
    أرجع 0;

  ilock(dp);

  𐤀𐤌 (dp->nlink == 0) {
    iunlockput(dp);
    أرجع 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  𐤀𐤌 (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    أرجع 0;
  }

  𐤀𐤌 ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    𐤀𐤌 (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      أرجع ip;
    iunlockput(ip);
    أرجع 0;
  }

  𐤀𐤌 ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    أرجع 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  𐤀𐤌 (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    𐤀𐤌 (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      اذهب fail;
  }

  𐤀𐤌 (dirlink(dp, name, ip->inum) < 0)
    اذهب fail;

  𐤀𐤌 (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  أرجع ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  أرجع 0;
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
  𐤀𐤌 ((n = argstr(0, path, MAXPATH)) < 0)
    أرجع -1;

  begin_op();

  𐤀𐤌 (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    𐤀𐤌 (ip == 0) {
      end_op();
      أرجع -1;
    }
  } وإلا {
    𐤀𐤌 ((ip = namei(path)) == 0) {
      end_op();
      أرجع -1;
    }
    ilock(ip);
    𐤀𐤌 (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      أرجع -1;
    }
  }

  𐤀𐤌 (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    أرجع -1;
  }

  𐤀𐤌 ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    𐤀𐤌 (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    أرجع -1;
  }

  𐤀𐤌 (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } وإلا {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  𐤀𐤌 ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  أرجع fd;
}

uint64
sys_mkdir(فراغ)
{
  حرف path[MAXPATH];
  بنية inode *ip;

  begin_op();
  𐤀𐤌 (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    أرجع -1;
  }
  iunlockput(ip);
  end_op();
  أرجع 0;
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
  𐤀𐤌 ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    أرجع -1;
  }
  iunlockput(ip);
  end_op();
  أرجع 0;
}

uint64
sys_chdir(فراغ)
{
  حرف path[MAXPATH];
  بنية inode *ip;
  بنية proc *p = myproc();

  begin_op();
  𐤀𐤌 (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    أرجع -1;
  }
  ilock(ip);
  𐤀𐤌 (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    أرجع -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  أرجع 0;
}

uint64
sys_exec(فراغ)
{
  حرف path[MAXPATH], *argv[MAXARG];
  صحيح i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  𐤀𐤌 (argstr(0, path, MAXPATH) < 0) {
    أرجع -1;
  }
  memset(argv, 0, حجم(argv));
  لكل (i = 0;; i++) {
    𐤀𐤌 (i >= NELEM(argv)) {
      اذهب bad;
    }
    𐤀𐤌 (fetchaddr(uargv + حجم(uint64) * i, (uint64 *)&uarg) < 0) {
      اذهب bad;
    }
    𐤀𐤌 (uarg == 0) {
      argv[i] = 0;
      اكسر;
    }
    argv[i] = kalloc();
    𐤀𐤌 (argv[i] == 0)
      اذهب bad;
    𐤀𐤌 (fetchstr(uarg, argv[i], PGSIZE) < 0)
      اذهب bad;
  }

  صحيح ret = kexec(path, argv);

  لكل (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  أرجع ret;

bad:
  لكل (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  أرجع -1;
}

uint64
sys_pipe(فراغ)
{
  uint64 fdarray; // user pointer to array of two integers
  بنية file *rf, *wf;
  صحيح fd0, fd1;
  بنية proc *p = myproc();

  argaddr(0, &fdarray);
  𐤀𐤌 (pipealloc(&rf, &wf) < 0)
    أرجع -1;
  fd0 = -1;
  𐤀𐤌 ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    𐤀𐤌 (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    أرجع -1;
  }
  𐤀𐤌 (copyout(p->pagetable, p->sz, fdarray, (حرف *)&fd0, حجم(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + حجم(fd0), (حرف *)&fd1,
              حجم(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    أرجع -1;
  }
  أرجع 0;
}
