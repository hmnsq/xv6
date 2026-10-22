// Shell.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

संरचना cmd {
  पूर्णाङ्क type;
};

संरचना execcmd {
  पूर्णाङ्क type;
  वर्ण *argv[MAXARGS];
  वर्ण *eargv[MAXARGS];
};

संरचना redircmd {
  पूर्णाङ्क type;
  संरचना cmd *cmd;
  वर्ण *file;
  वर्ण *efile;
  पूर्णाङ्क mode;
  पूर्णाङ्क fd;
};

संरचना pipecmd {
  पूर्णाङ्क type;
  संरचना cmd *वामः;
  संरचना cmd *दक्षिणः;
};

संरचना listcmd {
  पूर्णाङ्क type;
  संरचना cmd *वामः;
  संरचना cmd *दक्षिणः;
};

संरचना backcmd {
  पूर्णाङ्क type;
  संरचना cmd *cmd;
};

पूर्णाङ्क fork1(शून्य); // Fork but panics on failure.
शून्य panic(वर्ण *);
संरचना cmd *parsecmd(वर्ण *);
शून्य runcmd(संरचना cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
शून्य
runcmd(संरचना cmd *cmd)
{
  पूर्णाङ्क p[2];
  संरचना backcmd *bcmd;
  संरचना execcmd *ecmd;
  संरचना listcmd *lcmd;
  संरचना pipecmd *pcmd;
  संरचना redircmd *rcmd;

  जइ (cmd == 0)
    exit(1);

  विकल्प (cmd->type) {
  स्वभाव:
    panic("runcmd");

  पक्ष EXEC:
    ecmd = (संरचना execcmd *)cmd;
    जइ (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    भङ्ग;

  पक्ष REDIR:
    rcmd = (संरचना redircmd *)cmd;
    close(rcmd->fd);
    जइ (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    भङ्ग;

  पक्ष LIST:
    lcmd = (संरचना listcmd *)cmd;
    जइ (fork1() == 0)
      runcmd(lcmd->वामः);
    wait(0);
    runcmd(lcmd->दक्षिणः);
    भङ्ग;

  पक्ष PIPE:
    pcmd = (संरचना pipecmd *)cmd;
    जइ (pipe(p) < 0)
      panic("pipe");
    जइ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->वामः);
    }
    जइ (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->दक्षिणः);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    भङ्ग;

  पक्ष BACK:
    bcmd = (संरचना backcmd *)cmd;
    जइ (fork1() == 0)
      runcmd(bcmd->cmd);
    भङ्ग;
  }
  exit(0);
}

पूर्णाङ्क
getcmd(वर्ण *buf, पूर्णाङ्क nbuf)
{
  लिख(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  जइ (buf[0] == 0) // EOF
    प्रत्यागम -1;
  प्रत्यागम 0;
}

पूर्णाङ्क
main(शून्य)
{
  स्थिर वर्ण buf[100];
  पूर्णाङ्क fd;

  // Ensure that three file descriptors are open.
  जाव ((fd = open("console", O_RDWR)) >= 0) {
    जइ (fd >= 3) {
      close(fd);
      भङ्ग;
    }
  }

  // Read and run input commands.
  जाव (getcmd(buf, परिमाण(buf)) >= 0) {
    वर्ण *cmd = buf;
    जाव (*cmd == ' ' || *cmd == '\t')
      cmd++;
    जइ (*cmd == '\n') // is a blank command
      अनुवर्त;
    जइ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      जइ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } अण्णहा {
      जइ (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

शून्य
panic(वर्ण *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

पूर्णाङ्क
fork1(शून्य)
{
  पूर्णाङ्क pid;

  pid = fork();
  जइ (pid == -1)
    panic("fork");
  प्रत्यागम pid;
}

//PAGEBREAK!
// Constructors

संरचना cmd *
execcmd(शून्य)
{
  संरचना execcmd *cmd;

  cmd = malloc(परिमाण(*cmd));
  memset(cmd, 0, परिमाण(*cmd));
  cmd->type = EXEC;
  प्रत्यागम (संरचना cmd *)cmd;
}

संरचना cmd *
redircmd(संरचना cmd *subcmd, वर्ण *file, वर्ण *efile, पूर्णाङ्क mode, पूर्णाङ्क fd)
{
  संरचना redircmd *cmd;

  cmd = malloc(परिमाण(*cmd));
  memset(cmd, 0, परिमाण(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  प्रत्यागम (संरचना cmd *)cmd;
}

संरचना cmd *
pipecmd(संरचना cmd *वामः, संरचना cmd *दक्षिणः)
{
  संरचना pipecmd *cmd;

  cmd = malloc(परिमाण(*cmd));
  memset(cmd, 0, परिमाण(*cmd));
  cmd->type = PIPE;
  cmd->वामः = वामः;
  cmd->दक्षिणः = दक्षिणः;
  प्रत्यागम (संरचना cmd *)cmd;
}

संरचना cmd *
listcmd(संरचना cmd *वामः, संरचना cmd *दक्षिणः)
{
  संरचना listcmd *cmd;

  cmd = malloc(परिमाण(*cmd));
  memset(cmd, 0, परिमाण(*cmd));
  cmd->type = LIST;
  cmd->वामः = वामः;
  cmd->दक्षिणः = दक्षिणः;
  प्रत्यागम (संरचना cmd *)cmd;
}

संरचना cmd *
backcmd(संरचना cmd *subcmd)
{
  संरचना backcmd *cmd;

  cmd = malloc(परिमाण(*cmd));
  memset(cmd, 0, परिमाण(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  प्रत्यागम (संरचना cmd *)cmd;
}
//PAGEBREAK!
// Parsing

वर्ण whitespace[] = " \t\r\n\v";
वर्ण symbols[] = "<|>&;()";

पूर्णाङ्क
gettoken(वर्ण **ps, वर्ण *es, वर्ण **q, वर्ण **eq)
{
  वर्ण *s;
  पूर्णाङ्क ret;

  s = *ps;
  जाव (s < es && strchr(whitespace, *s))
    s++;
  जइ (q)
    *q = s;
  ret = *s;
  विकल्प (*s) {
  पक्ष 0:
    भङ्ग;
  पक्ष '|':
  पक्ष '(':
  पक्ष ')':
  पक्ष ';':
  पक्ष '&':
  पक्ष '<':
    s++;
    भङ्ग;
  पक्ष '>':
    s++;
    जइ (*s == '>') {
      ret = '+';
      s++;
    }
    भङ्ग;
  स्वभाव:
    ret = 'a';
    जाव (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    भङ्ग;
  }
  जइ (eq)
    *eq = s;

  जाव (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  प्रत्यागम ret;
}

पूर्णाङ्क
peek(वर्ण **ps, वर्ण *es, वर्ण *toks)
{
  वर्ण *s;

  s = *ps;
  जाव (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  प्रत्यागम *s && strchr(toks, *s);
}

संरचना cmd *parseline(वर्ण **, वर्ण *);
संरचना cmd *parsepipe(वर्ण **, वर्ण *);
संरचना cmd *parseexec(वर्ण **, वर्ण *);
संरचना cmd *nulterminate(संरचना cmd *);

संरचना cmd *
parsecmd(वर्ण *s)
{
  वर्ण *es;
  संरचना cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  जइ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  प्रत्यागम cmd;
}

संरचना cmd *
parseline(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  cmd = parsepipe(ps, es);
  जाव (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  जइ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  प्रत्यागम cmd;
}

संरचना cmd *
parsepipe(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  cmd = parseexec(ps, es);
  जइ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  प्रत्यागम cmd;
}

संरचना cmd *
parseredirs(संरचना cmd *cmd, वर्ण **ps, वर्ण *es)
{
  पूर्णाङ्क tok;
  वर्ण *q, *eq;

  जाव (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    जइ (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    विकल्प (tok) {
    पक्ष '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      भङ्ग;
    पक्ष '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      भङ्ग;
    पक्ष '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      भङ्ग;
    }
  }
  प्रत्यागम cmd;
}

संरचना cmd *
parseblock(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  जइ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  जइ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  प्रत्यागम cmd;
}

संरचना cmd *
parseexec(वर्ण **ps, वर्ण *es)
{
  वर्ण *q, *eq;
  पूर्णाङ्क tok, argc;
  संरचना execcmd *cmd;
  संरचना cmd *ret;

  जइ (peek(ps, es, "("))
    प्रत्यागम parseblock(ps, es);

  ret = execcmd();
  cmd = (संरचना execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  जाव (!peek(ps, es, "|)&;")) {
    जइ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      भङ्ग;
    जइ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    जइ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  प्रत्यागम ret;
}

// NUL-terminate all the counted strings.
संरचना cmd *
nulterminate(संरचना cmd *cmd)
{
  पूर्णाङ्क i;
  संरचना backcmd *bcmd;
  संरचना execcmd *ecmd;
  संरचना listcmd *lcmd;
  संरचना pipecmd *pcmd;
  संरचना redircmd *rcmd;

  जइ (cmd == 0)
    प्रत्यागम 0;

  विकल्प (cmd->type) {
  पक्ष EXEC:
    ecmd = (संरचना execcmd *)cmd;
    प्रति (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    भङ्ग;

  पक्ष REDIR:
    rcmd = (संरचना redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    भङ्ग;

  पक्ष PIPE:
    pcmd = (संरचना pipecmd *)cmd;
    nulterminate(pcmd->वामः);
    nulterminate(pcmd->दक्षिणः);
    भङ्ग;

  पक्ष LIST:
    lcmd = (संरचना listcmd *)cmd;
    nulterminate(lcmd->वामः);
    nulterminate(lcmd->दक्षिणः);
    भङ्ग;

  पक्ष BACK:
    bcmd = (संरचना backcmd *)cmd;
    nulterminate(bcmd->cmd);
    भङ्ग;
  }
  प्रत्यागम cmd;
}
