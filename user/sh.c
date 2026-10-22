// Shell.

#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "user/user.h"
#അന്തര്ഭാവ "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

ഘടന cmd {
  പൂർണ്ണസംഖ്യ type;
};

ഘടന execcmd {
  പൂർണ്ണസംഖ്യ type;
  അക്ഷരം *argv[MAXARGS];
  അക്ഷരം *eargv[MAXARGS];
};

ഘടന redircmd {
  പൂർണ്ണസംഖ്യ type;
  ഘടന cmd *cmd;
  അക്ഷരം *file;
  അക്ഷരം *efile;
  പൂർണ്ണസംഖ്യ mode;
  പൂർണ്ണസംഖ്യ fd;
};

ഘടന pipecmd {
  പൂർണ്ണസംഖ്യ type;
  ഘടന cmd *ഇടത്;
  ഘടന cmd *വലത്;
};

ഘടന listcmd {
  പൂർണ്ണസംഖ്യ type;
  ഘടന cmd *ഇടത്;
  ഘടന cmd *വലത്;
};

ഘടന backcmd {
  പൂർണ്ണസംഖ്യ type;
  ഘടന cmd *cmd;
};

പൂർണ്ണസംഖ്യ fork1(ശൂന്യം); // Fork but panics on failure.
ശൂന്യം panic(അക്ഷരം *);
ഘടന cmd *parsecmd(അക്ഷരം *);
ശൂന്യം runcmd(ഘടന cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
ശൂന്യം
runcmd(ഘടന cmd *cmd)
{
  പൂർണ്ണസംഖ്യ p[2];
  ഘടന backcmd *bcmd;
  ഘടന execcmd *ecmd;
  ഘടന listcmd *lcmd;
  ഘടന pipecmd *pcmd;
  ഘടന redircmd *rcmd;

  എങ്കിൽ (cmd == 0)
    exit(1);

  തിരഞ്ഞെടുപ്പ് (cmd->type) {
  സ്ഥിരസ്ഥിതി:
    panic("runcmd");

  അവസ്ഥ EXEC:
    ecmd = (ഘടന execcmd *)cmd;
    എങ്കിൽ (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    നിർത്തുക;

  അവസ്ഥ REDIR:
    rcmd = (ഘടന redircmd *)cmd;
    close(rcmd->fd);
    എങ്കിൽ (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    നിർത്തുക;

  അവസ്ഥ LIST:
    lcmd = (ഘടന listcmd *)cmd;
    എങ്കിൽ (fork1() == 0)
      runcmd(lcmd->ഇടത്);
    wait(0);
    runcmd(lcmd->വലത്);
    നിർത്തുക;

  അവസ്ഥ PIPE:
    pcmd = (ഘടന pipecmd *)cmd;
    എങ്കിൽ (pipe(p) < 0)
      panic("pipe");
    എങ്കിൽ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ഇടത്);
    }
    എങ്കിൽ (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->വലത്);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    നിർത്തുക;

  അവസ്ഥ BACK:
    bcmd = (ഘടന backcmd *)cmd;
    എങ്കിൽ (fork1() == 0)
      runcmd(bcmd->cmd);
    നിർത്തുക;
  }
  exit(0);
}

പൂർണ്ണസംഖ്യ
getcmd(അക്ഷരം *buf, പൂർണ്ണസംഖ്യ nbuf)
{
  ലിഖ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  എങ്കിൽ (buf[0] == 0) // EOF
    തിരികെ -1;
  തിരികെ 0;
}

പൂർണ്ണസംഖ്യ
പ്രധാന(ശൂന്യം)
{
  സ്ഥിതം അക്ഷരം buf[100];
  പൂർണ്ണസംഖ്യ fd;

  // Ensure that three file descriptors are open.
  ആകുമ്പോൾ ((fd = open("console", O_RDWR)) >= 0) {
    എങ്കിൽ (fd >= 3) {
      close(fd);
      നിർത്തുക;
    }
  }

  // Read and run input commands.
  ആകുമ്പോൾ (getcmd(buf, വലുപ്പം(buf)) >= 0) {
    അക്ഷരം *cmd = buf;
    ആകുമ്പോൾ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    എങ്കിൽ (*cmd == '\n') // is a blank command
      തുടരുക;
    എങ്കിൽ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      എങ്കിൽ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } അല്ലെങ്കിൽ {
      എങ്കിൽ (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

ശൂന്യം
panic(അക്ഷരം *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

പൂർണ്ണസംഖ്യ
fork1(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ pid;

  pid = fork();
  എങ്കിൽ (pid == -1)
    panic("fork");
  തിരികെ pid;
}

//PAGEBREAK!
// Constructors

ഘടന cmd *
execcmd(ശൂന്യം)
{
  ഘടന execcmd *cmd;

  cmd = malloc(വലുപ്പം(*cmd));
  memset(cmd, 0, വലുപ്പം(*cmd));
  cmd->type = EXEC;
  തിരികെ (ഘടന cmd *)cmd;
}

ഘടന cmd *
redircmd(ഘടന cmd *subcmd, അക്ഷരം *file, അക്ഷരം *efile, പൂർണ്ണസംഖ്യ mode, പൂർണ്ണസംഖ്യ fd)
{
  ഘടന redircmd *cmd;

  cmd = malloc(വലുപ്പം(*cmd));
  memset(cmd, 0, വലുപ്പം(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  തിരികെ (ഘടന cmd *)cmd;
}

ഘടന cmd *
pipecmd(ഘടന cmd *ഇടത്, ഘടന cmd *വലത്)
{
  ഘടന pipecmd *cmd;

  cmd = malloc(വലുപ്പം(*cmd));
  memset(cmd, 0, വലുപ്പം(*cmd));
  cmd->type = PIPE;
  cmd->ഇടത് = ഇടത്;
  cmd->വലത് = വലത്;
  തിരികെ (ഘടന cmd *)cmd;
}

ഘടന cmd *
listcmd(ഘടന cmd *ഇടത്, ഘടന cmd *വലത്)
{
  ഘടന listcmd *cmd;

  cmd = malloc(വലുപ്പം(*cmd));
  memset(cmd, 0, വലുപ്പം(*cmd));
  cmd->type = LIST;
  cmd->ഇടത് = ഇടത്;
  cmd->വലത് = വലത്;
  തിരികെ (ഘടന cmd *)cmd;
}

ഘടന cmd *
backcmd(ഘടന cmd *subcmd)
{
  ഘടന backcmd *cmd;

  cmd = malloc(വലുപ്പം(*cmd));
  memset(cmd, 0, വലുപ്പം(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  തിരികെ (ഘടന cmd *)cmd;
}
//PAGEBREAK!
// Parsing

അക്ഷരം whitespace[] = " \t\r\n\v";
അക്ഷരം symbols[] = "<|>&;()";

പൂർണ്ണസംഖ്യ
gettoken(അക്ഷരം **ps, അക്ഷരം *es, അക്ഷരം **q, അക്ഷരം **eq)
{
  അക്ഷരം *s;
  പൂർണ്ണസംഖ്യ ret;

  s = *ps;
  ആകുമ്പോൾ (s < es && strchr(whitespace, *s))
    s++;
  എങ്കിൽ (q)
    *q = s;
  ret = *s;
  തിരഞ്ഞെടുപ്പ് (*s) {
  അവസ്ഥ 0:
    നിർത്തുക;
  അവസ്ഥ '|':
  അവസ്ഥ '(':
  അവസ്ഥ ')':
  അവസ്ഥ ';':
  അവസ്ഥ '&':
  അവസ്ഥ '<':
    s++;
    നിർത്തുക;
  അവസ്ഥ '>':
    s++;
    എങ്കിൽ (*s == '>') {
      ret = '+';
      s++;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    ret = 'a';
    ആകുമ്പോൾ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    നിർത്തുക;
  }
  എങ്കിൽ (eq)
    *eq = s;

  ആകുമ്പോൾ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  തിരികെ ret;
}

പൂർണ്ണസംഖ്യ
peek(അക്ഷരം **ps, അക്ഷരം *es, അക്ഷരം *toks)
{
  അക്ഷരം *s;

  s = *ps;
  ആകുമ്പോൾ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  തിരികെ *s && strchr(toks, *s);
}

ഘടന cmd *parseline(അക്ഷരം **, അക്ഷരം *);
ഘടന cmd *parsepipe(അക്ഷരം **, അക്ഷരം *);
ഘടന cmd *parseexec(അക്ഷരം **, അക്ഷരം *);
ഘടന cmd *nulterminate(ഘടന cmd *);

ഘടന cmd *
parsecmd(അക്ഷരം *s)
{
  അക്ഷരം *es;
  ഘടന cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  എങ്കിൽ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  തിരികെ cmd;
}

ഘടന cmd *
parseline(അക്ഷരം **ps, അക്ഷരം *es)
{
  ഘടന cmd *cmd;

  cmd = parsepipe(ps, es);
  ആകുമ്പോൾ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  എങ്കിൽ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  തിരികെ cmd;
}

ഘടന cmd *
parsepipe(അക്ഷരം **ps, അക്ഷരം *es)
{
  ഘടന cmd *cmd;

  cmd = parseexec(ps, es);
  എങ്കിൽ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  തിരികെ cmd;
}

ഘടന cmd *
parseredirs(ഘടന cmd *cmd, അക്ഷരം **ps, അക്ഷരം *es)
{
  പൂർണ്ണസംഖ്യ tok;
  അക്ഷരം *q, *eq;

  ആകുമ്പോൾ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    എങ്കിൽ (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    തിരഞ്ഞെടുപ്പ് (tok) {
    അവസ്ഥ '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      നിർത്തുക;
    അവസ്ഥ '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      നിർത്തുക;
    അവസ്ഥ '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      നിർത്തുക;
    }
  }
  തിരികെ cmd;
}

ഘടന cmd *
parseblock(അക്ഷരം **ps, അക്ഷരം *es)
{
  ഘടന cmd *cmd;

  എങ്കിൽ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  എങ്കിൽ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  തിരികെ cmd;
}

ഘടന cmd *
parseexec(അക്ഷരം **ps, അക്ഷരം *es)
{
  അക്ഷരം *q, *eq;
  പൂർണ്ണസംഖ്യ tok, argc;
  ഘടന execcmd *cmd;
  ഘടന cmd *ret;

  എങ്കിൽ (peek(ps, es, "("))
    തിരികെ parseblock(ps, es);

  ret = execcmd();
  cmd = (ഘടന execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ആകുമ്പോൾ (!peek(ps, es, "|)&;")) {
    എങ്കിൽ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      നിർത്തുക;
    എങ്കിൽ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    എങ്കിൽ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  തിരികെ ret;
}

// NUL-terminate all the counted strings.
ഘടന cmd *
nulterminate(ഘടന cmd *cmd)
{
  പൂർണ്ണസംഖ്യ i;
  ഘടന backcmd *bcmd;
  ഘടന execcmd *ecmd;
  ഘടന listcmd *lcmd;
  ഘടന pipecmd *pcmd;
  ഘടന redircmd *rcmd;

  എങ്കിൽ (cmd == 0)
    തിരികെ 0;

  തിരഞ്ഞെടുപ്പ് (cmd->type) {
  അവസ്ഥ EXEC:
    ecmd = (ഘടന execcmd *)cmd;
    വേണ്ടി (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    നിർത്തുക;

  അവസ്ഥ REDIR:
    rcmd = (ഘടന redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    നിർത്തുക;

  അവസ്ഥ PIPE:
    pcmd = (ഘടന pipecmd *)cmd;
    nulterminate(pcmd->ഇടത്);
    nulterminate(pcmd->വലത്);
    നിർത്തുക;

  അവസ്ഥ LIST:
    lcmd = (ഘടന listcmd *)cmd;
    nulterminate(lcmd->ഇടത്);
    nulterminate(lcmd->വലത്);
    നിർത്തുക;

  അവസ്ഥ BACK:
    bcmd = (ഘടന backcmd *)cmd;
    nulterminate(bcmd->cmd);
    നിർത്തുക;
  }
  തിരികെ cmd;
}
