// Shell.

#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"

// Parsed command representation
#ವ್ಯಾಖ್ಯಾನಿಸು EXEC  1
#ವ್ಯಾಖ್ಯಾನಿಸು REDIR 2
#ವ್ಯಾಖ್ಯಾನಿಸು PIPE  3
#ವ್ಯಾಖ್ಯಾನಿಸು LIST  4
#ವ್ಯಾಖ್ಯಾನಿಸು ಹಿಂಭಾಗ  5

#ವ್ಯಾಖ್ಯಾನಿಸು MAXARGS 10

ರಚನೆ cmd {
  ಪೂರ್ಣಾಂಕ type;
};

ರಚನೆ execcmd {
  ಪೂರ್ಣಾಂಕ type;
  ಅಕ್ಷರ *argv[MAXARGS];
  ಅಕ್ಷರ *eargv[MAXARGS];
};

ರಚನೆ redircmd {
  ಪೂರ್ಣಾಂಕ type;
  ರಚನೆ cmd *cmd;
  ಅಕ್ಷರ *file;
  ಅಕ್ಷರ *efile;
  ಪೂರ್ಣಾಂಕ ಬಹುಲಕ;
  ಪೂರ್ಣಾಂಕ fd;
};

ರಚನೆ pipecmd {
  ಪೂರ್ಣಾಂಕ type;
  ರಚನೆ cmd *ಎಡ;
  ರಚನೆ cmd *ಬಲ;
};

ರಚನೆ listcmd {
  ಪೂರ್ಣಾಂಕ type;
  ರಚನೆ cmd *ಎಡ;
  ರಚನೆ cmd *ಬಲ;
};

ರಚನೆ backcmd {
  ಪೂರ್ಣಾಂಕ type;
  ರಚನೆ cmd *cmd;
};

ಪೂರ್ಣಾಂಕ fork1(ಶೂನ್ಯ); // Fork but panics on failure.
ಶೂನ್ಯ panic(ಅಕ್ಷರ *);
ರಚನೆ cmd *parsecmd(ಅಕ್ಷರ *);
ಶೂನ್ಯ runcmd(ರಚನೆ cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
ಶೂನ್ಯ
runcmd(ರಚನೆ cmd *cmd)
{
  ಪೂರ್ಣಾಂಕ p[2];
  ರಚನೆ backcmd *bcmd;
  ರಚನೆ execcmd *ecmd;
  ರಚನೆ listcmd *lcmd;
  ರಚನೆ pipecmd *pcmd;
  ರಚನೆ redircmd *rcmd;

  ಆದರೆ (cmd == 0)
    ನಿರ್ಗಮಿಸು(1);

  ಆಯ್ಕೆ (cmd->type) {
  ಪೂರ್ವನಿಯೋಜಿತ:
    panic("runcmd");

  ಸ್ಥಿತಿ EXEC:
    ecmd = (ರಚನೆ execcmd *)cmd;
    ಆದರೆ (ecmd->argv[0] == 0)
      ನಿರ್ಗಮಿಸು(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ REDIR:
    rcmd = (ರಚನೆ redircmd *)cmd;
    close(rcmd->fd);
    ಆದರೆ (open(rcmd->file, rcmd->ಬಹುಲಕ) < 0) {
      fprintf(2, "open %s failed\n", rcmd->file);
      ನಿರ್ಗಮಿಸು(1);
    }
    runcmd(rcmd->cmd);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LIST:
    lcmd = (ರಚನೆ listcmd *)cmd;
    ಆದರೆ (fork1() == 0)
      runcmd(lcmd->ಎಡ);
    wait(0);
    runcmd(lcmd->ಬಲ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ PIPE:
    pcmd = (ರಚನೆ pipecmd *)cmd;
    ಆದರೆ (pipe(p) < 0)
      panic("pipe");
    ಆದರೆ (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->ಎಡ);
    }
    ಆದರೆ (fork1() == 0) {
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
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಹಿಂಭಾಗ:
    bcmd = (ರಚನೆ backcmd *)cmd;
    ಆದರೆ (fork1() == 0)
      runcmd(bcmd->cmd);
    ನಿಲ್ಲಿಸು;
  }
  ನಿರ್ಗಮಿಸು(0);
}

ಪೂರ್ಣಾಂಕ
getcmd(ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ nbuf)
{
  ಲಿಖ(2, "$ ", 2);
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  ಆದರೆ (buf[0] == 0) // EOF
    ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಶೂನ್ಯ)
{
  ಸ್ಥಿರ ಅಕ್ಷರ buf[100];
  ಪೂರ್ಣಾಂಕ fd;

  // Ensure that three file descriptors are open.
  ಆಗ ((fd = open("console", O_RDWR)) >= 0) {
    ಆದರೆ (fd >= 3) {
      close(fd);
      ನಿಲ್ಲಿಸು;
    }
  }

  // Read and run input commands.
  ಆಗ (getcmd(buf, ಗಾತ್ರ(buf)) >= 0) {
    ಅಕ್ಷರ *cmd = buf;
    ಆಗ (*cmd == ' ' || *cmd == '\t')
      cmd++;
    ಆದರೆ (*cmd == '\n') // is a blank command
      ಮುಂದುವರಿ;
    ಆದರೆ (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cmd) - 1] = 0; // chop \n
      ಆದರೆ (chdir(cmd + 3) < 0)
        fprintf(2, "cannot cd %s\n", cmd + 3);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ (fork1() == 0)
        runcmd(parsecmd(cmd));
      wait(0);
    }
  }
  ನಿರ್ಗಮಿಸು(0);
}

ಶೂನ್ಯ
panic(ಅಕ್ಷರ *s)
{
  fprintf(2, "%s\n", s);
  ನಿರ್ಗಮಿಸು(1);
}

ಪೂರ್ಣಾಂಕ
fork1(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ pid;

  pid = fork();
  ಆದರೆ (pid == -1)
    panic("fork");
  ಹಿಂತಿರುಗಿ pid;
}

//PAGEBREAK!
// Constructors

ರಚನೆ cmd *
execcmd(ಶೂನ್ಯ)
{
  ರಚನೆ execcmd *cmd;

  cmd = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(*cmd));
  memset(cmd, 0, ಗಾತ್ರ(*cmd));
  cmd->type = EXEC;
  ಹಿಂತಿರುಗಿ (ರಚನೆ cmd *)cmd;
}

ರಚನೆ cmd *
redircmd(ರಚನೆ cmd *subcmd, ಅಕ್ಷರ *file, ಅಕ್ಷರ *efile, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ fd)
{
  ರಚನೆ redircmd *cmd;

  cmd = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(*cmd));
  memset(cmd, 0, ಗಾತ್ರ(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->ಬಹುಲಕ = ಬಹುಲಕ;
  cmd->fd = fd;
  ಹಿಂತಿರುಗಿ (ರಚನೆ cmd *)cmd;
}

ರಚನೆ cmd *
pipecmd(ರಚನೆ cmd *ಎಡ, ರಚನೆ cmd *ಬಲ)
{
  ರಚನೆ pipecmd *cmd;

  cmd = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(*cmd));
  memset(cmd, 0, ಗಾತ್ರ(*cmd));
  cmd->type = PIPE;
  cmd->ಎಡ = ಎಡ;
  cmd->ಬಲ = ಬಲ;
  ಹಿಂತಿರುಗಿ (ರಚನೆ cmd *)cmd;
}

ರಚನೆ cmd *
listcmd(ರಚನೆ cmd *ಎಡ, ರಚನೆ cmd *ಬಲ)
{
  ರಚನೆ listcmd *cmd;

  cmd = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(*cmd));
  memset(cmd, 0, ಗಾತ್ರ(*cmd));
  cmd->type = LIST;
  cmd->ಎಡ = ಎಡ;
  cmd->ಬಲ = ಬಲ;
  ಹಿಂತಿರುಗಿ (ರಚನೆ cmd *)cmd;
}

ರಚನೆ cmd *
backcmd(ರಚನೆ cmd *subcmd)
{
  ರಚನೆ backcmd *cmd;

  cmd = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(*cmd));
  memset(cmd, 0, ಗಾತ್ರ(*cmd));
  cmd->type = ಹಿಂಭಾಗ;
  cmd->cmd = subcmd;
  ಹಿಂತಿರುಗಿ (ರಚನೆ cmd *)cmd;
}
//PAGEBREAK!
// Parsing

ಅಕ್ಷರ whitespace[] = " \t\r\n\v";
ಅಕ್ಷರ symbols[] = "<|>&;()";

ಪೂರ್ಣಾಂಕ
gettoken(ಅಕ್ಷರ **ps, ಅಕ್ಷರ *es, ಅಕ್ಷರ **q, ಅಕ್ಷರ **eq)
{
  ಅಕ್ಷರ *s;
  ಪೂರ್ಣಾಂಕ ret;

  s = *ps;
  ಆಗ (s < es && strchr(whitespace, *s))
    s++;
  ಆದರೆ (q)
    *q = s;
  ret = *s;
  ಆಯ್ಕೆ (*s) {
  ಸ್ಥಿತಿ 0:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ '|':
  ಸ್ಥಿತಿ '(':
  ಸ್ಥಿತಿ ')':
  ಸ್ಥಿತಿ ';':
  ಸ್ಥಿತಿ '&':
  ಸ್ಥಿತಿ '<':
    s++;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ '>':
    s++;
    ಆದರೆ (*s == '>') {
      ret = '+';
      s++;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ret = 'a';
    ಆಗ (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ (eq)
    *eq = s;

  ಆಗ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ
peek(ಅಕ್ಷರ **ps, ಅಕ್ಷರ *es, ಅಕ್ಷರ *toks)
{
  ಅಕ್ಷರ *s;

  s = *ps;
  ಆಗ (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  ಹಿಂತಿರುಗಿ *s && strchr(toks, *s);
}

ರಚನೆ cmd *parseline(ಅಕ್ಷರ **, ಅಕ್ಷರ *);
ರಚನೆ cmd *parsepipe(ಅಕ್ಷರ **, ಅಕ್ಷರ *);
ರಚನೆ cmd *parseexec(ಅಕ್ಷರ **, ಅಕ್ಷರ *);
ರಚನೆ cmd *nulterminate(ರಚನೆ cmd *);

ರಚನೆ cmd *
parsecmd(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ *es;
  ರಚನೆ cmd *cmd;

  es = s + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  ಆದರೆ (s != es) {
    fprintf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  ಹಿಂತಿರುಗಿ cmd;
}

ರಚನೆ cmd *
parseline(ಅಕ್ಷರ **ps, ಅಕ್ಷರ *es)
{
  ರಚನೆ cmd *cmd;

  cmd = parsepipe(ps, es);
  ಆಗ (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  ಆದರೆ (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  ಹಿಂತಿರುಗಿ cmd;
}

ರಚನೆ cmd *
parsepipe(ಅಕ್ಷರ **ps, ಅಕ್ಷರ *es)
{
  ರಚನೆ cmd *cmd;

  cmd = parseexec(ps, es);
  ಆದರೆ (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  ಹಿಂತಿರುಗಿ cmd;
}

ರಚನೆ cmd *
parseredirs(ರಚನೆ cmd *cmd, ಅಕ್ಷರ **ps, ಅಕ್ಷರ *es)
{
  ಪೂರ್ಣಾಂಕ tok;
  ಅಕ್ಷರ *q, *eq;

  ಆಗ (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    ಆದರೆ (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    ಆಯ್ಕೆ (tok) {
    ಸ್ಥಿತಿ '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ cmd;
}

ರಚನೆ cmd *
parseblock(ಅಕ್ಷರ **ps, ಅಕ್ಷರ *es)
{
  ರಚನೆ cmd *cmd;

  ಆದರೆ (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  ಆದರೆ (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  ಹಿಂತಿರುಗಿ cmd;
}

ರಚನೆ cmd *
parseexec(ಅಕ್ಷರ **ps, ಅಕ್ಷರ *es)
{
  ಅಕ್ಷರ *q, *eq;
  ಪೂರ್ಣಾಂಕ tok, argc;
  ರಚನೆ execcmd *cmd;
  ರಚನೆ cmd *ret;

  ಆದರೆ (peek(ps, es, "("))
    ಹಿಂತಿರುಗಿ parseblock(ps, es);

  ret = execcmd();
  cmd = (ರಚನೆ execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  ಆಗ (!peek(ps, es, "|)&;")) {
    ಆದರೆ ((tok = gettoken(ps, es, &q, &eq)) == 0)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    ಆದರೆ (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  ಹಿಂತಿರುಗಿ ret;
}

// NUL-terminate all the counted strings.
ರಚನೆ cmd *
nulterminate(ರಚನೆ cmd *cmd)
{
  ಪೂರ್ಣಾಂಕ i;
  ರಚನೆ backcmd *bcmd;
  ರಚನೆ execcmd *ecmd;
  ರಚನೆ listcmd *lcmd;
  ರಚನೆ pipecmd *pcmd;
  ರಚನೆ redircmd *rcmd;

  ಆದರೆ (cmd == 0)
    ಹಿಂತಿರುಗಿ 0;

  ಆಯ್ಕೆ (cmd->type) {
  ಸ್ಥಿತಿ EXEC:
    ecmd = (ರಚನೆ execcmd *)cmd;
    ಗಾಗಿ (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ REDIR:
    rcmd = (ರಚನೆ redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ PIPE:
    pcmd = (ರಚನೆ pipecmd *)cmd;
    nulterminate(pcmd->ಎಡ);
    nulterminate(pcmd->ಬಲ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LIST:
    lcmd = (ರಚನೆ listcmd *)cmd;
    nulterminate(lcmd->ಎಡ);
    nulterminate(lcmd->ಬಲ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ ಹಿಂಭಾಗ:
    bcmd = (ರಚನೆ backcmd *)cmd;
    nulterminate(bcmd->cmd);
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cmd;
}
