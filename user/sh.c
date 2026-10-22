// Shell.

#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "user/user.h"
#અન્તર્ભાવ "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

સંરચના cmd {
  પૂર્ણાંક type;
};

સંરચના execcmd {
  પૂર્ણાંક type;
  અક્ષર *argv[MAXARGS];
  અક્ષર *eargv[MAXARGS];
};

સંરચના redircmd {
  પૂર્ણાંક type;
  સંરચના cmd *cmd;
  અક્ષર *file;
  અક્ષર *efile;
  પૂર્ણાંક mode;
  પૂર્ણાંક fd;
};

સંરચના pipecmd {
  પૂર્ણાંક type;
  સંરચના cmd *ડાબું;
  સંરચના cmd *જમણું;
};

સંરચના listcmd {
  પૂર્ણાંક type;
  સંરચના cmd *ડાબું;
  સંરચના cmd *જમણું;
};

સંરચના backcmd {
  પૂર્ણાંક type;
  સંરચના cmd *cmd;
};

પૂર્ણાંક fork1(શૂન્ય); // Fork but panics on failure.
શૂન્ય panic(અક્ષર *);
સંરચના cmd *parsecmd(અક્ષર *);
શૂન્ય runcmd(સંરચના cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
શૂન્ય
runcmd(સંરચના cmd *cmd)
{
  પૂર્ણાંક p[2];
  સંરચના backcmd *bcmd;
  સંરચના execcmd *ecmd;
  સંરચના listcmd *lcmd;
  સંરચના pipecmd *pcmd;
  સંરચના redircmd *rcmd;

  જો (cmd == 0)
    exit(1);

  પસંદગી (cmd->type) {
  મૂળભૂત:
    panic("runcmd");

  સ્થિતિ EXEC:
    ecmd = (સંરચના execcmd *)cmd;
    જો (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    તોડો;

  સ્થિતિ REDIR:
    rcmd = (સંરચના redircmd *)cmd;
    close(rcmd->fd);
    જો (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    તોડો;

  સ્થિતિ LIST:
    lcmd = (સંરચના listcmd *)cmd;
    જો (fork1() == 0)
      runcmd(lcmd->ડાબું);
    wait(0);
    runcmd(lcmd->જમણું);
    તોડો;

  સ્થિતિ PIPE:
    pcmd = (સંરચના pipecmd *)cmd;
    જો (pipe(p) < 0)
      panic("pipe");
    જો (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ડાબું);
    }
    જો (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->જમણું);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    તોડો;

  સ્થિતિ BACK:
    bcmd = (સંરચના backcmd *)cmd;
    જો (fork1() == 0)
      runcmd(bcmd->cmd);
    તોડો;
  }
  exit(0);
}

પૂર્ણાંક
getcmd(અક્ષર *buf, પૂર્ણાંક nbuf)
{
  લિખ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  જો (buf[0] == 0) // EOF
    પાછા -1;
  પાછા 0;
}

પૂર્ણાંક
મુખ્ય(શૂન્ય)
{
  સ્થિર અક્ષર buf[100];
  પૂર્ણાંક fd;

  // Ensure that three file descriptors are open.
  જ્યારે ((fd = open("console", O_RDWR)) >= 0) {
    જો (fd >= 3) {
      close(fd);
      તોડો;
    }
  }

  // Read and run input commands.
  જ્યારે (getcmd(buf, કદ(buf)) >= 0) {
    અક્ષર *cmd = buf;
    જ્યારે (*cmd == ' ' || *cmd == '\t')
      cmd++;
    જો (*cmd == '\n') // is a blank command
      ચાલુ;
    જો (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      જો (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } નહીંતર {
      જો (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

શૂન્ય
panic(અક્ષર *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

પૂર્ણાંક
fork1(શૂન્ય)
{
  પૂર્ણાંક pid;

  pid = fork();
  જો (pid == -1)
    panic("fork");
  પાછા pid;
}

//PAGEBREAK!
// Constructors

સંરચના cmd *
execcmd(શૂન્ય)
{
  સંરચના execcmd *cmd;

  cmd = malloc(કદ(*cmd));
  memset(cmd, 0, કદ(*cmd));
  cmd->type = EXEC;
  પાછા (સંરચના cmd *)cmd;
}

સંરચના cmd *
redircmd(સંરચના cmd *subcmd, અક્ષર *file, અક્ષર *efile, પૂર્ણાંક mode, પૂર્ણાંક fd)
{
  સંરચના redircmd *cmd;

  cmd = malloc(કદ(*cmd));
  memset(cmd, 0, કદ(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  પાછા (સંરચના cmd *)cmd;
}

સંરચના cmd *
pipecmd(સંરચના cmd *ડાબું, સંરચના cmd *જમણું)
{
  સંરચના pipecmd *cmd;

  cmd = malloc(કદ(*cmd));
  memset(cmd, 0, કદ(*cmd));
  cmd->type = PIPE;
  cmd->ડાબું = ડાબું;
  cmd->જમણું = જમણું;
  પાછા (સંરચના cmd *)cmd;
}

સંરચના cmd *
listcmd(સંરચના cmd *ડાબું, સંરચના cmd *જમણું)
{
  સંરચના listcmd *cmd;

  cmd = malloc(કદ(*cmd));
  memset(cmd, 0, કદ(*cmd));
  cmd->type = LIST;
  cmd->ડાબું = ડાબું;
  cmd->જમણું = જમણું;
  પાછા (સંરચના cmd *)cmd;
}

સંરચના cmd *
backcmd(સંરચના cmd *subcmd)
{
  સંરચના backcmd *cmd;

  cmd = malloc(કદ(*cmd));
  memset(cmd, 0, કદ(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  પાછા (સંરચના cmd *)cmd;
}
//PAGEBREAK!
// Parsing

અક્ષર whitespace[] = " \t\r\n\v";
અક્ષર symbols[] = "<|>&;()";

પૂર્ણાંક
gettoken(અક્ષર **ps, અક્ષર *es, અક્ષર **q, અક્ષર **eq)
{
  અક્ષર *s;
  પૂર્ણાંક ret;

  s = *ps;
  જ્યારે (s < es && strchr(whitespace, *s))
    s++;
  જો (q)
    *q = s;
  ret = *s;
  પસંદગી (*s) {
  સ્થિતિ 0:
    તોડો;
  સ્થિતિ '|':
  સ્થિતિ '(':
  સ્થિતિ ')':
  સ્થિતિ ';':
  સ્થિતિ '&':
  સ્થિતિ '<':
    s++;
    તોડો;
  સ્થિતિ '>':
    s++;
    જો (*s == '>') {
      ret = '+';
      s++;
    }
    તોડો;
  મૂળભૂત:
    ret = 'a';
    જ્યારે (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    તોડો;
  }
  જો (eq)
    *eq = s;

  જ્યારે (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  પાછા ret;
}

પૂર્ણાંક
peek(અક્ષર **ps, અક્ષર *es, અક્ષર *toks)
{
  અક્ષર *s;

  s = *ps;
  જ્યારે (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  પાછા *s && strchr(toks, *s);
}

સંરચના cmd *parseline(અક્ષર **, અક્ષર *);
સંરચના cmd *parsepipe(અક્ષર **, અક્ષર *);
સંરચના cmd *parseexec(અક્ષર **, અક્ષર *);
સંરચના cmd *nulterminate(સંરચના cmd *);

સંરચના cmd *
parsecmd(અક્ષર *s)
{
  અક્ષર *es;
  સંરચના cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  જો (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  પાછા cmd;
}

સંરચના cmd *
parseline(અક્ષર **ps, અક્ષર *es)
{
  સંરચના cmd *cmd;

  cmd = parsepipe(ps, es);
  જ્યારે (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  જો (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  પાછા cmd;
}

સંરચના cmd *
parsepipe(અક્ષર **ps, અક્ષર *es)
{
  સંરચના cmd *cmd;

  cmd = parseexec(ps, es);
  જો (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  પાછા cmd;
}

સંરચના cmd *
parseredirs(સંરચના cmd *cmd, અક્ષર **ps, અક્ષર *es)
{
  પૂર્ણાંક tok;
  અક્ષર *q, *eq;

  જ્યારે (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    જો (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    પસંદગી (tok) {
    સ્થિતિ '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      તોડો;
    સ્થિતિ '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      તોડો;
    સ્થિતિ '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      તોડો;
    }
  }
  પાછા cmd;
}

સંરચના cmd *
parseblock(અક્ષર **ps, અક્ષર *es)
{
  સંરચના cmd *cmd;

  જો (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  જો (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  પાછા cmd;
}

સંરચના cmd *
parseexec(અક્ષર **ps, અક્ષર *es)
{
  અક્ષર *q, *eq;
  પૂર્ણાંક tok, argc;
  સંરચના execcmd *cmd;
  સંરચના cmd *ret;

  જો (peek(ps, es, "("))
    પાછા parseblock(ps, es);

  ret = execcmd();
  cmd = (સંરચના execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  જ્યારે (!peek(ps, es, "|)&;")) {
    જો ((tok = gettoken(ps, es, &q, &eq)) == 0)
      તોડો;
    જો (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    જો (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  પાછા ret;
}

// NUL-terminate all the counted strings.
સંરચના cmd *
nulterminate(સંરચના cmd *cmd)
{
  પૂર્ણાંક i;
  સંરચના backcmd *bcmd;
  સંરચના execcmd *ecmd;
  સંરચના listcmd *lcmd;
  સંરચના pipecmd *pcmd;
  સંરચના redircmd *rcmd;

  જો (cmd == 0)
    પાછા 0;

  પસંદગી (cmd->type) {
  સ્થિતિ EXEC:
    ecmd = (સંરચના execcmd *)cmd;
    માટે (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    તોડો;

  સ્થિતિ REDIR:
    rcmd = (સંરચના redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    તોડો;

  સ્થિતિ PIPE:
    pcmd = (સંરચના pipecmd *)cmd;
    nulterminate(pcmd->ડાબું);
    nulterminate(pcmd->જમણું);
    તોડો;

  સ્થિતિ LIST:
    lcmd = (સંરચના listcmd *)cmd;
    nulterminate(lcmd->ડાબું);
    nulterminate(lcmd->જમણું);
    તોડો;

  સ્થિતિ BACK:
    bcmd = (સંરચના backcmd *)cmd;
    nulterminate(bcmd->cmd);
    તોડો;
  }
  પાછા cmd;
}
