"use strict";

var p = require('modules/utils.js').prettyPrint;
var test = require('modules/test.js').test;

// This tests using timers for a simple timeout.
// It also tests the handle close callback and
// makes sure self is passed in properly to callbacks.
test("simple timeout", function (assert, expect) {
  var timer = uv.new_timer();
  uv.timer_start(timer, 10, 0, expect(ontimeout));

  function ontimeout() {
    assert(this === timer);
    p("timeout", timer);
    uv.close(timer, expect(onclosed));
  }

  function onclosed() {
    assert(this === timer);
    p("closed", timer);
  }
});

// This is like the previous test, but using repeat.
test("simple interval", function (assert, expect) {
  var timer = uv.new_timer();
  var count = 3;
  uv.timer_start(timer, 10, 10, expect(oninterval, 3));

  function oninterval() {
    assert(this === timer);
    p("interval", timer);
    if (--count) { return; }
    uv.close(timer, expect(onclosed));
  }

  function onclosed() {
    assert(this === timer);
    p("closed", timer);
  }
});

// Test two concurrent timers
// There is a small race condition, but there are 50ms of wiggle room.
// 250ms is halfway between 2x100ms and 3x100ms
test("timeout with interval", function (asert, expect) {
  var a = uv.new_timer();
  var b = uv.new_timer();
  uv.timer_start(a, 250, 0, expect(ontimeout));
  uv.timer_start(b, 100, 100, expect(oninterval, 2));

  function ontimeout() {
    p("timeout", a);
    uv.timer_stop(b);
    uv.close(a);
    uv.close(b);
  }

  function oninterval() {
    p("interval", b);
  }

});

// This advanced test uses the rest of the uv_timer_t functions
// to create an interval that shrinks over time.
test("shrinking interval", function (assert, expect) {
  var timer = uv.new_timer();
  uv.timer_start(timer, 10, 0, expect(ontimeout, 4));

  function ontimeout() {
    var r = uv.timer_get_repeat(timer);
    p("interval", timer, r);
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

test("uv.guess_handle", function (assert) {
  p("stdio fd types", [
    uv.guess_handle(0),
    uv.guess_handle(1),
    uv.guess_handle(2),
  ]);
});

test("uv.version and uv.version_string", function (assert) {
  var version = uv.version();
  var version_string = uv.version_string();
  p({
    version: version,
    version_string: version_string,
  });
  assert(typeof version === "number");
  assert(typeof version_string === "string");
});

test("memory size", function (assert) {
  var rss = uv.resident_set_memory();
  var total = uv.get_total_memory();
  p({
    rss: rss,
    total: total
  });
  assert(rss < total);
});

test("uv.uptime", function (assert) {
  var uptime = uv.uptime();
  p({uptime: uptime});
});

test("uv.getrusage", function (assert) {
  var rusage = uv.getrusage();
  p(rusage);
});

test("uv.cpu_info", function (assert) {
  var info = uv.cpu_info();
  p(info);
});

test("uv.interface_addresses", function (assert) {
  var addresses = uv.interface_addresses();
  p(addresses);
});

test("uv.loadavg", function (assert) {
  var avg = uv.loadavg();
  p({loadavg:avg});
  assert(avg.length === 3);
});

test("uv.exepath", function (assert) {
  var path = uv.exepath();
  p({exepath: path});
});

test("uv.cwd and uv.chdir", function (assert) {
  var old = uv.cwd();
  uv.chdir("/");
  var cwd = uv.cwd();
  p({
    original: old,
    changed: cwd
  });
  assert(cwd !== old);
  uv.chdir(old);
});

test("uv.get_process_title and uv.set_process_title", function (assert) {
  var old = uv.get_process_title();
  uv.set_process_title("Magic");
  var changed = uv.get_process_title();
  p({
    original: old,
    changed: changed,
  });
  assert(old !== changed);
  uv.set_process_title(old);
});

test("uv.hrtime", function () {
  var time = uv.hrtime();
  p({"hrtime": time});
});

test("pretty printer", function () {

  p(global);

  p({
    numbers: [1,2,3],
    undefined: undefined,
    booleans: [true, false],
    null: null,
    string: "a string\nwith\0stuff",
  });

  p({
    regexp: /abc*/,
    native: print,
    user: function user() {},
    anonymous: function () {},
  });

  var timer = uv.new_timer();

  p({
    thread: new Duktape.Thread(test),
    buffer: timer,
    dynamic: new Duktape.Buffer("Hello"),
    pointer: new Duktape.Pointer(p),
    error: new Error("test"),
    typeError: new TypeError("test2"),
    date: new Date(),
    string: new String("Hello"),
    number: new Number(42),
  });

  uv.close(timer);

  p({
    thread: Duktape.Thread(test),
    dynamic: Duktape.Buffer("Hello"),
    pointer: Duktape.Pointer(p),
    error: Error("test"),
    typeError: TypeError("test2"),
    date: Date(),
  });
  p({name:"tim", age:32}, ["red", "blue", 1, 2, 3]);
  p([[[[]]]], {a:{b:{c:{}}}});

});


var numErrors = require('modules/test.js').errors;
if (numErrors) {
  throw new Error(numErrors + " errors.");
}
