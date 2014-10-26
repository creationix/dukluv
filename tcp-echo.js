"use strict";

var p = require('modules/utils.js').prettyPrint;

var server = uv.new_tcp();
p("server", server);
p("server.bind", "127.0.0.1", 1337);
uv.tcp_bind(server, "127.0.0.1", 1337);
p("server.listen", 128);
uv.listen(server, 128, function (err) {
  if (err) { throw err; }
  p("server.onconnection");
  var socket = uv.new_tcp();
  p("server.accept", socket);
  uv.accept(server, socket);
  uv.read_start(socket, function (err, chunk) {
    if (err) { throw err; }
    p("socket.onread", chunk, ""+chunk, Array.prototype.slice.call(chunk));
    if (chunk) {
      p("socket.write", chunk);
      uv.write(socket, chunk);
    }
    else {
      p("socket.shutdown");
      uv.shutdown(socket);
      p("socket.read_stop");
      uv.read_stop(socket);
      p("socket.close");
      uv.close(socket);
      p("server.close");
      uv.close(server);
    }
  });
});

var client = uv.new_tcp();
p("client", client);
p("client.connect");
uv.tcp_connect(client, "127.0.0.1", 1337, function (err) {
  if (err) { throw err; }
  p("client.onconnect");
  uv.read_start(client, function (err, chunk) {
    if (err) { throw err; }
    p("client.onread", chunk, ""+chunk, Array.prototype.slice.call(chunk));
    if (chunk) {
      p("client.shutdown");
      uv.shutdown(client);
    }
    else {
      p("client.close");
      uv.close(client);
    }
  });
  p("client.write", "Hello Server\n");
  uv.write(client, "Hello Server\n");
  var buffer = Duktape.Buffer(5);
  buffer[0] = 0;
  buffer[1] = 10;
  buffer[2] = 20;
  buffer[3] = 30;
  buffer[4] = 40;
  p("client.write", buffer);
  uv.write(client, buffer);
});


uv.run();
