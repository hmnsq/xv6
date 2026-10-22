#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

దిగుమతి argparse, os, inspect, re, signal, subprocess, sys, సమయం
తః subprocess దిగుమతి నిరంతరం

sys.stdout.reconfigure(పంక్తి_buffering=నిజం)

parser = argparse.వాదం_Parser()
parser.కూడు_వాదం('testrex', help="test name or regular expression")
parser.కూడు_వాదం("-q", action='store_true', help="usertests quick")
వాదనలు = parser.parse_వాదనలు()

తరగతి QEMU(object):

    నిర్వచనం __init__(self, reset=అబద్ధం):
        ఒకవేళ reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.సమితి_blocking(self.proc.stdout.fileno(), అబద్ధం)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        సమయం.సుషుప్తి(1)

    నిర్వచనం reset_fs(self):
        ప్రయత్నించు:
            నిరంతరం(["rm", "-f", "fs.img"], check=నిజం)
            నిరంతరం(["make", "fs.img"], check=నిజం)
        మినహాయింపు subprocess.CalledProcessError ఇవ e:
            ముద్రించు(f"Command failed with exit code {e.returncode}")

    నిర్వచనం build_xv6(self):
        ప్రయత్నించు:
            నిరంతరం(["make", "kernel/kernel"], check=నిజం)
        మినహాయింపు subprocess.CalledProcessError ఇవ e:
            ముద్రించు(f"Command failed with exit code {e.returncode}")

    నిర్వచనం save_output(self):
      ప్రయత్నించు:
        తోడుగా open("test-xv6.out", "w") ఇవ f:
            f.లిఖ(self.output)
            f.close()
      మినహాయింపు OSError ఇవ e:
        ముద్రించు("Provided a bad results path. Error:", e)     
        
    నిర్వచనం cmd(self, c):
        ఒకవేళ isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.లిఖ(c)
        self.proc.stdin.flush()
        
    నిర్వచనం crash(self):
        ps = నిరంతరం(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [పూర్ణాంకం(పంక్తి) కొరకు పంక్తి లో ps.stdout.splitlines()]
        ఒకవేళ పొడవు(kids) == 0:
            ముద్రించు("no qemu")
            sys.exit(1)
        ముద్రించు("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    నిర్వచనం stop(self):
        self.proc.terminate()

    నిర్వచనం పఠ(self):
        అయితే నిజం:
            ప్రయత్నించు:
                buf = os.పఠ(self.proc.stdout.fileno(), 4096)
            మినహాయింపు BlockingIOError:
                ఆపు
            ఒకవేళ పొడవు(buf) == 0:  # qemu exited
                ఆపు
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    నిర్వచనం పంక్తులు(self):
        తిరిగి self.output.splitlines()

    నిర్వచనం error(self, *regexps):
        ముద్రించు("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    నిర్వచనం సరిపోలిక(self, *regexps, exit=నిజం):
        దొరికింది = అబద్ధం
        కొరకు పంక్తి లో self.పంక్తులు():
            ఒకవేళ any(re.సరిపోలిక(r, పంక్తి) కొరకు r లో regexps):
                ముద్రించు(పంక్తి)
                దొరికింది = నిజం
        ఒకవేళ కాదు దొరికింది మరియు exit:
            self.error(*regexps)
        తిరిగి దొరికింది

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    నిర్వచనం పురోగతి(self, regexp):
        చివర = self.output.rfind("\n") + 1
        ఒకవేళ చివర <= self.reported:
            తిరిగి
        కొరకు పంక్తి లో self.output[self.reported:చివర].splitlines():
            ఒకవేళ re.సరిపోలిక(regexp, పంక్తి):
                ముద్రించు(పంక్తి)
        self.reported = చివర

    నిర్వచనం monitor(self, *regexps, పురోగతి="", timeout):
        deadline = సమయం.సమయం() + timeout
        అయితే నిజం:
            సమయం.సుషుప్తి(1)
            timeleft = deadline - సమయం.సమయం()
            ఒకవేళ timeleft < 0:
                self.error(*regexps)
            self.పఠ()
            ఒకవేళ పురోగతి:
                self.పురోగతి(పురోగతి)
            ఒకవేళ self.సరిపోలిక(*regexps, exit=అబద్ధం):
                తిరిగి

నిర్వచనం crash_log():
    q = QEMU(నిజం)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    సమయం.సుషుప్తి(2)
    q.crash()
    q.stop()

నిర్వచనం recover_log():
    q = QEMU()
    సమయం.సుషుప్తి(2)
    q.పఠ()
    ok = q.సరిపోలిక('^recovering', exit=అబద్ధం)
    ఒకవేళ ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    తిరిగి ok

నిర్వచనం forphan():
    q = QEMU(నిజం)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

నిర్వచనం dorphan():
    q = QEMU(నిజం)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

నిర్వచనం recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.stop()

నిర్వచనం test_log():
    ముద్రించు("Test recovery of log")
    కొరకు i లో శ్రేణి(20):
        crash_log()
        ok = recover_log()
        ఒకవేళ ok:
            ముద్రించు("OK")
            తిరిగి
        ముద్రించు("log attempt ", i+1)
    ముద్రించు("FAIL")
    sys.exit(1)
    
నిర్వచనం test_forphan():
    ముద్రించు("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    ముద్రించు("OK")

నిర్వచనం test_dorphan():
    ముద్రించు("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    ముద్రించు("OK")

నిర్వచనం test_crash():
    test_log()
    test_forphan()
    test_dorphan()

నిర్వచనం test_usertests(test=""):
    timeout = 600
    opt = ""
    ఒకవేళ వాదనలు.q:
        opt = " -q"
        timeout = 300
    లేదావేళ test != "":
        opt += " " + test
    q = QEMU(నిజం)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', పురోగతి='test', timeout=timeout)
    q.stop()

నిర్వచనం ప్రధాన():
    ముద్రించు(వాదనలు)
    rex = r'%s' % వాదనలు.testrex
    funcs = [(obj,name) కొరకు name,obj లో inspect.getmembers(sys.modules[__name__]) 
                     ఒకవేళ (inspect.isfunction(obj) మరియు 
                         name.startswith('test'))]
    none = నిజం
    కొరకు (f,n) లో funcs:
        ఒకవేళ re.శోధన(rex, n):
            none = అబద్ధం
            f()
    ఒకవేళ none:
        test_usertests(test=వాదనలు.testrex)

ప్రధాన()
