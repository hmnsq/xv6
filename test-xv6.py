#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

ଆମଦାନୀ argparse, os, inspect, re, signal, subprocess, sys, time
ତଃ subprocess ଆମଦାନୀ run

sys.stdout.reconfigure(ଧାଡ଼ି_buffering=ସତ୍ୟ)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

ଶ୍ରେଣୀ QEMU(object):

    def __init__(self, reset=ମିଥ୍ୟା):
        ଯଦି reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), ମିଥ୍ୟା)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.ସୁଷୁପ୍ତି(1)

    def reset_fs(self):
        ଚେଷ୍ଟା:
            run(["rm", "-f", "fs.img"], check=ସତ୍ୟ)
            run(["make", "fs.img"], check=ସତ୍ୟ)
        ବ୍ୟତିକ୍ରମ subprocess.CalledProcessError ଇଵ e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        ଚେଷ୍ଟା:
            run(["make", "kernel/kernel"], check=ସତ୍ୟ)
        ବ୍ୟତିକ୍ରମ subprocess.CalledProcessError ଇଵ e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      ଚେଷ୍ଟା:
        with open("test-xv6.out", "w") ଇଵ f:
            f.ଲିଖ(self.output)
            f.close()
      ବ୍ୟତିକ୍ରମ OSError ଇଵ e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        ଯଦି isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.ଲିଖ(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [ପୂର୍ଣ୍ଣସଂଖ୍ୟା(ଧାଡ଼ି) ପାଇଁ ଧାଡ଼ି ଭିତରେ ps.stdout.splitlines()]
        ଯଦି len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def ପଠ(self):
        ଯେତେବେଳେ ସତ୍ୟ:
            ଚେଷ୍ଟା:
                buf = os.ପଠ(self.proc.stdout.fileno(), 4096)
            ବ୍ୟତିକ୍ରମ BlockingIOError:
                ଭାଙ୍ଗ
            ଯଦି len(buf) == 0:  # qemu exited
                ଭାଙ୍ଗ
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def ଧାଡ଼ିଗୁଡ଼ିକ(self):
        ଫେରନ୍ତୁ self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=ସତ୍ୟ):
        found = ମିଥ୍ୟା
        ପାଇଁ ଧାଡ଼ି ଭିତରେ self.ଧାଡ଼ିଗୁଡ଼ିକ():
            ଯଦି any(re.match(r, ଧାଡ଼ି) ପାଇଁ r ଭିତରେ regexps):
                print(ଧାଡ଼ି)
                found = ସତ୍ୟ
        ଯଦି ନୁହେଁ found ଏବଂ exit:
            self.error(*regexps)
        ଫେରନ୍ତୁ found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        ଯଦି end <= self.reported:
            ଫେରନ୍ତୁ
        ପାଇଁ ଧାଡ଼ି ଭିତରେ self.output[self.reported:end].splitlines():
            ଯଦି re.match(regexp, ଧାଡ଼ି):
                print(ଧାଡ଼ି)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        ଯେତେବେଳେ ସତ୍ୟ:
            time.ସୁଷୁପ୍ତି(1)
            timeleft = deadline - time.time()
            ଯଦି timeleft < 0:
                self.error(*regexps)
            self.ପଠ()
            ଯଦି progress:
                self.progress(progress)
            ଯଦି self.match(*regexps, exit=ମିଥ୍ୟା):
                ଫେରନ୍ତୁ

def crash_log():
    q = QEMU(ସତ୍ୟ)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.ସୁଷୁପ୍ତି(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.ସୁଷୁପ୍ତି(2)
    q.ପଠ()
    ok = q.match('^recovering', exit=ମିଥ୍ୟା)
    ଯଦି ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    ଫେରନ୍ତୁ ok

def forphan():
    q = QEMU(ସତ୍ୟ)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(ସତ୍ୟ)
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
    ପାଇଁ i ଭିତରେ range(20):
        crash_log()
        ok = recover_log()
        ଯଦି ok:
            print("OK")
            ଫେରନ୍ତୁ
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
    ଯଦି args.q:
        opt = " -q"
        timeout = 300
    ନଚେତଯଦି test != "":
        opt += " " + test
    q = QEMU(ସତ୍ୟ)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def ମୁଖ୍ୟ():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) ପାଇଁ name,obj ଭିତରେ inspect.getmembers(sys.modules[__name__]) 
                     ଯଦି (inspect.isfunction(obj) ଏବଂ 
                         name.startswith('test'))]
    none = ସତ୍ୟ
    ପାଇଁ (f,n) ଭିତରେ funcs:
        ଯଦି re.ସନ୍ଧାନ(rex, n):
            none = ମିଥ୍ୟା
            f()
    ଯଦି none:
        test_usertests(test=args.testrex)

ମୁଖ୍ୟ()
