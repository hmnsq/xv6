// Shell.

#समावेश "kernel/types.h"
#समावेश "user/user.h"
#समावेश "kernel/fcntl.h"

// Parsed command representation
#घोषणा EXEC  1
#घोषणा REDIR 2
#घोषणा PIPE  3
#घोषणा LIST  4
#घोषणा BACK  5

#घोषणा MAXARGS 10

काष्ठा cmd {
  पूर्णांक type;
};

काष्ठा execcmd {
  पूर्णांक type;
  अक्षर *argv[MAXARGS];
  अक्षर *eargv[MAXARGS];
};

काष्ठा redircmd {
  पूर्णांक type;
  काष्ठा cmd *cmd;
  अक्षर *file;
  अक्षर *efile;
  पूर्णांक mode;
  पूर्णांक fd;
};

काष्ठा pipecmd {
  पूर्णांक type;
  काष्ठा cmd *बायाँ;
  काष्ठा cmd *दायाँ;
};

काष्ठा listcmd {
  पूर्णांक type;
  काष्ठा cmd *बायाँ;
  काष्ठा cmd *दायाँ;
};

काष्ठा backcmd {
  पूर्णांक type;
  काष्ठा cmd *cmd;
};

पूर्णांक fork1(व्योम); // Fork but panics on failure.
व्योम panic(अक्षर *);
काष्ठा cmd *parsecmd(अक्षर *);
व्योम runcmd(काष्ठा cmd *) __attribute__((noreturn));

// Execute cmd.  Never returns.
व्योम
runcmd(काष्ठा cmd *cmd)
{
  पूर्णांक p[2];
  काष्ठा backcmd *bcmd;
  काष्ठा execcmd *ecmd;
  काष्ठा listcmd *lcmd;
  काष्ठा pipecmd *pcmd;
  काष्ठा redircmd *rcmd;

  अगर (cmd == 0)
    निकास(1);

  चयन (cmd->type) {
  शेष:
    panic("runcmd");

  हाल EXEC:
    ecmd = (काष्ठा execcmd *)cmd;
    अगर (ecmd->argv[0] == 0)
      निकास(1);
    exec(ecmd->argv[0], ecmd->argv);
    ख_लिखो(2, "exec %s failed\n", ecmd->argv[0]);
    अवरोध;

  हाल REDIR:
    rcmd = (काष्ठा redircmd *)cmd;
    बंद(rcmd->fd);
    अगर (खोलो(rcmd->file, rcmd->mode) < 0) {
      ख_लिखो(2, "open %s failed\n", rcmd->file);
      निकास(1);
    }
    runcmd(rcmd->cmd);
    अवरोध;

  हाल LIST:
    lcmd = (काष्ठा listcmd *)cmd;
    अगर (fork1() == 0)
      runcmd(lcmd->बायाँ);
    रुको(0);
    runcmd(lcmd->दायाँ);
    अवरोध;

  हाल PIPE:
    pcmd = (काष्ठा pipecmd *)cmd;
    अगर (pipe(p) < 0)
      panic("pipe");
    अगर (fork1() == 0) {
      बंद(1);
      dup(p[1]);
      बंद(p[0]);
      बंद(p[1]);
      runcmd(pcmd->बायाँ);
    }
    अगर (fork1() == 0) {
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
    अवरोध;

  हाल BACK:
    bcmd = (काष्ठा backcmd *)cmd;
    अगर (fork1() == 0)
      runcmd(bcmd->cmd);
    अवरोध;
  }
  निकास(0);
}

पूर्णांक
getcmd(अक्षर *buf, पूर्णांक nbuf)
{
  ग_लिखो(2, "$ ", 2);
  स_रखो(buf, 0, nbuf);
  माला_लो(buf, nbuf);
  अगर (buf[0] == 0) // EOF
    वापस -1;
  वापस 0;
}

पूर्णांक
मुख्य(व्योम)
{
  अटल अक्षर buf[100];
  पूर्णांक fd;

  // Ensure that three file descriptors are open.
  जबतक ((fd = खोलो("console", O_RDWR)) >= 0) {
    अगर (fd >= 3) {
      बंद(fd);
      अवरोध;
    }
  }

  // Read and run input commands.
  जबतक (getcmd(buf, माप(buf)) >= 0) {
    अक्षर *cmd = buf;
    जबतक (*cmd == ' ' || *cmd == '\t')
      cmd++;
    अगर (*cmd == '\n') // is a blank command
      जारी;
    अगर (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      cmd[म_माप(cmd) - 1] = 0; // chop \n
      अगर (स_बदलो(cmd + 3) < 0)
        ख_लिखो(2, "cannot cd %s\n", cmd + 3);
    } अन्यथा {
      अगर (fork1() == 0)
        runcmd(parsecmd(cmd));
      रुको(0);
    }
  }
  निकास(0);
}

व्योम
panic(अक्षर *s)
{
  ख_लिखो(2, "%s\n", s);
  निकास(1);
}

पूर्णांक
fork1(व्योम)
{
  पूर्णांक pid;

  pid = विभाजन();
  अगर (pid == -1)
    panic("fork");
  वापस pid;
}

//PAGEBREAK!
// Constructors

काष्ठा cmd *
execcmd(व्योम)
{
  काष्ठा execcmd *cmd;

  cmd = दो_स्मृति(माप(*cmd));
  स_रखो(cmd, 0, माप(*cmd));
  cmd->type = EXEC;
  वापस (काष्ठा cmd *)cmd;
}

काष्ठा cmd *
redircmd(काष्ठा cmd *subcmd, अक्षर *file, अक्षर *efile, पूर्णांक mode, पूर्णांक fd)
{
  काष्ठा redircmd *cmd;

  cmd = दो_स्मृति(माप(*cmd));
  स_रखो(cmd, 0, माप(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  वापस (काष्ठा cmd *)cmd;
}

काष्ठा cmd *
pipecmd(काष्ठा cmd *बायाँ, काष्ठा cmd *दायाँ)
{
  काष्ठा pipecmd *cmd;

  cmd = दो_स्मृति(माप(*cmd));
  स_रखो(cmd, 0, माप(*cmd));
  cmd->type = PIPE;
  cmd->बायाँ = बायाँ;
  cmd->दायाँ = दायाँ;
  वापस (काष्ठा cmd *)cmd;
}

काष्ठा cmd *
listcmd(काष्ठा cmd *बायाँ, काष्ठा cmd *दायाँ)
{
  काष्ठा listcmd *cmd;

  cmd = दो_स्मृति(माप(*cmd));
  स_रखो(cmd, 0, माप(*cmd));
  cmd->type = LIST;
  cmd->बायाँ = बायाँ;
  cmd->दायाँ = दायाँ;
  वापस (काष्ठा cmd *)cmd;
}

काष्ठा cmd *
backcmd(काष्ठा cmd *subcmd)
{
  काष्ठा backcmd *cmd;

  cmd = दो_स्मृति(माप(*cmd));
  स_रखो(cmd, 0, माप(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  वापस (काष्ठा cmd *)cmd;
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
  जबतक (s < es && म_अक्षर(whitespace, *s))
    s++;
  अगर (q)
    *q = s;
  ret = *s;
  चयन (*s) {
  हाल 0:
    अवरोध;
  हाल '|':
  हाल '(':
  हाल ')':
  हाल ';':
  हाल '&':
  हाल '<':
    s++;
    अवरोध;
  हाल '>':
    s++;
    अगर (*s == '>') {
      ret = '+';
      s++;
    }
    अवरोध;
  शेष:
    ret = 'a';
    जबतक (s < es && !म_अक्षर(whitespace, *s) && !म_अक्षर(symbols, *s))
      s++;
    अवरोध;
  }
  अगर (eq)
    *eq = s;

  जबतक (s < es && म_अक्षर(whitespace, *s))
    s++;
  *ps = s;
  वापस ret;
}

पूर्णांक
peek(अक्षर **ps, अक्षर *es, अक्षर *toks)
{
  अक्षर *s;

  s = *ps;
  जबतक (s < es && म_अक्षर(whitespace, *s))
    s++;
  *ps = s;
  वापस *s && म_अक्षर(toks, *s);
}

काष्ठा cmd *parseline(अक्षर **, अक्षर *);
काष्ठा cmd *parsepipe(अक्षर **, अक्षर *);
काष्ठा cmd *parseexec(अक्षर **, अक्षर *);
काष्ठा cmd *nulterminate(काष्ठा cmd *);

काष्ठा cmd *
parsecmd(अक्षर *s)
{
  अक्षर *es;
  काष्ठा cmd *cmd;

  es = s + म_माप(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  अगर (s != es) {
    ख_लिखो(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  वापस cmd;
}

काष्ठा cmd *
parseline(अक्षर **ps, अक्षर *es)
{
  काष्ठा cmd *cmd;

  cmd = parsepipe(ps, es);
  जबतक (peek(ps, es, "&")) {
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  अगर (peek(ps, es, ";")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  वापस cmd;
}

काष्ठा cmd *
parsepipe(अक्षर **ps, अक्षर *es)
{
  काष्ठा cmd *cmd;

  cmd = parseexec(ps, es);
  अगर (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  वापस cmd;
}

काष्ठा cmd *
parseredirs(काष्ठा cmd *cmd, अक्षर **ps, अक्षर *es)
{
  पूर्णांक tok;
  अक्षर *q, *eq;

  जबतक (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    अगर (gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    चयन (tok) {
    हाल '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      अवरोध;
    हाल '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE | O_TRUNC, 1);
      अवरोध;
    हाल '+': // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY | O_CREATE, 1);
      अवरोध;
    }
  }
  वापस cmd;
}

काष्ठा cmd *
parseblock(अक्षर **ps, अक्षर *es)
{
  काष्ठा cmd *cmd;

  अगर (!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  अगर (!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  वापस cmd;
}

काष्ठा cmd *
parseexec(अक्षर **ps, अक्षर *es)
{
  अक्षर *q, *eq;
  पूर्णांक tok, argc;
  काष्ठा execcmd *cmd;
  काष्ठा cmd *ret;

  अगर (peek(ps, es, "("))
    वापस parseblock(ps, es);

  ret = execcmd();
  cmd = (काष्ठा execcmd *)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  जबतक (!peek(ps, es, "|)&;")) {
    अगर ((tok = gettoken(ps, es, &q, &eq)) == 0)
      अवरोध;
    अगर (tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    अगर (argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  वापस ret;
}

// NUL-terminate all the counted strings.
काष्ठा cmd *
nulterminate(काष्ठा cmd *cmd)
{
  पूर्णांक i;
  काष्ठा backcmd *bcmd;
  काष्ठा execcmd *ecmd;
  काष्ठा listcmd *lcmd;
  काष्ठा pipecmd *pcmd;
  काष्ठा redircmd *rcmd;

  अगर (cmd == 0)
    वापस 0;

  चयन (cmd->type) {
  हाल EXEC:
    ecmd = (काष्ठा execcmd *)cmd;
    क्रम (i = 0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    अवरोध;

  हाल REDIR:
    rcmd = (काष्ठा redircmd *)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    अवरोध;

  हाल PIPE:
    pcmd = (काष्ठा pipecmd *)cmd;
    nulterminate(pcmd->बायाँ);
    nulterminate(pcmd->दायाँ);
    अवरोध;

  हाल LIST:
    lcmd = (काष्ठा listcmd *)cmd;
    nulterminate(lcmd->बायाँ);
    nulterminate(lcmd->दायाँ);
    अवरोध;

  हाल BACK:
    bcmd = (काष्ठा backcmd *)cmd;
    nulterminate(bcmd->cmd);
    अवरोध;
  }
  वापस cmd;
}
