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
  ຈຳນວນເຕັມ type;
};

sa_nracanaa execcmd {
  ຈຳນວນເຕັມ type;
  var_na *argv[MAXARGS];
  var_na *eargv[MAXARGS];
};

sa_nracanaa redircmd {
  ຈຳນວນເຕັມ type;
  sa_nracanaa cmd *cmd;
  var_na *file;
  var_na *efile;
  ຈຳນວນເຕັມ mode;
  ຈຳນວນເຕັມ fd;
};

sa_nracanaa pipecmd {
  ຈຳນວນເຕັມ type;
  sa_nracanaa cmd *left;
  sa_nracanaa cmd *right;
};

sa_nracanaa listcmd {
  ຈຳນວນເຕັມ type;
  sa_nracanaa cmd *left;
  sa_nracanaa cmd *right;
};

sa_nracanaa backcmd {
  ຈຳນວນເຕັມ type;
  sa_nracanaa cmd *cmd;
};

ຈຳນວນເຕັມ fork1(void); // Fork but panics on failure.
void panic(var_na *);
sa_nracanaa cmd *parsecmd(var_na *);
void runcmd(sa_nracanaa cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
void
runcmd(sa_nracanaa cmd *cmd)
{
  ຈຳນວນເຕັມ p[2];
  sa_nracanaa backcmd *bcmd;
  sa_nracanaa execcmd *ecmd;
  sa_nracanaa listcmd *lcmd;
  sa_nracanaa pipecmd *pcmd;
  sa_nracanaa redircmd *rcmd;

  ຖ້າ (cmd == 0)
    exit(1);

  switch (cmd->type) {
  default:
    panic("runcmd");

  pak_sa EXEC:
    ecmd = (sa_nracanaa execcmd *)cmd;
    ຖ້າ (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    bha_nga;

  pak_sa REDIR:
    rcmd = (sa_nracanaa redircmd *)cmd;
    close(rcmd->fd);
    ຖ້າ (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    bha_nga;

  pak_sa LIST:
    lcmd = (sa_nracanaa listcmd *)cmd;
    ຖ້າ (fork1() == 0)
      runcmd(lcmd->left);
    wait(0);
    runcmd(lcmd->right);
    bha_nga;

  pak_sa PIPE:
    pcmd = (sa_nracanaa pipecmd *)cmd;
    ຖ້າ (pipe(p) < 0)
      panic("pipe");
    ຖ້າ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    ຖ້າ (fork1() == 0) {
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
    ຖ້າ (fork1() == 0)
      runcmd(bcmd->cmd);
    bha_nga;
  }
  exit(0);
}

ຈຳນວນເຕັມ
getcmd(var_na *buf, ຈຳນວນເຕັມ nbuf)
{
  write(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ຖ້າ (buf[0] == 0) // EOF
    ສົ່ງຄືນ -1;
  ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ
main(void)
{
  static var_na buf[100];
  ຈຳນວນເຕັມ fd;

  // Ensure that three file descriptors are open.
  ໃນຂະນະທີ່ ((fd = open("console", O_RDWR)) >= 0) {
    ຖ້າ (fd >= 3) {
      close(fd);
      bha_nga;
    }
  }

  // Read and run input commands.
  ໃນຂະນະທີ່ (getcmd(buf, parimaa_na(buf)) >= 0) {
    var_na *cmd = buf;
    ໃນຂະນະທີ່ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ຖ້າ (*cmd == '\n') // is a blank command
      continue;
    ຖ້າ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      ຖ້າ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } ບໍ່ດັ່ງນັ້ນ {
      ຖ້າ (fork1() == 0)
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

ຈຳນວນເຕັມ
fork1(void)
{
  ຈຳນວນເຕັມ pid;

  pid = fork();
  ຖ້າ (pid == -1)
    panic("fork");
  ສົ່ງຄືນ pid;
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
  ສົ່ງຄືນ (sa_nracanaa cmd *)cmd;
}

sa_nracanaa cmd *
redircmd(sa_nracanaa cmd *subcmd, var_na *file, var_na *efile, ຈຳນວນເຕັມ mode, ຈຳນວນເຕັມ fd)
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
  ສົ່ງຄືນ (sa_nracanaa cmd *)cmd;
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
  ສົ່ງຄືນ (sa_nracanaa cmd *)cmd;
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
  ສົ່ງຄືນ (sa_nracanaa cmd *)cmd;
}

sa_nracanaa cmd *
backcmd(sa_nracanaa cmd *subcmd)
{
  sa_nracanaa backcmd *cmd;

  cmd = malloc(parimaa_na(*cmd));
  memset(cmd, 0, parimaa_na(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  ສົ່ງຄືນ (sa_nracanaa cmd *)cmd;
}
//PAGEBREAK!
// Parsing

var_na whitespace[] = " \t\r\n\v";
var_na symbols[] = "<|>&;()";

ຈຳນວນເຕັມ
gettoken(var_na **ps, var_na *es, var_na **q, var_na **eq)
{
  var_na *s;
  ຈຳນວນເຕັມ ret;

  s = *ps;
  ໃນຂະນະທີ່ (s < es && strchr(whitespace, *s))
    s++;
  ຖ້າ (q)
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
    ຖ້າ (*s == '>') {
      ret = '+';
      s++;
    }
    bha_nga;
  default:
    ret = 'a';
    ໃນຂະນະທີ່ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    bha_nga;
  }
  ຖ້າ (eq)
    *eq = s;

  ໃນຂະນະທີ່ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ສົ່ງຄືນ ret;
}

ຈຳນວນເຕັມ
peek(var_na **ps, var_na *es, var_na *toks)
{
  var_na *s;

  s = *ps;
  ໃນຂະນະທີ່ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ສົ່ງຄືນ *s && strchr(toks, *s);
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
  ຖ້າ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ສົ່ງຄືນ cmd;
}

sa_nracanaa cmd *
parseline(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  cmd = parsepipe(ps, es);
  ໃນຂະນະທີ່ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ຖ້າ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ສົ່ງຄືນ cmd;
}

sa_nracanaa cmd *
parsepipe(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  cmd = parseexec(ps, es);
  ຖ້າ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ສົ່ງຄືນ cmd;
}

sa_nracanaa cmd *
parseredirs(sa_nracanaa cmd *cmd, var_na **ps, var_na *es)
{
  ຈຳນວນເຕັມ tok;
  var_na *q, *eq;

  ໃນຂະນະທີ່ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ຖ້າ (gettoken(ps, es, &q, &eq) != 'a')
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
  ສົ່ງຄືນ cmd;
}

sa_nracanaa cmd *
parseblock(var_na **ps, var_na *es)
{
  sa_nracanaa cmd *cmd;

  ຖ້າ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ຖ້າ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ສົ່ງຄືນ cmd;
}

sa_nracanaa cmd *
parseexec(var_na **ps, var_na *es)
{
  var_na *q, *eq;
  ຈຳນວນເຕັມ tok, argc;
  sa_nracanaa execcmd *cmd;
  sa_nracanaa cmd *ret;

  ຖ້າ (peek(ps, es, "("))
    ສົ່ງຄືນ parseblock(ps, es);

  ret = execcmd();
  cmd = (sa_nracanaa execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ໃນຂະນະທີ່ (!peek(ps, es, "|)&;")) {
    ຖ້າ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      bha_nga;
    ຖ້າ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ຖ້າ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  ສົ່ງຄືນ ret;
}

// NUL-terminate all the counted strings.
sa_nracanaa cmd *
nulterminate(sa_nracanaa cmd *cmd)
{
  ຈຳນວນເຕັມ i;
  sa_nracanaa backcmd *bcmd;
  sa_nracanaa execcmd *ecmd;
  sa_nracanaa listcmd *lcmd;
  sa_nracanaa pipecmd *pcmd;
  sa_nracanaa redircmd *rcmd;

  ຖ້າ (cmd == 0)
    ສົ່ງຄືນ 0;

  switch (cmd->type) {
  pak_sa EXEC:
    ecmd = (sa_nracanaa execcmd *)cmd;
    ສຳລັບ (i = 0; ecmd->argv[i]; i++)
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
  ສົ່ງຄືນ cmd;
}
