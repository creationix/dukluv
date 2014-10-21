try {

  var timer = uv.new_timer()
  uv.timer_start(timer, 300, 300, function () {
    print("TIMEOUT!", timer);
    assert(this === timer);
    uv.timer_stop(timer);
    uv.close(timer, function () {
      print("CLOSE!", timer);
      assert(this === timer);
    }
  });

  uv.run();

}
catch (err) {
  print(err.stack);
}
