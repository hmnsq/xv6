//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "stat.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "file.h"
#ಅನ್ತರ್ಭಾವ "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
argfd(ಪೂರ್ಣಾಙ್ಕ n, ಪೂರ್ಣಾಙ್ಕ *pfd, ಸಂರಚನಾ file **pf)
{
  ಪೂರ್ಣಾಙ್ಕ fd;
  ಸಂರಚನಾ file *f;

  argint(n, &fd);
  ಯದಿ (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ (pfd)
    *pfd = fd;
  ಯದಿ (pf)
    *pf = f;
  ಪ್ರತ್ಯಾಗಮ 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fdalloc(ಸಂರಚನಾ file *f)
{
  ಪೂರ್ಣಾಙ್ಕ fd;
  ಸಂರಚನಾ proc *p = myproc();

  ಪ್ರತಿ (fd = 0; fd < NOFILE; fd++) {
    ಯದಿ (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      ಪ್ರತ್ಯಾಗಮ fd;
    }
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

uint64
sys_dup(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ file *f;
  ಪೂರ್ಣಾಙ್ಕ fd;

  ಯದಿ (argfd(0, 0, &f) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ ((fd = fdalloc(f)) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  filedup(f);
  ಪ್ರತ್ಯಾಗಮ fd;
}

uint64
sys_read(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ file *f;
  ಪೂರ್ಣಾಙ್ಕ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ಯದಿ (argfd(0, 0, &f) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಪ್ರತ್ಯಾಗಮ fileread(f, p, n);
}

uint64
sys_write(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ file *f;
  ಪೂರ್ಣಾಙ್ಕ n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  ಯದಿ (argfd(0, 0, &f) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;

  ಪ್ರತ್ಯಾಗಮ filewrite(f, p, n);
}

uint64
sys_close(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ fd;
  ಸಂರಚನಾ file *f;

  ಯದಿ (argfd(0, &fd, &f) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  ಪ್ರತ್ಯಾಗಮ 0;
}

uint64
sys_fstat(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  ಯದಿ (argfd(0, 0, &f) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  ಪ್ರತ್ಯಾಗಮ filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(ಶೂನ್ಯ)
{
  ವರ್ಣ name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  ಸಂರಚನಾ inode *dp, *ip;

  ಯದಿ (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;

  begin_op();
  ಯದಿ ((ip = namei(old)) == 0) {
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  ilock(ip);
  ಯದಿ (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  ಯದಿ (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  ಯದಿ ((dp = nameiparent(new, name)) == 0)
    ಗಚ್ಛ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  ಯದಿ (dp->nlink == 0) {
    iunlockput(dp);
    ಗಚ್ಛ bad;
  }
  ಯದಿ (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    ಗಚ್ಛ bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  ಪ್ರತ್ಯಾಗಮ 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  ಪ್ರತ್ಯಾಗಮ -1;
}

// Is the directory dp empty except for "." and ".." ?
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
isdirempty(ಸಂರಚನಾ inode *dp)
{
  ಪೂರ್ಣಾಙ್ಕ off;
  ಸಂರಚನಾ dirent de;

  ಪ್ರತಿ (off = 2 * ಪರಿಮಾಣ(de); off < dp->size; off += ಪರಿಮಾಣ(de)) {
    ಯದಿ (readi(dp, 0, (uint64)&de, off, ಪರಿಮಾಣ(de)) != ಪರಿಮಾಣ(de))
      panic("isdirempty: readi");
    ಯದಿ (de.inum != 0)
      ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ 1;
}

uint64
sys_unlink(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ inode *ip, *dp;
  ಸಂರಚನಾ dirent de;
  ವರ್ಣ name[DIRSIZ], path[MAXPATH];
  uint off;

  ಯದಿ (argstr(0, path, MAXPATH) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;

  begin_op();
  ಯದಿ ((dp = nameiparent(path, name)) == 0) {
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  ಯದಿ (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    ಗಚ್ಛ bad;

  ಯದಿ ((ip = dirlookup(dp, name, &off)) == 0)
    ಗಚ್ಛ bad;
  ilock(ip);

  ಯದಿ (ip->nlink < 1)
    panic("unlink: nlink < 1");
  ಯದಿ (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    ಗಚ್ಛ bad;
  }

  memset(&de, 0, ಪರಿಮಾಣ(de));
  ಯದಿ (writei(dp, 0, (uint64)&de, off, ಪರಿಮಾಣ(de)) != ಪರಿಮಾಣ(de))
    panic("unlink: writei");
  ಯದಿ (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  ಪ್ರತ್ಯಾಗಮ 0;

bad:
  iunlockput(dp);
  end_op();
  ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಸಂರಚನಾ inode *
create(ವರ್ಣ *path, short type, short major, short minor)
{
  ಸಂರಚನಾ inode *ip, *dp;
  ವರ್ಣ name[DIRSIZ];

  ಯದಿ ((dp = nameiparent(path, name)) == 0)
    ಪ್ರತ್ಯಾಗಮ 0;

  ilock(dp);

  ಯದಿ (dp->nlink == 0) {
    iunlockput(dp);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  ಯದಿ (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    ಯದಿ (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      ಪ್ರತ್ಯಾಗಮ ip;
    iunlockput(ip);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  ಯದಿ (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    ಯದಿ (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      ಗಚ್ಛ fail;
  }

  ಯದಿ (dirlink(dp, name, ip->inum) < 0)
    ಗಚ್ಛ fail;

  ಯದಿ (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  ಪ್ರತ್ಯಾಗಮ ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  ಪ್ರತ್ಯಾಗಮ 0;
}

uint64
sys_open(ಶೂನ್ಯ)
{
  ವರ್ಣ path[MAXPATH];
  ಪೂರ್ಣಾಙ್ಕ fd, omode;
  ಸಂರಚನಾ file *f;
  ಸಂರಚನಾ inode *ip;
  ಪೂರ್ಣಾಙ್ಕ n;

  argint(1, &omode);
  ಯದಿ ((n = argstr(0, path, MAXPATH)) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;

  begin_op();

  ಯದಿ (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    ಯದಿ (ip == 0) {
      end_op();
      ಪ್ರತ್ಯಾಗಮ -1;
    }
  } ಅನ್ಯಥಾ {
    ಯದಿ ((ip = namei(path)) == 0) {
      end_op();
      ಪ್ರತ್ಯಾಗಮ -1;
    }
    ilock(ip);
    ಯದಿ (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      ಪ್ರತ್ಯಾಗಮ -1;
    }
  }

  ಯದಿ (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  ಯದಿ ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    ಯದಿ (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  ಯದಿ (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } ಅನ್ಯಥಾ {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  ಯದಿ ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  ಪ್ರತ್ಯಾಗಮ fd;
}

uint64
sys_mkdir(ಶೂನ್ಯ)
{
  ವರ್ಣ path[MAXPATH];
  ಸಂರಚನಾ inode *ip;

  begin_op();
  ಯದಿ (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  iunlockput(ip);
  end_op();
  ಪ್ರತ್ಯಾಗಮ 0;
}

uint64
sys_mknod(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ inode *ip;
  ವರ್ಣ path[MAXPATH];
  ಪೂರ್ಣಾಙ್ಕ major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  ಯದಿ ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  iunlockput(ip);
  end_op();
  ಪ್ರತ್ಯಾಗಮ 0;
}

uint64
sys_chdir(ಶೂನ್ಯ)
{
  ವರ್ಣ path[MAXPATH];
  ಸಂರಚನಾ inode *ip;
  ಸಂರಚನಾ proc *p = myproc();

  begin_op();
  ಯದಿ (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ilock(ip);
  ಯದಿ (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  ಪ್ರತ್ಯಾಗಮ 0;
}

uint64
sys_exec(ಶೂನ್ಯ)
{
  ವರ್ಣ path[MAXPATH], *argv[MAXARG];
  ಪೂರ್ಣಾಙ್ಕ i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  ಯದಿ (argstr(0, path, MAXPATH) < 0) {
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  memset(argv, 0, ಪರಿಮಾಣ(argv));
  ಪ್ರತಿ (i = 0;; i++) {
    ಯದಿ (i >= NELEM(argv)) {
      ಗಚ್ಛ bad;
    }
    ಯದಿ (fetchaddr(uargv + ಪರಿಮಾಣ(uint64) * i, (uint64 *)&uarg) < 0) {
      ಗಚ್ಛ bad;
    }
    ಯದಿ (uarg == 0) {
      argv[i] = 0;
      ಭಙ್ಗ;
    }
    argv[i] = kalloc();
    ಯದಿ (argv[i] == 0)
      ಗಚ್ಛ bad;
    ಯದಿ (fetchstr(uarg, argv[i], PGSIZE) < 0)
      ಗಚ್ಛ bad;
  }

  ಪೂರ್ಣಾಙ್ಕ ret = kexec(path, argv);

  ಪ್ರತಿ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  ಪ್ರತ್ಯಾಗಮ ret;

bad:
  ಪ್ರತಿ (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  ಪ್ರತ್ಯಾಗಮ -1;
}

uint64
sys_pipe(ಶೂನ್ಯ)
{
  uint64 fdarray; // user pointer to array of two integers
  ಸಂರಚನಾ file *rf, *wf;
  ಪೂರ್ಣಾಙ್ಕ fd0, fd1;
  ಸಂರಚನಾ proc *p = myproc();

  argaddr(0, &fdarray);
  ಯದಿ (pipealloc(&rf, &wf) < 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  fd0 = -1;
  ಯದಿ ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    ಯದಿ (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಯದಿ (copyout(p->pagetable, p->sz, fdarray, (ವರ್ಣ *)&fd0, ಪರಿಮಾಣ(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + ಪರಿಮಾಣ(fd0), (ವರ್ಣ *)&fd1,
              ಪರಿಮಾಣ(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
