#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

આયાત argparse, os, inspect, re, signal, subprocess, sys, time
તઃ subprocess આયાત run

sys.stdout.reconfigure(લીટી_buffering=સાચું)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

વર્ગ QEMU(object):

    def __init__(self, reset=ખોટું):
        જો reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), ખોટું)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.સુષુપ્તિ(1)

    def reset_fs(self):
        પ્રયાસ:
            run(["rm", "-f", "fs.img"], check=સાચું)
            run(["make", "fs.img"], check=સાચું)
        અપવાદ subprocess.CalledProcessError ઇવ e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        પ્રયાસ:
            run(["make", "kernel/kernel"], check=સાચું)
        અપવાદ subprocess.CalledProcessError ઇવ e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      પ્રયાસ:
        with open("test-xv6.out", "w") ઇવ f:
            f.લિખ(self.output)
            f.close()
      અપવાદ OSError ઇવ e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        જો isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.લિખ(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [પૂર્ણાંક(લીટી) માટે લીટી માં ps.stdout.splitlines()]
        જો len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def પઠ(self):
        જ્યારે સાચું:
            પ્રયાસ:
                buf = os.પઠ(self.proc.stdout.fileno(), 4096)
            અપવાદ BlockingIOError:
                તોડો
            જો len(buf) == 0:  # qemu exited
                તોડો
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def લીટીઓ(self):
        પાછા self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=સાચું):
        found = ખોટું
        માટે લીટી માં self.લીટીઓ():
            જો any(re.match(r, લીટી) માટે r માં regexps):
                print(લીટી)
                found = સાચું
        જો નહીં found અને exit:
            self.error(*regexps)
        પાછા found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        જો end <= self.reported:
            પાછા
        માટે લીટી માં self.output[self.reported:end].splitlines():
            જો re.match(regexp, લીટી):
                print(લીટી)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        જ્યારે સાચું:
            time.સુષુપ્તિ(1)
            timeleft = deadline - time.time()
            જો timeleft < 0:
                self.error(*regexps)
            self.પઠ()
            જો progress:
                self.progress(progress)
            જો self.match(*regexps, exit=ખોટું):
                પાછા

def crash_log():
    q = QEMU(સાચું)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.સુષુપ્તિ(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.સુષુપ્તિ(2)
    q.પઠ()
    ok = q.match('^recovering', exit=ખોટું)
    જો ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    પાછા ok

def forphan():
    q = QEMU(સાચું)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(સાચું)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.stop()

def test_log():
    print("Test recovery of log")
    માટે i માં range(20):
        crash_log()
        ok = recover_log()
        જો ok:
            print("OK")
            પાછા
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
    જો args.q:
        opt = " -q"
        timeout = 300
    અથવાજો test != "":
        opt += " " + test
    q = QEMU(સાચું)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def મુખ્ય():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) માટે name,obj માં inspect.getmembers(sys.modules[__name__]) 
                     જો (inspect.isfunction(obj) અને 
                         name.startswith('test'))]
    none = સાચું
    માટે (f,n) માં funcs:
        જો re.શોધ(rex, n):
            none = ખોટું
            f()
    જો none:
        test_usertests(test=args.testrex)

મુખ્ય()
