// Shell.

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

sa_nracanaa cmd {
  ចំនួនគត់ type;
};

sa_nracanaa execcmd {
  ចំនួនគត់ type;
  var_na *argv[MAXARGS];
  var_na *eargv[MAXARGS];
};

sa_nracanaa redircmd {
  ចំនួនគត់ type;
  sa_nracanaa cmd *cmd;
  var_na *file;
  var_na *efile;
  ចំនួនគត់ mode;
  ចំនួនគត់ fd;
};

sa_nracanaa pipecmd {
  ចំនួនគត់ type;
  sa_nracanaa cmd *left;
  sa_nracanaa cmd *right;
};

sa_nracanaa listcmd {
  ចំនួនគត់ type;
  sa_nracanaa cmd *left;
  sa_nracanaa cmd *right;
};

sa_nracanaa backcmd {
  ចំនួនគត់ type;
  sa_nracanaa cmd *cmd;
};

ចំនួនគត់ fork1(void); // Fork but panics on failure.
void panic(var_na *);
sa_nracanaa cmd *parsecmd(var_na *);
void runcmd(sa_nracanaa cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
void
runcmd(sa_nracanaa cmd *cmd)
{
  ចំនួនគត់ p[2];
  sa_nracanaa backcmd *bcmd;
  sa_nracanaa execcmd *ecmd;
  sa_nracanaa listcmd *lcmd;
  sa_nracanaa pipecmd *pcmd;
  sa_nracanaa redircmd *rcmd;

  ប្រសិនបើ (cmd == 0)
    exit(1);

  switch (cmd->type) {
  default:
    panic("runcmd");

  pak_sa EXEC:
    ecmd = (sa_nracanaa execcmd *)cmd;
    ប្រសិនបើ (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    bha_nga;

  pak_sa REDIR:
    rcmd = (sa_nracanaa redircmd *)cmd;
    close(rcmd->fd);
    ប្រសិនបើ (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    bha_nga;

  pak_sa LIST:
    lcmd = (sa_nracanaa listcmd *)cmd;
    ប្រសិនបើ (fork1() == 0)
      runcmd(lcmd->left);
    wait(0);
    runcmd(lcmd->right);
    bha_nga;

  pak_sa PIPE:
    pcmd = (sa_nracanaa pipecmd *)cmd;
    ប្រសិនបើ (pipe(p) < 0)
      panic("pipe");
    ប្រសិនបើ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    ប្រសិនបើ (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    bha_nga;

  pak_sa BACK:
    bcmd = (sa_nracanaa backcmd *)cmd;
    ប្រសិនបើ (fork1() == 0)
      runcmd(bcmd->cmd);
    bha_nga;
  }
  exit(0);
}

ចំនួនគត់
getcmd(var_na *buf, ចំនួនគត់ nbuf)
{
  write(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ប្រសិនបើ (buf[0] == 0) // EOF
    ត្រឡប់ -1;
  ត្រឡប់ 0;
}

ចំនួនគត់
main(void)
{
  static var_na buf[100];
  ចំនួនគត់ fd;

  // Ensure that three file descriptors are open.
  ខណៈពេល ((fd = open("console", O_RDWR)) >= 0) {
    ប្រសិនបើ (fd >= 3) {
      close(fd);
      bha_nga;
    }
  }

  // Read and run input commands.
  ខណៈពេល (getcmd(buf, parimaa_na(buf)) >= 0) {
    var_na *cmd = buf;
    ខណៈពេល (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ប្រសិនបើ (*cmd == '\n') // is a blank command
      continue;
    ប្រសិនបើ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      ប្រសិនបើ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } បើមិនដូច្នេះ {
      ប្រសិនបើ (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

void
panic(var_na *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

ចំនួនគត់
fork1(void)
{
  ចំនួនគត់ pid;

  pid = fork();
  ប្រសិនបើ (pid == -1)
    panic("fork");
  ត្រឡប់ pid;
}

//PAGEBREAK!
// Constructors

sa_nracanaa cmd *
execcmd(void)
{
  sa_nracanaa execcmd *cmd;

  cmd = malloc(parimaa_na(*cmd));
  memset(cmd, 0, parimaa_na(*cmd));
  cmd->type = EXEC;
  ត្រឡប់ (sa_nracanaa cmd *)cmd;
}

sa_nracanaa cmd *
redircmd(sa_nracanaa cmd *subcmd, var_na *file, var_na *efile, ចំនួនគត់ mode, ចំនួនគត់ fd)
{
  sa_nracanaa redircmd *cmd;

  cmd = malloc(parimaa_na(*cmd));
  memset(cmd, 0, parimaa_na(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  ត្រឡប់ (sa_nracanaa cmd *)cmd;
}

sa_nracanaa cmd *
pipecmd(sa_nracanaa cmd *left, sa_nracanaa cmd *right)
{
  sa_nracanaa pipecmd *cmd;

  cmd = malloc(parimaa_na(*cmd));
  memset(cmd, 0, parimaa_na(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  ត្រឡប់ (sa_nracanaa cmd *)cmd;
}

sa_nracanaa cmd *
listcmd(sa_nracanaa cmd *left, sa_nracanaa cmd *right)
{
  sa_nracanaa listcmd *cmd;

  cmd = malloc(parimaa_na(*cmd));
  memset(cmd, 0, parimaa_na(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  ត្រឡប់ (sa_nracanaa cmd *)cmd;
}

sa_nracanaa cmd *
backcmd(sa_nracanaa cmd *subcmd)
{
  sa_nracanaa backcmd *cmd;

  cmd = malloc(parimaa_na(*cmd));
  memset(cmd, 0, parimaa_na(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  ត្រឡប់ (sa_nracanaa cmd *)cmd;
}
//PAGEBREAK!
// Parsing

var_na whitespace[] = " \t\r\n\v";
var_na symbols[] = "<|>&;()";

ចំនួនគត់
gettoken(var_na **ps, var_na *es, var_na **q, var_na **eq)
{
  var_na *s;
  ចំនួនគត់ ret;

  s = *ps;
  ខណៈពេល (s < es && strchr(whitespace, *s))
    s++;
  ប្រសិនបើ (q)
    *q = s;
  ret = *s;
  switch (*s) {
  pak_sa 0:
    bha_nga;
  pak_sa '|':
  pak_sa '(':
  pak_sa ')':
  pak_sa ';':
  pak_sa '&':
  pak_sa '<':
    s++;
    bha_nga;
  pak_sa '>':
    s++;
    ប្រសិនបើ (*s == '>') {
      ret = '+';
      s++;
    }
    bha_nga;
  default:
    ret = 'a';
    ខណៈពេល (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    bha_nga;
  }
  ប្រសិនបើ (eq)
    *eq = s;

  ខណៈពេល (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ត្រឡប់ ret;
}

ចំនួនគត់
peek(var_na **ps, var_na *es, var_na *toks)
{
  var_na *s;

  s = *ps;
  ខណៈពេល (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ត្រឡប់ *s && strchr(toks, *s);
}

sa_nracanaa cmd *parseline(var_na **, var_na *);
sa_nracanaa cmd *parsepipe(var_na **, var_na *);
sa_nracanaa cmd *parseexec(var_na **, var_na *);
sa_nracanaa cmd *nulterminate(sa_nracanaa cmd *);

sa_nracanaa cmd *
parsecmd(var_na *s)
{
  var_na *es;
  sa_nracanaa cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  ប្រសិនបើ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ត្រឡប់ cmd;
}

sa_nracanaa cmd *
parseline(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  cmd = parsepipe(ps, es);
  ខណៈពេល (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ប្រសិនបើ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ត្រឡប់ cmd;
}

sa_nracanaa cmd *
parsepipe(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  cmd = parseexec(ps, es);
  ប្រសិនបើ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ត្រឡប់ cmd;
}

sa_nracanaa cmd *
parseredirs(sa_nracanaa cmd *cmd, var_na **ps, var_na *es)
{
  ចំនួនគត់ tok;
  var_na *q, *eq;

  ខណៈពេល (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ប្រសិនបើ (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    switch (tok) {
    pak_sa '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      bha_nga;
    pak_sa '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      bha_nga;
    pak_sa '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      bha_nga;
    }
  }
  ត្រឡប់ cmd;
}

sa_nracanaa cmd *
parseblock(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  ប្រសិនបើ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ប្រសិនបើ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ត្រឡប់ cmd;
}

sa_nracanaa cmd *
parseexec(var_na **ps, var_na *es)
{
  var_na *q, *eq;
  ចំនួនគត់ tok, argc;
  sa_nracanaa execcmd *cmd;
  sa_nracanaa cmd *ret;

  ប្រសិនបើ (peek(ps, es, "("))
    ត្រឡប់ parseblock(ps, es);

  ret = execcmd();
  cmd = (sa_nracanaa execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ខណៈពេល (!peek(ps, es, "|)&;")) {
    ប្រសិនបើ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      bha_nga;
    ប្រសិនបើ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ប្រសិនបើ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  ត្រឡប់ ret;
}

// NUL-terminate all the counted strings.
sa_nracanaa cmd *
nulterminate(sa_nracanaa cmd *cmd)
{
  ចំនួនគត់ i;
  sa_nracanaa backcmd *bcmd;
  sa_nracanaa execcmd *ecmd;
  sa_nracanaa listcmd *lcmd;
  sa_nracanaa pipecmd *pcmd;
  sa_nracanaa redircmd *rcmd;

  ប្រសិនបើ (cmd == 0)
    ត្រឡប់ 0;

  switch (cmd->type) {
  pak_sa EXEC:
    ecmd = (sa_nracanaa execcmd *)cmd;
    សម្រាប់ (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    bha_nga;

  pak_sa REDIR:
    rcmd = (sa_nracanaa redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    bha_nga;

  pak_sa PIPE:
    pcmd = (sa_nracanaa pipecmd *)cmd;
    nulterminate(pcmd->left);
    nulterminate(pcmd->right);
    bha_nga;

  pak_sa LIST:
    lcmd = (sa_nracanaa listcmd *)cmd;
    nulterminate(lcmd->left);
    nulterminate(lcmd->right);
    bha_nga;

  pak_sa BACK:
    bcmd = (sa_nracanaa backcmd *)cmd;
    nulterminate(bcmd->cmd);
    bha_nga;
  }
  ត្រឡប់ cmd;
}
