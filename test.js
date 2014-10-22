"use strict";

function assert(cond, message) {
  if (!cond) throw new Error(message || "Assertion failed");
}

try {

  var timer = uv.new_timer()
  print("timer", timer);
  uv.timer_start(timer, 300, 300, function () {
    print("TIMEOUT!", timer);
    assert(this === timer);
    uv.timer_stop(timer);
    uv.close(timer, function () {
      print("CLOSE!", timer);
      assert(this === timer);
    });
  });

  uv.walk(function (handle) {
    print("Walk handle", handle);
  });

  uv.run();

}
catch (err) {
  print(err.stack);
}
