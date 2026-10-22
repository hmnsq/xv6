#!/usr/bin/env python3

#
# python script that tests xv6 without having to boot it and type to its shell
#
# ./test-xv6.py usertests  (runs usertests)
# ./test-xv6.py -q usertests (runs the quick tests of usertests)
# ./test-xv6.py crash  (runs the crash tests)
# ./test-xv6.py log (runs the log crash test)

ਆਯਾਤ argparse, os, inspect, re, signal, subprocess, sys, ਸਮਾਂ
ਤਃ subprocess ਆਯਾਤ ਲੜੀਵਾਰ

sys.stdout.reconfigure(ਸਤਰ_buffering=ਸੱਚ)

parser = argparse.ਦਲੀਲ_Parser()
parser.ਜੋੜੋ_ਦਲੀਲ('testrex', help="test name or regular expression")
parser.ਜੋੜੋ_ਦਲੀਲ("-q", action='store_true', help="usertests quick")
ਦਲੀਲਾਂ = parser.parse_ਦਲੀਲਾਂ()

ਵਰਗ QEMU(object):

    ਪਰਿਭਾਸ਼ਾ __init__(self, reset=ਝੂਠ):
        ਜੇ reset:
            self.build_xv6()
            self.reset_fs()
        q = ["make", "qemu"]
        self.proc = subprocess.Popen(q, stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT)
        os.ਸੈੱਟ_blocking(self.proc.stdout.fileno(), ਝੂਠ)
        self.output = ""
        self.outbytes = bytearray()
        self.reported = 0
        ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(1)

    ਪਰਿਭਾਸ਼ਾ reset_fs(self):
        ਕੋਸ਼ਿਸ਼:
            ਲੜੀਵਾਰ(["rm", "-f", "fs.img"], check=ਸੱਚ)
            ਲੜੀਵਾਰ(["make", "fs.img"], check=ਸੱਚ)
        ਅਪਵਾਦ subprocess.CalledProcessError ਇਵ e:
            ਛਾਪੋ(f"Command failed with exit code {e.returncode}")

    ਪਰਿਭਾਸ਼ਾ build_xv6(self):
        ਕੋਸ਼ਿਸ਼:
            ਲੜੀਵਾਰ(["make", "kernel/kernel"], check=ਸੱਚ)
        ਅਪਵਾਦ subprocess.CalledProcessError ਇਵ e:
            ਛਾਪੋ(f"Command failed with exit code {e.returncode}")

    ਪਰਿਭਾਸ਼ਾ save_output(self):
      ਕੋਸ਼ਿਸ਼:
        ਨਾਲ open("test-xv6.out", "w") ਇਵ f:
            f.ਲਿਖ(self.output)
            f.close()
      ਅਪਵਾਦ OSError ਇਵ e:
        ਛਾਪੋ("Provided a bad results path. Error:", e)     
        
    ਪਰਿਭਾਸ਼ਾ cmd(self, c):
        ਜੇ isinstance(c, str):
            c = c.encode('utf-8')
        self.proc.stdin.ਲਿਖ(c)
        self.proc.stdin.flush()
        
    ਪਰਿਭਾਸ਼ਾ crash(self):
        ps = ਲੜੀਵਾਰ(['ps', '-opid', '--no-headers', '--ppid', str(self.proc.pid)], stdout=subprocess.PIPE, encoding='utf8')
        kids = [ਪੂਰਨ(ਸਤਰ) ਲਈ ਸਤਰ ਵਿੱਚ ps.stdout.splitlines()]
        ਜੇ ਲੰਬਾਈ(kids) == 0:
            ਛਾਪੋ("no qemu")
            sys.exit(1)
        ਛਾਪੋ("kill", kids[0])
        os.kill(kids[0], signal.SIGKILL)

    ਪਰਿਭਾਸ਼ਾ stop(self):
        self.proc.terminate()

    ਪਰਿਭਾਸ਼ਾ ਪਠ(self):
        ਜਦੋਂ ਸੱਚ:
            ਕੋਸ਼ਿਸ਼:
                buf = os.ਪਠ(self.proc.stdout.fileno(), 4096)
            ਅਪਵਾਦ BlockingIOError:
                ਰੋਕੋ
            ਜੇ ਲੰਬਾਈ(buf) == 0:  # qemu exited
                ਰੋਕੋ
            self.outbytes.extend(buf)
        self.output = self.outbytes.decode("utf-8", "replace")

    ਪਰਿਭਾਸ਼ਾ ਸਤਰਾਂ(self):
        ਵਾਪਸ self.output.splitlines()

    ਪਰਿਭਾਸ਼ਾ error(self, *regexps):
        ਛਾਪੋ("FAIL: match failed", regexps)
        self.save_output()
        self.stop()
        sys.exit(1)

    ਪਰਿਭਾਸ਼ਾ ਮੇਲ(self, *regexps, exit=ਸੱਚ):
        ਮਿਲਿਆ = ਝੂਠ
        ਲਈ ਸਤਰ ਵਿੱਚ self.ਸਤਰਾਂ():
            ਜੇ any(re.ਮੇਲ(r, ਸਤਰ) ਲਈ r ਵਿੱਚ regexps):
                ਛਾਪੋ(ਸਤਰ)
                ਮਿਲਿਆ = ਸੱਚ
        ਜੇ ਨਹੀਂ ਮਿਲਿਆ ਅਤੇ exit:
            self.error(*regexps)
        ਵਾਪਸ ਮਿਲਿਆ

    # Print the lines matching regexp that have arrived since the last
    # call.  A trailing partial line is left for the next call, so that
    # each line is printed once, after all of it has been read.
    ਪਰਿਭਾਸ਼ਾ ਤਰੱਕੀ(self, regexp):
        ਸਿਰਾ = self.output.rfind("\n") + 1
        ਜੇ ਸਿਰਾ <= self.reported:
            ਵਾਪਸ
        ਲਈ ਸਤਰ ਵਿੱਚ self.output[self.reported:ਸਿਰਾ].splitlines():
            ਜੇ re.ਮੇਲ(regexp, ਸਤਰ):
                ਛਾਪੋ(ਸਤਰ)
        self.reported = ਸਿਰਾ

    ਪਰਿਭਾਸ਼ਾ monitor(self, *regexps, ਤਰੱਕੀ="", timeout):
        deadline = ਸਮਾਂ.ਸਮਾਂ() + timeout
        ਜਦੋਂ ਸੱਚ:
            ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(1)
            timeleft = deadline - ਸਮਾਂ.ਸਮਾਂ()
            ਜੇ timeleft < 0:
                self.error(*regexps)
            self.ਪਠ()
            ਜੇ ਤਰੱਕੀ:
                self.ਤਰੱਕੀ(ਤਰੱਕੀ)
            ਜੇ self.ਮੇਲ(*regexps, exit=ਝੂਠ):
                ਵਾਪਸ

ਪਰਿਭਾਸ਼ਾ crash_log():
    q = QEMU(ਸੱਚ)
    q.cmd("logstress f0 f1 f2 f3 f4 f5\n")
    ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(2)
    q.crash()
    q.stop()

ਪਰਿਭਾਸ਼ਾ recover_log():
    q = QEMU()
    ਸਮਾਂ.ਸੁਸ਼ੁਪ੍ਤਿ(2)
    q.ਪਠ()
    ok = q.ਮੇਲ('^recovering', exit=ਝੂਠ)
    ਜੇ ok:
        q.cmd("ls\n")
        q.monitor('f5', timeout=30)
    q.stop()
    ਵਾਪਸ ok

ਪਰਿਭਾਸ਼ਾ forphan():
    q = QEMU(ਸੱਚ)
    q.cmd("forphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

ਪਰਿਭਾਸ਼ਾ dorphan():
    q = QEMU(ਸੱਚ)
    q.cmd("dorphan\n")
    q.monitor('wait', timeout=30)
    q.crash()
    q.stop()

ਪਰਿਭਾਸ਼ਾ recover_orphan():
    q = QEMU()
    q.monitor('^ireclaim', timeout=30)
    q.stop()

ਪਰਿਭਾਸ਼ਾ test_log():
    ਛਾਪੋ("Test recovery of log")
    ਲਈ i ਵਿੱਚ ਸੀਮਾ(20):
        crash_log()
        ok = recover_log()
        ਜੇ ok:
            ਛਾਪੋ("OK")
            ਵਾਪਸ
        ਛਾਪੋ("log attempt ", i+1)
    ਛਾਪੋ("FAIL")
    sys.exit(1)
    
ਪਰਿਭਾਸ਼ਾ test_forphan():
    ਛਾਪੋ("Test recovery of an orphaned file")
    forphan()
    recover_orphan()
    ਛਾਪੋ("OK")

ਪਰਿਭਾਸ਼ਾ test_dorphan():
    ਛਾਪੋ("Test recovery of an orphaned file")
    dorphan()
    recover_orphan()
    ਛਾਪੋ("OK")

ਪਰਿਭਾਸ਼ਾ test_crash():
    test_log()
    test_forphan()
    test_dorphan()

ਪਰਿਭਾਸ਼ਾ test_usertests(test=""):
    timeout = 600
    opt = ""
    ਜੇ ਦਲੀਲਾਂ.q:
        opt = " -q"
        timeout = 300
    ਜਾਂਜੇ test != "":
        opt += " " + test
    q = QEMU(ਸੱਚ)
    q.cmd("usertests" + opt + "\n")
    q.monitor('^ALL TESTS PASSED', ਤਰੱਕੀ='test', timeout=timeout)
    q.stop()

ਪਰਿਭਾਸ਼ਾ ਮੁੱਖ():
    ਛਾਪੋ(ਦਲੀਲਾਂ)
    rex = r'%s' % ਦਲੀਲਾਂ.testrex
    funcs = [(obj,name) ਲਈ name,obj ਵਿੱਚ inspect.getmembers(sys.modules[__name__]) 
                     ਜੇ (inspect.isfunction(obj) ਅਤੇ 
                         name.startswith('test'))]
    none = ਸੱਚ
    ਲਈ (f,n) ਵਿੱਚ funcs:
        ਜੇ re.ਖੋਜ(rex, n):
            none = ਝੂਠ
            f()
    ਜੇ none:
        test_usertests(test=ਦਲੀਲਾਂ.testrex)

ਮੁੱਖ()
