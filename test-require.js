"use strict";

// Replace Duktape 1.x require with polyfilled 2.x require
// This won't be needed when new require is native.
var module = {id:uv.cwd()+"/test-require.js",exports:{}};
require = newRequire.bind(module);

// Set up our app-specefic require hooks.
Duktape.modResolve = function (parent, id) {
  if (id[0] !== ".") { return id; }
  return pathJoin(parent.id, "..", id);
};

Duktape.modLoad = function (module) {
  var code = loadFile(module.id);
  return Duktape.modCompile(module, code);
};

// Test require
var p = require('./modules/utils.js').prettyPrint;
p(require('./b.js'));

require('./tcp-echo.js');

function loadFile(path) {
  var fd = uv.fs_open(path, "r", 420 /*0644*/);
  try {
    var stat = uv.fs_fstat(fd);
    var chunk = uv.fs_read(fd, stat.size, 0);
    return chunk;
  }
  finally {
    uv.fs_close(fd);
  }
}

function pathJoin() {
  var parts = [].join.call(arguments, "/").split("/").filter(Boolean);
  var skip = 0;
  for (var i = parts.length - 1; i >= 0; --i) {
    var part = parts[i];
    if (part === '.') {
      parts.splice(i, 1);
    }
    else if (part === '..') {
      parts.splice(i, 1);
      ++skip;
    }
    else if (skip) {
      parts.splice(i, 1);
      --skip;
    }
  }
  var path = parts.join("/");
  if (arguments[0][0] === "/") {
    path = "/" + path;
  }
  return path;
}
