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
  puur_naa_nka type;
};

sa_nracanaa execcmd {
  puur_naa_nka type;
  var_na *argv[MAXARGS];
  var_na *eargv[MAXARGS];
};

sa_nracanaa redircmd {
  puur_naa_nka type;
  sa_nracanaa cmd *cmd;
  var_na *file;
  var_na *efile;
  puur_naa_nka mode;
  puur_naa_nka fd;
};

sa_nracanaa pipecmd {
  puur_naa_nka type;
  sa_nracanaa cmd *left;
  sa_nracanaa cmd *right;
};

sa_nracanaa listcmd {
  puur_naa_nka type;
  sa_nracanaa cmd *left;
  sa_nracanaa cmd *right;
};

sa_nracanaa backcmd {
  puur_naa_nka type;
  sa_nracanaa cmd *cmd;
};

puur_naa_nka fork1(void); // Fork but panics on failure.
void panic(var_na *);
sa_nracanaa cmd *parsecmd(var_na *);
void runcmd(sa_nracanaa cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
void
runcmd(sa_nracanaa cmd *cmd)
{
  puur_naa_nka p[2];
  sa_nracanaa backcmd *bcmd;
  sa_nracanaa execcmd *ecmd;
  sa_nracanaa listcmd *lcmd;
  sa_nracanaa pipecmd *pcmd;
  sa_nracanaa redircmd *rcmd;

  ᤀᤡ᤺ (cmd == 0)
    exit(1);

  switch (cmd->type) {
  default:
    panic("runcmd");

  pak_sa EXEC:
    ecmd = (sa_nracanaa execcmd *)cmd;
    ᤀᤡ᤺ (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    bha_nga;

  pak_sa REDIR:
    rcmd = (sa_nracanaa redircmd *)cmd;
    close(rcmd->fd);
    ᤀᤡ᤺ (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    bha_nga;

  pak_sa LIST:
    lcmd = (sa_nracanaa listcmd *)cmd;
    ᤀᤡ᤺ (fork1() == 0)
      runcmd(lcmd->left);
    wait(0);
    runcmd(lcmd->right);
    bha_nga;

  pak_sa PIPE:
    pcmd = (sa_nracanaa pipecmd *)cmd;
    ᤀᤡ᤺ (pipe(p) < 0)
      panic("pipe");
    ᤀᤡ᤺ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    ᤀᤡ᤺ (fork1() == 0) {
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
    ᤀᤡ᤺ (fork1() == 0)
      runcmd(bcmd->cmd);
    bha_nga;
  }
  exit(0);
}

puur_naa_nka
getcmd(var_na *buf, puur_naa_nka nbuf)
{
  write(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ᤀᤡ᤺ (buf[0] == 0) // EOF
    ᤀᤡᤴ -1;
  ᤀᤡᤴ 0;
}

puur_naa_nka
main(void)
{
  static var_na buf[100];
  puur_naa_nka fd;

  // Ensure that three file descriptors are open.
  ᤀᤠ᤺ ((fd = open("console", O_RDWR)) >= 0) {
    ᤀᤡ᤺ (fd >= 3) {
      close(fd);
      bha_nga;
    }
  }

  // Read and run input commands.
  ᤀᤠ᤺ (getcmd(buf, parimaa_na(buf)) >= 0) {
    var_na *cmd = buf;
    ᤀᤠ᤺ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ᤀᤡ᤺ (*cmd == '\n') // is a blank command
      continue;
    ᤀᤡ᤺ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      ᤀᤡ᤺ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } ᤀᤡ᤺ᤆᤡ {
      ᤀᤡ᤺ (fork1() == 0)
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

puur_naa_nka
fork1(void)
{
  puur_naa_nka pid;

  pid = fork();
  ᤀᤡ᤺ (pid == -1)
    panic("fork");
  ᤀᤡᤴ pid;
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
  ᤀᤡᤴ (sa_nracanaa cmd *)cmd;
}

sa_nracanaa cmd *
redircmd(sa_nracanaa cmd *subcmd, var_na *file, var_na *efile, puur_naa_nka mode, puur_naa_nka fd)
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
  ᤀᤡᤴ (sa_nracanaa cmd *)cmd;
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
  ᤀᤡᤴ (sa_nracanaa cmd *)cmd;
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
  ᤀᤡᤴ (sa_nracanaa cmd *)cmd;
}

sa_nracanaa cmd *
backcmd(sa_nracanaa cmd *subcmd)
{
  sa_nracanaa backcmd *cmd;

  cmd = malloc(parimaa_na(*cmd));
  memset(cmd, 0, parimaa_na(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  ᤀᤡᤴ (sa_nracanaa cmd *)cmd;
}
//PAGEBREAK!
// Parsing

var_na whitespace[] = " \t\r\n\v";
var_na symbols[] = "<|>&;()";

puur_naa_nka
gettoken(var_na **ps, var_na *es, var_na **q, var_na **eq)
{
  var_na *s;
  puur_naa_nka ret;

  s = *ps;
  ᤀᤠ᤺ (s < es && strchr(whitespace, *s))
    s++;
  ᤀᤡ᤺ (q)
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
    ᤀᤡ᤺ (*s == '>') {
      ret = '+';
      s++;
    }
    bha_nga;
  default:
    ret = 'a';
    ᤀᤠ᤺ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    bha_nga;
  }
  ᤀᤡ᤺ (eq)
    *eq = s;

  ᤀᤠ᤺ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ᤀᤡᤴ ret;
}

puur_naa_nka
peek(var_na **ps, var_na *es, var_na *toks)
{
  var_na *s;

  s = *ps;
  ᤀᤠ᤺ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ᤀᤡᤴ *s && strchr(toks, *s);
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
  ᤀᤡ᤺ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ᤀᤡᤴ cmd;
}

sa_nracanaa cmd *
parseline(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  cmd = parsepipe(ps, es);
  ᤀᤠ᤺ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ᤀᤡ᤺ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ᤀᤡᤴ cmd;
}

sa_nracanaa cmd *
parsepipe(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  cmd = parseexec(ps, es);
  ᤀᤡ᤺ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ᤀᤡᤴ cmd;
}

sa_nracanaa cmd *
parseredirs(sa_nracanaa cmd *cmd, var_na **ps, var_na *es)
{
  puur_naa_nka tok;
  var_na *q, *eq;

  ᤀᤠ᤺ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ᤀᤡ᤺ (gettoken(ps, es, &q, &eq) != 'a')
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
  ᤀᤡᤴ cmd;
}

sa_nracanaa cmd *
parseblock(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  ᤀᤡ᤺ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ᤀᤡ᤺ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ᤀᤡᤴ cmd;
}

sa_nracanaa cmd *
parseexec(var_na **ps, var_na *es)
{
  var_na *q, *eq;
  puur_naa_nka tok, argc;
  sa_nracanaa execcmd *cmd;
  sa_nracanaa cmd *ret;

  ᤀᤡ᤺ (peek(ps, es, "("))
    ᤀᤡᤴ parseblock(ps, es);

  ret = execcmd();
  cmd = (sa_nracanaa execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ᤀᤠ᤺ (!peek(ps, es, "|)&;")) {
    ᤀᤡ᤺ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      bha_nga;
    ᤀᤡ᤺ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ᤀᤡ᤺ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  ᤀᤡᤴ ret;
}

// NUL-terminate all the counted strings.
sa_nracanaa cmd *
nulterminate(sa_nracanaa cmd *cmd)
{
  puur_naa_nka i;
  sa_nracanaa backcmd *bcmd;
  sa_nracanaa execcmd *ecmd;
  sa_nracanaa listcmd *lcmd;
  sa_nracanaa pipecmd *pcmd;
  sa_nracanaa redircmd *rcmd;

  ᤀᤡ᤺ (cmd == 0)
    ᤀᤡᤴ 0;

  switch (cmd->type) {
  pak_sa EXEC:
    ecmd = (sa_nracanaa execcmd *)cmd;
    for (i = 0; ecmd->argv[i]; i++)
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
  ᤀᤡᤴ cmd;
}
