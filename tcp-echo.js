"use strict";
var p = require('modules/utils.js').prettyPrint;
var Tcp = require('modules/classes.js').Tcp;

var server, socket, client;

function Server(host, port) {
  Tcp.call(this);
  p("server", this);
  this.bind(host, port);
  this.listen(128, this.onConnection);
}
Server.prototype.__proto__ = Tcp.prototype;
Server.prototype.onConnection = function onConnection(err) {
  assert(this === server);
  if (err) { throw err; }
  p("server.onconnection");
  socket = new ClientHandler(this);
  p("server.accept", socket);
  this.accept(socket);
  p("socket.readStart");
  socket.readStart(socket.onRead);
};

function ClientHandler(server) {
  Tcp.call(this);
  p("socket", this);
  this.server = server;
}
ClientHandler.prototype.__proto__ = Tcp.prototype;
ClientHandler.prototype.onRead = function onRead(err, data) {
  assert(this === socket);
  if (err) { throw err; }
  p("socket.onread", data);
  if (data) {
    p("socket.write", data);
    this.write(data);
  }
  else {
    p("socket.shutdown");
    this.shutdown();
    p("socket.readStop");
    this.readStop();
    p("socket.close");
    this.close();
    p("server.close");
    this.server.close();
  }
};

function Client(host, port) {
  Tcp.call(this);
  p("client", this);
  p("client.connect");
  this.connect(host, port, this.onConnect);
}
Client.prototype.__proto__ = Tcp.prototype;
Client.prototype.onConnect = function onConnect(err) {
  assert(this === client);
  if (err) { throw err; }
  this.readStart(this.onRead);
  var buffer = Duktape.Buffer(3);
  buffer[0] = 0x10;
  buffer[1] = 0x00;
  buffer[2] = 0x50;
  p("client.write", buffer);
  this.write(buffer);
  p("client.write", "A \0 B");
  this.write("A \0 B", this.onWrite);
};
Client.prototype.onRead = function onRead(err, data) {
  assert(this === client);
  if (err) { throw err; }
  p("client.onread", data);
  if (data) {
    p("client.shutdown");
    this.shutdown();
  }
  else {
    p("client.close");
    this.close();
  }
};
Client.prototype.onWrite = function onWrite(err) {
  assert(this === client);
  if (err) { throw err; }
  p("client.onwrite");
};

server = new Server("127.0.0.1", 1337);
client = new Client("127.0.0.1", 1337);

function assert(cond, message) {
  if (!cond) {
    throw new Error(message || "Assertion Failure");
  }
}
