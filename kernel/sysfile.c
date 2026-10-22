//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "stat.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "fs.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "file.h"
#அந்தர்பாவ "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
நிலையான முழு
argfd(முழு n, முழு *pfd, கட்டமைப்பு file **pf)
{
  முழு fd;
  கட்டமைப்பு file *f;

  argint(n, &fd);
  எனில் (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    திரும்பு -1;
  எனில் (pfd)
    *pfd = fd;
  எனில் (pf)
    *pf = f;
  திரும்பு 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
நிலையான முழு
fdalloc(கட்டமைப்பு file *f)
{
  முழு fd;
  கட்டமைப்பு proc *p = myproc();

  க்கு (fd = 0; fd < NOFILE; fd++) {
    எனில் (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      திரும்பு fd;
    }
  }
  திரும்பு -1;
}

uint64
sys_dup(வெறுமை)
{
  கட்டமைப்பு file *f;
  முழு fd;

  எனில் (argfd(0, 0, &f) < 0)
    திரும்பு -1;
  எனில் ((fd = fdalloc(f)) < 0)
    திரும்பு -1;
  filedup(f);
  திரும்பு fd;
}

uint64
sys_read(வெறுமை)
{
  கட்டமைப்பு file *f;
  முழு n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  எனில் (argfd(0, 0, &f) < 0)
    திரும்பு -1;
  திரும்பு fileread(f, p, n);
}

uint64
sys_write(வெறுமை)
{
  கட்டமைப்பு file *f;
  முழு n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  எனில் (argfd(0, 0, &f) < 0)
    திரும்பு -1;

  திரும்பு filewrite(f, p, n);
}

uint64
sys_close(வெறுமை)
{
  முழு fd;
  கட்டமைப்பு file *f;

  எனில் (argfd(0, &fd, &f) < 0)
    திரும்பு -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  திரும்பு 0;
}

uint64
sys_fstat(வெறுமை)
{
  கட்டமைப்பு file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  எனில் (argfd(0, 0, &f) < 0)
    திரும்பு -1;
  திரும்பு filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(வெறுமை)
{
  எழுத்து name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  கட்டமைப்பு inode *dp, *ip;

  எனில் (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    திரும்பு -1;

  begin_op();
  எனில் ((ip = namei(old)) == 0) {
    end_op();
    திரும்பு -1;
  }

  ilock(ip);
  எனில் (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    திரும்பு -1;
  }

  எனில் (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    திரும்பு -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  எனில் ((dp = nameiparent(new, name)) == 0)
    செல் bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  எனில் (dp->nlink == 0) {
    iunlockput(dp);
    செல் bad;
  }
  எனில் (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    செல் bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  திரும்பு 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  திரும்பு -1;
}

// Is the directory dp empty except for "." and ".." ?
நிலையான முழு
isdirempty(கட்டமைப்பு inode *dp)
{
  முழு off;
  கட்டமைப்பு dirent de;

  க்கு (off = 2 * அளவு(de); off < dp->size; off += அளவு(de)) {
    எனில் (readi(dp, 0, (uint64)&de, off, அளவு(de)) != அளவு(de))
      panic("isdirempty: readi");
    எனில் (de.inum != 0)
      திரும்பு 0;
  }
  திரும்பு 1;
}

uint64
sys_unlink(வெறுமை)
{
  கட்டமைப்பு inode *ip, *dp;
  கட்டமைப்பு dirent de;
  எழுத்து name[DIRSIZ], path[MAXPATH];
  uint off;

  எனில் (argstr(0, path, MAXPATH) < 0)
    திரும்பு -1;

  begin_op();
  எனில் ((dp = nameiparent(path, name)) == 0) {
    end_op();
    திரும்பு -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  எனில் (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    செல் bad;

  எனில் ((ip = dirlookup(dp, name, &off)) == 0)
    செல் bad;
  ilock(ip);

  எனில் (ip->nlink < 1)
    panic("unlink: nlink < 1");
  எனில் (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    செல் bad;
  }

  memset(&de, 0, அளவு(de));
  எனில் (writei(dp, 0, (uint64)&de, off, அளவு(de)) != அளவு(de))
    panic("unlink: writei");
  எனில் (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  திரும்பு 0;

bad:
  iunlockput(dp);
  end_op();
  திரும்பு -1;
}

நிலையான கட்டமைப்பு inode *
create(எழுத்து *path, short type, short major, short minor)
{
  கட்டமைப்பு inode *ip, *dp;
  எழுத்து name[DIRSIZ];

  எனில் ((dp = nameiparent(path, name)) == 0)
    திரும்பு 0;

  ilock(dp);

  எனில் (dp->nlink == 0) {
    iunlockput(dp);
    திரும்பு 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  எனில் (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    திரும்பு 0;
  }

  எனில் ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    எனில் (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      திரும்பு ip;
    iunlockput(ip);
    திரும்பு 0;
  }

  எனில் ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    திரும்பு 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  எனில் (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    எனில் (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      செல் fail;
  }

  எனில் (dirlink(dp, name, ip->inum) < 0)
    செல் fail;

  எனில் (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  திரும்பு ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  திரும்பு 0;
}

uint64
sys_open(வெறுமை)
{
  எழுத்து path[MAXPATH];
  முழு fd, omode;
  கட்டமைப்பு file *f;
  கட்டமைப்பு inode *ip;
  முழு n;

  argint(1, &omode);
  எனில் ((n = argstr(0, path, MAXPATH)) < 0)
    திரும்பு -1;

  begin_op();

  எனில் (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    எனில் (ip == 0) {
      end_op();
      திரும்பு -1;
    }
  } இல்லையெனில் {
    எனில் ((ip = namei(path)) == 0) {
      end_op();
      திரும்பு -1;
    }
    ilock(ip);
    எனில் (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      திரும்பு -1;
    }
  }

  எனில் (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    திரும்பு -1;
  }

  எனில் ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    எனில் (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    திரும்பு -1;
  }

  எனில் (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } இல்லையெனில் {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  எனில் ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  திரும்பு fd;
}

uint64
sys_mkdir(வெறுமை)
{
  எழுத்து path[MAXPATH];
  கட்டமைப்பு inode *ip;

  begin_op();
  எனில் (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    திரும்பு -1;
  }
  iunlockput(ip);
  end_op();
  திரும்பு 0;
}

uint64
sys_mknod(வெறுமை)
{
  கட்டமைப்பு inode *ip;
  எழுத்து path[MAXPATH];
  முழு major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  எனில் ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    திரும்பு -1;
  }
  iunlockput(ip);
  end_op();
  திரும்பு 0;
}

uint64
sys_chdir(வெறுமை)
{
  எழுத்து path[MAXPATH];
  கட்டமைப்பு inode *ip;
  கட்டமைப்பு proc *p = myproc();

  begin_op();
  எனில் (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    திரும்பு -1;
  }
  ilock(ip);
  எனில் (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    திரும்பு -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  திரும்பு 0;
}

uint64
sys_exec(வெறுமை)
{
  எழுத்து path[MAXPATH], *argv[MAXARG];
  முழு i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  எனில் (argstr(0, path, MAXPATH) < 0) {
    திரும்பு -1;
  }
  memset(argv, 0, அளவு(argv));
  க்கு (i = 0;; i++) {
    எனில் (i >= NELEM(argv)) {
      செல் bad;
    }
    எனில் (fetchaddr(uargv + அளவு(uint64) * i, (uint64 *)&uarg) < 0) {
      செல் bad;
    }
    எனில் (uarg == 0) {
      argv[i] = 0;
      நிறுத்து;
    }
    argv[i] = kalloc();
    எனில் (argv[i] == 0)
      செல் bad;
    எனில் (fetchstr(uarg, argv[i], PGSIZE) < 0)
      செல் bad;
  }

  முழு ret = kexec(path, argv);

  க்கு (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  திரும்பு ret;

bad:
  க்கு (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  திரும்பு -1;
}

uint64
sys_pipe(வெறுமை)
{
  uint64 fdarray; // user pointer to array of two integers
  கட்டமைப்பு file *rf, *wf;
  முழு fd0, fd1;
  கட்டமைப்பு proc *p = myproc();

  argaddr(0, &fdarray);
  எனில் (pipealloc(&rf, &wf) < 0)
    திரும்பு -1;
  fd0 = -1;
  எனில் ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    எனில் (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    திரும்பு -1;
  }
  எனில் (copyout(p->pagetable, p->sz, fdarray, (எழுத்து *)&fd0, அளவு(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + அளவு(fd0), (எழுத்து *)&fd1,
              அளவு(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    திரும்பு -1;
  }
  திரும்பு 0;
}
