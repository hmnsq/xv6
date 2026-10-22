#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

আমদানি argparse, os, inspect, re, signal, subprocess, sys, time
তঃ subprocess আমদানি run

sys.stdout.reconfigure(সারি_buffering=সত্য)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

শ্রেণী QEMU(object):

    def __init__(self, reset=মিথ্যা):
        যদি reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), মিথ্যা)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.সুষুপ্তি(1)

    def reset_fs(self):
        চেষ্টা:
            run(["rm", "-f", "fs.img"], check=সত্য)
            run(["make", "fs.img"], check=সত্য)
        ব্যতিক্রম subprocess.CalledProcessError ইব e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        চেষ্টা:
            run(["make", "kernel/kernel"], check=সত্য)
        ব্যতিক্রম subprocess.CalledProcessError ইব e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      চেষ্টা:
        with open("test-xv6.out", "w") ইব f:
            f.লিখ(self.output)
            f.close()
      ব্যতিক্রম OSError ইব e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        যদি isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.লিখ(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [পূর্ণসংখ্যা(সারি) জন্য সারি মধ্যে ps.stdout.splitlines()]
        যদি len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def পঠ(self):
        যখন সত্য:
            চেষ্টা:
                buf = os.পঠ(self.proc.stdout.fileno(), 4096)
            ব্যতিক্রম BlockingIOError:
                বিরতি
            যদি len(buf) == 0:  # qemu exited
                বিরতি
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def সারিগুলি(self):
        ফেরত self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=সত্য):
        found = মিথ্যা
        জন্য সারি মধ্যে self.সারিগুলি():
            যদি any(re.match(r, সারি) জন্য r মধ্যে regexps):
                print(সারি)
                found = সত্য
        যদি নয় found এবং exit:
            self.error(*regexps)
        ফেরত found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        যদি end <= self.reported:
            ফেরত
        জন্য সারি মধ্যে self.output[self.reported:end].splitlines():
            যদি re.match(regexp, সারি):
                print(সারি)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        যখন সত্য:
            time.সুষুপ্তি(1)
            timeleft = deadline - time.time()
            যদি timeleft < 0:
                self.error(*regexps)
            self.পঠ()
            যদি progress:
                self.progress(progress)
            যদি self.match(*regexps, exit=মিথ্যা):
                ফেরত

def crash_log():
    q = QEMU(সত্য)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.সুষুপ্তি(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.সুষুপ্তি(2)
    q.পঠ()
    ok = q.match('^recovering', exit=মিথ্যা)
    যদি ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    ফেরত ok

def forphan():
    q = QEMU(সত্য)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(সত্য)
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
    জন্য i মধ্যে range(20):
        crash_log()
        ok = recover_log()
        যদি ok:
            print("OK")
            ফেরত
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
    যদি args.q:
        opt = " -q"
        timeout = 300
    নতুবা_যদি test != "":
        opt += " " + test
    q = QEMU(সত্য)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def প্রধান():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) জন্য name,obj মধ্যে inspect.getmembers(sys.modules[__name__]) 
                     যদি (inspect.isfunction(obj) এবং 
                         name.startswith('test'))]
    none = সত্য
    জন্য (f,n) মধ্যে funcs:
        যদি re.অনুসন্ধান(rex, n):
            none = মিথ্যা
            f()
    যদি none:
        test_usertests(test=args.testrex)

প্রধান()
