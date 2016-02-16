"use strict";

// uv.argv() returns the C argv[] array
uv.argv().forEach(function (v, i) {
  print('arg', i, v);
});
