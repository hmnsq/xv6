#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

আনযন argparse, os, inspect, re, signal, subprocess, sys, time
তঃ subprocess আনযন run

sys.stdout.reconfigure(সাৰি_buffering=সঁচা)

parser = argparse.ArgumentParser()
parser.add_argument('testrex', help="test name or regular expression")
parser.add_argument("-q", action='store_true', help="usertests quick")
args = parser.parse_args()

শ্ৰেণী QEMU(object):

    def __init__(self, reset=মিছা):
        যদি reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.set_blocking(self.proc.stdout.fileno(), মিছা)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        time.সুষুপ্তি(1)

    def reset_fs(self):
        যত্ন:
            run(["rm", "-f", "fs.img"], check=সঁচা)
            run(["make", "fs.img"], check=সঁচা)
        অপৱাদ subprocess.CalledProcessError ইৱ e:
            print(f"Command failed with exit code {e.returncode}")

    def build_xv6(self):
        যত্ন:
            run(["make", "kernel/kernel"], check=সঁচা)
        অপৱাদ subprocess.CalledProcessError ইৱ e:
            print(f"Command failed with exit code {e.returncode}")

    def save_output(self):
      যত্ন:
        with open("test-xv6.out", "w") ইৱ f:
            f.লিখ(self.output)
            f.close()
      অপৱাদ OSError ইৱ e:
        print("Provided a bad results path. Error:", e)     
        
    def cmd(self, c):
        যদি isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.লিখ(c)
        self.proc.stdin.flush()
        
    def crash(self):
        ps = run(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [পূর্ণাঙ্ক(সাৰি) বাবে সাৰি অন্তঃ ps.stdout.splitlines()]
        যদি len(kids) == 0:
            print("no qemu")
            sys.exit(1)
        print("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    def stop(self):
        self.proc.terminate()

    def পঠ(self):
        যেতিয়ালৈ সঁচা:
            যত্ন:
                buf = os.পঠ(self.proc.stdout.fileno(), 4096)
            অপৱাদ BlockingIOError:
                ভঙ্গ
            যদি len(buf) == 0:  # qemu exited
                ভঙ্গ
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    def সাৰিগুলি(self):
        ঘূৰাই self.output.splitlines()

    def error(self, *regexps):
        print("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    def match(self, *regexps, exit=সঁচা):
        found = মিছা
        বাবে সাৰি অন্তঃ self.সাৰিগুলি():
            যদি any(re.match(r, সাৰি) বাবে r অন্তঃ regexps):
                print(সাৰি)
                found = সঁচা
        যদি নহয় found আৰু exit:
            self.error(*regexps)
        ঘূৰাই found

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    def progress(self, regexp):
        end = self.output.rfind("\n") + 1
        যদি end <= self.reported:
            ঘূৰাই
        বাবে সাৰি অন্তঃ self.output[self.reported:end].splitlines():
            যদি re.match(regexp, সাৰি):
                print(সাৰি)
        self.reported = end

    def monitor(self, *regexps, progress="", timeout):
        deadline = time.time() + timeout
        যেতিয়ালৈ সঁচা:
            time.সুষুপ্তি(1)
            timeleft = deadline - time.time()
            যদি timeleft < 0:
                self.error(*regexps)
            self.পঠ()
            যদি progress:
                self.progress(progress)
            যদি self.match(*regexps, exit=মিছা):
                ঘূৰাই

def crash_log():
    q = QEMU(সঁচা)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    time.সুষুপ্তি(2)
    q.crash()
    q.stop()

def recover_log():
    q = QEMU()
    time.সুষুপ্তি(2)
    q.পঠ()
    ok = q.match('^recovering', exit=মিছা)
    যদি ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    ঘূৰাই ok

def forphan():
    q = QEMU(সঁচা)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

def dorphan():
    q = QEMU(সঁচা)
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
    বাবে i অন্তঃ range(20):
        crash_log()
        ok = recover_log()
        যদি ok:
            print("OK")
            ঘূৰাই
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
    নোচেদ্যদি test != "":
        opt += " " + test
    q = QEMU(সঁচা)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', progress='test', timeout=timeout)
    q.stop()

def main():
    print(args)
    rex = r'%s' % args.testrex
    funcs = [(obj,name) বাবে name,obj অন্তঃ inspect.getmembers(sys.modules[__name__]) 
                     যদি (inspect.isfunction(obj) আৰু 
                         name.startswith('test'))]
    none = সঁচা
    বাবে (f,n) অন্তঃ funcs:
        যদি re.অনুসন্ধান(rex, n):
            none = মিছা
            f()
    যদি none:
        test_usertests(test=args.testrex)

main()
