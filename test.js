"use strict";

var dump = getDump();

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
  p("stdio fd types", [
    uv.guess_handle(0),
    uv.guess_handle(1),
    uv.guess_handle(2),
  ]);
});

test("uv.version and uv.version_string", function () {
  var version = uv.version();
  var version_string = uv.version_string();
  p({
    version: version,
    version_string: version_string,
  });
  assert(typeof version === "number");
  assert(typeof version_string === "string");
});

test("memory size", function () {
  var rss = uv.resident_set_memory();
  var total = uv.get_total_memory();
  p({
    rss: rss,
    total: total
  });
  assert(rss < total);
});

test("uv.uptime", function () {
  var uptime = uv.uptime();
  p({uptime: uptime});
});

test("uv.getrusage", function () {
  var rusage = uv.getrusage();
  p(rusage);
});

test("uv.cpu_info", function () {
  var info = uv.cpu_info();
  p(info);
});

test("uv.interface_addresses", function () {
  var addresses = uv.interface_addresses();
  p(addresses);
});

test("uv.loadavg", function () {
  var avg = uv.loadavg();
  p({loadavg:avg});
  assert(avg.length === 3);
});

test("uv.exepath", function () {
  var path = uv.exepath();
  p({exepath: path});
});

test("uv.cwd and uv.chdir", function () {
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

test("uv.hrtime", function () {
  var time = uv.hrtime();
  p({"hrtime": time});
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
    print("\n\x1b[38;5;254mStarting test: \x1b[38;5;69m" + name + "\x1b[0m");
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
    print("\x1b[38;5;46mPassed!\x1b[0m");
  }
  catch (err) {
    print("\x1b[38;5;196m" + err.stack + "\x1b[0m");
  }
  finally {
    uv.walk(uv.close);
    uv.chdir(cwd);
    uv.run();
  }
}

function p() {
  print(Array.prototype.map.call(arguments, dump).join(" "));
}

function getDump() {
  var quote, quote2, obracket, cbracket, obrace, cbrace, comma, colon;

  var theme = {
    quotes: "38;5;33",
    text: "38;5;26",
    escape: "38;5;45",
    null: "38;5;240",
    boolean: "38;5;113",
    number: "38;5;166",
    property: "38;5;255",
    special: "38;5;131",
    special2: "38;5;126",
    regexp: "38;5;160",
    punc: "38;5;238"
  };


  quote = colorize("quotes", '"', "text");
  quote2 = colorize("quotes", '"');
  obracket = colorize("punc", '[');
  cbracket = colorize("punc", ']');
  obrace = colorize("punc", '{');
  cbrace = colorize("punc", '}');
  comma = colorize("punc", ',');
  colon = colorize("punc", ':');

  function color(color_name) {
   return "\x1b[" + (theme[color_name] || "0") + "m";
  }

  function colorize(color_name, string, reset_name) {
    return color(color_name) + string + color(reset_name);
  }

  function dump(value) {
    var seen = [];
    return dumper(value, 0);
    function dumper(value, depth) {
      var type = typeof(value);
      if (type === 'string') {
        var str = JSON.stringify(value);
        return (quote + str.substring(1, str.length - 1) + quote2).
          replace(/(\\u[0-9a-f]{4}|\\["\\/bfnrt])/g, function (match) {
            return colorize("escape", match, "text");
          });
      }
      if (type === 'undefined' || value === null) {
        return colorize("null", "" + value);
      }
      if (type === 'boolean') {
        return colorize("boolean", "" + value);
      }
      if (type === 'number') {
        return colorize("number", "" + value);
      }
      if (type === "function") {
        return colorize("special", "[", "special2") +
          "Function" + (value.name ? " " + value.name : "") +
          colorize("special", "]");
      }
      if (type !== "object") {
        return "" + value;
      }
      if (value instanceof RegExp) {
        return colorize("regexp", "" + value);
      }
      var index = seen.indexOf(value);
      if (depth > 2 || index >= 0) {
        return colorize("special", "[", "special2") +
          (Array.isArray(value) ? "Array" : "Object") +
          colorize("special", "]");
      }
      seen.push(value);
      var parts, opener, closer;
      if (Array.isArray(value)) {
        opener = obracket;
        closer = cbracket;
        parts = value.map(function (item) {
          return dumper(item, depth + 1);
        });
      }
      else {
        opener = obrace;
        closer = cbrace;
        parts = Object.keys(value).map(function (key) {
          return colorize("property", key) + colon + " " + dumper(value[key], depth + 1);
        });
      }

      var line = opener + " " + parts.join(comma + " ") + " " + closer;
      if (strip(line).length > 120 - depth * 2) {
        line = opener + "\n  " + parts.join(comma + "\n").split("\n").join("\n  ") + "\n" + closer;
      }

      return line;
    }
  }

  function strip(string) {
    return string.replace(/\x1b\[[^m]*m/g, '');
  }

  return dump;

}
