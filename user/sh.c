// Shell.

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"

// Parsed command representation
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXEC  1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REDIR 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIPE  3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST  4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਿੱਠ  5

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXARGS 10

ਬਣਤਰ cmd {
  ਪੂਰਨ type;
};

ਬਣਤਰ execcmd {
  ਪੂਰਨ type;
  ਅੱਖਰ *argv[MAXARGS];
  ਅੱਖਰ *eargv[MAXARGS];
};

ਬਣਤਰ redircmd {
  ਪੂਰਨ type;
  ਬਣਤਰ cmd *cmd;
  ਅੱਖਰ *file;
  ਅੱਖਰ *efile;
  ਪੂਰਨ ਬਹੁਲਕ;
  ਪੂਰਨ fd;
};

ਬਣਤਰ pipecmd {
  ਪੂਰਨ type;
  ਬਣਤਰ cmd *ਖੱਬਾ;
  ਬਣਤਰ cmd *ਸੱਜਾ;
};

ਬਣਤਰ listcmd {
  ਪੂਰਨ type;
  ਬਣਤਰ cmd *ਖੱਬਾ;
  ਬਣਤਰ cmd *ਸੱਜਾ;
};

ਬਣਤਰ backcmd {
  ਪੂਰਨ type;
  ਬਣਤਰ cmd *cmd;
};

ਪੂਰਨ fork1(ਖਾਲੀ); // Fork but panics on failure.
ਖਾਲੀ panic(ਅੱਖਰ *);
ਬਣਤਰ cmd *parsecmd(ਅੱਖਰ *);
ਖਾਲੀ runcmd(ਬਣਤਰ cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
ਖਾਲੀ
runcmd(ਬਣਤਰ cmd *cmd)
{
  ਪੂਰਨ p[2];
  ਬਣਤਰ backcmd *bcmd;
  ਬਣਤਰ execcmd *ecmd;
  ਬਣਤਰ listcmd *lcmd;
  ਬਣਤਰ pipecmd *pcmd;
  ਬਣਤਰ redircmd *rcmd;

  ਜੇ (cmd == 0)
    ਬਾਹਰ_ਨਿਕਲੋ(1);

  ਚੋਣ (cmd->type) {
  ਮੂਲ:
    panic("runcmd");

  ਹਾਲਤ EXEC:
    ecmd = (ਬਣਤਰ execcmd *)cmd;
    ਜੇ (ecmd->argv[0] == 0)
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    ਰੋਕੋ;

  ਹਾਲਤ REDIR:
    rcmd = (ਬਣਤਰ redircmd *)cmd;
    close(rcmd->fd);
    ਜੇ (open(rcmd->file, rcmd->ਬਹੁਲਕ) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    runcmd(rcmd->cmd);
    ਰੋਕੋ;

  ਹਾਲਤ LIST:
    lcmd = (ਬਣਤਰ listcmd *)cmd;
    ਜੇ (fork1() == 0)
      runcmd(lcmd->ਖੱਬਾ);
    wait(0);
    runcmd(lcmd->ਸੱਜਾ);
    ਰੋਕੋ;

  ਹਾਲਤ PIPE:
    pcmd = (ਬਣਤਰ pipecmd *)cmd;
    ਜੇ (pipe(p) < 0)
      panic("pipe");
    ਜੇ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ਖੱਬਾ);
    }
    ਜੇ (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ਸੱਜਾ);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    ਰੋਕੋ;

  ਹਾਲਤ ਪਿੱਠ:
    bcmd = (ਬਣਤਰ backcmd *)cmd;
    ਜੇ (fork1() == 0)
      runcmd(bcmd->cmd);
    ਰੋਕੋ;
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

ਪੂਰਨ
getcmd(ਅੱਖਰ *buf, ਪੂਰਨ nbuf)
{
  ਲਿਖ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ਜੇ (buf[0] == 0) // EOF
    ਵਾਪਸ -1;
  ਵਾਪਸ 0;
}

ਪੂਰਨ
ਮੁੱਖ(ਖਾਲੀ)
{
  ਸਥਿਰ ਅੱਖਰ buf[100];
  ਪੂਰਨ fd;

  // Ensure that three file descriptors are open.
  ਜਦੋਂ ((fd = open("console", O_RDWR)) >= 0) {
    ਜੇ (fd >= 3) {
      close(fd);
      ਰੋਕੋ;
    }
  }

  // Read and run input commands.
  ਜਦੋਂ (getcmd(buf, ਆਕਾਰ(buf)) >= 0) {
    ਅੱਖਰ *cmd = buf;
    ਜਦੋਂ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ਜੇ (*cmd == '\n') // is a blank command
      ਜਾਰੀ;
    ਜੇ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[ਲੜੀ_ਲੰਬਾਈ(cmd) - 1] = 0; // chop \n
      ਜੇ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } ਨਹੀਂਤਾਂ {
      ਜੇ (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

ਖਾਲੀ
panic(ਅੱਖਰ *s)
{
  fprintf(2, "%s\n", s);
  ਬਾਹਰ_ਨਿਕਲੋ(1);
}

ਪੂਰਨ
fork1(ਖਾਲੀ)
{
  ਪੂਰਨ pid;

  pid = fork();
  ਜੇ (pid == -1)
    panic("fork");
  ਵਾਪਸ pid;
}

//PAGEBREAK!
// Constructors

ਬਣਤਰ cmd *
execcmd(ਖਾਲੀ)
{
  ਬਣਤਰ execcmd *cmd;

  cmd = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(*cmd));
  memset(cmd, 0, ਆਕਾਰ(*cmd));
  cmd->type = EXEC;
  ਵਾਪਸ (ਬਣਤਰ cmd *)cmd;
}

ਬਣਤਰ cmd *
redircmd(ਬਣਤਰ cmd *subcmd, ਅੱਖਰ *file, ਅੱਖਰ *efile, ਪੂਰਨ ਬਹੁਲਕ, ਪੂਰਨ fd)
{
  ਬਣਤਰ redircmd *cmd;

  cmd = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(*cmd));
  memset(cmd, 0, ਆਕਾਰ(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->ਬਹੁਲਕ = ਬਹੁਲਕ;
  cmd->fd = fd;
  ਵਾਪਸ (ਬਣਤਰ cmd *)cmd;
}

ਬਣਤਰ cmd *
pipecmd(ਬਣਤਰ cmd *ਖੱਬਾ, ਬਣਤਰ cmd *ਸੱਜਾ)
{
  ਬਣਤਰ pipecmd *cmd;

  cmd = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(*cmd));
  memset(cmd, 0, ਆਕਾਰ(*cmd));
  cmd->type = PIPE;
  cmd->ਖੱਬਾ = ਖੱਬਾ;
  cmd->ਸੱਜਾ = ਸੱਜਾ;
  ਵਾਪਸ (ਬਣਤਰ cmd *)cmd;
}

ਬਣਤਰ cmd *
listcmd(ਬਣਤਰ cmd *ਖੱਬਾ, ਬਣਤਰ cmd *ਸੱਜਾ)
{
  ਬਣਤਰ listcmd *cmd;

  cmd = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(*cmd));
  memset(cmd, 0, ਆਕਾਰ(*cmd));
  cmd->type = LIST;
  cmd->ਖੱਬਾ = ਖੱਬਾ;
  cmd->ਸੱਜਾ = ਸੱਜਾ;
  ਵਾਪਸ (ਬਣਤਰ cmd *)cmd;
}

ਬਣਤਰ cmd *
backcmd(ਬਣਤਰ cmd *subcmd)
{
  ਬਣਤਰ backcmd *cmd;

  cmd = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(*cmd));
  memset(cmd, 0, ਆਕਾਰ(*cmd));
  cmd->type = ਪਿੱਠ;
  cmd->cmd = subcmd;
  ਵਾਪਸ (ਬਣਤਰ cmd *)cmd;
}
//PAGEBREAK!
// Parsing

ਅੱਖਰ whitespace[] = " \t\r\n\v";
ਅੱਖਰ symbols[] = "<|>&;()";

ਪੂਰਨ
gettoken(ਅੱਖਰ **ps, ਅੱਖਰ *es, ਅੱਖਰ **q, ਅੱਖਰ **eq)
{
  ਅੱਖਰ *s;
  ਪੂਰਨ ret;

  s = *ps;
  ਜਦੋਂ (s < es && strchr(whitespace, *s))
    s++;
  ਜੇ (q)
    *q = s;
  ret = *s;
  ਚੋਣ (*s) {
  ਹਾਲਤ 0:
    ਰੋਕੋ;
  ਹਾਲਤ '|':
  ਹਾਲਤ '(':
  ਹਾਲਤ ')':
  ਹਾਲਤ ';':
  ਹਾਲਤ '&':
  ਹਾਲਤ '<':
    s++;
    ਰੋਕੋ;
  ਹਾਲਤ '>':
    s++;
    ਜੇ (*s == '>') {
      ret = '+';
      s++;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ret = 'a';
    ਜਦੋਂ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    ਰੋਕੋ;
  }
  ਜੇ (eq)
    *eq = s;

  ਜਦੋਂ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ਵਾਪਸ ret;
}

ਪੂਰਨ
peek(ਅੱਖਰ **ps, ਅੱਖਰ *es, ਅੱਖਰ *toks)
{
  ਅੱਖਰ *s;

  s = *ps;
  ਜਦੋਂ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ਵਾਪਸ *s && strchr(toks, *s);
}

ਬਣਤਰ cmd *parseline(ਅੱਖਰ **, ਅੱਖਰ *);
ਬਣਤਰ cmd *parsepipe(ਅੱਖਰ **, ਅੱਖਰ *);
ਬਣਤਰ cmd *parseexec(ਅੱਖਰ **, ਅੱਖਰ *);
ਬਣਤਰ cmd *nulterminate(ਬਣਤਰ cmd *);

ਬਣਤਰ cmd *
parsecmd(ਅੱਖਰ *s)
{
  ਅੱਖਰ *es;
  ਬਣਤਰ cmd *cmd;

  es = s + ਲੜੀ_ਲੰਬਾਈ(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  ਜੇ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ਵਾਪਸ cmd;
}

ਬਣਤਰ cmd *
parseline(ਅੱਖਰ **ps, ਅੱਖਰ *es)
{
  ਬਣਤਰ cmd *cmd;

  cmd = parsepipe(ps, es);
  ਜਦੋਂ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ਜੇ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ਵਾਪਸ cmd;
}

ਬਣਤਰ cmd *
parsepipe(ਅੱਖਰ **ps, ਅੱਖਰ *es)
{
  ਬਣਤਰ cmd *cmd;

  cmd = parseexec(ps, es);
  ਜੇ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ਵਾਪਸ cmd;
}

ਬਣਤਰ cmd *
parseredirs(ਬਣਤਰ cmd *cmd, ਅੱਖਰ **ps, ਅੱਖਰ *es)
{
  ਪੂਰਨ tok;
  ਅੱਖਰ *q, *eq;

  ਜਦੋਂ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ਜੇ (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    ਚੋਣ (tok) {
    ਹਾਲਤ '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      ਰੋਕੋ;
    ਹਾਲਤ '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      ਰੋਕੋ;
    ਹਾਲਤ '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ cmd;
}

ਬਣਤਰ cmd *
parseblock(ਅੱਖਰ **ps, ਅੱਖਰ *es)
{
  ਬਣਤਰ cmd *cmd;

  ਜੇ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ਜੇ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ਵਾਪਸ cmd;
}

ਬਣਤਰ cmd *
parseexec(ਅੱਖਰ **ps, ਅੱਖਰ *es)
{
  ਅੱਖਰ *q, *eq;
  ਪੂਰਨ tok, argc;
  ਬਣਤਰ execcmd *cmd;
  ਬਣਤਰ cmd *ret;

  ਜੇ (peek(ps, es, "("))
    ਵਾਪਸ parseblock(ps, es);

  ret = execcmd();
  cmd = (ਬਣਤਰ execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ਜਦੋਂ (!peek(ps, es, "|)&;")) {
    ਜੇ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      ਰੋਕੋ;
    ਜੇ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ਜੇ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  ਵਾਪਸ ret;
}

// NUL-terminate all the counted strings.
ਬਣਤਰ cmd *
nulterminate(ਬਣਤਰ cmd *cmd)
{
  ਪੂਰਨ i;
  ਬਣਤਰ backcmd *bcmd;
  ਬਣਤਰ execcmd *ecmd;
  ਬਣਤਰ listcmd *lcmd;
  ਬਣਤਰ pipecmd *pcmd;
  ਬਣਤਰ redircmd *rcmd;

  ਜੇ (cmd == 0)
    ਵਾਪਸ 0;

  ਚੋਣ (cmd->type) {
  ਹਾਲਤ EXEC:
    ecmd = (ਬਣਤਰ execcmd *)cmd;
    ਲਈ (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    ਰੋਕੋ;

  ਹਾਲਤ REDIR:
    rcmd = (ਬਣਤਰ redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    ਰੋਕੋ;

  ਹਾਲਤ PIPE:
    pcmd = (ਬਣਤਰ pipecmd *)cmd;
    nulterminate(pcmd->ਖੱਬਾ);
    nulterminate(pcmd->ਸੱਜਾ);
    ਰੋਕੋ;

  ਹਾਲਤ LIST:
    lcmd = (ਬਣਤਰ listcmd *)cmd;
    nulterminate(lcmd->ਖੱਬਾ);
    nulterminate(lcmd->ਸੱਜਾ);
    ਰੋਕੋ;

  ਹਾਲਤ ਪਿੱਠ:
    bcmd = (ਬਣਤਰ backcmd *)cmd;
    nulterminate(bcmd->cmd);
    ਰੋਕੋ;
  }
  ਵਾਪਸ cmd;
}
