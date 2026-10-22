// Shell.

#شامل "kernel/types.h"
#شامل "user/user.h"
#شامل "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

ساختار cmd {
  صحیح type;
};

ساختار execcmd {
  صحیح type;
  نویسه *argv[MAXARGS];
  نویسه *eargv[MAXARGS];
};

ساختار redircmd {
  صحیح type;
  ساختار cmd *cmd;
  نویسه *file;
  نویسه *efile;
  صحیح mode;
  صحیح fd;
};

ساختار pipecmd {
  صحیح type;
  ساختار cmd *چپ;
  ساختار cmd *راست;
};

ساختار listcmd {
  صحیح type;
  ساختار cmd *چپ;
  ساختار cmd *راست;
};

ساختار backcmd {
  صحیح type;
  ساختار cmd *cmd;
};

صحیح fork1(تهی); // Fork but panics on failure.
تهی panic(نویسه *);
ساختار cmd *parsecmd(نویسه *);
تهی runcmd(ساختار cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
تهی
runcmd(ساختار cmd *cmd)
{
  صحیح p[2];
  ساختار backcmd *bcmd;
  ساختار execcmd *ecmd;
  ساختار listcmd *lcmd;
  ساختار pipecmd *pcmd;
  ساختار redircmd *rcmd;

  اگر (cmd == 0)
    exit(1);

  انتخاب (cmd->type) {
  پیشفرض:
    panic("runcmd");

  حالت EXEC:
    ecmd = (ساختار execcmd *)cmd;
    اگر (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    بشکن;

  حالت REDIR:
    rcmd = (ساختار redircmd *)cmd;
    close(rcmd->fd);
    اگر (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    بشکن;

  حالت LIST:
    lcmd = (ساختار listcmd *)cmd;
    اگر (fork1() == 0)
      runcmd(lcmd->چپ);
    wait(0);
    runcmd(lcmd->راست);
    بشکن;

  حالت PIPE:
    pcmd = (ساختار pipecmd *)cmd;
    اگر (pipe(p) < 0)
      panic("pipe");
    اگر (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->چپ);
    }
    اگر (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->راست);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    بشکن;

  حالت BACK:
    bcmd = (ساختار backcmd *)cmd;
    اگر (fork1() == 0)
      runcmd(bcmd->cmd);
    بشکن;
  }
  exit(0);
}

صحیح
getcmd(نویسه *buf, صحیح nbuf)
{
  اكتب(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  اگر (buf[0] == 0) // EOF
    بازگشت -1;
  بازگشت 0;
}

صحیح
اصلی(تهی)
{
  ایستا نویسه buf[100];
  صحیح fd;

  // Ensure that three file descriptors are open.
  وقتی ((fd = open("console", O_RDWR)) >= 0) {
    اگر (fd >= 3) {
      close(fd);
      بشکن;
    }
  }

  // Read and run input commands.
  وقتی (getcmd(buf, اندازه(buf)) >= 0) {
    نویسه *cmd = buf;
    وقتی (*cmd == ' ' || *cmd == '\t')
      cmd++;
    اگر (*cmd == '\n') // is a blank command
      ادامه;
    اگر (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      اگر (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } وگرنه {
      اگر (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

تهی
panic(نویسه *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

صحیح
fork1(تهی)
{
  صحیح pid;

  pid = fork();
  اگر (pid == -1)
    panic("fork");
  بازگشت pid;
}

//PAGEBREAK!
// Constructors

ساختار cmd *
execcmd(تهی)
{
  ساختار execcmd *cmd;

  cmd = malloc(اندازه(*cmd));
  memset(cmd, 0, اندازه(*cmd));
  cmd->type = EXEC;
  بازگشت (ساختار cmd *)cmd;
}

ساختار cmd *
redircmd(ساختار cmd *subcmd, نویسه *file, نویسه *efile, صحیح mode, صحیح fd)
{
  ساختار redircmd *cmd;

  cmd = malloc(اندازه(*cmd));
  memset(cmd, 0, اندازه(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  بازگشت (ساختار cmd *)cmd;
}

ساختار cmd *
pipecmd(ساختار cmd *چپ, ساختار cmd *راست)
{
  ساختار pipecmd *cmd;

  cmd = malloc(اندازه(*cmd));
  memset(cmd, 0, اندازه(*cmd));
  cmd->type = PIPE;
  cmd->چپ = چپ;
  cmd->راست = راست;
  بازگشت (ساختار cmd *)cmd;
}

ساختار cmd *
listcmd(ساختار cmd *چپ, ساختار cmd *راست)
{
  ساختار listcmd *cmd;

  cmd = malloc(اندازه(*cmd));
  memset(cmd, 0, اندازه(*cmd));
  cmd->type = LIST;
  cmd->چپ = چپ;
  cmd->راست = راست;
  بازگشت (ساختار cmd *)cmd;
}

ساختار cmd *
backcmd(ساختار cmd *subcmd)
{
  ساختار backcmd *cmd;

  cmd = malloc(اندازه(*cmd));
  memset(cmd, 0, اندازه(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  بازگشت (ساختار cmd *)cmd;
}
//PAGEBREAK!
// Parsing

نویسه whitespace[] = " \t\r\n\v";
نویسه symbols[] = "<|>&;()";

صحیح
gettoken(نویسه **ps, نویسه *es, نویسه **q, نویسه **eq)
{
  نویسه *s;
  صحیح ret;

  s = *ps;
  وقتی (s < es && strchr(whitespace, *s))
    s++;
  اگر (q)
    *q = s;
  ret = *s;
  انتخاب (*s) {
  حالت 0:
    بشکن;
  حالت '|':
  حالت '(':
  حالت ')':
  حالت ';':
  حالت '&':
  حالت '<':
    s++;
    بشکن;
  حالت '>':
    s++;
    اگر (*s == '>') {
      ret = '+';
      s++;
    }
    بشکن;
  پیشفرض:
    ret = 'a';
    وقتی (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    بشکن;
  }
  اگر (eq)
    *eq = s;

  وقتی (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  بازگشت ret;
}

صحیح
peek(نویسه **ps, نویسه *es, نویسه *toks)
{
  نویسه *s;

  s = *ps;
  وقتی (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  بازگشت *s && strchr(toks, *s);
}

ساختار cmd *parseline(نویسه **, نویسه *);
ساختار cmd *parsepipe(نویسه **, نویسه *);
ساختار cmd *parseexec(نویسه **, نویسه *);
ساختار cmd *nulterminate(ساختار cmd *);

ساختار cmd *
parsecmd(نویسه *s)
{
  نویسه *es;
  ساختار cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  اگر (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  بازگشت cmd;
}

ساختار cmd *
parseline(نویسه **ps, نویسه *es)
{
  ساختار cmd *cmd;

  cmd = parsepipe(ps, es);
  وقتی (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  اگر (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  بازگشت cmd;
}

ساختار cmd *
parsepipe(نویسه **ps, نویسه *es)
{
  ساختار cmd *cmd;

  cmd = parseexec(ps, es);
  اگر (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  بازگشت cmd;
}

ساختار cmd *
parseredirs(ساختار cmd *cmd, نویسه **ps, نویسه *es)
{
  صحیح tok;
  نویسه *q, *eq;

  وقتی (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    اگر (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    انتخاب (tok) {
    حالت '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      بشکن;
    حالت '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      بشکن;
    حالت '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      بشکن;
    }
  }
  بازگشت cmd;
}

ساختار cmd *
parseblock(نویسه **ps, نویسه *es)
{
  ساختار cmd *cmd;

  اگر (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  اگر (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  بازگشت cmd;
}

ساختار cmd *
parseexec(نویسه **ps, نویسه *es)
{
  نویسه *q, *eq;
  صحیح tok, argc;
  ساختار execcmd *cmd;
  ساختار cmd *ret;

  اگر (peek(ps, es, "("))
    بازگشت parseblock(ps, es);

  ret = execcmd();
  cmd = (ساختار execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  وقتی (!peek(ps, es, "|)&;")) {
    اگر ((tok = gettoken(ps, es, &q, &eq)) == 0)
      بشکن;
    اگر (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    اگر (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  بازگشت ret;
}

// NUL-terminate all the counted strings.
ساختار cmd *
nulterminate(ساختار cmd *cmd)
{
  صحیح i;
  ساختار backcmd *bcmd;
  ساختار execcmd *ecmd;
  ساختار listcmd *lcmd;
  ساختار pipecmd *pcmd;
  ساختار redircmd *rcmd;

  اگر (cmd == 0)
    بازگشت 0;

  انتخاب (cmd->type) {
  حالت EXEC:
    ecmd = (ساختار execcmd *)cmd;
    برای (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    بشکن;

  حالت REDIR:
    rcmd = (ساختار redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    بشکن;

  حالت PIPE:
    pcmd = (ساختار pipecmd *)cmd;
    nulterminate(pcmd->چپ);
    nulterminate(pcmd->راست);
    بشکن;

  حالت LIST:
    lcmd = (ساختار listcmd *)cmd;
    nulterminate(lcmd->چپ);
    nulterminate(lcmd->راست);
    بشکن;

  حالت BACK:
    bcmd = (ساختار backcmd *)cmd;
    nulterminate(bcmd->cmd);
    بشکن;
  }
  بازگشت cmd;
}
