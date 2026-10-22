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
  पूर्णांक type;
};

संरचना execcmd {
  पूर्णांक type;
  अक्षर *argv[MAXARGS];
  अक्षर *eargv[MAXARGS];
};

संरचना redircmd {
  पूर्णांक type;
  संरचना cmd *cmd;
  अक्षर *file;
  अक्षर *efile;
  पूर्णांक mode;
  पूर्णांक fd;
};

संरचना pipecmd {
  पूर्णांक type;
  संरचना cmd *डावा;
  संरचना cmd *उजवा;
};

संरचना listcmd {
  पूर्णांक type;
  संरचना cmd *डावा;
  संरचना cmd *उजवा;
};

संरचना backcmd {
  पूर्णांक type;
  संरचना cmd *cmd;
};

पूर्णांक fork1(शून्य); // Fork but panics on failure.
शून्य panic(अक्षर *);
संरचना cmd *parsecmd(अक्षर *);
शून्य runcmd(संरचना cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
शून्य
runcmd(संरचना cmd *cmd)
{
  पूर्णांक p[2];
  संरचना backcmd *bcmd;
  संरचना execcmd *ecmd;
  संरचना listcmd *lcmd;
  संरचना pipecmd *pcmd;
  संरचना redircmd *rcmd;

  जर (cmd == 0)
    निकास(1);

  निवड (cmd->type) {
  पूर्वनिर्धारित:
    panic("runcmd");

  स्थिती EXEC:
    ecmd = (संरचना execcmd *)cmd;
    जर (ecmd->argv[0] == 0)
      निकास(1);
    exec(ecmd->argv[0], ecmd->argv);
    ख_लिखो(2, "exec %s failed\n", ecmd->argv[0]);
    थांबा;

  स्थिती REDIR:
    rcmd = (संरचना redircmd *)cmd;
    बंद(rcmd->fd);
    जर (खोलो(rcmd->file, rcmd->mode) < 0) {
      ख_लिखो(2, "open %s failed\n", rcmd->file);
      निकास(1);
    }
    runcmd(rcmd->cmd);
    थांबा;

  स्थिती LIST:
    lcmd = (संरचना listcmd *)cmd;
    जर (fork1() == 0)
      runcmd(lcmd->डावा);
    रुको(0);
    runcmd(lcmd->उजवा);
    थांबा;

  स्थिती PIPE:
    pcmd = (संरचना pipecmd *)cmd;
    जर (pipe(p) < 0)
      panic("pipe");
    जर (fork1() == 0) {
      बंद(1);
      dup(p[1]);
      बंद(p[0]);
      बंद(p[1]);
      runcmd(pcmd->डावा);
    }
    जर (fork1() == 0) {
      बंद(0);
      dup(p[0]);
      बंद(p[0]);
      बंद(p[1]);
      runcmd(pcmd->उजवा);
    }
    बंद(p[0]);
    बंद(p[1]);
    रुको(0);
    रुको(0);
    थांबा;

  स्थिती BACK:
    bcmd = (संरचना backcmd *)cmd;
    जर (fork1() == 0)
      runcmd(bcmd->cmd);
    थांबा;
  }
  निकास(0);
}

पूर्णांक
getcmd(अक्षर *buf, पूर्णांक nbuf)
{
  लिख(2, "$ ", 2);
  स_रखो(buf, 0, nbuf);
  माला_लो(buf, nbuf);
  जर (buf[0] == 0) // EOF
    परत -1;
  परत 0;
}

पूर्णांक
main(शून्य)
{
  स्थिर अक्षर buf[100];
  पूर्णांक fd;

  // Ensure that three file descriptors are open.
  पर्यंत ((fd = खोलो("console", O_RDWR)) >= 0) {
    जर (fd >= 3) {
      बंद(fd);
      थांबा;
    }
  }

  // Read and run input commands.
  पर्यंत (getcmd(buf, आकार(buf)) >= 0) {
    अक्षर *cmd = buf;
    पर्यंत (*cmd == ' ' || *cmd == '\t')
      cmd++;
    जर (*cmd == '\n') // is a blank command
      सुरूठेवा;
    जर (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[म_माप(cmd) - 1] = 0; // chop \n
      जर (स_बदलो(cmd + 3) < 0)
        ख_लिखो(2, "cannot cd %s\n", cmd + 3);
    } अन्यथा {
      जर (fork1() == 0)
        runcmd(parsecmd(cmd));
      रुको(0);
    }
  }
  निकास(0);
}

शून्य
panic(अक्षर *s)
{
  ख_लिखो(2, "%s\n", s);
  निकास(1);
}

पूर्णांक
fork1(शून्य)
{
  पूर्णांक pid;

  pid = विभाजन();
  जर (pid == -1)
    panic("fork");
  परत pid;
}

//PAGEBREAK!
// Constructors

संरचना cmd *
execcmd(शून्य)
{
  संरचना execcmd *cmd;

  cmd = दो_स्मृति(आकार(*cmd));
  स_रखो(cmd, 0, आकार(*cmd));
  cmd->type = EXEC;
  परत (संरचना cmd *)cmd;
}

संरचना cmd *
redircmd(संरचना cmd *subcmd, अक्षर *file, अक्षर *efile, पूर्णांक mode, पूर्णांक fd)
{
  संरचना redircmd *cmd;

  cmd = दो_स्मृति(आकार(*cmd));
  स_रखो(cmd, 0, आकार(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  परत (संरचना cmd *)cmd;
}

संरचना cmd *
pipecmd(संरचना cmd *डावा, संरचना cmd *उजवा)
{
  संरचना pipecmd *cmd;

  cmd = दो_स्मृति(आकार(*cmd));
  स_रखो(cmd, 0, आकार(*cmd));
  cmd->type = PIPE;
  cmd->डावा = डावा;
  cmd->उजवा = उजवा;
  परत (संरचना cmd *)cmd;
}

संरचना cmd *
listcmd(संरचना cmd *डावा, संरचना cmd *उजवा)
{
  संरचना listcmd *cmd;

  cmd = दो_स्मृति(आकार(*cmd));
  स_रखो(cmd, 0, आकार(*cmd));
  cmd->type = LIST;
  cmd->डावा = डावा;
  cmd->उजवा = उजवा;
  परत (संरचना cmd *)cmd;
}

संरचना cmd *
backcmd(संरचना cmd *subcmd)
{
  संरचना backcmd *cmd;

  cmd = दो_स्मृति(आकार(*cmd));
  स_रखो(cmd, 0, आकार(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  परत (संरचना cmd *)cmd;
}
//PAGEBREAK!
// Parsing

अक्षर whitespace[] = " \t\r\n\v";
अक्षर symbols[] = "<|>&;()";

पूर्णांक
gettoken(अक्षर **ps, अक्षर *es, अक्षर **q, अक्षर **eq)
{
  अक्षर *s;
  पूर्णांक ret;

  s = *ps;
  पर्यंत (s < es && म_अक्षर(whitespace, *s))
    s++;
  जर (q)
    *q = s;
  ret = *s;
  निवड (*s) {
  स्थिती 0:
    थांबा;
  स्थिती '|':
  स्थिती '(':
  स्थिती ')':
  स्थिती ';':
  स्थिती '&':
  स्थिती '<':
    s++;
    थांबा;
  स्थिती '>':
    s++;
    जर (*s == '>') {
      ret = '+';
      s++;
    }
    थांबा;
  पूर्वनिर्धारित:
    ret = 'a';
    पर्यंत (s < es && !म_अक्षर(whitespace, *s) && !म_अक्षर(symbols, *s))
      s++;
    थांबा;
  }
  जर (eq)
    *eq = s;

  पर्यंत (s < es && म_अक्षर(whitespace, *s))
    s++;
  *ps = s;
  परत ret;
}

पूर्णांक
peek(अक्षर **ps, अक्षर *es, अक्षर *toks)
{
  अक्षर *s;

  s = *ps;
  पर्यंत (s < es && म_अक्षर(whitespace, *s))
    s++;
  *ps = s;
  परत *s && म_अक्षर(toks, *s);
}

संरचना cmd *parseline(अक्षर **, अक्षर *);
संरचना cmd *parsepipe(अक्षर **, अक्षर *);
संरचना cmd *parseexec(अक्षर **, अक्षर *);
संरचना cmd *nulterminate(संरचना cmd *);

संरचना cmd *
parsecmd(अक्षर *s)
{
  अक्षर *es;
  संरचना cmd *cmd;

  es = s + म_माप(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  जर (s != es) {
    ख_लिखो(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  परत cmd;
}

संरचना cmd *
parseline(अक्षर **ps, अक्षर *es)
{
  संरचना cmd *cmd;

  cmd = parsepipe(ps, es);
  पर्यंत (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  जर (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  परत cmd;
}

संरचना cmd *
parsepipe(अक्षर **ps, अक्षर *es)
{
  संरचना cmd *cmd;

  cmd = parseexec(ps, es);
  जर (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  परत cmd;
}

संरचना cmd *
parseredirs(संरचना cmd *cmd, अक्षर **ps, अक्षर *es)
{
  पूर्णांक tok;
  अक्षर *q, *eq;

  पर्यंत (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    जर (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    निवड (tok) {
    स्थिती '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      थांबा;
    स्थिती '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      थांबा;
    स्थिती '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      थांबा;
    }
  }
  परत cmd;
}

संरचना cmd *
parseblock(अक्षर **ps, अक्षर *es)
{
  संरचना cmd *cmd;

  जर (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  जर (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  परत cmd;
}

संरचना cmd *
parseexec(अक्षर **ps, अक्षर *es)
{
  अक्षर *q, *eq;
  पूर्णांक tok, argc;
  संरचना execcmd *cmd;
  संरचना cmd *ret;

  जर (peek(ps, es, "("))
    परत parseblock(ps, es);

  ret = execcmd();
  cmd = (संरचना execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  पर्यंत (!peek(ps, es, "|)&;")) {
    जर ((tok = gettoken(ps, es, &q, &eq)) == 0)
      थांबा;
    जर (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    जर (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  परत ret;
}

// NUL-terminate all the counted strings.
संरचना cmd *
nulterminate(संरचना cmd *cmd)
{
  पूर्णांक i;
  संरचना backcmd *bcmd;
  संरचना execcmd *ecmd;
  संरचना listcmd *lcmd;
  संरचना pipecmd *pcmd;
  संरचना redircmd *rcmd;

  जर (cmd == 0)
    परत 0;

  निवड (cmd->type) {
  स्थिती EXEC:
    ecmd = (संरचना execcmd *)cmd;
    साठी (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    थांबा;

  स्थिती REDIR:
    rcmd = (संरचना redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    थांबा;

  स्थिती PIPE:
    pcmd = (संरचना pipecmd *)cmd;
    nulterminate(pcmd->डावा);
    nulterminate(pcmd->उजवा);
    थांबा;

  स्थिती LIST:
    lcmd = (संरचना listcmd *)cmd;
    nulterminate(lcmd->डावा);
    nulterminate(lcmd->उजवा);
    थांबा;

  स्थिती BACK:
    bcmd = (संरचना backcmd *)cmd;
    nulterminate(bcmd->cmd);
    थांबा;
  }
  परत cmd;
}
