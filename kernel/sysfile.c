//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "stat.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "fs.h"
#અન્તર્ભાવ "sleeplock.h"
#અન્તર્ભાવ "file.h"
#અન્તર્ભાવ "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
સ્થિર પૂર્ણાંક
argfd(પૂર્ણાંક n, પૂર્ણાંક *pfd, સંરચના file **pf)
{
  પૂર્ણાંક fd;
  સંરચના file *f;

  argint(n, &fd);
  જો (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    પાછા -1;
  જો (pfd)
    *pfd = fd;
  જો (pf)
    *pf = f;
  પાછા 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
સ્થિર પૂર્ણાંક
fdalloc(સંરચના file *f)
{
  પૂર્ણાંક fd;
  સંરચના proc *p = myproc();

  માટે (fd = 0; fd < NOFILE; fd++) {
    જો (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      પાછા fd;
    }
  }
  પાછા -1;
}

uint64
sys_dup(શૂન્ય)
{
  સંરચના file *f;
  પૂર્ણાંક fd;

  જો (argfd(0, 0, &f) < 0)
    પાછા -1;
  જો ((fd = fdalloc(f)) < 0)
    પાછા -1;
  filedup(f);
  પાછા fd;
}

uint64
sys_read(શૂન્ય)
{
  સંરચના file *f;
  પૂર્ણાંક n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  જો (argfd(0, 0, &f) < 0)
    પાછા -1;
  પાછા fileread(f, p, n);
}

uint64
sys_write(શૂન્ય)
{
  સંરચના file *f;
  પૂર્ણાંક n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  જો (argfd(0, 0, &f) < 0)
    પાછા -1;

  પાછા filewrite(f, p, n);
}

uint64
sys_close(શૂન્ય)
{
  પૂર્ણાંક fd;
  સંરચના file *f;

  જો (argfd(0, &fd, &f) < 0)
    પાછા -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  પાછા 0;
}

uint64
sys_fstat(શૂન્ય)
{
  સંરચના file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  જો (argfd(0, 0, &f) < 0)
    પાછા -1;
  પાછા filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(શૂન્ય)
{
  અક્ષર name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  સંરચના inode *dp, *ip;

  જો (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    પાછા -1;

  begin_op();
  જો ((ip = namei(old)) == 0) {
    end_op();
    પાછા -1;
  }

  ilock(ip);
  જો (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    પાછા -1;
  }

  જો (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    પાછા -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  જો ((dp = nameiparent(new, name)) == 0)
    જાઓ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  જો (dp->nlink == 0) {
    iunlockput(dp);
    જાઓ bad;
  }
  જો (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    જાઓ bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  પાછા 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  પાછા -1;
}

// Is the directory dp empty except for "." and ".." ?
સ્થિર પૂર્ણાંક
isdirempty(સંરચના inode *dp)
{
  પૂર્ણાંક off;
  સંરચના dirent de;

  માટે (off = 2 * કદ(de); off < dp->size; off += કદ(de)) {
    જો (readi(dp, 0, (uint64)&de, off, કદ(de)) != કદ(de))
      panic("isdirempty: readi");
    જો (de.inum != 0)
      પાછા 0;
  }
  પાછા 1;
}

uint64
sys_unlink(શૂન્ય)
{
  સંરચના inode *ip, *dp;
  સંરચના dirent de;
  અક્ષર name[DIRSIZ], path[MAXPATH];
  uint off;

  જો (argstr(0, path, MAXPATH) < 0)
    પાછા -1;

  begin_op();
  જો ((dp = nameiparent(path, name)) == 0) {
    end_op();
    પાછા -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  જો (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    જાઓ bad;

  જો ((ip = dirlookup(dp, name, &off)) == 0)
    જાઓ bad;
  ilock(ip);

  જો (ip->nlink < 1)
    panic("unlink: nlink < 1");
  જો (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    જાઓ bad;
  }

  memset(&de, 0, કદ(de));
  જો (writei(dp, 0, (uint64)&de, off, કદ(de)) != કદ(de))
    panic("unlink: writei");
  જો (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  પાછા 0;

bad:
  iunlockput(dp);
  end_op();
  પાછા -1;
}

સ્થિર સંરચના inode *
create(અક્ષર *path, short type, short major, short minor)
{
  સંરચના inode *ip, *dp;
  અક્ષર name[DIRSIZ];

  જો ((dp = nameiparent(path, name)) == 0)
    પાછા 0;

  ilock(dp);

  જો (dp->nlink == 0) {
    iunlockput(dp);
    પાછા 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  જો (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    પાછા 0;
  }

  જો ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    જો (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      પાછા ip;
    iunlockput(ip);
    પાછા 0;
  }

  જો ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    પાછા 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  જો (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    જો (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      જાઓ fail;
  }

  જો (dirlink(dp, name, ip->inum) < 0)
    જાઓ fail;

  જો (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  પાછા ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  પાછા 0;
}

uint64
sys_open(શૂન્ય)
{
  અક્ષર path[MAXPATH];
  પૂર્ણાંક fd, omode;
  સંરચના file *f;
  સંરચના inode *ip;
  પૂર્ણાંક n;

  argint(1, &omode);
  જો ((n = argstr(0, path, MAXPATH)) < 0)
    પાછા -1;

  begin_op();

  જો (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    જો (ip == 0) {
      end_op();
      પાછા -1;
    }
  } નહીંતર {
    જો ((ip = namei(path)) == 0) {
      end_op();
      પાછા -1;
    }
    ilock(ip);
    જો (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      પાછા -1;
    }
  }

  જો (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    પાછા -1;
  }

  જો ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    જો (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    પાછા -1;
  }

  જો (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } નહીંતર {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  જો ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  પાછા fd;
}

uint64
sys_mkdir(શૂન્ય)
{
  અક્ષર path[MAXPATH];
  સંરચના inode *ip;

  begin_op();
  જો (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    પાછા -1;
  }
  iunlockput(ip);
  end_op();
  પાછા 0;
}

uint64
sys_mknod(શૂન્ય)
{
  સંરચના inode *ip;
  અક્ષર path[MAXPATH];
  પૂર્ણાંક major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  જો ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    પાછા -1;
  }
  iunlockput(ip);
  end_op();
  પાછા 0;
}

uint64
sys_chdir(શૂન્ય)
{
  અક્ષર path[MAXPATH];
  સંરચના inode *ip;
  સંરચના proc *p = myproc();

  begin_op();
  જો (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    પાછા -1;
  }
  ilock(ip);
  જો (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    પાછા -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  પાછા 0;
}

uint64
sys_exec(શૂન્ય)
{
  અક્ષર path[MAXPATH], *argv[MAXARG];
  પૂર્ણાંક i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  જો (argstr(0, path, MAXPATH) < 0) {
    પાછા -1;
  }
  memset(argv, 0, કદ(argv));
  માટે (i = 0;; i++) {
    જો (i >= NELEM(argv)) {
      જાઓ bad;
    }
    જો (fetchaddr(uargv + કદ(uint64) * i, (uint64 *)&uarg) < 0) {
      જાઓ bad;
    }
    જો (uarg == 0) {
      argv[i] = 0;
      તોડો;
    }
    argv[i] = kalloc();
    જો (argv[i] == 0)
      જાઓ bad;
    જો (fetchstr(uarg, argv[i], PGSIZE) < 0)
      જાઓ bad;
  }

  પૂર્ણાંક ret = kexec(path, argv);

  માટે (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  પાછા ret;

bad:
  માટે (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  પાછા -1;
}

uint64
sys_pipe(શૂન્ય)
{
  uint64 fdarray; // user pointer to array of two integers
  સંરચના file *rf, *wf;
  પૂર્ણાંક fd0, fd1;
  સંરચના proc *p = myproc();

  argaddr(0, &fdarray);
  જો (pipealloc(&rf, &wf) < 0)
    પાછા -1;
  fd0 = -1;
  જો ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    જો (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    પાછા -1;
  }
  જો (copyout(p->pagetable, p->sz, fdarray, (અક્ષર *)&fd0, કદ(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + કદ(fd0), (અક્ષર *)&fd1,
              કદ(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    પાછા -1;
  }
  પાછા 0;
}
