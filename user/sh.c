// Shell.

#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "user/user.h"
#அந்தர்பாவ "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

கட்டமைப்பு cmd {
  முழு type;
};

கட்டமைப்பு execcmd {
  முழு type;
  எழுத்து *argv[MAXARGS];
  எழுத்து *eargv[MAXARGS];
};

கட்டமைப்பு redircmd {
  முழு type;
  கட்டமைப்பு cmd *cmd;
  எழுத்து *file;
  எழுத்து *efile;
  முழு mode;
  முழு fd;
};

கட்டமைப்பு pipecmd {
  முழு type;
  கட்டமைப்பு cmd *இடது;
  கட்டமைப்பு cmd *வலது;
};

கட்டமைப்பு listcmd {
  முழு type;
  கட்டமைப்பு cmd *இடது;
  கட்டமைப்பு cmd *வலது;
};

கட்டமைப்பு backcmd {
  முழு type;
  கட்டமைப்பு cmd *cmd;
};

முழு fork1(வெறுமை); // Fork but panics on failure.
வெறுமை panic(எழுத்து *);
கட்டமைப்பு cmd *parsecmd(எழுத்து *);
வெறுமை runcmd(கட்டமைப்பு cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
வெறுமை
runcmd(கட்டமைப்பு cmd *cmd)
{
  முழு p[2];
  கட்டமைப்பு backcmd *bcmd;
  கட்டமைப்பு execcmd *ecmd;
  கட்டமைப்பு listcmd *lcmd;
  கட்டமைப்பு pipecmd *pcmd;
  கட்டமைப்பு redircmd *rcmd;

  எனில் (cmd == 0)
    exit(1);

  தேர்வு (cmd->type) {
  இயல்பு:
    panic("runcmd");

  நிலை EXEC:
    ecmd = (கட்டமைப்பு execcmd *)cmd;
    எனில் (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    நிறுத்து;

  நிலை REDIR:
    rcmd = (கட்டமைப்பு redircmd *)cmd;
    close(rcmd->fd);
    எனில் (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    நிறுத்து;

  நிலை LIST:
    lcmd = (கட்டமைப்பு listcmd *)cmd;
    எனில் (fork1() == 0)
      runcmd(lcmd->இடது);
    wait(0);
    runcmd(lcmd->வலது);
    நிறுத்து;

  நிலை PIPE:
    pcmd = (கட்டமைப்பு pipecmd *)cmd;
    எனில் (pipe(p) < 0)
      panic("pipe");
    எனில் (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->இடது);
    }
    எனில் (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->வலது);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    நிறுத்து;

  நிலை BACK:
    bcmd = (கட்டமைப்பு backcmd *)cmd;
    எனில் (fork1() == 0)
      runcmd(bcmd->cmd);
    நிறுத்து;
  }
  exit(0);
}

முழு
getcmd(எழுத்து *buf, முழு nbuf)
{
  லிக(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  எனில் (buf[0] == 0) // EOF
    திரும்பு -1;
  திரும்பு 0;
}

முழு
முதன்மை(வெறுமை)
{
  நிலையான எழுத்து buf[100];
  முழு fd;

  // Ensure that three file descriptors are open.
  போது ((fd = open("console", O_RDWR)) >= 0) {
    எனில் (fd >= 3) {
      close(fd);
      நிறுத்து;
    }
  }

  // Read and run input commands.
  போது (getcmd(buf, அளவு(buf)) >= 0) {
    எழுத்து *cmd = buf;
    போது (*cmd == ' ' || *cmd == '\t')
      cmd++;
    எனில் (*cmd == '\n') // is a blank command
      தொடர்;
    எனில் (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      எனில் (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } இல்லையெனில் {
      எனில் (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

வெறுமை
panic(எழுத்து *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

முழு
fork1(வெறுமை)
{
  முழு pid;

  pid = fork();
  எனில் (pid == -1)
    panic("fork");
  திரும்பு pid;
}

//PAGEBREAK!
// Constructors

கட்டமைப்பு cmd *
execcmd(வெறுமை)
{
  கட்டமைப்பு execcmd *cmd;

  cmd = malloc(அளவு(*cmd));
  memset(cmd, 0, அளவு(*cmd));
  cmd->type = EXEC;
  திரும்பு (கட்டமைப்பு cmd *)cmd;
}

கட்டமைப்பு cmd *
redircmd(கட்டமைப்பு cmd *subcmd, எழுத்து *file, எழுத்து *efile, முழு mode, முழு fd)
{
  கட்டமைப்பு redircmd *cmd;

  cmd = malloc(அளவு(*cmd));
  memset(cmd, 0, அளவு(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  திரும்பு (கட்டமைப்பு cmd *)cmd;
}

கட்டமைப்பு cmd *
pipecmd(கட்டமைப்பு cmd *இடது, கட்டமைப்பு cmd *வலது)
{
  கட்டமைப்பு pipecmd *cmd;

  cmd = malloc(அளவு(*cmd));
  memset(cmd, 0, அளவு(*cmd));
  cmd->type = PIPE;
  cmd->இடது = இடது;
  cmd->வலது = வலது;
  திரும்பு (கட்டமைப்பு cmd *)cmd;
}

கட்டமைப்பு cmd *
listcmd(கட்டமைப்பு cmd *இடது, கட்டமைப்பு cmd *வலது)
{
  கட்டமைப்பு listcmd *cmd;

  cmd = malloc(அளவு(*cmd));
  memset(cmd, 0, அளவு(*cmd));
  cmd->type = LIST;
  cmd->இடது = இடது;
  cmd->வலது = வலது;
  திரும்பு (கட்டமைப்பு cmd *)cmd;
}

கட்டமைப்பு cmd *
backcmd(கட்டமைப்பு cmd *subcmd)
{
  கட்டமைப்பு backcmd *cmd;

  cmd = malloc(அளவு(*cmd));
  memset(cmd, 0, அளவு(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  திரும்பு (கட்டமைப்பு cmd *)cmd;
}
//PAGEBREAK!
// Parsing

எழுத்து whitespace[] = " \t\r\n\v";
எழுத்து symbols[] = "<|>&;()";

முழு
gettoken(எழுத்து **ps, எழுத்து *es, எழுத்து **q, எழுத்து **eq)
{
  எழுத்து *s;
  முழு ret;

  s = *ps;
  போது (s < es && strchr(whitespace, *s))
    s++;
  எனில் (q)
    *q = s;
  ret = *s;
  தேர்வு (*s) {
  நிலை 0:
    நிறுத்து;
  நிலை '|':
  நிலை '(':
  நிலை ')':
  நிலை ';':
  நிலை '&':
  நிலை '<':
    s++;
    நிறுத்து;
  நிலை '>':
    s++;
    எனில் (*s == '>') {
      ret = '+';
      s++;
    }
    நிறுத்து;
  இயல்பு:
    ret = 'a';
    போது (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    நிறுத்து;
  }
  எனில் (eq)
    *eq = s;

  போது (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  திரும்பு ret;
}

முழு
peek(எழுத்து **ps, எழுத்து *es, எழுத்து *toks)
{
  எழுத்து *s;

  s = *ps;
  போது (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  திரும்பு *s && strchr(toks, *s);
}

கட்டமைப்பு cmd *parseline(எழுத்து **, எழுத்து *);
கட்டமைப்பு cmd *parsepipe(எழுத்து **, எழுத்து *);
கட்டமைப்பு cmd *parseexec(எழுத்து **, எழுத்து *);
கட்டமைப்பு cmd *nulterminate(கட்டமைப்பு cmd *);

கட்டமைப்பு cmd *
parsecmd(எழுத்து *s)
{
  எழுத்து *es;
  கட்டமைப்பு cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  எனில் (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  திரும்பு cmd;
}

கட்டமைப்பு cmd *
parseline(எழுத்து **ps, எழுத்து *es)
{
  கட்டமைப்பு cmd *cmd;

  cmd = parsepipe(ps, es);
  போது (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  எனில் (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  திரும்பு cmd;
}

கட்டமைப்பு cmd *
parsepipe(எழுத்து **ps, எழுத்து *es)
{
  கட்டமைப்பு cmd *cmd;

  cmd = parseexec(ps, es);
  எனில் (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  திரும்பு cmd;
}

கட்டமைப்பு cmd *
parseredirs(கட்டமைப்பு cmd *cmd, எழுத்து **ps, எழுத்து *es)
{
  முழு tok;
  எழுத்து *q, *eq;

  போது (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    எனில் (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    தேர்வு (tok) {
    நிலை '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      நிறுத்து;
    நிலை '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      நிறுத்து;
    நிலை '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      நிறுத்து;
    }
  }
  திரும்பு cmd;
}

கட்டமைப்பு cmd *
parseblock(எழுத்து **ps, எழுத்து *es)
{
  கட்டமைப்பு cmd *cmd;

  எனில் (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  எனில் (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  திரும்பு cmd;
}

கட்டமைப்பு cmd *
parseexec(எழுத்து **ps, எழுத்து *es)
{
  எழுத்து *q, *eq;
  முழு tok, argc;
  கட்டமைப்பு execcmd *cmd;
  கட்டமைப்பு cmd *ret;

  எனில் (peek(ps, es, "("))
    திரும்பு parseblock(ps, es);

  ret = execcmd();
  cmd = (கட்டமைப்பு execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  போது (!peek(ps, es, "|)&;")) {
    எனில் ((tok = gettoken(ps, es, &q, &eq)) == 0)
      நிறுத்து;
    எனில் (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    எனில் (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  திரும்பு ret;
}

// NUL-terminate all the counted strings.
கட்டமைப்பு cmd *
nulterminate(கட்டமைப்பு cmd *cmd)
{
  முழு i;
  கட்டமைப்பு backcmd *bcmd;
  கட்டமைப்பு execcmd *ecmd;
  கட்டமைப்பு listcmd *lcmd;
  கட்டமைப்பு pipecmd *pcmd;
  கட்டமைப்பு redircmd *rcmd;

  எனில் (cmd == 0)
    திரும்பு 0;

  தேர்வு (cmd->type) {
  நிலை EXEC:
    ecmd = (கட்டமைப்பு execcmd *)cmd;
    க்கு (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    நிறுத்து;

  நிலை REDIR:
    rcmd = (கட்டமைப்பு redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    நிறுத்து;

  நிலை PIPE:
    pcmd = (கட்டமைப்பு pipecmd *)cmd;
    nulterminate(pcmd->இடது);
    nulterminate(pcmd->வலது);
    நிறுத்து;

  நிலை LIST:
    lcmd = (கட்டமைப்பு listcmd *)cmd;
    nulterminate(lcmd->இடது);
    nulterminate(lcmd->வலது);
    நிறுத்து;

  நிலை BACK:
    bcmd = (கட்டமைப்பு backcmd *)cmd;
    nulterminate(bcmd->cmd);
    நிறுத்து;
  }
  திரும்பு cmd;
}
