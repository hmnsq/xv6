// Shell.

#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

संरचना cmd {
  पूर्णाङ्क type;
};

संरचना execcmd {
  पूर्णाङ्क type;
  वर्ण *argv[MAXARGS];
  वर्ण *eargv[MAXARGS];
};

संरचना redircmd {
  पूर्णाङ्क type;
  संरचना cmd *cmd;
  वर्ण *file;
  वर्ण *efile;
  पूर्णाङ्क mode;
  पूर्णाङ्क fd;
};

संरचना pipecmd {
  पूर्णाङ्क type;
  संरचना cmd *बायाँ;
  संरचना cmd *दायाँ;
};

संरचना listcmd {
  पूर्णाङ्क type;
  संरचना cmd *बायाँ;
  संरचना cmd *दायाँ;
};

संरचना backcmd {
  पूर्णाङ्क type;
  संरचना cmd *cmd;
};

पूर्णाङ्क fork1(शून्य); // Fork but panics on failure.
शून्य panic(वर्ण *);
संरचना cmd *parsecmd(वर्ण *);
शून्य runcmd(संरचना cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
शून्य
runcmd(संरचना cmd *cmd)
{
  पूर्णाङ्क p[2];
  संरचना backcmd *bcmd;
  संरचना execcmd *ecmd;
  संरचना listcmd *lcmd;
  संरचना pipecmd *pcmd;
  संरचना redircmd *rcmd;

  यदि (cmd == 0)
    निकास(1);

  विकल्प (cmd->type) {
  स्वभाव:
    panic("runcmd");

  पक्ष EXEC:
    ecmd = (संरचना execcmd *)cmd;
    यदि (ecmd->argv[0] == 0)
      निकास(1);
    exec(ecmd->argv[0], ecmd->argv);
    ख_लिखो(2, "exec %s failed\n", ecmd->argv[0]);
    रोक;

  पक्ष REDIR:
    rcmd = (संरचना redircmd *)cmd;
    बंद(rcmd->fd);
    यदि (खोलो(rcmd->file, rcmd->mode) < 0) {
      ख_लिखो(2, "open %s failed\n", rcmd->file);
      निकास(1);
    }
    runcmd(rcmd->cmd);
    रोक;

  पक्ष LIST:
    lcmd = (संरचना listcmd *)cmd;
    यदि (fork1() == 0)
      runcmd(lcmd->बायाँ);
    रुको(0);
    runcmd(lcmd->दायाँ);
    रोक;

  पक्ष PIPE:
    pcmd = (संरचना pipecmd *)cmd;
    यदि (pipe(p) < 0)
      panic("pipe");
    यदि (fork1() == 0) {
      बंद(1);
      dup(p[1]);
      बंद(p[0]);
      बंद(p[1]);
      runcmd(pcmd->बायाँ);
    }
    यदि (fork1() == 0) {
      बंद(0);
      dup(p[0]);
      बंद(p[0]);
      बंद(p[1]);
      runcmd(pcmd->दायाँ);
    }
    बंद(p[0]);
    बंद(p[1]);
    रुको(0);
    रुको(0);
    रोक;

  पक्ष BACK:
    bcmd = (संरचना backcmd *)cmd;
    यदि (fork1() == 0)
      runcmd(bcmd->cmd);
    रोक;
  }
  निकास(0);
}

पूर्णाङ्क
getcmd(वर्ण *buf, पूर्णाङ्क nbuf)
{
  लिख(2, "$ ", 2);
  स_रखो(buf, 0, nbuf);
  माला_लो(buf, nbuf);
  यदि (buf[0] == 0) // EOF
    फर्काऊ -1;
  फर्काऊ 0;
}

पूर्णाङ्क
main(शून्य)
{
  स्थिर वर्ण buf[100];
  पूर्णाङ्क fd;

  // Ensure that three file descriptors are open.
  जबसम्म ((fd = खोलो("console", O_RDWR)) >= 0) {
    यदि (fd >= 3) {
      बंद(fd);
      रोक;
    }
  }

  // Read and run input commands.
  जबसम्म (getcmd(buf, परिमाण(buf)) >= 0) {
    वर्ण *cmd = buf;
    जबसम्म (*cmd == ' ' || *cmd == '\t')
      cmd++;
    यदि (*cmd == '\n') // is a blank command
      जारी;
    यदि (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[म_माप(cmd) - 1] = 0; // chop \n
      यदि (स_बदलो(cmd + 3) < 0)
        ख_लिखो(2, "cannot cd %s\n", cmd + 3);
    } अन्यथा {
      यदि (fork1() == 0)
        runcmd(parsecmd(cmd));
      रुको(0);
    }
  }
  निकास(0);
}

शून्य
panic(वर्ण *s)
{
  ख_लिखो(2, "%s\n", s);
  निकास(1);
}

पूर्णाङ्क
fork1(शून्य)
{
  पूर्णाङ्क pid;

  pid = विभाजन();
  यदि (pid == -1)
    panic("fork");
  फर्काऊ pid;
}

//PAGEBREAK!
// Constructors

संरचना cmd *
execcmd(शून्य)
{
  संरचना execcmd *cmd;

  cmd = दो_स्मृति(परिमाण(*cmd));
  स_रखो(cmd, 0, परिमाण(*cmd));
  cmd->type = EXEC;
  फर्काऊ (संरचना cmd *)cmd;
}

संरचना cmd *
redircmd(संरचना cmd *subcmd, वर्ण *file, वर्ण *efile, पूर्णाङ्क mode, पूर्णाङ्क fd)
{
  संरचना redircmd *cmd;

  cmd = दो_स्मृति(परिमाण(*cmd));
  स_रखो(cmd, 0, परिमाण(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  फर्काऊ (संरचना cmd *)cmd;
}

संरचना cmd *
pipecmd(संरचना cmd *बायाँ, संरचना cmd *दायाँ)
{
  संरचना pipecmd *cmd;

  cmd = दो_स्मृति(परिमाण(*cmd));
  स_रखो(cmd, 0, परिमाण(*cmd));
  cmd->type = PIPE;
  cmd->बायाँ = बायाँ;
  cmd->दायाँ = दायाँ;
  फर्काऊ (संरचना cmd *)cmd;
}

संरचना cmd *
listcmd(संरचना cmd *बायाँ, संरचना cmd *दायाँ)
{
  संरचना listcmd *cmd;

  cmd = दो_स्मृति(परिमाण(*cmd));
  स_रखो(cmd, 0, परिमाण(*cmd));
  cmd->type = LIST;
  cmd->बायाँ = बायाँ;
  cmd->दायाँ = दायाँ;
  फर्काऊ (संरचना cmd *)cmd;
}

संरचना cmd *
backcmd(संरचना cmd *subcmd)
{
  संरचना backcmd *cmd;

  cmd = दो_स्मृति(परिमाण(*cmd));
  स_रखो(cmd, 0, परिमाण(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  फर्काऊ (संरचना cmd *)cmd;
}
//PAGEBREAK!
// Parsing

वर्ण whitespace[] = " \t\r\n\v";
वर्ण symbols[] = "<|>&;()";

पूर्णाङ्क
gettoken(वर्ण **ps, वर्ण *es, वर्ण **q, वर्ण **eq)
{
  वर्ण *s;
  पूर्णाङ्क ret;

  s = *ps;
  जबसम्म (s < es && म_अक्षर(whitespace, *s))
    s++;
  यदि (q)
    *q = s;
  ret = *s;
  विकल्प (*s) {
  पक्ष 0:
    रोक;
  पक्ष '|':
  पक्ष '(':
  पक्ष ')':
  पक्ष ';':
  पक्ष '&':
  पक्ष '<':
    s++;
    रोक;
  पक्ष '>':
    s++;
    यदि (*s == '>') {
      ret = '+';
      s++;
    }
    रोक;
  स्वभाव:
    ret = 'a';
    जबसम्म (s < es && !म_अक्षर(whitespace, *s) && !म_अक्षर(symbols, *s))
      s++;
    रोक;
  }
  यदि (eq)
    *eq = s;

  जबसम्म (s < es && म_अक्षर(whitespace, *s))
    s++;
  *ps = s;
  फर्काऊ ret;
}

पूर्णाङ्क
peek(वर्ण **ps, वर्ण *es, वर्ण *toks)
{
  वर्ण *s;

  s = *ps;
  जबसम्म (s < es && म_अक्षर(whitespace, *s))
    s++;
  *ps = s;
  फर्काऊ *s && म_अक्षर(toks, *s);
}

संरचना cmd *parseline(वर्ण **, वर्ण *);
संरचना cmd *parsepipe(वर्ण **, वर्ण *);
संरचना cmd *parseexec(वर्ण **, वर्ण *);
संरचना cmd *nulterminate(संरचना cmd *);

संरचना cmd *
parsecmd(वर्ण *s)
{
  वर्ण *es;
  संरचना cmd *cmd;

  es = s + म_माप(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  यदि (s != es) {
    ख_लिखो(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  फर्काऊ cmd;
}

संरचना cmd *
parseline(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  cmd = parsepipe(ps, es);
  जबसम्म (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  यदि (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  फर्काऊ cmd;
}

संरचना cmd *
parsepipe(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  cmd = parseexec(ps, es);
  यदि (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  फर्काऊ cmd;
}

संरचना cmd *
parseredirs(संरचना cmd *cmd, वर्ण **ps, वर्ण *es)
{
  पूर्णाङ्क tok;
  वर्ण *q, *eq;

  जबसम्म (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    यदि (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    विकल्प (tok) {
    पक्ष '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      रोक;
    पक्ष '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      रोक;
    पक्ष '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      रोक;
    }
  }
  फर्काऊ cmd;
}

संरचना cmd *
parseblock(वर्ण **ps, वर्ण *es)
{
  संरचना cmd *cmd;

  यदि (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  यदि (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  फर्काऊ cmd;
}

संरचना cmd *
parseexec(वर्ण **ps, वर्ण *es)
{
  वर्ण *q, *eq;
  पूर्णाङ्क tok, argc;
  संरचना execcmd *cmd;
  संरचना cmd *ret;

  यदि (peek(ps, es, "("))
    फर्काऊ parseblock(ps, es);

  ret = execcmd();
  cmd = (संरचना execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  जबसम्म (!peek(ps, es, "|)&;")) {
    यदि ((tok = gettoken(ps, es, &q, &eq)) == 0)
      रोक;
    यदि (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    यदि (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  फर्काऊ ret;
}

// NUL-terminate all the counted strings.
संरचना cmd *
nulterminate(संरचना cmd *cmd)
{
  पूर्णाङ्क i;
  संरचना backcmd *bcmd;
  संरचना execcmd *ecmd;
  संरचना listcmd *lcmd;
  संरचना pipecmd *pcmd;
  संरचना redircmd *rcmd;

  यदि (cmd == 0)
    फर्काऊ 0;

  विकल्प (cmd->type) {
  पक्ष EXEC:
    ecmd = (संरचना execcmd *)cmd;
    लागि (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    रोक;

  पक्ष REDIR:
    rcmd = (संरचना redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    रोक;

  पक्ष PIPE:
    pcmd = (संरचना pipecmd *)cmd;
    nulterminate(pcmd->बायाँ);
    nulterminate(pcmd->दायाँ);
    रोक;

  पक्ष LIST:
    lcmd = (संरचना listcmd *)cmd;
    nulterminate(lcmd->बायाँ);
    nulterminate(lcmd->दायाँ);
    रोक;

  पक्ष BACK:
    bcmd = (संरचना backcmd *)cmd;
    nulterminate(bcmd->cmd);
    रोक;
  }
  फर्काऊ cmd;
}
