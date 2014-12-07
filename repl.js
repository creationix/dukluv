"use strict";

var utils = require('./modules/utils.js');
var p = utils.prettyPrint;

function Timer() {
  var obj = new Duktape.Buffer(uv.new_timer());
  obj.__proto__ = Timer.prototype;
  return obj;
}

Timer.prototype.start = uv.timer_start;
Timer.prototype.stop = uv.timer_stop;

uv.read_start(utils.stdin, function (err, chunk) {
  if (err) { throw err; }
  if (!chunk) { return uv.read_stop(utils.stdin); }
  try {
    p(eval(chunk.toString()));
  }
  catch (error) {
    uv.write(utils.stderr, utils.colorize("error", error.toString()) + "\n");
  }
  uv.write(utils.stdout, "> ");
});
uv.write(utils.stdout, "> ");

uv.run();

uv.write(utils.stdout, "\n");
