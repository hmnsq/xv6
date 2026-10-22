// Shell.

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

ಸಂರಚನಾ cmd {
  ಪೂರ್ಣಾಙ್ಕ type;
};

ಸಂರಚನಾ execcmd {
  ಪೂರ್ಣಾಙ್ಕ type;
  ವರ್ಣ *argv[MAXARGS];
  ವರ್ಣ *eargv[MAXARGS];
};

ಸಂರಚನಾ redircmd {
  ಪೂರ್ಣಾಙ್ಕ type;
  ಸಂರಚನಾ cmd *cmd;
  ವರ್ಣ *file;
  ವರ್ಣ *efile;
  ಪೂರ್ಣಾಙ್ಕ mode;
  ಪೂರ್ಣಾಙ್ಕ fd;
};

ಸಂರಚನಾ pipecmd {
  ಪೂರ್ಣಾಙ್ಕ type;
  ಸಂರಚನಾ cmd *ಎಡ;
  ಸಂರಚನಾ cmd *ಬಲ;
};

ಸಂರಚನಾ listcmd {
  ಪೂರ್ಣಾಙ್ಕ type;
  ಸಂರಚನಾ cmd *ಎಡ;
  ಸಂರಚನಾ cmd *ಬಲ;
};

ಸಂರಚನಾ backcmd {
  ಪೂರ್ಣಾಙ್ಕ type;
  ಸಂರಚನಾ cmd *cmd;
};

ಪೂರ್ಣಾಙ್ಕ fork1(ಶೂನ್ಯ); // Fork but panics on failure.
ಶೂನ್ಯ panic(ವರ್ಣ *);
ಸಂರಚನಾ cmd *parsecmd(ವರ್ಣ *);
ಶೂನ್ಯ runcmd(ಸಂರಚನಾ cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
ಶೂನ್ಯ
runcmd(ಸಂರಚನಾ cmd *cmd)
{
  ಪೂರ್ಣಾಙ್ಕ p[2];
  ಸಂರಚನಾ backcmd *bcmd;
  ಸಂರಚನಾ execcmd *ecmd;
  ಸಂರಚನಾ listcmd *lcmd;
  ಸಂರಚನಾ pipecmd *pcmd;
  ಸಂರಚನಾ redircmd *rcmd;

  ಯದಿ (cmd == 0)
    exit(1);

  ವಿಕಲ್ಪ (cmd->type) {
  ಸ್ವಭಾವ:
    panic("runcmd");

  ಪಕ್ಷ EXEC:
    ecmd = (ಸಂರಚನಾ execcmd *)cmd;
    ಯದಿ (ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    ಭಙ್ಗ;

  ಪಕ್ಷ REDIR:
    rcmd = (ಸಂರಚನಾ redircmd *)cmd;
    close(rcmd->fd);
    ಯದಿ (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    ಭಙ್ಗ;

  ಪಕ್ಷ LIST:
    lcmd = (ಸಂರಚನಾ listcmd *)cmd;
    ಯದಿ (fork1() == 0)
      runcmd(lcmd->ಎಡ);
    wait(0);
    runcmd(lcmd->ಬಲ);
    ಭಙ್ಗ;

  ಪಕ್ಷ PIPE:
    pcmd = (ಸಂರಚನಾ pipecmd *)cmd;
    ಯದಿ (pipe(p) < 0)
      panic("pipe");
    ಯದಿ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ಎಡ);
    }
    ಯದಿ (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ಬಲ);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    ಭಙ್ಗ;

  ಪಕ್ಷ BACK:
    bcmd = (ಸಂರಚನಾ backcmd *)cmd;
    ಯದಿ (fork1() == 0)
      runcmd(bcmd->cmd);
    ಭಙ್ಗ;
  }
  exit(0);
}

ಪೂರ್ಣಾಙ್ಕ
getcmd(ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ nbuf)
{
  ಲಿಖ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ಯದಿ (buf[0] == 0) // EOF
    ಪ್ರತ್ಯಾಗಮ -1;
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ
main(ಶೂನ್ಯ)
{
  ಸ್ಥಿರ ವರ್ಣ buf[100];
  ಪೂರ್ಣಾಙ್ಕ fd;

  // Ensure that three file descriptors are open.
  ಯಾವತ್ ((fd = open("console", O_RDWR)) >= 0) {
    ಯದಿ (fd >= 3) {
      close(fd);
      ಭಙ್ಗ;
    }
  }

  // Read and run input commands.
  ಯಾವತ್ (getcmd(buf, ಪರಿಮಾಣ(buf)) >= 0) {
    ವರ್ಣ *cmd = buf;
    ಯಾವತ್ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ಯದಿ (*cmd == '\n') // is a blank command
      ಅನುವರ್ತ;
    ಯದಿ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[strlen(cmd) - 1] = 0; // chop \n
      ಯದಿ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } ಅನ್ಯಥಾ {
      ಯದಿ (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  exit(0);
}

ಶೂನ್ಯ
panic(ವರ್ಣ *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}

ಪೂರ್ಣಾಙ್ಕ
fork1(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ pid;

  pid = fork();
  ಯದಿ (pid == -1)
    panic("fork");
  ಪ್ರತ್ಯಾಗಮ pid;
}

//PAGEBREAK!
// Constructors

ಸಂರಚನಾ cmd *
execcmd(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ execcmd *cmd;

  cmd = malloc(ಪರಿಮಾಣ(*cmd));
  memset(cmd, 0, ಪರಿಮಾಣ(*cmd));
  cmd->type = EXEC;
  ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ cmd *)cmd;
}

ಸಂರಚನಾ cmd *
redircmd(ಸಂರಚನಾ cmd *subcmd, ವರ್ಣ *file, ವರ್ಣ *efile, ಪೂರ್ಣಾಙ್ಕ mode, ಪೂರ್ಣಾಙ್ಕ fd)
{
  ಸಂರಚನಾ redircmd *cmd;

  cmd = malloc(ಪರಿಮಾಣ(*cmd));
  memset(cmd, 0, ಪರಿಮಾಣ(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ cmd *)cmd;
}

ಸಂರಚನಾ cmd *
pipecmd(ಸಂರಚನಾ cmd *ಎಡ, ಸಂರಚನಾ cmd *ಬಲ)
{
  ಸಂರಚನಾ pipecmd *cmd;

  cmd = malloc(ಪರಿಮಾಣ(*cmd));
  memset(cmd, 0, ಪರಿಮಾಣ(*cmd));
  cmd->type = PIPE;
  cmd->ಎಡ = ಎಡ;
  cmd->ಬಲ = ಬಲ;
  ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ cmd *)cmd;
}

ಸಂರಚನಾ cmd *
listcmd(ಸಂರಚನಾ cmd *ಎಡ, ಸಂರಚನಾ cmd *ಬಲ)
{
  ಸಂರಚನಾ listcmd *cmd;

  cmd = malloc(ಪರಿಮಾಣ(*cmd));
  memset(cmd, 0, ಪರಿಮಾಣ(*cmd));
  cmd->type = LIST;
  cmd->ಎಡ = ಎಡ;
  cmd->ಬಲ = ಬಲ;
  ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ cmd *)cmd;
}

ಸಂರಚನಾ cmd *
backcmd(ಸಂರಚನಾ cmd *subcmd)
{
  ಸಂರಚನಾ backcmd *cmd;

  cmd = malloc(ಪರಿಮಾಣ(*cmd));
  memset(cmd, 0, ಪರಿಮಾಣ(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ cmd *)cmd;
}
//PAGEBREAK!
// Parsing

ವರ್ಣ whitespace[] = " \t\r\n\v";
ವರ್ಣ symbols[] = "<|>&;()";

ಪೂರ್ಣಾಙ್ಕ
gettoken(ವರ್ಣ **ps, ವರ್ಣ *es, ವರ್ಣ **q, ವರ್ಣ **eq)
{
  ವರ್ಣ *s;
  ಪೂರ್ಣಾಙ್ಕ ret;

  s = *ps;
  ಯಾವತ್ (s < es && strchr(whitespace, *s))
    s++;
  ಯದಿ (q)
    *q = s;
  ret = *s;
  ವಿಕಲ್ಪ (*s) {
  ಪಕ್ಷ 0:
    ಭಙ್ಗ;
  ಪಕ್ಷ '|':
  ಪಕ್ಷ '(':
  ಪಕ್ಷ ')':
  ಪಕ್ಷ ';':
  ಪಕ್ಷ '&':
  ಪಕ್ಷ '<':
    s++;
    ಭಙ್ಗ;
  ಪಕ್ಷ '>':
    s++;
    ಯದಿ (*s == '>') {
      ret = '+';
      s++;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ret = 'a';
    ಯಾವತ್ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    ಭಙ್ಗ;
  }
  ಯದಿ (eq)
    *eq = s;

  ಯಾವತ್ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ
peek(ವರ್ಣ **ps, ವರ್ಣ *es, ವರ್ಣ *toks)
{
  ವರ್ಣ *s;

  s = *ps;
  ಯಾವತ್ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ಪ್ರತ್ಯಾಗಮ *s && strchr(toks, *s);
}

ಸಂರಚನಾ cmd *parseline(ವರ್ಣ **, ವರ್ಣ *);
ಸಂರಚನಾ cmd *parsepipe(ವರ್ಣ **, ವರ್ಣ *);
ಸಂರಚನಾ cmd *parseexec(ವರ್ಣ **, ವರ್ಣ *);
ಸಂರಚನಾ cmd *nulterminate(ಸಂರಚನಾ cmd *);

ಸಂರಚನಾ cmd *
parsecmd(ವರ್ಣ *s)
{
  ವರ್ಣ *es;
  ಸಂರಚನಾ cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  ಯದಿ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ಪ್ರತ್ಯಾಗಮ cmd;
}

ಸಂರಚನಾ cmd *
parseline(ವರ್ಣ **ps, ವರ್ಣ *es)
{
  ಸಂರಚನಾ cmd *cmd;

  cmd = parsepipe(ps, es);
  ಯಾವತ್ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ಯದಿ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ಪ್ರತ್ಯಾಗಮ cmd;
}

ಸಂರಚನಾ cmd *
parsepipe(ವರ್ಣ **ps, ವರ್ಣ *es)
{
  ಸಂರಚನಾ cmd *cmd;

  cmd = parseexec(ps, es);
  ಯದಿ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ಪ್ರತ್ಯಾಗಮ cmd;
}

ಸಂರಚನಾ cmd *
parseredirs(ಸಂರಚನಾ cmd *cmd, ವರ್ಣ **ps, ವರ್ಣ *es)
{
  ಪೂರ್ಣಾಙ್ಕ tok;
  ವರ್ಣ *q, *eq;

  ಯಾವತ್ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ಯದಿ (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    ವಿಕಲ್ಪ (tok) {
    ಪಕ್ಷ '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      ಭಙ್ಗ;
    ಪಕ್ಷ '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      ಭಙ್ಗ;
    ಪಕ್ಷ '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ cmd;
}

ಸಂರಚನಾ cmd *
parseblock(ವರ್ಣ **ps, ವರ್ಣ *es)
{
  ಸಂರಚನಾ cmd *cmd;

  ಯದಿ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ಯದಿ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ಪ್ರತ್ಯಾಗಮ cmd;
}

ಸಂರಚನಾ cmd *
parseexec(ವರ್ಣ **ps, ವರ್ಣ *es)
{
  ವರ್ಣ *q, *eq;
  ಪೂರ್ಣಾಙ್ಕ tok, argc;
  ಸಂರಚನಾ execcmd *cmd;
  ಸಂರಚನಾ cmd *ret;

  ಯದಿ (peek(ps, es, "("))
    ಪ್ರತ್ಯಾಗಮ parseblock(ps, es);

  ret = execcmd();
  cmd = (ಸಂರಚನಾ execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ಯಾವತ್ (!peek(ps, es, "|)&;")) {
    ಯದಿ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      ಭಙ್ಗ;
    ಯದಿ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ಯದಿ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  ಪ್ರತ್ಯಾಗಮ ret;
}

// NUL-terminate all the counted strings.
ಸಂರಚನಾ cmd *
nulterminate(ಸಂರಚನಾ cmd *cmd)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ಸಂರಚನಾ backcmd *bcmd;
  ಸಂರಚನಾ execcmd *ecmd;
  ಸಂರಚನಾ listcmd *lcmd;
  ಸಂರಚನಾ pipecmd *pcmd;
  ಸಂರಚನಾ redircmd *rcmd;

  ಯದಿ (cmd == 0)
    ಪ್ರತ್ಯಾಗಮ 0;

  ವಿಕಲ್ಪ (cmd->type) {
  ಪಕ್ಷ EXEC:
    ecmd = (ಸಂರಚನಾ execcmd *)cmd;
    ಪ್ರತಿ (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ REDIR:
    rcmd = (ಸಂರಚನಾ redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ PIPE:
    pcmd = (ಸಂರಚನಾ pipecmd *)cmd;
    nulterminate(pcmd->ಎಡ);
    nulterminate(pcmd->ಬಲ);
    ಭಙ್ಗ;

  ಪಕ್ಷ LIST:
    lcmd = (ಸಂರಚನಾ listcmd *)cmd;
    nulterminate(lcmd->ಎಡ);
    nulterminate(lcmd->ಬಲ);
    ಭಙ್ಗ;

  ಪಕ್ಷ BACK:
    bcmd = (ಸಂರಚನಾ backcmd *)cmd;
    nulterminate(bcmd->cmd);
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cmd;
}
