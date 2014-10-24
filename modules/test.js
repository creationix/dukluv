"use strict";

var stdout = require('../utils.js').stdout;

exports.test = test;
exports.errors = 0;

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
    fn(assert, function (fn, left) {
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
      if (handle === stdout) { return; }
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
    exports.errors++;
    alert("\x1b[38;5;196m" + err.stack + "\x1b[0m");
  }
  finally {
    uv.chdir(cwd);
    cleanup();
  }
}

function cleanup() {
  try {
    uv.run();
    uv.walk(function (handle) {
      if (handle === stdout) { return; }
      uv.close(handle);
    });
    uv.run();
  }
  catch (err) {
    exports.errors++;
    alert("\x1b[38;5;196m" + err.stack + "\x1b[0m");
    cleanup();
  }
}
