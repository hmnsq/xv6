//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "stat.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "file.h"
#अन्तर्भाव "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
स्थिर पूर्णाङ्क
argfd(पूर्णाङ्क n, पूर्णाङ्क *pfd, संरचना file **pf)
{
  पूर्णाङ्क fd;
  संरचना file *f;

  argint(n, &fd);
  यदि (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    प्रत्यागम -1;
  यदि (pfd)
    *pfd = fd;
  यदि (pf)
    *pf = f;
  प्रत्यागम 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
स्थिर पूर्णाङ्क
fdalloc(संरचना file *f)
{
  पूर्णाङ्क fd;
  संरचना proc *p = myproc();

  प्रति (fd = 0; fd < NOFILE; fd++) {
    यदि (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      प्रत्यागम fd;
    }
  }
  प्रत्यागम -1;
}

uint64
sys_dup(शून्य)
{
  संरचना file *f;
  पूर्णाङ्क fd;

  यदि (argfd(0, 0, &f) < 0)
    प्रत्यागम -1;
  यदि ((fd = fdalloc(f)) < 0)
    प्रत्यागम -1;
  filedup(f);
  प्रत्यागम fd;
}

uint64
sys_read(शून्य)
{
  संरचना file *f;
  पूर्णाङ्क n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  यदि (argfd(0, 0, &f) < 0)
    प्रत्यागम -1;
  प्रत्यागम fileread(f, p, n);
}

uint64
sys_write(शून्य)
{
  संरचना file *f;
  पूर्णाङ्क n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  यदि (argfd(0, 0, &f) < 0)
    प्रत्यागम -1;

  प्रत्यागम filewrite(f, p, n);
}

uint64
sys_close(शून्य)
{
  पूर्णाङ्क fd;
  संरचना file *f;

  यदि (argfd(0, &fd, &f) < 0)
    प्रत्यागम -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  प्रत्यागम 0;
}

uint64
sys_fstat(शून्य)
{
  संरचना file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  यदि (argfd(0, 0, &f) < 0)
    प्रत्यागम -1;
  प्रत्यागम filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(शून्य)
{
  वर्ण name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  संरचना inode *dp, *ip;

  यदि (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    प्रत्यागम -1;

  begin_op();
  यदि ((ip = namei(old)) == 0) {
    अन्तः_op();
    प्रत्यागम -1;
  }

  ilock(ip);
  यदि (ip->type == T_DIR) {
    iunlockput(ip);
    अन्तः_op();
    प्रत्यागम -1;
  }

  यदि (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    अन्तः_op();
    प्रत्यागम -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  यदि ((dp = nameiparent(new, name)) == 0)
    गच्छ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  यदि (dp->nlink == 0) {
    iunlockput(dp);
    गच्छ bad;
  }
  यदि (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    गच्छ bad;
  }
  iunlockput(dp);
  iput(ip);

  अन्तः_op();

  प्रत्यागम 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  अन्तः_op();
  प्रत्यागम -1;
}

// Is the directory dp empty except for "." and ".." ?
स्थिर पूर्णाङ्क
isdirempty(संरचना inode *dp)
{
  पूर्णाङ्क off;
  संरचना dirent de;

  प्रति (off = 2 * परिमाण(de); off < dp->आकार; off += परिमाण(de)) {
    यदि (readi(dp, 0, (uint64)&de, off, परिमाण(de)) != परिमाण(de))
      panic("isdirempty: readi");
    यदि (de.inum != 0)
      प्रत्यागम 0;
  }
  प्रत्यागम 1;
}

uint64
sys_unlink(शून्य)
{
  संरचना inode *ip, *dp;
  संरचना dirent de;
  वर्ण name[DIRSIZ], मार्गः[MAXPATH];
  uint off;

  यदि (argstr(0, मार्गः, MAXPATH) < 0)
    प्रत्यागम -1;

  begin_op();
  यदि ((dp = nameiparent(मार्गः, name)) == 0) {
    अन्तः_op();
    प्रत्यागम -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  यदि (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    गच्छ bad;

  यदि ((ip = dirlookup(dp, name, &off)) == 0)
    गच्छ bad;
  ilock(ip);

  यदि (ip->nlink < 1)
    panic("unlink: nlink < 1");
  यदि (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    गच्छ bad;
  }

  स्मृतिं_पूरय(&de, 0, परिमाण(de));
  यदि (writei(dp, 0, (uint64)&de, off, परिमाण(de)) != परिमाण(de))
    panic("unlink: writei");
  यदि (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  अन्तः_op();

  प्रत्यागम 0;

bad:
  iunlockput(dp);
  अन्तः_op();
  प्रत्यागम -1;
}

स्थिर संरचना inode *
create(वर्ण *मार्गः, ह्रस्व type, ह्रस्व major, ह्रस्व minor)
{
  संरचना inode *ip, *dp;
  वर्ण name[DIRSIZ];

  यदि ((dp = nameiparent(मार्गः, name)) == 0)
    प्रत्यागम 0;

  ilock(dp);

  यदि (dp->nlink == 0) {
    iunlockput(dp);
    प्रत्यागम 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  यदि (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    प्रत्यागम 0;
  }

  यदि ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    यदि (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      प्रत्यागम ip;
    iunlockput(ip);
    प्रत्यागम 0;
  }

  यदि ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    प्रत्यागम 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  यदि (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    यदि (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      गच्छ fail;
  }

  यदि (dirlink(dp, name, ip->inum) < 0)
    गच्छ fail;

  यदि (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  प्रत्यागम ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  प्रत्यागम 0;
}

uint64
sys_open(शून्य)
{
  वर्ण मार्गः[MAXPATH];
  पूर्णाङ्क fd, omode;
  संरचना file *f;
  संरचना inode *ip;
  पूर्णाङ्क n;

  argint(1, &omode);
  यदि ((n = argstr(0, मार्गः, MAXPATH)) < 0)
    प्रत्यागम -1;

  begin_op();

  यदि (omode & O_CREATE) {
    ip = create(मार्गः, T_FILE, 0, 0);
    यदि (ip == 0) {
      अन्तः_op();
      प्रत्यागम -1;
    }
  } अन्यथा {
    यदि ((ip = namei(मार्गः)) == 0) {
      अन्तः_op();
      प्रत्यागम -1;
    }
    ilock(ip);
    यदि (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      अन्तः_op();
      प्रत्यागम -1;
    }
  }

  यदि (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    अन्तः_op();
    प्रत्यागम -1;
  }

  यदि ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    यदि (f)
      fileclose(f);
    iunlockput(ip);
    अन्तः_op();
    प्रत्यागम -1;
  }

  यदि (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } अन्यथा {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  यदि ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  अन्तः_op();

  प्रत्यागम fd;
}

uint64
sys_mkdir(शून्य)
{
  वर्ण मार्गः[MAXPATH];
  संरचना inode *ip;

  begin_op();
  यदि (argstr(0, मार्गः, MAXPATH) < 0 || (ip = create(मार्गः, T_DIR, 0, 0)) == 0) {
    अन्तः_op();
    प्रत्यागम -1;
  }
  iunlockput(ip);
  अन्तः_op();
  प्रत्यागम 0;
}

uint64
sys_mknod(शून्य)
{
  संरचना inode *ip;
  वर्ण मार्गः[MAXPATH];
  पूर्णाङ्क major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  यदि ((argstr(0, मार्गः, MAXPATH)) < 0 ||
      (ip = create(मार्गः, T_DEVICE, major, minor)) == 0) {
    अन्तः_op();
    प्रत्यागम -1;
  }
  iunlockput(ip);
  अन्तः_op();
  प्रत्यागम 0;
}

uint64
sys_chdir(शून्य)
{
  वर्ण मार्गः[MAXPATH];
  संरचना inode *ip;
  संरचना proc *p = myproc();

  begin_op();
  यदि (argstr(0, मार्गः, MAXPATH) < 0 || (ip = namei(मार्गः)) == 0) {
    अन्तः_op();
    प्रत्यागम -1;
  }
  ilock(ip);
  यदि (ip->type != T_DIR) {
    iunlockput(ip);
    अन्तः_op();
    प्रत्यागम -1;
  }
  iunlock(ip);
  iput(p->cwd);
  अन्तः_op();
  p->cwd = ip;
  प्रत्यागम 0;
}

uint64
sys_exec(शून्य)
{
  वर्ण मार्गः[MAXPATH], *argv[MAXARG];
  पूर्णाङ्क i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  यदि (argstr(0, मार्गः, MAXPATH) < 0) {
    प्रत्यागम -1;
  }
  स्मृतिं_पूरय(argv, 0, परिमाण(argv));
  प्रति (i = 0;; i++) {
    यदि (i >= NELEM(argv)) {
      गच्छ bad;
    }
    यदि (fetchaddr(uargv + परिमाण(uint64) * i, (uint64 *)&uarg) < 0) {
      गच्छ bad;
    }
    यदि (uarg == 0) {
      argv[i] = 0;
      भङ्ग;
    }
    argv[i] = kalloc();
    यदि (argv[i] == 0)
      गच्छ bad;
    यदि (fetchstr(uarg, argv[i], PGSIZE) < 0)
      गच्छ bad;
  }

  पूर्णाङ्क ret = kexec(मार्गः, argv);

  प्रति (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  प्रत्यागम ret;

bad:
  प्रति (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  प्रत्यागम -1;
}

uint64
sys_pipe(शून्य)
{
  uint64 fdarray; // user pointer to array of two integers
  संरचना file *rf, *wf;
  पूर्णाङ्क fd0, fd1;
  संरचना proc *p = myproc();

  argaddr(0, &fdarray);
  यदि (pipealloc(&rf, &wf) < 0)
    प्रत्यागम -1;
  fd0 = -1;
  यदि ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    यदि (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    प्रत्यागम -1;
  }
  यदि (copyout(p->pagetable, p->sz, fdarray, (वर्ण *)&fd0, परिमाण(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + परिमाण(fd0), (वर्ण *)&fd1,
              परिमाण(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    प्रत्यागम -1;
  }
  प्रत्यागम 0;
}
