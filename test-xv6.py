#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

आयात argparse, os, inspect, re, signal, subprocess, sys, time
from subprocess आयात आरम्भ

sys.stdout.reconfigure(पंक्ति_buffering=True)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
तर्क = parser.parse_args()

श्रेणी QEMU(object):

    def __init__(self, reset=False):
        अगर reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), False)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.सो_जाओ(1)

    def reset_fs(self):
        प्रयास:
            आरम्भ(["rm", "-f", "fs.img"], check=True)
            आरम्भ(["make", "fs.img"], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        प्रयास:
            आरम्भ(["make", "kernel/kernel"], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      प्रयास:
        with open("test-xv6.out", "w") as f:
            f.म_लिखो(self.output)
            f.बंद()
      except OSError as e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        अगर isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.म_लिखो(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = आरम्भ(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [पूर्णांक(पंक्ति_2) क्रम पंक्ति_2 in ps.stdout.splitlines()]
        अगर len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def विराम(self):
        self.proc.terminate()

    def म_पढ़ो(self):
        जबतक True:
            प्रयास:
                buf = os.म_पढ़ो(self.proc.stdout.fileno(), 4096)
            except BlockingIOError:
                अवरोध
            अगर len(buf) == 0:  # qemu exited
                अवरोध
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def पंक्तियाँ(self):
        वापस self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.विराम()
        sys.exit(1)

    def match(self, *regexps, exit=True):
        found = False
        क्रम पंक्ति_2 in self.पंक्तियाँ():
            अगर any(re.match(r, पंक्ति_2) क्रम r in regexps):
                print(पंक्ति_2)
                found = True
        अगर not found and exit:
            self.error(*regexps)
        वापस found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        अगर end <= self.reported:
            वापस
        क्रम पंक्ति_2 in self.output[self.reported:end].splitlines():
            अगर re.match(regexp, पंक्ति_2):
                print(पंक्ति_2)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        जबतक True:
            time.सो_जाओ(1)
            timeleft = deadline - time.time()
            अगर timeleft < 0:
                self.error(*regexps)
            self.म_पढ़ो()
            अगर progress:
                self.progress(progress)
            अगर self.match(*regexps, exit=False):
                वापस

def crash_log():
    q = QEMU(True)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.सो_जाओ(2)
    q.crash()
    q.विराम()

def recover_log():
    q = QEMU()
    time.सो_जाओ(2)
    q.म_पढ़ो()
    ok = q.match('^recovering', exit=False)
    अगर ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.विराम()
    वापस ok

def forphan():
    q = QEMU(True)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.विराम()

def dorphan():
    q = QEMU(True)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.विराम()

def recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.विराम()

def test_log():
    print("Test recovery of log")
    क्रम i in range(20):
        crash_log()
        ok = recover_log()
        अगर ok:
            print("OK")
            वापस
        print("log attempt ", i+1)
    print("FAIL")
    sys.exit(1)
    
def test_forphan():
    print("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    print("OK")

def test_dorphan():
    print("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    print("OK")

def test_crash():
    test_log()
    test_forphan()
    test_dorphan()

def test_usertests(test=""):
    timeout = 600
    opt = ""
    अगर तर्क.q:
        opt = " -q"
        timeout = 300
    elif test != "":
        opt += " " + test
    q = QEMU(True)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.विराम()

def मुख्य():
    print(तर्क)
    rex = r'%s' % तर्क.testrex
    funcs = [(obj,name) क्रम name,obj in inspect.getmembers(sys.modules[__name__]) 
                     अगर (inspect.isfunction(obj) and 
                         name.startswith('test'))]
    none = True
    क्रम (f,n) in funcs:
        अगर re.खोज(rex, n):
            none = False
            f()
    अगर none:
        test_usertests(test=तर्क.testrex)

मुख्य()
