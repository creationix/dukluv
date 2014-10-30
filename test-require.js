"use strict";


// These override the built-in resolve and load with
// the exact same semantics, but with logging added.

// Set up our app-specefic require hooks.
var modResolve = Duktape.modResolve;
Duktape.modResolve = function (parent, id) {
  if (p) { p("modResolve", parent, id); }
  else { print("modResolve", parent.id, id); }
  return modResolve.call(this, parent, id);
};

var modLoad = Duktape.modLoad;
Duktape.modLoad = function (module) {
  if (p) { p("modLoad", module); }
  else { print("modLoad", module.id); }
  return modLoad.call(this, module);
};

// Test require
var p = require('./modules/utils.js').prettyPrint;

p("B", require('./b.js'));

require('./tcp-echo.js');

p(Duktape.modLoaded);
