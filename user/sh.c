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

  सचे (cmd == 0)
    exit(1);

  विकल्प (cmd->type) {
  स्वभाव:
    panic("runcmd");

  पक्ष EXEC:
    ecmd = (संरचना execcmd *)cmd;
    सचे (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    भङ्ग;

  पक्ष REDIR:
    rcmd = (संरचना redircmd *)cmd;
    close(rcmd->fd);
    सचे (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    भङ्ग;

  पक्ष LIST:
    lcmd = (संरचना listcmd *)cmd;
    सचे (fork1() == 0)
      runcmd(lcmd->वामः);
    wait(0);
    runcmd(lcmd->दक्षिणः);
    भङ्ग;

  पक्ष PIPE:
    pcmd = (संरचना pipecmd *)cmd;
    सचे (pipe(p) < 0)
      panic("pipe");
    सचे (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->वामः);
    }
    सचे (fork1() == 0) {
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
    सचे (fork1() == 0)
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
  सचे (buf[0] == 0) // EOF
    पटिनिवत्त -1;
  पटिनिवत्त 0;
}

पूर्णाङ्क
main(शून्य)
{
  स्थिर वर्ण buf[100];
  पूर्णाङ्क fd;

  // Ensure that three file descriptors are open.
  याव ((fd = open("console", O_RDWR)) >= 0) {
    सचे (fd >= 3) {
      close(fd);
      भङ्ग;
    }
  }

  // Read and run input commands.
  याव (getcmd(buf, परिमाण(buf)) >= 0) {
    वर्ण *cmd = buf;
    याव (*cmd == ' ' || *cmd == '\t')
      cmd++;
    सचे (*cmd == '\n') // is a blank command
      अनुवर्त;
    सचे (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      सचे (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } अञ्ञथा {
      सचे (fork1() == 0)
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
  सचे (pid == -1)
    panic("fork");
  पटिनिवत्त pid;
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
  पटिनिवत्त (संरचना cmd *)cmd;
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
  पटिनिवत्त (संरचना cmd *)cmd;
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
  पटिनिवत्त (संरचना cmd *)cmd;
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
  पटिनिवत्त (संरचना cmd *)cmd;
}

संरचना cmd *
backcmd(संरचना cmd *subcmd)
{
  संरचना backcmd *cmd;

  cmd = malloc(परिमाण(*cmd));
  memset(cmd, 0, परिमाण(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  पटिनिवत्त (संरचना cmd *)cmd;
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
  याव (s < es && strchr(whitespace, *s))
    s++;
  सचे (q)
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
    सचे (*s == '>') {
      ret = '+';
      s++;
    }
    भङ्ग;
  स्वभाव:
    ret = 'a';
    याव (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    भङ्ग;
  }
  सचे (eq)
    *eq = s;

  याव (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  पटिनिवत्त ret;
}

पूर्णाङ्क
peek(वर्ण **ps, वर्ण *es, वर्ण *toks)
{
  वर्ण *s;

  s = *ps;
  याव (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  पटिनिवत्त *s && strchr(toks, *s);
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
  सचे (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  पटिनिवत्त cmd;
}

संरचना cmd *
parseline(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  cmd = parsepipe(ps, es);
  याव (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  सचे (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  पटिनिवत्त cmd;
}

संरचना cmd *
parsepipe(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  cmd = parseexec(ps, es);
  सचे (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  पटिनिवत्त cmd;
}

संरचना cmd *
parseredirs(संरचना cmd *cmd, वर्ण **ps, वर्ण *es)
{
  पूर्णाङ्क tok;
  वर्ण *q, *eq;

  याव (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    सचे (gettoken(ps, es, &q, &eq) != 'a')
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
  पटिनिवत्त cmd;
}

संरचना cmd *
parseblock(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  सचे (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  सचे (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  पटिनिवत्त cmd;
}

संरचना cmd *
parseexec(वर्ण **ps, वर्ण *es)
{
  वर्ण *q, *eq;
  पूर्णाङ्क tok, argc;
  संरचना execcmd *cmd;
  संरचना cmd *ret;

  सचे (peek(ps, es, "("))
    पटिनिवत्त parseblock(ps, es);

  ret = execcmd();
  cmd = (संरचना execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  याव (!peek(ps, es, "|)&;")) {
    सचे ((tok = gettoken(ps, es, &q, &eq)) == 0)
      भङ्ग;
    सचे (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    सचे (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  पटिनिवत्त ret;
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

  सचे (cmd == 0)
    पटिनिवत्त 0;

  विकल्प (cmd->type) {
  पक्ष EXEC:
    ecmd = (संरचना execcmd *)cmd;
    अत्थाय (i = 0; ecmd->argv[i]; i++)
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
  पटिनिवत्त cmd;
}
