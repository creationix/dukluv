"use strict";


// These override the built-in resolve and load with
// the exact same semantics, but with logging added.

// Set up our app-specefic require hooks.
var modResolve = Duktape.modResolve;
Duktape.modResolve = function (id) {
  print("modResolve", this.id, id);
  return modResolve.call(this, id);
};

var modLoad = Duktape.modLoad;
Duktape.modLoad = function () {
  print("modLoad", this.id);
  return modLoad.call(this);
};

var modCompile = Duktape.modCompile;
Duktape.modCompile = function (code) {
  print("modCompile", this.id, code.length);
  return modCompile.call(this, code);
};

// Test require
var p = require('./modules/utils.js').prettyPrint;

p("B", require('./b.js'));

p("THIS", this);

require('./tcp-echo.js');

p(global);
p(Duktape);
p(Duktape.modLoaded);
