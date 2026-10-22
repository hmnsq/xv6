//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "stat.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "fs.h"
#شامل "sleeplock.h"
#شامل "file.h"
#شامل "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
ساکن صحیح
argfd(صحیح n, صحیح *pfd, ساخت file **pf)
{
  صحیح fd;
  ساخت file *f;

  argint(n, &fd);
  اگر (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    واپس -1;
  اگر (pfd)
    *pfd = fd;
  اگر (pf)
    *pf = f;
  واپس 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
ساکن صحیح
fdalloc(ساخت file *f)
{
  صحیح fd;
  ساخت proc *p = myproc();

  برائے (fd = 0; fd < NOFILE; fd++) {
    اگر (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      واپس fd;
    }
  }
  واپس -1;
}

uint64
sys_dup(خالی)
{
  ساخت file *f;
  صحیح fd;

  اگر (argfd(0, 0, &f) < 0)
    واپس -1;
  اگر ((fd = fdalloc(f)) < 0)
    واپس -1;
  filedup(f);
  واپس fd;
}

uint64
sys_read(خالی)
{
  ساخت file *f;
  صحیح n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  اگر (argfd(0, 0, &f) < 0)
    واپس -1;
  واپس fileread(f, p, n);
}

uint64
sys_write(خالی)
{
  ساخت file *f;
  صحیح n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  اگر (argfd(0, 0, &f) < 0)
    واپس -1;

  واپس filewrite(f, p, n);
}

uint64
sys_close(خالی)
{
  صحیح fd;
  ساخت file *f;

  اگر (argfd(0, &fd, &f) < 0)
    واپس -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  واپس 0;
}

uint64
sys_fstat(خالی)
{
  ساخت file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  اگر (argfd(0, 0, &f) < 0)
    واپس -1;
  واپس filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(خالی)
{
  حرف name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  ساخت inode *dp, *ip;

  اگر (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    واپس -1;

  begin_op();
  اگر ((ip = namei(old)) == 0) {
    انتہا_op();
    واپس -1;
  }

  ilock(ip);
  اگر (ip->type == T_DIR) {
    iunlockput(ip);
    انتہا_op();
    واپس -1;
  }

  اگر (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    انتہا_op();
    واپس -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  اگر ((dp = nameiparent(new, name)) == 0)
    جاؤ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  اگر (dp->nlink == 0) {
    iunlockput(dp);
    جاؤ bad;
  }
  اگر (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    جاؤ bad;
  }
  iunlockput(dp);
  iput(ip);

  انتہا_op();

  واپس 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  انتہا_op();
  واپس -1;
}

// Is the directory dp empty except for "." and ".." ?
ساکن صحیح
isdirempty(ساخت inode *dp)
{
  صحیح off;
  ساخت dirent de;

  برائے (off = 2 * حجم(de); off < dp->سائز; off += حجم(de)) {
    اگر (readi(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
      panic("isdirempty: readi");
    اگر (de.inum != 0)
      واپس 0;
  }
  واپس 1;
}

uint64
sys_unlink(خالی)
{
  ساخت inode *ip, *dp;
  ساخت dirent de;
  حرف name[DIRSIZ], راستہ[MAXPATH];
  uint off;

  اگر (argstr(0, راستہ, MAXPATH) < 0)
    واپس -1;

  begin_op();
  اگر ((dp = nameiparent(راستہ, name)) == 0) {
    انتہا_op();
    واپس -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  اگر (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    جاؤ bad;

  اگر ((ip = dirlookup(dp, name, &off)) == 0)
    جاؤ bad;
  ilock(ip);

  اگر (ip->nlink < 1)
    panic("unlink: nlink < 1");
  اگر (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    جاؤ bad;
  }

  memset(&de, 0, حجم(de));
  اگر (writei(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
    panic("unlink: writei");
  اگر (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  انتہا_op();

  واپس 0;

bad:
  iunlockput(dp);
  انتہا_op();
  واپس -1;
}

ساکن ساخت inode *
create(حرف *راستہ, مختصر type, مختصر major, مختصر minor)
{
  ساخت inode *ip, *dp;
  حرف name[DIRSIZ];

  اگر ((dp = nameiparent(راستہ, name)) == 0)
    واپس 0;

  ilock(dp);

  اگر (dp->nlink == 0) {
    iunlockput(dp);
    واپس 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  اگر (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    واپس 0;
  }

  اگر ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    اگر (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      واپس ip;
    iunlockput(ip);
    واپس 0;
  }

  اگر ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    واپس 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  اگر (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    اگر (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      جاؤ fail;
  }

  اگر (dirlink(dp, name, ip->inum) < 0)
    جاؤ fail;

  اگر (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  واپس ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  واپس 0;
}

uint64
sys_open(خالی)
{
  حرف راستہ[MAXPATH];
  صحیح fd, omode;
  ساخت file *f;
  ساخت inode *ip;
  صحیح n;

  argint(1, &omode);
  اگر ((n = argstr(0, راستہ, MAXPATH)) < 0)
    واپس -1;

  begin_op();

  اگر (omode & O_CREATE) {
    ip = create(راستہ, T_FILE, 0, 0);
    اگر (ip == 0) {
      انتہا_op();
      واپس -1;
    }
  } ورنہ {
    اگر ((ip = namei(راستہ)) == 0) {
      انتہا_op();
      واپس -1;
    }
    ilock(ip);
    اگر (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      انتہا_op();
      واپس -1;
    }
  }

  اگر (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    انتہا_op();
    واپس -1;
  }

  اگر ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    اگر (f)
      fileclose(f);
    iunlockput(ip);
    انتہا_op();
    واپس -1;
  }

  اگر (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } ورنہ {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  اگر ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  انتہا_op();

  واپس fd;
}

uint64
sys_mkdir(خالی)
{
  حرف راستہ[MAXPATH];
  ساخت inode *ip;

  begin_op();
  اگر (argstr(0, راستہ, MAXPATH) < 0 || (ip = create(راستہ, T_DIR, 0, 0)) == 0) {
    انتہا_op();
    واپس -1;
  }
  iunlockput(ip);
  انتہا_op();
  واپس 0;
}

uint64
sys_mknod(خالی)
{
  ساخت inode *ip;
  حرف راستہ[MAXPATH];
  صحیح major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  اگر ((argstr(0, راستہ, MAXPATH)) < 0 ||
      (ip = create(راستہ, T_DEVICE, major, minor)) == 0) {
    انتہا_op();
    واپس -1;
  }
  iunlockput(ip);
  انتہا_op();
  واپس 0;
}

uint64
sys_chdir(خالی)
{
  حرف راستہ[MAXPATH];
  ساخت inode *ip;
  ساخت proc *p = myproc();

  begin_op();
  اگر (argstr(0, راستہ, MAXPATH) < 0 || (ip = namei(راستہ)) == 0) {
    انتہا_op();
    واپس -1;
  }
  ilock(ip);
  اگر (ip->type != T_DIR) {
    iunlockput(ip);
    انتہا_op();
    واپس -1;
  }
  iunlock(ip);
  iput(p->cwd);
  انتہا_op();
  p->cwd = ip;
  واپس 0;
}

uint64
sys_exec(خالی)
{
  حرف راستہ[MAXPATH], *argv[MAXARG];
  صحیح i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  اگر (argstr(0, راستہ, MAXPATH) < 0) {
    واپس -1;
  }
  memset(argv, 0, حجم(argv));
  برائے (i = 0;; i++) {
    اگر (i >= NELEM(argv)) {
      جاؤ bad;
    }
    اگر (fetchaddr(uargv + حجم(uint64) * i, (uint64 *)&uarg) < 0) {
      جاؤ bad;
    }
    اگر (uarg == 0) {
      argv[i] = 0;
      توڑو;
    }
    argv[i] = kalloc();
    اگر (argv[i] == 0)
      جاؤ bad;
    اگر (fetchstr(uarg, argv[i], PGSIZE) < 0)
      جاؤ bad;
  }

  صحیح ret = kexec(راستہ, argv);

  برائے (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  واپس ret;

bad:
  برائے (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  واپس -1;
}

uint64
sys_pipe(خالی)
{
  uint64 fdarray; // user pointer to array of two integers
  ساخت file *rf, *wf;
  صحیح fd0, fd1;
  ساخت proc *p = myproc();

  argaddr(0, &fdarray);
  اگر (pipealloc(&rf, &wf) < 0)
    واپس -1;
  fd0 = -1;
  اگر ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    اگر (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    واپس -1;
  }
  اگر (copyout(p->pagetable, p->sz, fdarray, (حرف *)&fd0, حجم(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + حجم(fd0), (حرف *)&fd1,
              حجم(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    واپس -1;
  }
  واپس 0;
}
