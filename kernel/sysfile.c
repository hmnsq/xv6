//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#समावेश "types.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "param.h"
#समावेश "stat.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "fs.h"
#समावेश "sleeplock.h"
#समावेश "file.h"
#समावेश "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
अटल पूर्णांक
argfd(पूर्णांक n, पूर्णांक *pfd, काष्ठा file **pf)
{
  पूर्णांक fd;
  काष्ठा file *f;

  argint(n, &fd);
  अगर (fd < 0 || fd >= NOFILE || (f = myproc()->ofile[fd]) == 0)
    वापस -1;
  अगर (pfd)
    *pfd = fd;
  अगर (pf)
    *pf = f;
  वापस 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
अटल पूर्णांक
fdalloc(काष्ठा file *f)
{
  पूर्णांक fd;
  काष्ठा proc *p = myproc();

  क्रम (fd = 0; fd < NOFILE; fd++) {
    अगर (p->ofile[fd] == 0) {
      p->ofile[fd] = f;
      वापस fd;
    }
  }
  वापस -1;
}

uint64
sys_dup(व्योम)
{
  काष्ठा file *f;
  पूर्णांक fd;

  अगर (argfd(0, 0, &f) < 0)
    वापस -1;
  अगर ((fd = fdalloc(f)) < 0)
    वापस -1;
  filedup(f);
  वापस fd;
}

uint64
sys_read(व्योम)
{
  काष्ठा file *f;
  पूर्णांक n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  अगर (argfd(0, 0, &f) < 0)
    वापस -1;
  वापस fileread(f, p, n);
}

uint64
sys_write(व्योम)
{
  काष्ठा file *f;
  पूर्णांक n;
  uint64 p;

  argaddr(1, &p);
  argint(2, &n);
  अगर (argfd(0, 0, &f) < 0)
    वापस -1;

  वापस filewrite(f, p, n);
}

uint64
sys_close(व्योम)
{
  पूर्णांक fd;
  काष्ठा file *f;

  अगर (argfd(0, &fd, &f) < 0)
    वापस -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  वापस 0;
}

uint64
sys_fstat(व्योम)
{
  काष्ठा file *f;
  uint64 st; // user pointer to struct stat

  argaddr(1, &st);
  अगर (argfd(0, 0, &f) < 0)
    वापस -1;
  वापस filestat(f, st);
}

// Create the path new as a link to the same inode as old.
uint64
sys_link(व्योम)
{
  अक्षर name[DIRSIZ], new[MAXPATH], old[MAXPATH];
  काष्ठा inode *dp, *ip;

  अगर (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0)
    वापस -1;

  begin_op();
  अगर ((ip = namei(old)) == 0) {
    end_op();
    वापस -1;
  }

  ilock(ip);
  अगर (ip->type == T_DIR) {
    iunlockput(ip);
    end_op();
    वापस -1;
  }

  अगर (ip->nlink >= NLINK_MAX) {
    iunlockput(ip);
    end_op();
    वापस -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  अगर ((dp = nameiparent(new, name)) == 0)
    जाओ bad;
  ilock(dp);
  // dp may have been unlinked while we resolved it; linking into an
  // orphaned directory leaks ip (itrunc discards the record without
  // dropping ip->nlink).  create() has the same guard.
  अगर (dp->nlink == 0) {
    iunlockput(dp);
    जाओ bad;
  }
  अगर (dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0) {
    iunlockput(dp);
    जाओ bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  वापस 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  वापस -1;
}

// Is the directory dp empty except for "." and ".." ?
अटल पूर्णांक
isdirempty(काष्ठा inode *dp)
{
  पूर्णांक off;
  काष्ठा dirent de;

  क्रम (off = 2 * माप(de); off < dp->आकार; off += माप(de)) {
    अगर (readi(dp, 0, (uint64)&de, off, माप(de)) != माप(de))
      panic("isdirempty: readi");
    अगर (de.inum != 0)
      वापस 0;
  }
  वापस 1;
}

uint64
sys_unlink(व्योम)
{
  काष्ठा inode *ip, *dp;
  काष्ठा dirent de;
  अक्षर name[DIRSIZ], path[MAXPATH];
  uint off;

  अगर (argstr(0, path, MAXPATH) < 0)
    वापस -1;

  begin_op();
  अगर ((dp = nameiparent(path, name)) == 0) {
    end_op();
    वापस -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  अगर (namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    जाओ bad;

  अगर ((ip = dirlookup(dp, name, &off)) == 0)
    जाओ bad;
  ilock(ip);

  अगर (ip->nlink < 1)
    panic("unlink: nlink < 1");
  अगर (ip->type == T_DIR && !isdirempty(ip)) {
    iunlockput(ip);
    जाओ bad;
  }

  स_रखो(&de, 0, माप(de));
  अगर (writei(dp, 0, (uint64)&de, off, माप(de)) != माप(de))
    panic("unlink: writei");
  अगर (ip->type == T_DIR) {
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  वापस 0;

bad:
  iunlockput(dp);
  end_op();
  वापस -1;
}

अटल काष्ठा inode *
create(अक्षर *path, लघु type, लघु major, लघु minor)
{
  काष्ठा inode *ip, *dp;
  अक्षर name[DIRSIZ];

  अगर ((dp = nameiparent(path, name)) == 0)
    वापस 0;

  ilock(dp);

  अगर (dp->nlink == 0) {
    iunlockput(dp);
    वापस 0;
  }

  // a new directory's ".." would push dp->nlink past its maximum
  अगर (type == T_DIR && dp->nlink >= NLINK_MAX) {
    iunlockput(dp);
    वापस 0;
  }

  अगर ((ip = dirlookup(dp, name, 0)) != 0) {
    iunlockput(dp);
    ilock(ip);
    अगर (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      वापस ip;
    iunlockput(ip);
    वापस 0;
  }

  अगर ((ip = ialloc(dp->dev, type)) == 0) {
    iunlockput(dp);
    वापस 0;
  }

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  अगर (type == T_DIR) { // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    अगर (dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      जाओ fail;
  }

  अगर (dirlink(dp, name, ip->inum) < 0)
    जाओ fail;

  अगर (type == T_DIR) {
    // now that success is guaranteed:
    dp->nlink++; // for ".."
    iupdate(dp);
  }

  iunlockput(dp);

  वापस ip;

fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  वापस 0;
}

uint64
sys_open(व्योम)
{
  अक्षर path[MAXPATH];
  पूर्णांक fd, omode;
  काष्ठा file *f;
  काष्ठा inode *ip;
  पूर्णांक n;

  argint(1, &omode);
  अगर ((n = argstr(0, path, MAXPATH)) < 0)
    वापस -1;

  begin_op();

  अगर (omode & O_CREATE) {
    ip = create(path, T_FILE, 0, 0);
    अगर (ip == 0) {
      end_op();
      वापस -1;
    }
  } अन्यथा {
    अगर ((ip = namei(path)) == 0) {
      end_op();
      वापस -1;
    }
    ilock(ip);
    अगर (ip->type == T_DIR && omode != O_RDONLY) {
      iunlockput(ip);
      end_op();
      वापस -1;
    }
  }

  अगर (ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)) {
    iunlockput(ip);
    end_op();
    वापस -1;
  }

  अगर ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
    अगर (f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    वापस -1;
  }

  अगर (ip->type == T_DEVICE) {
    f->type = FD_DEVICE;
    f->major = ip->major;
  } अन्यथा {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  अगर ((omode & O_TRUNC) && ip->type == T_FILE) {
    itrunc(ip);
  }

  iunlock(ip);
  end_op();

  वापस fd;
}

uint64
sys_mkdir(व्योम)
{
  अक्षर path[MAXPATH];
  काष्ठा inode *ip;

  begin_op();
  अगर (argstr(0, path, MAXPATH) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0) {
    end_op();
    वापस -1;
  }
  iunlockput(ip);
  end_op();
  वापस 0;
}

uint64
sys_mknod(व्योम)
{
  काष्ठा inode *ip;
  अक्षर path[MAXPATH];
  पूर्णांक major, minor;

  begin_op();
  argint(1, &major);
  argint(2, &minor);
  अगर ((argstr(0, path, MAXPATH)) < 0 ||
      (ip = create(path, T_DEVICE, major, minor)) == 0) {
    end_op();
    वापस -1;
  }
  iunlockput(ip);
  end_op();
  वापस 0;
}

uint64
sys_chdir(व्योम)
{
  अक्षर path[MAXPATH];
  काष्ठा inode *ip;
  काष्ठा proc *p = myproc();

  begin_op();
  अगर (argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
    end_op();
    वापस -1;
  }
  ilock(ip);
  अगर (ip->type != T_DIR) {
    iunlockput(ip);
    end_op();
    वापस -1;
  }
  iunlock(ip);
  iput(p->cwd);
  end_op();
  p->cwd = ip;
  वापस 0;
}

uint64
sys_exec(व्योम)
{
  अक्षर path[MAXPATH], *argv[MAXARG];
  पूर्णांक i;
  uint64 uargv, uarg;

  argaddr(1, &uargv);
  अगर (argstr(0, path, MAXPATH) < 0) {
    वापस -1;
  }
  स_रखो(argv, 0, माप(argv));
  क्रम (i = 0;; i++) {
    अगर (i >= NELEM(argv)) {
      जाओ bad;
    }
    अगर (fetchaddr(uargv + माप(uint64) * i, (uint64 *)&uarg) < 0) {
      जाओ bad;
    }
    अगर (uarg == 0) {
      argv[i] = 0;
      अवरोध;
    }
    argv[i] = kalloc();
    अगर (argv[i] == 0)
      जाओ bad;
    अगर (fetchstr(uarg, argv[i], PGSIZE) < 0)
      जाओ bad;
  }

  पूर्णांक ret = kexec(path, argv);

  क्रम (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);

  वापस ret;

bad:
  क्रम (i = 0; i < NELEM(argv) && argv[i] != 0; i++)
    kfree(argv[i]);
  वापस -1;
}

uint64
sys_pipe(व्योम)
{
  uint64 fdarray; // user pointer to array of two integers
  काष्ठा file *rf, *wf;
  पूर्णांक fd0, fd1;
  काष्ठा proc *p = myproc();

  argaddr(0, &fdarray);
  अगर (pipealloc(&rf, &wf) < 0)
    वापस -1;
  fd0 = -1;
  अगर ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
    अगर (fd0 >= 0)
      p->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    वापस -1;
  }
  अगर (copyout(p->pagetable, p->sz, fdarray, (अक्षर *)&fd0, माप(fd0)) < 0 ||
      copyout(p->pagetable, p->sz, fdarray + माप(fd0), (अक्षर *)&fd1,
              माप(fd1)) < 0) {
    p->ofile[fd0] = 0;
    p->ofile[fd1] = 0;
    fileclose(rf);
    fileclose(wf);
    वापस -1;
  }
  वापस 0;
}
