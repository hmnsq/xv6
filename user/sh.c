// Shell.

#ضمن "kernel/types.h"
#ضمن "user/user.h"
#ضمن "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

بنية cmd {
  صحيح type;
};

بنية execcmd {
  صحيح type;
  حرف *argv[MAXARGS];
  حرف *eargv[MAXARGS];
};

بنية redircmd {
  صحيح type;
  بنية cmd *cmd;
  حرف *file;
  حرف *efile;
  صحيح mode;
  صحيح fd;
};

بنية pipecmd {
  صحيح type;
  بنية cmd *left;
  بنية cmd *right;
};

بنية listcmd {
  صحيح type;
  بنية cmd *left;
  بنية cmd *right;
};

بنية backcmd {
  صحيح type;
  بنية cmd *cmd;
};

صحيح fork1(فراغ); // Fork but panics on failure.
فراغ panic(حرف *);
بنية cmd *parsecmd(حرف *);
فراغ runcmd(بنية cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
فراغ
runcmd(بنية cmd *cmd)
{
  صحيح p[2];
  بنية backcmd *bcmd;
  بنية execcmd *ecmd;
  بنية listcmd *lcmd;
  بنية pipecmd *pcmd;
  بنية redircmd *rcmd;

  ܐܢ (cmd == 0)
    exit(1);

  بدل (cmd->type) {
  افتراضي:
    panic("runcmd");

  حالة EXEC:
    ecmd = (بنية execcmd *)cmd;
    ܐܢ (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    اكسر;

  حالة REDIR:
    rcmd = (بنية redircmd *)cmd;
    close(rcmd->fd);
    ܐܢ (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    اكسر;

  حالة LIST:
    lcmd = (بنية listcmd *)cmd;
    ܐܢ (fork1() == 0)
      runcmd(lcmd->left);
    wait(0);
    runcmd(lcmd->right);
    اكسر;

  حالة PIPE:
    pcmd = (بنية pipecmd *)cmd;
    ܐܢ (pipe(p) < 0)
      panic("pipe");
    ܐܢ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    ܐܢ (fork1() == 0) {
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
    اكسر;

  حالة BACK:
    bcmd = (بنية backcmd *)cmd;
    ܐܢ (fork1() == 0)
      runcmd(bcmd->cmd);
    اكسر;
  }
  exit(0);
}

صحيح
getcmd(حرف *buf, صحيح nbuf)
{
  اكتب(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ܐܢ (buf[0] == 0) // EOF
    ܦܢܐ -1;
  ܦܢܐ 0;
}

صحيح
main(فراغ)
{
  ساكن حرف buf[100];
  صحيح fd;

  // Ensure that three file descriptors are open.
  ܟܡܐ ((fd = open("console", O_RDWR)) >= 0) {
    ܐܢ (fd >= 3) {
      close(fd);
      اكسر;
    }
  }

  // Read and run input commands.
  ܟܡܐ (getcmd(buf, حجم(buf)) >= 0) {
    حرف *cmd = buf;
    ܟܡܐ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ܐܢ (*cmd == '\n') // is a blank command
      استمر;
    ܐܢ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      ܐܢ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } ܐܚܪܢܐ {
      ܐܢ (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

فراغ
panic(حرف *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

صحيح
fork1(فراغ)
{
  صحيح pid;

  pid = fork();
  ܐܢ (pid == -1)
    panic("fork");
  ܦܢܐ pid;
}

//PAGEBREAK!
// Constructors

بنية cmd *
execcmd(فراغ)
{
  بنية execcmd *cmd;

  cmd = malloc(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = EXEC;
  ܦܢܐ (بنية cmd *)cmd;
}

بنية cmd *
redircmd(بنية cmd *subcmd, حرف *file, حرف *efile, صحيح mode, صحيح fd)
{
  بنية redircmd *cmd;

  cmd = malloc(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  ܦܢܐ (بنية cmd *)cmd;
}

بنية cmd *
pipecmd(بنية cmd *left, بنية cmd *right)
{
  بنية pipecmd *cmd;

  cmd = malloc(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  ܦܢܐ (بنية cmd *)cmd;
}

بنية cmd *
listcmd(بنية cmd *left, بنية cmd *right)
{
  بنية listcmd *cmd;

  cmd = malloc(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  ܦܢܐ (بنية cmd *)cmd;
}

بنية cmd *
backcmd(بنية cmd *subcmd)
{
  بنية backcmd *cmd;

  cmd = malloc(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  ܦܢܐ (بنية cmd *)cmd;
}
//PAGEBREAK!
// Parsing

حرف whitespace[] = " \t\r\n\v";
حرف symbols[] = "<|>&;()";

صحيح
gettoken(حرف **ps, حرف *es, حرف **q, حرف **eq)
{
  حرف *s;
  صحيح ret;

  s = *ps;
  ܟܡܐ (s < es && strchr(whitespace, *s))
    s++;
  ܐܢ (q)
    *q = s;
  ret = *s;
  بدل (*s) {
  حالة 0:
    اكسر;
  حالة '|':
  حالة '(':
  حالة ')':
  حالة ';':
  حالة '&':
  حالة '<':
    s++;
    اكسر;
  حالة '>':
    s++;
    ܐܢ (*s == '>') {
      ret = '+';
      s++;
    }
    اكسر;
  افتراضي:
    ret = 'a';
    ܟܡܐ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    اكسر;
  }
  ܐܢ (eq)
    *eq = s;

  ܟܡܐ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ܦܢܐ ret;
}

صحيح
peek(حرف **ps, حرف *es, حرف *toks)
{
  حرف *s;

  s = *ps;
  ܟܡܐ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ܦܢܐ *s && strchr(toks, *s);
}

بنية cmd *parseline(حرف **, حرف *);
بنية cmd *parsepipe(حرف **, حرف *);
بنية cmd *parseexec(حرف **, حرف *);
بنية cmd *nulterminate(بنية cmd *);

بنية cmd *
parsecmd(حرف *s)
{
  حرف *es;
  بنية cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  ܐܢ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ܦܢܐ cmd;
}

بنية cmd *
parseline(حرف **ps, حرف *es)
{
  بنية cmd *cmd;

  cmd = parsepipe(ps, es);
  ܟܡܐ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ܐܢ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ܦܢܐ cmd;
}

بنية cmd *
parsepipe(حرف **ps, حرف *es)
{
  بنية cmd *cmd;

  cmd = parseexec(ps, es);
  ܐܢ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ܦܢܐ cmd;
}

بنية cmd *
parseredirs(بنية cmd *cmd, حرف **ps, حرف *es)
{
  صحيح tok;
  حرف *q, *eq;

  ܟܡܐ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ܐܢ (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    بدل (tok) {
    حالة '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      اكسر;
    حالة '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      اكسر;
    حالة '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      اكسر;
    }
  }
  ܦܢܐ cmd;
}

بنية cmd *
parseblock(حرف **ps, حرف *es)
{
  بنية cmd *cmd;

  ܐܢ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ܐܢ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ܦܢܐ cmd;
}

بنية cmd *
parseexec(حرف **ps, حرف *es)
{
  حرف *q, *eq;
  صحيح tok, argc;
  بنية execcmd *cmd;
  بنية cmd *ret;

  ܐܢ (peek(ps, es, "("))
    ܦܢܐ parseblock(ps, es);

  ret = execcmd();
  cmd = (بنية execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ܟܡܐ (!peek(ps, es, "|)&;")) {
    ܐܢ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      اكسر;
    ܐܢ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ܐܢ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  ܦܢܐ ret;
}

// NUL-terminate all the counted strings.
بنية cmd *
nulterminate(بنية cmd *cmd)
{
  صحيح i;
  بنية backcmd *bcmd;
  بنية execcmd *ecmd;
  بنية listcmd *lcmd;
  بنية pipecmd *pcmd;
  بنية redircmd *rcmd;

  ܐܢ (cmd == 0)
    ܦܢܐ 0;

  بدل (cmd->type) {
  حالة EXEC:
    ecmd = (بنية execcmd *)cmd;
    لكل (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    اكسر;

  حالة REDIR:
    rcmd = (بنية redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    اكسر;

  حالة PIPE:
    pcmd = (بنية pipecmd *)cmd;
    nulterminate(pcmd->left);
    nulterminate(pcmd->right);
    اكسر;

  حالة LIST:
    lcmd = (بنية listcmd *)cmd;
    nulterminate(lcmd->left);
    nulterminate(lcmd->right);
    اكسر;

  حالة BACK:
    bcmd = (بنية backcmd *)cmd;
    nulterminate(bcmd->cmd);
    اكسر;
  }
  ܦܢܐ cmd;
}
