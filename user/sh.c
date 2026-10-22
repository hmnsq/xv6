// Shell.

#شامل "kernel/types.h"
#شامل "user/user.h"
#شامل "kernel/fcntl.h"

// Parsed command representation
#تعریف_کرو EXEC  1
#تعریف_کرو REDIR 2
#تعریف_کرو PIPE  3
#تعریف_کرو LIST  4
#تعریف_کرو عقب  5

#تعریف_کرو MAXARGS 10

ساخت cmd {
  صحیح type;
};

ساخت execcmd {
  صحیح type;
  حرف *argv[MAXARGS];
  حرف *eargv[MAXARGS];
};

ساخت redircmd {
  صحیح type;
  ساخت cmd *cmd;
  حرف *file;
  حرف *efile;
  صحیح منوال;
  صحیح fd;
};

ساخت pipecmd {
  صحیح type;
  ساخت cmd *بایاں;
  ساخت cmd *دایاں;
};

ساخت listcmd {
  صحیح type;
  ساخت cmd *بایاں;
  ساخت cmd *دایاں;
};

ساخت backcmd {
  صحیح type;
  ساخت cmd *cmd;
};

صحیح fork1(خالی); // Fork but panics on failure.
خالی panic(حرف *);
ساخت cmd *parsecmd(حرف *);
خالی runcmd(ساخت cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
خالی
runcmd(ساخت cmd *cmd)
{
  صحیح p[2];
  ساخت backcmd *bcmd;
  ساخت execcmd *ecmd;
  ساخت listcmd *lcmd;
  ساخت pipecmd *pcmd;
  ساخت redircmd *rcmd;

  اگر (cmd == 0)
    باہر_نکلو(1);

  انتخاب (cmd->type) {
  طےشدہ:
    panic("runcmd");

  حالت EXEC:
    ecmd = (ساخت execcmd *)cmd;
    اگر (ecmd->argv[0] == 0)
      باہر_نکلو(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    توڑو;

  حالت REDIR:
    rcmd = (ساخت redircmd *)cmd;
    close(rcmd->fd);
    اگر (open(rcmd->file, rcmd->منوال) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      باہر_نکلو(1);
    }
    runcmd(rcmd->cmd);
    توڑو;

  حالت LIST:
    lcmd = (ساخت listcmd *)cmd;
    اگر (fork1() == 0)
      runcmd(lcmd->بایاں);
    wait(0);
    runcmd(lcmd->دایاں);
    توڑو;

  حالت PIPE:
    pcmd = (ساخت pipecmd *)cmd;
    اگر (pipe(p) < 0)
      panic("pipe");
    اگر (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->بایاں);
    }
    اگر (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->دایاں);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    توڑو;

  حالت عقب:
    bcmd = (ساخت backcmd *)cmd;
    اگر (fork1() == 0)
      runcmd(bcmd->cmd);
    توڑو;
  }
  باہر_نکلو(0);
}

صحیح
getcmd(حرف *buf, صحیح nbuf)
{
  اكتب(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  اگر (buf[0] == 0) // EOF
    واپس -1;
  واپس 0;
}

صحیح
مین(خالی)
{
  ساکن حرف buf[100];
  صحیح fd;

  // Ensure that three file descriptors are open.
  جب ((fd = open("console", O_RDWR)) >= 0) {
    اگر (fd >= 3) {
      close(fd);
      توڑو;
    }
  }

  // Read and run input commands.
  جب (getcmd(buf, حجم(buf)) >= 0) {
    حرف *cmd = buf;
    جب (*cmd == ' ' || *cmd == '\t')
      cmd++;
    اگر (*cmd == '\n') // is a blank command
      جاری;
    اگر (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[لڑی_لمبائی(cmd) - 1] = 0; // chop \n
      اگر (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } ورنہ {
      اگر (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  باہر_نکلو(0);
}

خالی
panic(حرف *s)
{
  fprintf(2, "%s\n", s);
  باہر_نکلو(1);
}

صحیح
fork1(خالی)
{
  صحیح pid;

  pid = fork();
  اگر (pid == -1)
    panic("fork");
  واپس pid;
}

//PAGEBREAK!
// Constructors

ساخت cmd *
execcmd(خالی)
{
  ساخت execcmd *cmd;

  cmd = حافظہ_دو(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = EXEC;
  واپس (ساخت cmd *)cmd;
}

ساخت cmd *
redircmd(ساخت cmd *subcmd, حرف *file, حرف *efile, صحیح منوال, صحیح fd)
{
  ساخت redircmd *cmd;

  cmd = حافظہ_دو(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->منوال = منوال;
  cmd->fd = fd;
  واپس (ساخت cmd *)cmd;
}

ساخت cmd *
pipecmd(ساخت cmd *بایاں, ساخت cmd *دایاں)
{
  ساخت pipecmd *cmd;

  cmd = حافظہ_دو(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = PIPE;
  cmd->بایاں = بایاں;
  cmd->دایاں = دایاں;
  واپس (ساخت cmd *)cmd;
}

ساخت cmd *
listcmd(ساخت cmd *بایاں, ساخت cmd *دایاں)
{
  ساخت listcmd *cmd;

  cmd = حافظہ_دو(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = LIST;
  cmd->بایاں = بایاں;
  cmd->دایاں = دایاں;
  واپس (ساخت cmd *)cmd;
}

ساخت cmd *
backcmd(ساخت cmd *subcmd)
{
  ساخت backcmd *cmd;

  cmd = حافظہ_دو(حجم(*cmd));
  memset(cmd, 0, حجم(*cmd));
  cmd->type = عقب;
  cmd->cmd = subcmd;
  واپس (ساخت cmd *)cmd;
}
//PAGEBREAK!
// Parsing

حرف whitespace[] = " \t\r\n\v";
حرف symbols[] = "<|>&;()";

صحیح
gettoken(حرف **ps, حرف *es, حرف **q, حرف **eq)
{
  حرف *s;
  صحیح ret;

  s = *ps;
  جب (s < es && strchr(whitespace, *s))
    s++;
  اگر (q)
    *q = s;
  ret = *s;
  انتخاب (*s) {
  حالت 0:
    توڑو;
  حالت '|':
  حالت '(':
  حالت ')':
  حالت ';':
  حالت '&':
  حالت '<':
    s++;
    توڑو;
  حالت '>':
    s++;
    اگر (*s == '>') {
      ret = '+';
      s++;
    }
    توڑو;
  طےشدہ:
    ret = 'a';
    جب (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    توڑو;
  }
  اگر (eq)
    *eq = s;

  جب (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  واپس ret;
}

صحیح
peek(حرف **ps, حرف *es, حرف *toks)
{
  حرف *s;

  s = *ps;
  جب (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  واپس *s && strchr(toks, *s);
}

ساخت cmd *parseline(حرف **, حرف *);
ساخت cmd *parsepipe(حرف **, حرف *);
ساخت cmd *parseexec(حرف **, حرف *);
ساخت cmd *nulterminate(ساخت cmd *);

ساخت cmd *
parsecmd(حرف *s)
{
  حرف *es;
  ساخت cmd *cmd;

  es = s + لڑی_لمبائی(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  اگر (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  واپس cmd;
}

ساخت cmd *
parseline(حرف **ps, حرف *es)
{
  ساخت cmd *cmd;

  cmd = parsepipe(ps, es);
  جب (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  اگر (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  واپس cmd;
}

ساخت cmd *
parsepipe(حرف **ps, حرف *es)
{
  ساخت cmd *cmd;

  cmd = parseexec(ps, es);
  اگر (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  واپس cmd;
}

ساخت cmd *
parseredirs(ساخت cmd *cmd, حرف **ps, حرف *es)
{
  صحیح tok;
  حرف *q, *eq;

  جب (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    اگر (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    انتخاب (tok) {
    حالت '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      توڑو;
    حالت '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      توڑو;
    حالت '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      توڑو;
    }
  }
  واپس cmd;
}

ساخت cmd *
parseblock(حرف **ps, حرف *es)
{
  ساخت cmd *cmd;

  اگر (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  اگر (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  واپس cmd;
}

ساخت cmd *
parseexec(حرف **ps, حرف *es)
{
  حرف *q, *eq;
  صحیح tok, argc;
  ساخت execcmd *cmd;
  ساخت cmd *ret;

  اگر (peek(ps, es, "("))
    واپس parseblock(ps, es);

  ret = execcmd();
  cmd = (ساخت execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  جب (!peek(ps, es, "|)&;")) {
    اگر ((tok = gettoken(ps, es, &q, &eq)) == 0)
      توڑو;
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
  واپس ret;
}

// NUL-terminate all the counted strings.
ساخت cmd *
nulterminate(ساخت cmd *cmd)
{
  صحیح i;
  ساخت backcmd *bcmd;
  ساخت execcmd *ecmd;
  ساخت listcmd *lcmd;
  ساخت pipecmd *pcmd;
  ساخت redircmd *rcmd;

  اگر (cmd == 0)
    واپس 0;

  انتخاب (cmd->type) {
  حالت EXEC:
    ecmd = (ساخت execcmd *)cmd;
    برائے (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    توڑو;

  حالت REDIR:
    rcmd = (ساخت redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    توڑو;

  حالت PIPE:
    pcmd = (ساخت pipecmd *)cmd;
    nulterminate(pcmd->بایاں);
    nulterminate(pcmd->دایاں);
    توڑو;

  حالت LIST:
    lcmd = (ساخت listcmd *)cmd;
    nulterminate(lcmd->بایاں);
    nulterminate(lcmd->دایاں);
    توڑو;

  حالت عقب:
    bcmd = (ساخت backcmd *)cmd;
    nulterminate(bcmd->cmd);
    توڑو;
  }
  واپس cmd;
}
