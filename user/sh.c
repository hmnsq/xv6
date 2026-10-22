// Shell.

#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

ସଂରଚନା cmd {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
};

ସଂରଚନା execcmd {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
  ଅକ୍ଷର *argv[MAXARGS];
  ଅକ୍ଷର *eargv[MAXARGS];
};

ସଂରଚନା redircmd {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
  ସଂରଚନା cmd *cmd;
  ଅକ୍ଷର *file;
  ଅକ୍ଷର *efile;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
};

ସଂରଚନା pipecmd {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
  ସଂରଚନା cmd *ବାମ;
  ସଂରଚନା cmd *ଡାହାଣ;
};

ସଂରଚନା listcmd {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
  ସଂରଚନା cmd *ବାମ;
  ସଂରଚନା cmd *ଡାହାଣ;
};

ସଂରଚନା backcmd {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
  ସଂରଚନା cmd *cmd;
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fork1(ଶୂନ୍ୟ); // Fork but panics on failure.
ଶୂନ୍ୟ panic(ଅକ୍ଷର *);
ସଂରଚନା cmd *parsecmd(ଅକ୍ଷର *);
ଶୂନ୍ୟ runcmd(ସଂରଚନା cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
ଶୂନ୍ୟ
runcmd(ସଂରଚନା cmd *cmd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା p[2];
  ସଂରଚନା backcmd *bcmd;
  ସଂରଚନା execcmd *ecmd;
  ସଂରଚନା listcmd *lcmd;
  ସଂରଚନା pipecmd *pcmd;
  ସଂରଚନା redircmd *rcmd;

  ଯଦି (cmd == 0)
    exit(1);

  ବାଛ (cmd->type) {
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    panic("runcmd");

  ସ୍ଥିତି EXEC:
    ecmd = (ସଂରଚନା execcmd *)cmd;
    ଯଦି (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି REDIR:
    rcmd = (ସଂରଚନା redircmd *)cmd;
    close(rcmd->fd);
    ଯଦି (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LIST:
    lcmd = (ସଂରଚନା listcmd *)cmd;
    ଯଦି (fork1() == 0)
      runcmd(lcmd->ବାମ);
    wait(0);
    runcmd(lcmd->ଡାହାଣ);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି PIPE:
    pcmd = (ସଂରଚନା pipecmd *)cmd;
    ଯଦି (pipe(p) < 0)
      panic("pipe");
    ଯଦି (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ବାମ);
    }
    ଯଦି (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ଡାହାଣ);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି BACK:
    bcmd = (ସଂରଚନା backcmd *)cmd;
    ଯଦି (fork1() == 0)
      runcmd(bcmd->cmd);
    ଭାଙ୍ଗ;
  }
  exit(0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getcmd(ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nbuf)
{
  ଲିଖ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ଯଦି (buf[0] == 0) // EOF
    ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ଶୂନ୍ୟ)
{
  ସ୍ଥିର ଅକ୍ଷର buf[100];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

  // Ensure that three file descriptors are open.
  ଯେତେବେଳେ ((fd = open("console", O_RDWR)) >= 0) {
    ଯଦି (fd >= 3) {
      close(fd);
      ଭାଙ୍ଗ;
    }
  }

  // Read and run input commands.
  ଯେତେବେଳେ (getcmd(buf, ଆକାର(buf)) >= 0) {
    ଅକ୍ଷର *cmd = buf;
    ଯେତେବେଳେ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ଯଦି (*cmd == '\n') // is a blank command
      ଜାରି;
    ଯଦି (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      ଯଦି (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } ଅନ୍ୟଥା {
      ଯଦି (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

ଶୂନ୍ୟ
panic(ଅକ୍ଷର *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fork1(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;

  pid = fork();
  ଯଦି (pid == -1)
    panic("fork");
  ଫେରନ୍ତୁ pid;
}

//PAGEBREAK!
// Constructors

ସଂରଚନା cmd *
execcmd(ଶୂନ୍ୟ)
{
  ସଂରଚନା execcmd *cmd;

  cmd = malloc(ଆକାର(*cmd));
  memset(cmd, 0, ଆକାର(*cmd));
  cmd->type = EXEC;
  ଫେରନ୍ତୁ (ସଂରଚନା cmd *)cmd;
}

ସଂରଚନା cmd *
redircmd(ସଂରଚନା cmd *subcmd, ଅକ୍ଷର *file, ଅକ୍ଷର *efile, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
  ସଂରଚନା redircmd *cmd;

  cmd = malloc(ଆକାର(*cmd));
  memset(cmd, 0, ଆକାର(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  ଫେରନ୍ତୁ (ସଂରଚନା cmd *)cmd;
}

ସଂରଚନା cmd *
pipecmd(ସଂରଚନା cmd *ବାମ, ସଂରଚନା cmd *ଡାହାଣ)
{
  ସଂରଚନା pipecmd *cmd;

  cmd = malloc(ଆକାର(*cmd));
  memset(cmd, 0, ଆକାର(*cmd));
  cmd->type = PIPE;
  cmd->ବାମ = ବାମ;
  cmd->ଡାହାଣ = ଡାହାଣ;
  ଫେରନ୍ତୁ (ସଂରଚନା cmd *)cmd;
}

ସଂରଚନା cmd *
listcmd(ସଂରଚନା cmd *ବାମ, ସଂରଚନା cmd *ଡାହାଣ)
{
  ସଂରଚନା listcmd *cmd;

  cmd = malloc(ଆକାର(*cmd));
  memset(cmd, 0, ଆକାର(*cmd));
  cmd->type = LIST;
  cmd->ବାମ = ବାମ;
  cmd->ଡାହାଣ = ଡାହାଣ;
  ଫେରନ୍ତୁ (ସଂରଚନା cmd *)cmd;
}

ସଂରଚନା cmd *
backcmd(ସଂରଚନା cmd *subcmd)
{
  ସଂରଚନା backcmd *cmd;

  cmd = malloc(ଆକାର(*cmd));
  memset(cmd, 0, ଆକାର(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  ଫେରନ୍ତୁ (ସଂରଚନା cmd *)cmd;
}
//PAGEBREAK!
// Parsing

ଅକ୍ଷର whitespace[] = " \t\r\n\v";
ଅକ୍ଷର symbols[] = "<|>&;()";

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gettoken(ଅକ୍ଷର **ps, ଅକ୍ଷର *es, ଅକ୍ଷର **q, ଅକ୍ଷର **eq)
{
  ଅକ୍ଷର *s;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

  s = *ps;
  ଯେତେବେଳେ (s < es && strchr(whitespace, *s))
    s++;
  ଯଦି (q)
    *q = s;
  ret = *s;
  ବାଛ (*s) {
  ସ୍ଥିତି 0:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି '|':
  ସ୍ଥିତି '(':
  ସ୍ଥିତି ')':
  ସ୍ଥିତି ';':
  ସ୍ଥିତି '&':
  ସ୍ଥିତି '<':
    s++;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି '>':
    s++;
    ଯଦି (*s == '>') {
      ret = '+';
      s++;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ret = 'a';
    ଯେତେବେଳେ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    ଭାଙ୍ଗ;
  }
  ଯଦି (eq)
    *eq = s;

  ଯେତେବେଳେ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
peek(ଅକ୍ଷର **ps, ଅକ୍ଷର *es, ଅକ୍ଷର *toks)
{
  ଅକ୍ଷର *s;

  s = *ps;
  ଯେତେବେଳେ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ଫେରନ୍ତୁ *s && strchr(toks, *s);
}

ସଂରଚନା cmd *parseline(ଅକ୍ଷର **, ଅକ୍ଷର *);
ସଂରଚନା cmd *parsepipe(ଅକ୍ଷର **, ଅକ୍ଷର *);
ସଂରଚନା cmd *parseexec(ଅକ୍ଷର **, ଅକ୍ଷର *);
ସଂରଚନା cmd *nulterminate(ସଂରଚନା cmd *);

ସଂରଚନା cmd *
parsecmd(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର *es;
  ସଂରଚନା cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  ଯଦି (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ଫେରନ୍ତୁ cmd;
}

ସଂରଚନା cmd *
parseline(ଅକ୍ଷର **ps, ଅକ୍ଷର *es)
{
  ସଂରଚନା cmd *cmd;

  cmd = parsepipe(ps, es);
  ଯେତେବେଳେ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ଯଦି (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ଫେରନ୍ତୁ cmd;
}

ସଂରଚନା cmd *
parsepipe(ଅକ୍ଷର **ps, ଅକ୍ଷର *es)
{
  ସଂରଚନା cmd *cmd;

  cmd = parseexec(ps, es);
  ଯଦି (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ଫେରନ୍ତୁ cmd;
}

ସଂରଚନା cmd *
parseredirs(ସଂରଚନା cmd *cmd, ଅକ୍ଷର **ps, ଅକ୍ଷର *es)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା tok;
  ଅକ୍ଷର *q, *eq;

  ଯେତେବେଳେ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ଯଦି (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    ବାଛ (tok) {
    ସ୍ଥିତି '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ cmd;
}

ସଂରଚନା cmd *
parseblock(ଅକ୍ଷର **ps, ଅକ୍ଷର *es)
{
  ସଂରଚନା cmd *cmd;

  ଯଦି (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ଯଦି (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ଫେରନ୍ତୁ cmd;
}

ସଂରଚନା cmd *
parseexec(ଅକ୍ଷର **ps, ଅକ୍ଷର *es)
{
  ଅକ୍ଷର *q, *eq;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା tok, argc;
  ସଂରଚନା execcmd *cmd;
  ସଂରଚନା cmd *ret;

  ଯଦି (peek(ps, es, "("))
    ଫେରନ୍ତୁ parseblock(ps, es);

  ret = execcmd();
  cmd = (ସଂରଚନା execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ଯେତେବେଳେ (!peek(ps, es, "|)&;")) {
    ଯଦି ((tok = gettoken(ps, es, &q, &eq)) == 0)
      ଭାଙ୍ଗ;
    ଯଦି (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ଯଦି (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  ଫେରନ୍ତୁ ret;
}

// NUL-terminate all the counted strings.
ସଂରଚନା cmd *
nulterminate(ସଂରଚନା cmd *cmd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ସଂରଚନା backcmd *bcmd;
  ସଂରଚନା execcmd *ecmd;
  ସଂରଚନା listcmd *lcmd;
  ସଂରଚନା pipecmd *pcmd;
  ସଂରଚନା redircmd *rcmd;

  ଯଦି (cmd == 0)
    ଫେରନ୍ତୁ 0;

  ବାଛ (cmd->type) {
  ସ୍ଥିତି EXEC:
    ecmd = (ସଂରଚନା execcmd *)cmd;
    ପାଇଁ (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି REDIR:
    rcmd = (ସଂରଚନା redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି PIPE:
    pcmd = (ସଂରଚନା pipecmd *)cmd;
    nulterminate(pcmd->ବାମ);
    nulterminate(pcmd->ଡାହାଣ);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LIST:
    lcmd = (ସଂରଚନା listcmd *)cmd;
    nulterminate(lcmd->ବାମ);
    nulterminate(lcmd->ଡାହାଣ);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି BACK:
    bcmd = (ସଂରଚନା backcmd *)cmd;
    nulterminate(bcmd->cmd);
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cmd;
}
