"use strict";

// This tests using timers for a simple timeout.
// It also tests the handle close callback and
// makes sure self is passed in properly to callbacks.
test("simple timeout", function (expect) {
  var timer = uv.new_timer();
  uv.timer_start(timer, 10, 0, expect(ontimeout));

  function ontimeout() {
    assert(this === timer);
    print("timeout", timer);
    uv.close(timer, expect(onclosed));
  }

  function onclosed() {
    assert(this === timer);
    print("closed", timer);
  }
});

// This is like the previous test, but using repeat.
test("simple interval", function (expect) {
  var timer = uv.new_timer();
  var count = 3;
  uv.timer_start(timer, 10, 10, expect(oninterval, 3));

  function oninterval() {
    assert(this === timer);
    print("interval", timer);
    if (--count) { return; }
    uv.close(timer, expect(onclosed));
  }

  function onclosed() {
    assert(this === timer);
    print("closed", timer);
  }
});

// Test two concurrent timers
// There is a small race condition, but there are 50ms of wiggle room.
// 250ms is halfway between 2x100ms and 3x100ms
test("timeout with interval", function (expect) {
  var a = uv.new_timer();
  var b = uv.new_timer();
  uv.timer_start(a, 250, 0, expect(ontimeout));
  uv.timer_start(b, 100, 100, expect(oninterval, 2));

  function ontimeout() {
    print("timeout", a);
    uv.timer_stop(b);
    uv.close(a);
    uv.close(b);
  }

  function oninterval() {
    print("interval", b);
  }

});

// This advanced test uses the rest of the uv_timer_t functions
// to create an interval that shrinks over time.
test("shrinking interval", function (expect) {
  var timer = uv.new_timer();
  uv.timer_start(timer, 10, 0, expect(ontimeout, 4));

  function ontimeout() {
    var r = uv.timer_get_repeat(timer);
    print("interval", timer, r);
    if (r === 0) {
      uv.timer_set_repeat(timer, 8);
      uv.timer_again(timer);
    }
    else if (r === 2) {
      uv.timer_stop(timer);
      uv.close(timer);
    }
    else {
      uv.timer_set_repeat(timer, r / 2);
    }
  }
});

////////////////////////////////////////////////////////////////////////////////

test("uv.guess_handle", function () {
  print("stdio fd types",
    0, uv.guess_handle(0),
    1, uv.guess_handle(1),
    2, uv.guess_handle(2)
  );
});

test("uv.version and uv.version_string", function () {
  var version = uv.version();
  var version_string = uv.version_string();
  print("version", version, "version-string", version_string);
  assert(typeof version === "number");
  assert(typeof version_string === "string");
});

test("memory size", function () {
  var rss = uv.resident_set_memory();
  var total = uv.get_total_memory();
  print("rss", rss, "total", total);
  assert(rss < total);
});

test("uv.uptime", function () {
  var uptime = uv.uptime();
  print("uptime", uptime);
});

test("uv.getrusage", function () {
  var rusage = uv.getrusage();
  print(JSON.stringify(rusage));
});

test("uv.cpu_info", function () {
  var info = uv.cpu_info();
  print(JSON.stringify(info));
});

test("uv.interface_addresses", function () {
  var addresses = uv.interface_addresses();
  print(JSON.stringify(addresses));
});

test("uv.loadavg", function () {
  var avg = uv.loadavg();
  print.apply(print, avg);
  assert(avg.length === 3);
});

test("uv.exepath", function () {
  var path = uv.exepath();
  print("exepath", path);
});

test("uv.cwd and uv.chdir", function () {
  var old = uv.cwd();
  print("original", old);
  uv.chdir("/");
  var cwd = uv.cwd();
  print("new", cwd);
  assert(cwd !== old);
  uv.chdir(old);
});

test("uv.hrtime", function () {
  var time = uv.hrtime();
  print("hrtime", time);
});

////////////////////////////////////////////////////////////////////////////////

// Micro assert library
function assert(cond, message) {
  if (!cond) {
    throw new Error(message || "Assertion failed");
  }
}

// Mini test framework
function test(name, fn) {
  var cwd;
  try {
    cwd = uv.cwd();
    var expects = [];
    print("\x1b[33mStarting test: " + name + "\x1b[39m");
    fn(function (fn, left) {
      left = left || 1;
      expects.push(fn);
      return wrapped;
      function wrapped() {
        if (!--left) {
          var index = expects.indexOf(fn);
          if (index >= 0) {
            expects.splice(index, 1);
          }
        }
        if (left < 0) {
          throw new Error("Function called more than expected: " + fn);
        }
        return fn.apply(this, arguments);
      }
    });
    uv.run();
    uv.walk(function (handle) {
      throw new Error("Unclosed handle: " + handle);
    });
    expects.forEach(function (fn) {
      throw new Error("Missing expected call: " + fn);
    });
    if (uv.cwd() !== cwd) {
      throw new Error("Test left cwd modified: " + cwd + " != " + uv.cwd());
    }
    print("\x1b[32mPassed!\x1b[39m");
  }
  catch (err) {
    print("\x1b[31mFailed\x1b[39m");
    print(err.stack);
  }
  finally {
    uv.walk(uv.close);
    uv.chdir(cwd);
    uv.run();
  }
}

