// Shell.

#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "user/user.h"
#అన్తర్భావ "kernel/fcntl.h"

// Parsed command representation
#నిర్వచించు EXEC  1
#నిర్వచించు REDIR 2
#నిర్వచించు PIPE  3
#నిర్వచించు LIST  4
#నిర్వచించు వెనుక  5

#నిర్వచించు MAXARGS 10

నిర్మాణం cmd {
  పూర్ణాంకం type;
};

నిర్మాణం execcmd {
  పూర్ణాంకం type;
  అక్షరం *argv[MAXARGS];
  అక్షరం *eargv[MAXARGS];
};

నిర్మాణం redircmd {
  పూర్ణాంకం type;
  నిర్మాణం cmd *cmd;
  అక్షరం *file;
  అక్షరం *efile;
  పూర్ణాంకం బహుళకం;
  పూర్ణాంకం fd;
};

నిర్మాణం pipecmd {
  పూర్ణాంకం type;
  నిర్మాణం cmd *ఎడమ;
  నిర్మాణం cmd *కుడి;
};

నిర్మాణం listcmd {
  పూర్ణాంకం type;
  నిర్మాణం cmd *ఎడమ;
  నిర్మాణం cmd *కుడి;
};

నిర్మాణం backcmd {
  పూర్ణాంకం type;
  నిర్మాణం cmd *cmd;
};

పూర్ణాంకం fork1(శూన్య); // Fork but panics on failure.
శూన్య panic(అక్షరం *);
నిర్మాణం cmd *parsecmd(అక్షరం *);
శూన్య runcmd(నిర్మాణం cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
శూన్య
runcmd(నిర్మాణం cmd *cmd)
{
  పూర్ణాంకం p[2];
  నిర్మాణం backcmd *bcmd;
  నిర్మాణం execcmd *ecmd;
  నిర్మాణం listcmd *lcmd;
  నిర్మాణం pipecmd *pcmd;
  నిర్మాణం redircmd *rcmd;

  ఒకవేళ (cmd == 0)
    నిష్క్రమించు(1);

  ఎంపిక (cmd->type) {
  అప్రమేయ:
    panic("runcmd");

  స్థితి EXEC:
    ecmd = (నిర్మాణం execcmd *)cmd;
    ఒకవేళ (ecmd->argv[0] == 0)
      నిష్క్రమించు(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    ఆపు;

  స్థితి REDIR:
    rcmd = (నిర్మాణం redircmd *)cmd;
    close(rcmd->fd);
    ఒకవేళ (open(rcmd->file, rcmd->బహుళకం) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      నిష్క్రమించు(1);
    }
    runcmd(rcmd->cmd);
    ఆపు;

  స్థితి LIST:
    lcmd = (నిర్మాణం listcmd *)cmd;
    ఒకవేళ (fork1() == 0)
      runcmd(lcmd->ఎడమ);
    wait(0);
    runcmd(lcmd->కుడి);
    ఆపు;

  స్థితి PIPE:
    pcmd = (నిర్మాణం pipecmd *)cmd;
    ఒకవేళ (pipe(p) < 0)
      panic("pipe");
    ఒకవేళ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ఎడమ);
    }
    ఒకవేళ (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->కుడి);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    ఆపు;

  స్థితి వెనుక:
    bcmd = (నిర్మాణం backcmd *)cmd;
    ఒకవేళ (fork1() == 0)
      runcmd(bcmd->cmd);
    ఆపు;
  }
  నిష్క్రమించు(0);
}

పూర్ణాంకం
getcmd(అక్షరం *buf, పూర్ణాంకం nbuf)
{
  లిఖ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ఒకవేళ (buf[0] == 0) // EOF
    తిరిగి -1;
  తిరిగి 0;
}

పూర్ణాంకం
ప్రధాన(శూన్య)
{
  స్థిర అక్షరం buf[100];
  పూర్ణాంకం fd;

  // Ensure that three file descriptors are open.
  అయితే ((fd = open("console", O_RDWR)) >= 0) {
    ఒకవేళ (fd >= 3) {
      close(fd);
      ఆపు;
    }
  }

  // Read and run input commands.
  అయితే (getcmd(buf, పరిమాణం(buf)) >= 0) {
    అక్షరం *cmd = buf;
    అయితే (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ఒకవేళ (*cmd == '\n') // is a blank command
      కొనసాగు;
    ఒకవేళ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[అక్షరశ్రేణి_పొడవు(cmd) - 1] = 0; // chop \n
      ఒకవేళ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } లేకపోతే {
      ఒకవేళ (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  నిష్క్రమించు(0);
}

శూన్య
panic(అక్షరం *s)
{
  fprintf(2, "%s\n", s);
  నిష్క్రమించు(1);
}

పూర్ణాంకం
fork1(శూన్య)
{
  పూర్ణాంకం pid;

  pid = fork();
  ఒకవేళ (pid == -1)
    panic("fork");
  తిరిగి pid;
}

//PAGEBREAK!
// Constructors

నిర్మాణం cmd *
execcmd(శూన్య)
{
  నిర్మాణం execcmd *cmd;

  cmd = జ్ఞాపకం_ఇవ్వు(పరిమాణం(*cmd));
  memset(cmd, 0, పరిమాణం(*cmd));
  cmd->type = EXEC;
  తిరిగి (నిర్మాణం cmd *)cmd;
}

నిర్మాణం cmd *
redircmd(నిర్మాణం cmd *subcmd, అక్షరం *file, అక్షరం *efile, పూర్ణాంకం బహుళకం, పూర్ణాంకం fd)
{
  నిర్మాణం redircmd *cmd;

  cmd = జ్ఞాపకం_ఇవ్వు(పరిమాణం(*cmd));
  memset(cmd, 0, పరిమాణం(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->బహుళకం = బహుళకం;
  cmd->fd = fd;
  తిరిగి (నిర్మాణం cmd *)cmd;
}

నిర్మాణం cmd *
pipecmd(నిర్మాణం cmd *ఎడమ, నిర్మాణం cmd *కుడి)
{
  నిర్మాణం pipecmd *cmd;

  cmd = జ్ఞాపకం_ఇవ్వు(పరిమాణం(*cmd));
  memset(cmd, 0, పరిమాణం(*cmd));
  cmd->type = PIPE;
  cmd->ఎడమ = ఎడమ;
  cmd->కుడి = కుడి;
  తిరిగి (నిర్మాణం cmd *)cmd;
}

నిర్మాణం cmd *
listcmd(నిర్మాణం cmd *ఎడమ, నిర్మాణం cmd *కుడి)
{
  నిర్మాణం listcmd *cmd;

  cmd = జ్ఞాపకం_ఇవ్వు(పరిమాణం(*cmd));
  memset(cmd, 0, పరిమాణం(*cmd));
  cmd->type = LIST;
  cmd->ఎడమ = ఎడమ;
  cmd->కుడి = కుడి;
  తిరిగి (నిర్మాణం cmd *)cmd;
}

నిర్మాణం cmd *
backcmd(నిర్మాణం cmd *subcmd)
{
  నిర్మాణం backcmd *cmd;

  cmd = జ్ఞాపకం_ఇవ్వు(పరిమాణం(*cmd));
  memset(cmd, 0, పరిమాణం(*cmd));
  cmd->type = వెనుక;
  cmd->cmd = subcmd;
  తిరిగి (నిర్మాణం cmd *)cmd;
}
//PAGEBREAK!
// Parsing

అక్షరం whitespace[] = " \t\r\n\v";
అక్షరం symbols[] = "<|>&;()";

పూర్ణాంకం
gettoken(అక్షరం **ps, అక్షరం *es, అక్షరం **q, అక్షరం **eq)
{
  అక్షరం *s;
  పూర్ణాంకం ret;

  s = *ps;
  అయితే (s < es && strchr(whitespace, *s))
    s++;
  ఒకవేళ (q)
    *q = s;
  ret = *s;
  ఎంపిక (*s) {
  స్థితి 0:
    ఆపు;
  స్థితి '|':
  స్థితి '(':
  స్థితి ')':
  స్థితి ';':
  స్థితి '&':
  స్థితి '<':
    s++;
    ఆపు;
  స్థితి '>':
    s++;
    ఒకవేళ (*s == '>') {
      ret = '+';
      s++;
    }
    ఆపు;
  అప్రమేయ:
    ret = 'a';
    అయితే (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    ఆపు;
  }
  ఒకవేళ (eq)
    *eq = s;

  అయితే (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  తిరిగి ret;
}

పూర్ణాంకం
peek(అక్షరం **ps, అక్షరం *es, అక్షరం *toks)
{
  అక్షరం *s;

  s = *ps;
  అయితే (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  తిరిగి *s && strchr(toks, *s);
}

నిర్మాణం cmd *parseline(అక్షరం **, అక్షరం *);
నిర్మాణం cmd *parsepipe(అక్షరం **, అక్షరం *);
నిర్మాణం cmd *parseexec(అక్షరం **, అక్షరం *);
నిర్మాణం cmd *nulterminate(నిర్మాణం cmd *);

నిర్మాణం cmd *
parsecmd(అక్షరం *s)
{
  అక్షరం *es;
  నిర్మాణం cmd *cmd;

  es = s + అక్షరశ్రేణి_పొడవు(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  ఒకవేళ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  తిరిగి cmd;
}

నిర్మాణం cmd *
parseline(అక్షరం **ps, అక్షరం *es)
{
  నిర్మాణం cmd *cmd;

  cmd = parsepipe(ps, es);
  అయితే (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ఒకవేళ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  తిరిగి cmd;
}

నిర్మాణం cmd *
parsepipe(అక్షరం **ps, అక్షరం *es)
{
  నిర్మాణం cmd *cmd;

  cmd = parseexec(ps, es);
  ఒకవేళ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  తిరిగి cmd;
}

నిర్మాణం cmd *
parseredirs(నిర్మాణం cmd *cmd, అక్షరం **ps, అక్షరం *es)
{
  పూర్ణాంకం tok;
  అక్షరం *q, *eq;

  అయితే (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ఒకవేళ (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    ఎంపిక (tok) {
    స్థితి '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      ఆపు;
    స్థితి '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      ఆపు;
    స్థితి '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      ఆపు;
    }
  }
  తిరిగి cmd;
}

నిర్మాణం cmd *
parseblock(అక్షరం **ps, అక్షరం *es)
{
  నిర్మాణం cmd *cmd;

  ఒకవేళ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ఒకవేళ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  తిరిగి cmd;
}

నిర్మాణం cmd *
parseexec(అక్షరం **ps, అక్షరం *es)
{
  అక్షరం *q, *eq;
  పూర్ణాంకం tok, argc;
  నిర్మాణం execcmd *cmd;
  నిర్మాణం cmd *ret;

  ఒకవేళ (peek(ps, es, "("))
    తిరిగి parseblock(ps, es);

  ret = execcmd();
  cmd = (నిర్మాణం execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  అయితే (!peek(ps, es, "|)&;")) {
    ఒకవేళ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      ఆపు;
    ఒకవేళ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ఒకవేళ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  తిరిగి ret;
}

// NUL-terminate all the counted strings.
నిర్మాణం cmd *
nulterminate(నిర్మాణం cmd *cmd)
{
  పూర్ణాంకం i;
  నిర్మాణం backcmd *bcmd;
  నిర్మాణం execcmd *ecmd;
  నిర్మాణం listcmd *lcmd;
  నిర్మాణం pipecmd *pcmd;
  నిర్మాణం redircmd *rcmd;

  ఒకవేళ (cmd == 0)
    తిరిగి 0;

  ఎంపిక (cmd->type) {
  స్థితి EXEC:
    ecmd = (నిర్మాణం execcmd *)cmd;
    కొరకు (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    ఆపు;

  స్థితి REDIR:
    rcmd = (నిర్మాణం redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    ఆపు;

  స్థితి PIPE:
    pcmd = (నిర్మాణం pipecmd *)cmd;
    nulterminate(pcmd->ఎడమ);
    nulterminate(pcmd->కుడి);
    ఆపు;

  స్థితి LIST:
    lcmd = (నిర్మాణం listcmd *)cmd;
    nulterminate(lcmd->ఎడమ);
    nulterminate(lcmd->కుడి);
    ఆపు;

  స్థితి వెనుక:
    bcmd = (నిర్మాణం backcmd *)cmd;
    nulterminate(bcmd->cmd);
    ఆపు;
  }
  తిరిగి cmd;
}
