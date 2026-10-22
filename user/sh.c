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

struct cmd {
  整数 type;
};

struct execcmd {
  整数 type;
  char *argv[MAXARGS];
  char *eargv[MAXARGS];
};

struct redircmd {
  整数 type;
  struct cmd *cmd;
  char *file;
  char *efile;
  整数 mode;
  整数 fd;
};

struct pipecmd {
  整数 type;
  struct cmd *left;
  struct cmd *right;
};

struct listcmd {
  整数 type;
  struct cmd *left;
  struct cmd *right;
};

struct backcmd {
  整数 type;
  struct cmd *cmd;
};

整数 fork1(void); // Fork but panics on failure.
void panic(char *);
struct cmd *parsecmd(char *);
void runcmd(struct cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  整数 p[2];
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  もし (cmd == 0)
    exit(1);

  switch (cmd->type) {
  default:
    panic("runcmd");

  case EXEC:
    ecmd = (struct execcmd *)cmd;
    もし (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    break;

  case REDIR:
    rcmd = (struct redircmd *)cmd;
    close(rcmd->fd);
    もし (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd *)cmd;
    もし (fork1() == 0)
      runcmd(lcmd->left);
    wait(0);
    runcmd(lcmd->right);
    break;

  case PIPE:
    pcmd = (struct pipecmd *)cmd;
    もし (pipe(p) < 0)
      panic("pipe");
    もし (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    もし (fork1() == 0) {
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
    break;

  case BACK:
    bcmd = (struct backcmd *)cmd;
    もし (fork1() == 0)
      runcmd(bcmd->cmd);
    break;
  }
  exit(0);
}

整数
getcmd(char *buf, 整数 nbuf)
{
  write(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  もし (buf[0] == 0) // EOF
    戻る -1;
  戻る 0;
}

整数
メインページ(void)
{
  static char buf[100];
  整数 fd;

  // Ensure that three file descriptors are open.
  ながら ((fd = open("console", O_RDWR)) >= 0) {
    もし (fd >= 3) {
      close(fd);
      break;
    }
  }

  // Read and run input commands.
  ながら (getcmd(buf, sizeof(buf)) >= 0) {
    char *cmd = buf;
    ながら (*cmd == ' ' || *cmd == '\t')
      cmd++;
    もし (*cmd == '\n') // is a blank command
      continue;
    もし (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      もし (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } 否则 {
      もし (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

整数
fork1(void)
{
  整数 pid;

  pid = fork();
  もし (pid == -1)
    panic("fork");
  戻る pid;
}

//PAGEBREAK!
// Constructors

struct cmd *
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = EXEC;
  戻る (struct cmd *)cmd;
}

struct cmd *
redircmd(struct cmd *subcmd, char *file, char *efile, 整数 mode, 整数 fd)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  戻る (struct cmd *)cmd;
}

struct cmd *
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  戻る (struct cmd *)cmd;
}

struct cmd *
listcmd(struct cmd *left, struct cmd *right)
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  戻る (struct cmd *)cmd;
}

struct cmd *
backcmd(struct cmd *subcmd)
{
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  戻る (struct cmd *)cmd;
}
//PAGEBREAK!
// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

整数
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  整数 ret;

  s = *ps;
  ながら (s < es && strchr(whitespace, *s))
    s++;
  もし (q)
    *q = s;
  ret = *s;
  switch (*s) {
  case 0:
    break;
  case '|':
  case '(':
  case ')':
  case ';':
  case '&':
  case '<':
    s++;
    break;
  case '>':
    s++;
    もし (*s == '>') {
      ret = '+';
      s++;
    }
    break;
  default:
    ret = 'a';
    ながら (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  もし (eq)
    *eq = s;

  ながら (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  戻る ret;
}

整数
peek(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  ながら (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  戻る *s && strchr(toks, *s);
}

struct cmd *parseline(char **, char *);
struct cmd *parsepipe(char **, char *);
struct cmd *parseexec(char **, char *);
struct cmd *nulterminate(struct cmd *);

struct cmd *
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  もし (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  戻る cmd;
}

struct cmd *
parseline(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parsepipe(ps, es);
  ながら (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  もし (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  戻る cmd;
}

struct cmd *
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  もし (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  戻る cmd;
}

struct cmd *
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  整数 tok;
  char *q, *eq;

  ながら (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    もし (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    switch (tok) {
    case '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      break;
    case '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      break;
    }
  }
  戻る cmd;
}

struct cmd *
parseblock(char **ps, char *es)
{
  struct cmd *cmd;

  もし (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  もし (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  戻る cmd;
}

struct cmd *
parseexec(char **ps, char *es)
{
  char *q, *eq;
  整数 tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  もし (peek(ps, es, "("))
    戻る parseblock(ps, es);

  ret = execcmd();
  cmd = (struct execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ながら (!peek(ps, es, "|)&;")) {
    もし ((tok = gettoken(ps, es, &q, &eq)) == 0)
      break;
    もし (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    もし (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  戻る ret;
}

// NUL-terminate all the counted strings.
struct cmd *
nulterminate(struct cmd *cmd)
{
  整数 i;
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  もし (cmd == 0)
    戻る 0;

  switch (cmd->type) {
  case EXEC:
    ecmd = (struct execcmd *)cmd;
    ために (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    break;

  case REDIR:
    rcmd = (struct redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    break;

  case PIPE:
    pcmd = (struct pipecmd *)cmd;
    nulterminate(pcmd->left);
    nulterminate(pcmd->right);
    break;

  case LIST:
    lcmd = (struct listcmd *)cmd;
    nulterminate(lcmd->left);
    nulterminate(lcmd->right);
    break;

  case BACK:
    bcmd = (struct backcmd *)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  戻る cmd;
}
