"use strict";

var stdout = uv.new_tty(1, false);
var width = uv.tty_get_winsize(stdout).width;
var Handle = require('./classes.js').Handle;
var Req = require('./classes.js').Req;

exports.prettyPrint = prettyPrint;
exports.dump = dump;
exports.stdout = stdout;
exports.color = color;
exports.colorize = colorize;

var quote, quote2, obracket, cbracket, obrace, cbrace, comma, colon;

var theme = require('./theme-256.js');

quote = colorize("quotes", '"', "string");
quote2 = colorize("quotes", '"');
obracket = colorize("braces", '[');
cbracket = colorize("braces", ']');
obrace = colorize("braces", '{');
cbrace = colorize("braces", '}');
comma = colorize("sep", ',');
colon = colorize("sep", ':');

function color(color_name) {
 return "\x1b[" + (color_name ? theme[color_name] : "0") + "m";
}

function colorize(color_name, string, reset_name) {
  return color(color_name) + string + color(reset_name);
}

function dump(value) {

  var seen = [];
  return dumper(value, 0);
  function dumper(value, depth) {
    var type = typeof value;

    if (type === "undefined") {
      return colorize("undefined", "undefined");
    }
    if (value === null) {
      return colorize("null", "null");
    }
    if (type === "boolean") {
      return colorize("boolean", "" + value);
    }
    if (type === "number") {
      return colorize("number", "" + value);
    }
    if (type === "string") {
      var str = JSON.stringify(value);
      return (quote + str.substring(1, str.length - 1) + quote2).
        replace(/(\\u[0-9a-f]{4}|\\["\\/bfnrt])/g, function (match) {
          return colorize("escape", match, "string");
        });
    }
    var info = Duktape.info(value);
    if (type === "function") {
      var fname = value.name || info[1];
      // Native CFunctions don't have a .prototype property.
      if (value.prototype) {
        return colorize("function", "[Function " + fname + "]");
      }
      return colorize("cfunction", "[Native " + fname + "]");
    }
    var fullName = Object.prototype.toString.call(value);
    var name = fullName.substring(8, fullName.length - 1);
    if (name === "RegExp") {
      return colorize("regexp", "[RegExp " + value + "]");
    }
    if (name === "Thread") {
      return colorize("thread", "[Thread " + info[1] + "]");
    }
    if (name === "Buffer") {
      var preview = Array.prototype.slice.call(value, 0, 10).map(function (byte) {
        return byte < 16 ? "0" + byte.toString(16) : byte.toString(16);
      }).join(" ");
      if (value.length > 10) { preview += "..."; }
      // Fixed buffers have undefined for info[4]
      if (info[4] === undefined) {
        return colorize("buffer", "[Buffer " + preview + "]");
      }
      return colorize("dbuffer", "[Dynamic Buffer " + preview + "]");
    }
    if (name === "Pointer") {
      return colorize("pointer", "[Pointer " + info[1] + "]");
    }
    if (name === "Error") {
      return colorize("error", "[" + value.constructor.name + " " + value.message + "]");
    }
    if (name === "Date") {
      return colorize("date", "[Date " + value + "]");
    }
    if (name === "String") {
      return colorize("string", "[String " + JSON.stringify(value) + "]");
    }
    if (name === "Number") {
      return colorize("number", "[Number " + value + "]");
    }
    if (value instanceof Handle || value instanceof Req) {
      return colorize("object", "[" + value.constructor.name + " " + info[1] + "]");
    }
    if (name !== "Object" && name !== "Array" && name !== "global") {
      return colorize("object", "[" + name + " " + info[1] + "]");
    }

    var index = seen.indexOf(value);
    if (depth > 2 || index >= 0) {
      return colorize("object", "[" + name + " " + info[1] + "]");
    }
    seen.push(value);

    var parts, opener, closer;
    if (name === "Array") {
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
    var max = width - depth * 2;
    if (strip(line).length > max) {
      var lines = [];
      line = [];
      max -= 2;
      var left = max;
      parts.forEach(function (part) {
        var len = strip(part).length + 2;
        if (left < len) {
          if (line.length) {
            lines.push(line);
          }
          left = max;
          line = [];
        }
        line.push(part);
        left -= len;
      });
      if (line.length) {
        lines.push(line);
      }
      lines = lines.map(function (line) {
        return line.join(comma + " ");
      });

      line = opener + "\n  " + lines.join(comma + "\n").split("\n").join("\n  ") + "\n" + closer;
    }

    return line;
  }
}

function strip(string) {
  return string.replace(/\x1b\[[^m]*m/g, '');
}

function prettyPrint() {
  print(Array.prototype.map.call(arguments, dump).join(" "));
}
