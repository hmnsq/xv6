#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

இறக்குமதி argparse, os, inspect, re, signal, subprocess, sys, time
தஃ subprocess இறக்குமதி run

sys.stdout.reconfigure(வரி_buffering=உண்மை)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

வகுப்பு QEMU(object):

    def __init__(self, reset=பொய்):
        எனில் reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), பொய்)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.ஸுஷுப்தி(1)

    def reset_fs(self):
        முயல்:
            run(["rm", "-f", "fs.img"], check=உண்மை)
            run(["make", "fs.img"], check=உண்மை)
        விதிவிலக்கு subprocess.CalledProcessError இவ e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        முயல்:
            run(["make", "kernel/kernel"], check=உண்மை)
        விதிவிலக்கு subprocess.CalledProcessError இவ e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      முயல்:
        with open("test-xv6.out", "w") இவ f:
            f.லிக(self.output)
            f.close()
      விதிவிலக்கு OSError இவ e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        எனில் isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.லிக(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [முழு(வரி) க்கு வரி உள் ps.stdout.splitlines()]
        எனில் len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def பட(self):
        போது உண்மை:
            முயல்:
                buf = os.பட(self.proc.stdout.fileno(), 4096)
            விதிவிலக்கு BlockingIOError:
                நிறுத்து
            எனில் len(buf) == 0:  # qemu exited
                நிறுத்து
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def வரிகள்(self):
        திரும்பு self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=உண்மை):
        found = பொய்
        க்கு வரி உள் self.வரிகள்():
            எனில் any(re.match(r, வரி) க்கு r உள் regexps):
                print(வரி)
                found = உண்மை
        எனில் இல்லை found மற்றும் exit:
            self.error(*regexps)
        திரும்பு found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        எனில் end <= self.reported:
            திரும்பு
        க்கு வரி உள் self.output[self.reported:end].splitlines():
            எனில் re.match(regexp, வரி):
                print(வரி)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        போது உண்மை:
            time.ஸுஷுப்தி(1)
            timeleft = deadline - time.time()
            எனில் timeleft < 0:
                self.error(*regexps)
            self.பட()
            எனில் progress:
                self.progress(progress)
            எனில் self.match(*regexps, exit=பொய்):
                திரும்பு

def crash_log():
    q = QEMU(உண்மை)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.ஸுஷுப்தி(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.ஸுஷுப்தி(2)
    q.பட()
    ok = q.match('^recovering', exit=பொய்)
    எனில் ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    திரும்பு ok

def forphan():
    q = QEMU(உண்மை)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(உண்மை)
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
    க்கு i உள் range(20):
        crash_log()
        ok = recover_log()
        எனில் ok:
            print("OK")
            திரும்பு
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
    எனில் args.q:
        opt = " -q"
        timeout = 300
    அல்லதெனில் test != "":
        opt += " " + test
    q = QEMU(உண்மை)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def முதன்மை():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) க்கு name,obj உள் inspect.getmembers(sys.modules[__name__]) 
                     எனில் (inspect.isfunction(obj) மற்றும் 
                         name.startswith('test'))]
    none = உண்மை
    க்கு (f,n) உள் funcs:
        எனில் re.தேடல்(rex, n):
            none = பொய்
            f()
    எனில் none:
        test_usertests(test=args.testrex)

முதன்மை()
