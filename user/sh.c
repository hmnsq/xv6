// Shell.

#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

সংরচনা cmd {
  পূর্ণাঙ্ক type;
};

সংরচনা execcmd {
  পূর্ণাঙ্ক type;
  বর্ণ *argv[MAXARGS];
  বর্ণ *eargv[MAXARGS];
};

সংরচনা redircmd {
  পূর্ণাঙ্ক type;
  সংরচনা cmd *cmd;
  বর্ণ *file;
  বর্ণ *efile;
  পূর্ণাঙ্ক mode;
  পূর্ণাঙ্ক fd;
};

সংরচনা pipecmd {
  পূর্ণাঙ্ক type;
  সংরচনা cmd *বাম;
  সংরচনা cmd *ডান;
};

সংরচনা listcmd {
  পূর্ণাঙ্ক type;
  সংরচনা cmd *বাম;
  সংরচনা cmd *ডান;
};

সংরচনা backcmd {
  পূর্ণাঙ্ক type;
  সংরচনা cmd *cmd;
};

পূর্ণাঙ্ক fork1(শূন্য); // Fork but panics on failure.
শূন্য panic(বর্ণ *);
সংরচনা cmd *parsecmd(বর্ণ *);
শূন্য runcmd(সংরচনা cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
শূন্য
runcmd(সংরচনা cmd *cmd)
{
  পূর্ণাঙ্ক p[2];
  সংরচনা backcmd *bcmd;
  সংরচনা execcmd *ecmd;
  সংরচনা listcmd *lcmd;
  সংরচনা pipecmd *pcmd;
  সংরচনা redircmd *rcmd;

  যদি (cmd == 0)
    exit(1);

  বিকল্প (cmd->type) {
  স্বভাব:
    panic("runcmd");

  পক্ষ EXEC:
    ecmd = (সংরচনা execcmd *)cmd;
    যদি (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    ভঙ্গ;

  পক্ষ REDIR:
    rcmd = (সংরচনা redircmd *)cmd;
    close(rcmd->fd);
    যদি (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    ভঙ্গ;

  পক্ষ LIST:
    lcmd = (সংরচনা listcmd *)cmd;
    যদি (fork1() == 0)
      runcmd(lcmd->বাম);
    wait(0);
    runcmd(lcmd->ডান);
    ভঙ্গ;

  পক্ষ PIPE:
    pcmd = (সংরচনা pipecmd *)cmd;
    যদি (pipe(p) < 0)
      panic("pipe");
    যদি (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->বাম);
    }
    যদি (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ডান);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    ভঙ্গ;

  পক্ষ BACK:
    bcmd = (সংরচনা backcmd *)cmd;
    যদি (fork1() == 0)
      runcmd(bcmd->cmd);
    ভঙ্গ;
  }
  exit(0);
}

পূর্ণাঙ্ক
getcmd(বর্ণ *buf, পূর্ণাঙ্ক nbuf)
{
  লিখ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  যদি (buf[0] == 0) // EOF
    প্রত্যাগম -1;
  প্রত্যাগম 0;
}

পূর্ণাঙ্ক
main(শূন্য)
{
  স্থির বর্ণ buf[100];
  পূর্ণাঙ্ক fd;

  // Ensure that three file descriptors are open.
  যাবত্ ((fd = open("console", O_RDWR)) >= 0) {
    যদি (fd >= 3) {
      close(fd);
      ভঙ্গ;
    }
  }

  // Read and run input commands.
  যাবত্ (getcmd(buf, পরিমাণ(buf)) >= 0) {
    বর্ণ *cmd = buf;
    যাবত্ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    যদি (*cmd == '\n') // is a blank command
      অনুবর্ত;
    যদি (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      যদি (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } অন্যথা {
      যদি (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

শূন্য
panic(বর্ণ *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

পূর্ণাঙ্ক
fork1(শূন্য)
{
  পূর্ণাঙ্ক pid;

  pid = fork();
  যদি (pid == -1)
    panic("fork");
  প্রত্যাগম pid;
}

//PAGEBREAK!
// Constructors

সংরচনা cmd *
execcmd(শূন্য)
{
  সংরচনা execcmd *cmd;

  cmd = malloc(পরিমাণ(*cmd));
  memset(cmd, 0, পরিমাণ(*cmd));
  cmd->type = EXEC;
  প্রত্যাগম (সংরচনা cmd *)cmd;
}

সংরচনা cmd *
redircmd(সংরচনা cmd *subcmd, বর্ণ *file, বর্ণ *efile, পূর্ণাঙ্ক mode, পূর্ণাঙ্ক fd)
{
  সংরচনা redircmd *cmd;

  cmd = malloc(পরিমাণ(*cmd));
  memset(cmd, 0, পরিমাণ(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  প্রত্যাগম (সংরচনা cmd *)cmd;
}

সংরচনা cmd *
pipecmd(সংরচনা cmd *বাম, সংরচনা cmd *ডান)
{
  সংরচনা pipecmd *cmd;

  cmd = malloc(পরিমাণ(*cmd));
  memset(cmd, 0, পরিমাণ(*cmd));
  cmd->type = PIPE;
  cmd->বাম = বাম;
  cmd->ডান = ডান;
  প্রত্যাগম (সংরচনা cmd *)cmd;
}

সংরচনা cmd *
listcmd(সংরচনা cmd *বাম, সংরচনা cmd *ডান)
{
  সংরচনা listcmd *cmd;

  cmd = malloc(পরিমাণ(*cmd));
  memset(cmd, 0, পরিমাণ(*cmd));
  cmd->type = LIST;
  cmd->বাম = বাম;
  cmd->ডান = ডান;
  প্রত্যাগম (সংরচনা cmd *)cmd;
}

সংরচনা cmd *
backcmd(সংরচনা cmd *subcmd)
{
  সংরচনা backcmd *cmd;

  cmd = malloc(পরিমাণ(*cmd));
  memset(cmd, 0, পরিমাণ(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  প্রত্যাগম (সংরচনা cmd *)cmd;
}
//PAGEBREAK!
// Parsing

বর্ণ whitespace[] = " \t\r\n\v";
বর্ণ symbols[] = "<|>&;()";

পূর্ণাঙ্ক
gettoken(বর্ণ **ps, বর্ণ *es, বর্ণ **q, বর্ণ **eq)
{
  বর্ণ *s;
  পূর্ণাঙ্ক ret;

  s = *ps;
  যাবত্ (s < es && strchr(whitespace, *s))
    s++;
  যদি (q)
    *q = s;
  ret = *s;
  বিকল্প (*s) {
  পক্ষ 0:
    ভঙ্গ;
  পক্ষ '|':
  পক্ষ '(':
  পক্ষ ')':
  পক্ষ ';':
  পক্ষ '&':
  পক্ষ '<':
    s++;
    ভঙ্গ;
  পক্ষ '>':
    s++;
    যদি (*s == '>') {
      ret = '+';
      s++;
    }
    ভঙ্গ;
  স্বভাব:
    ret = 'a';
    যাবত্ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    ভঙ্গ;
  }
  যদি (eq)
    *eq = s;

  যাবত্ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  প্রত্যাগম ret;
}

পূর্ণাঙ্ক
peek(বর্ণ **ps, বর্ণ *es, বর্ণ *toks)
{
  বর্ণ *s;

  s = *ps;
  যাবত্ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  প্রত্যাগম *s && strchr(toks, *s);
}

সংরচনা cmd *parseline(বর্ণ **, বর্ণ *);
সংরচনা cmd *parsepipe(বর্ণ **, বর্ণ *);
সংরচনা cmd *parseexec(বর্ণ **, বর্ণ *);
সংরচনা cmd *nulterminate(সংরচনা cmd *);

সংরচনা cmd *
parsecmd(বর্ণ *s)
{
  বর্ণ *es;
  সংরচনা cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  যদি (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  প্রত্যাগম cmd;
}

সংরচনা cmd *
parseline(বর্ণ **ps, বর্ণ *es)
{
  সংরচনা cmd *cmd;

  cmd = parsepipe(ps, es);
  যাবত্ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  যদি (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  প্রত্যাগম cmd;
}

সংরচনা cmd *
parsepipe(বর্ণ **ps, বর্ণ *es)
{
  সংরচনা cmd *cmd;

  cmd = parseexec(ps, es);
  যদি (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  প্রত্যাগম cmd;
}

সংরচনা cmd *
parseredirs(সংরচনা cmd *cmd, বর্ণ **ps, বর্ণ *es)
{
  পূর্ণাঙ্ক tok;
  বর্ণ *q, *eq;

  যাবত্ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    যদি (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    বিকল্প (tok) {
    পক্ষ '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      ভঙ্গ;
    পক্ষ '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      ভঙ্গ;
    পক্ষ '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      ভঙ্গ;
    }
  }
  প্রত্যাগম cmd;
}

সংরচনা cmd *
parseblock(বর্ণ **ps, বর্ণ *es)
{
  সংরচনা cmd *cmd;

  যদি (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  যদি (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  প্রত্যাগম cmd;
}

সংরচনা cmd *
parseexec(বর্ণ **ps, বর্ণ *es)
{
  বর্ণ *q, *eq;
  পূর্ণাঙ্ক tok, argc;
  সংরচনা execcmd *cmd;
  সংরচনা cmd *ret;

  যদি (peek(ps, es, "("))
    প্রত্যাগম parseblock(ps, es);

  ret = execcmd();
  cmd = (সংরচনা execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  যাবত্ (!peek(ps, es, "|)&;")) {
    যদি ((tok = gettoken(ps, es, &q, &eq)) == 0)
      ভঙ্গ;
    যদি (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    যদি (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  প্রত্যাগম ret;
}

// NUL-terminate all the counted strings.
সংরচনা cmd *
nulterminate(সংরচনা cmd *cmd)
{
  পূর্ণাঙ্ক i;
  সংরচনা backcmd *bcmd;
  সংরচনা execcmd *ecmd;
  সংরচনা listcmd *lcmd;
  সংরচনা pipecmd *pcmd;
  সংরচনা redircmd *rcmd;

  যদি (cmd == 0)
    প্রত্যাগম 0;

  বিকল্প (cmd->type) {
  পক্ষ EXEC:
    ecmd = (সংরচনা execcmd *)cmd;
    প্রতি (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    ভঙ্গ;

  পক্ষ REDIR:
    rcmd = (সংরচনা redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    ভঙ্গ;

  পক্ষ PIPE:
    pcmd = (সংরচনা pipecmd *)cmd;
    nulterminate(pcmd->বাম);
    nulterminate(pcmd->ডান);
    ভঙ্গ;

  পক্ষ LIST:
    lcmd = (সংরচনা listcmd *)cmd;
    nulterminate(lcmd->বাম);
    nulterminate(lcmd->ডান);
    ভঙ্গ;

  পক্ষ BACK:
    bcmd = (সংরচনা backcmd *)cmd;
    nulterminate(bcmd->cmd);
    ভঙ্গ;
  }
  প্রত্যাগম cmd;
}
