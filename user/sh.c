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

কাঠামো cmd {
  পূর্ণসংখ্যা type;
};

কাঠামো execcmd {
  পূর্ণসংখ্যা type;
  অক্ষর *argv[MAXARGS];
  অক্ষর *eargv[MAXARGS];
};

কাঠামো redircmd {
  পূর্ণসংখ্যা type;
  কাঠামো cmd *cmd;
  অক্ষর *file;
  অক্ষর *efile;
  পূর্ণসংখ্যা mode;
  পূর্ণসংখ্যা fd;
};

কাঠামো pipecmd {
  পূর্ণসংখ্যা type;
  কাঠামো cmd *বাম;
  কাঠামো cmd *ডান;
};

কাঠামো listcmd {
  পূর্ণসংখ্যা type;
  কাঠামো cmd *বাম;
  কাঠামো cmd *ডান;
};

কাঠামো backcmd {
  পূর্ণসংখ্যা type;
  কাঠামো cmd *cmd;
};

পূর্ণসংখ্যা fork1(শূন্য); // Fork but panics on failure.
শূন্য panic(অক্ষর *);
কাঠামো cmd *parsecmd(অক্ষর *);
শূন্য runcmd(কাঠামো cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
শূন্য
runcmd(কাঠামো cmd *cmd)
{
  পূর্ণসংখ্যা p[2];
  কাঠামো backcmd *bcmd;
  কাঠামো execcmd *ecmd;
  কাঠামো listcmd *lcmd;
  কাঠামো pipecmd *pcmd;
  কাঠামো redircmd *rcmd;

  যদি (cmd == 0)
    exit(1);

  নির্বাচন (cmd->type) {
  পূর্বনির্ধারিত:
    panic("runcmd");

  ক্ষেত্র EXEC:
    ecmd = (কাঠামো execcmd *)cmd;
    যদি (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    বিরতি;

  ক্ষেত্র REDIR:
    rcmd = (কাঠামো redircmd *)cmd;
    close(rcmd->fd);
    যদি (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    বিরতি;

  ক্ষেত্র LIST:
    lcmd = (কাঠামো listcmd *)cmd;
    যদি (fork1() == 0)
      runcmd(lcmd->বাম);
    wait(0);
    runcmd(lcmd->ডান);
    বিরতি;

  ক্ষেত্র PIPE:
    pcmd = (কাঠামো pipecmd *)cmd;
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
    বিরতি;

  ক্ষেত্র BACK:
    bcmd = (কাঠামো backcmd *)cmd;
    যদি (fork1() == 0)
      runcmd(bcmd->cmd);
    বিরতি;
  }
  exit(0);
}

পূর্ণসংখ্যা
getcmd(অক্ষর *buf, পূর্ণসংখ্যা nbuf)
{
  লিখ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  যদি (buf[0] == 0) // EOF
    ফেরত -1;
  ফেরত 0;
}

পূর্ণসংখ্যা
প্রধান(শূন্য)
{
  স্থির অক্ষর buf[100];
  পূর্ণসংখ্যা fd;

  // Ensure that three file descriptors are open.
  যখন ((fd = open("console", O_RDWR)) >= 0) {
    যদি (fd >= 3) {
      close(fd);
      বিরতি;
    }
  }

  // Read and run input commands.
  যখন (getcmd(buf, আকার(buf)) >= 0) {
    অক্ষর *cmd = buf;
    যখন (*cmd == ' ' || *cmd == '\t')
      cmd++;
    যদি (*cmd == '\n') // is a blank command
      চালু;
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
panic(অক্ষর *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

পূর্ণসংখ্যা
fork1(শূন্য)
{
  পূর্ণসংখ্যা pid;

  pid = fork();
  যদি (pid == -1)
    panic("fork");
  ফেরত pid;
}

//PAGEBREAK!
// Constructors

কাঠামো cmd *
execcmd(শূন্য)
{
  কাঠামো execcmd *cmd;

  cmd = malloc(আকার(*cmd));
  memset(cmd, 0, আকার(*cmd));
  cmd->type = EXEC;
  ফেরত (কাঠামো cmd *)cmd;
}

কাঠামো cmd *
redircmd(কাঠামো cmd *subcmd, অক্ষর *file, অক্ষর *efile, পূর্ণসংখ্যা mode, পূর্ণসংখ্যা fd)
{
  কাঠামো redircmd *cmd;

  cmd = malloc(আকার(*cmd));
  memset(cmd, 0, আকার(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  ফেরত (কাঠামো cmd *)cmd;
}

কাঠামো cmd *
pipecmd(কাঠামো cmd *বাম, কাঠামো cmd *ডান)
{
  কাঠামো pipecmd *cmd;

  cmd = malloc(আকার(*cmd));
  memset(cmd, 0, আকার(*cmd));
  cmd->type = PIPE;
  cmd->বাম = বাম;
  cmd->ডান = ডান;
  ফেরত (কাঠামো cmd *)cmd;
}

কাঠামো cmd *
listcmd(কাঠামো cmd *বাম, কাঠামো cmd *ডান)
{
  কাঠামো listcmd *cmd;

  cmd = malloc(আকার(*cmd));
  memset(cmd, 0, আকার(*cmd));
  cmd->type = LIST;
  cmd->বাম = বাম;
  cmd->ডান = ডান;
  ফেরত (কাঠামো cmd *)cmd;
}

কাঠামো cmd *
backcmd(কাঠামো cmd *subcmd)
{
  কাঠামো backcmd *cmd;

  cmd = malloc(আকার(*cmd));
  memset(cmd, 0, আকার(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  ফেরত (কাঠামো cmd *)cmd;
}
//PAGEBREAK!
// Parsing

অক্ষর whitespace[] = " \t\r\n\v";
অক্ষর symbols[] = "<|>&;()";

পূর্ণসংখ্যা
gettoken(অক্ষর **ps, অক্ষর *es, অক্ষর **q, অক্ষর **eq)
{
  অক্ষর *s;
  পূর্ণসংখ্যা ret;

  s = *ps;
  যখন (s < es && strchr(whitespace, *s))
    s++;
  যদি (q)
    *q = s;
  ret = *s;
  নির্বাচন (*s) {
  ক্ষেত্র 0:
    বিরতি;
  ক্ষেত্র '|':
  ক্ষেত্র '(':
  ক্ষেত্র ')':
  ক্ষেত্র ';':
  ক্ষেত্র '&':
  ক্ষেত্র '<':
    s++;
    বিরতি;
  ক্ষেত্র '>':
    s++;
    যদি (*s == '>') {
      ret = '+';
      s++;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    ret = 'a';
    যখন (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    বিরতি;
  }
  যদি (eq)
    *eq = s;

  যখন (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ফেরত ret;
}

পূর্ণসংখ্যা
peek(অক্ষর **ps, অক্ষর *es, অক্ষর *toks)
{
  অক্ষর *s;

  s = *ps;
  যখন (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ফেরত *s && strchr(toks, *s);
}

কাঠামো cmd *parseline(অক্ষর **, অক্ষর *);
কাঠামো cmd *parsepipe(অক্ষর **, অক্ষর *);
কাঠামো cmd *parseexec(অক্ষর **, অক্ষর *);
কাঠামো cmd *nulterminate(কাঠামো cmd *);

কাঠামো cmd *
parsecmd(অক্ষর *s)
{
  অক্ষর *es;
  কাঠামো cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  যদি (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ফেরত cmd;
}

কাঠামো cmd *
parseline(অক্ষর **ps, অক্ষর *es)
{
  কাঠামো cmd *cmd;

  cmd = parsepipe(ps, es);
  যখন (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  যদি (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ফেরত cmd;
}

কাঠামো cmd *
parsepipe(অক্ষর **ps, অক্ষর *es)
{
  কাঠামো cmd *cmd;

  cmd = parseexec(ps, es);
  যদি (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ফেরত cmd;
}

কাঠামো cmd *
parseredirs(কাঠামো cmd *cmd, অক্ষর **ps, অক্ষর *es)
{
  পূর্ণসংখ্যা tok;
  অক্ষর *q, *eq;

  যখন (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    যদি (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    নির্বাচন (tok) {
    ক্ষেত্র '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      বিরতি;
    ক্ষেত্র '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      বিরতি;
    ক্ষেত্র '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      বিরতি;
    }
  }
  ফেরত cmd;
}

কাঠামো cmd *
parseblock(অক্ষর **ps, অক্ষর *es)
{
  কাঠামো cmd *cmd;

  যদি (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  যদি (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ফেরত cmd;
}

কাঠামো cmd *
parseexec(অক্ষর **ps, অক্ষর *es)
{
  অক্ষর *q, *eq;
  পূর্ণসংখ্যা tok, argc;
  কাঠামো execcmd *cmd;
  কাঠামো cmd *ret;

  যদি (peek(ps, es, "("))
    ফেরত parseblock(ps, es);

  ret = execcmd();
  cmd = (কাঠামো execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  যখন (!peek(ps, es, "|)&;")) {
    যদি ((tok = gettoken(ps, es, &q, &eq)) == 0)
      বিরতি;
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
  ফেরত ret;
}

// NUL-terminate all the counted strings.
কাঠামো cmd *
nulterminate(কাঠামো cmd *cmd)
{
  পূর্ণসংখ্যা i;
  কাঠামো backcmd *bcmd;
  কাঠামো execcmd *ecmd;
  কাঠামো listcmd *lcmd;
  কাঠামো pipecmd *pcmd;
  কাঠামো redircmd *rcmd;

  যদি (cmd == 0)
    ফেরত 0;

  নির্বাচন (cmd->type) {
  ক্ষেত্র EXEC:
    ecmd = (কাঠামো execcmd *)cmd;
    জন্য (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    বিরতি;

  ক্ষেত্র REDIR:
    rcmd = (কাঠামো redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    বিরতি;

  ক্ষেত্র PIPE:
    pcmd = (কাঠামো pipecmd *)cmd;
    nulterminate(pcmd->বাম);
    nulterminate(pcmd->ডান);
    বিরতি;

  ক্ষেত্র LIST:
    lcmd = (কাঠামো listcmd *)cmd;
    nulterminate(lcmd->বাম);
    nulterminate(lcmd->ডান);
    বিরতি;

  ক্ষেত্র BACK:
    bcmd = (কাঠামো backcmd *)cmd;
    nulterminate(bcmd->cmd);
    বিরতি;
  }
  ফেরত cmd;
}
