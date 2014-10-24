"use strict";

var stdout = uv.new_tty(1, false);
var width = uv.tty_get_winsize(stdout).width;

exports.prettyPrint = prettyPrint;
exports.dump = dump;
exports.stdout = stdout;

var quote, quote2, obracket, cbracket, obrace, cbrace, comma, colon;

var theme = require('../theme-256.js');

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
