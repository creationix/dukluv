"use strict";
exports.Emitter = Emitter;
function Emitter() {}
Emitter.prototype.on = function on(name, callback) {
  var handlers = this.handlers || (this.handlers = {});
  var list = handlers[name] || (handlers[name] = []);
  list.push(callback);
};
Emitter.prototype.off = function off(name, callback) {
  var handlers = this.handlers;
  if (!handlers) { return; }
  var list = handlers[name];
  if (!list) { return; }
  var index = list.indexOf(callback);
  if (callback < 0) { return; }
  list.splice(index, 1);
};

exports.Handle = Handle;
function Handle() {
  throw new Error("Can't create Handle directly");
}
Handle.prototype.__proto__ = Emitter.prototype;
Handle.prototype.close = function close(callback) {
  return uv.close(this.handle, callback);
};

exports.Stream = Stream;
function Stream() {
  throw new Error("Can't reate Stream directly");
}
Stream.prototype.__proto__ = Handle.prototype;
Stream.prototype.readStart = function readStart(callback) {
  return uv.read_start(this.handle, callback);
};
Stream.prototype.readStop = function readStop() {
  return uv.read_stop(this.handle);
};
Stream.prototype.listen = function listen(backlog, callback) {
  return uv.listen(this.handle, backlog, callback);
};
Stream.prototype.accept = function accept(client) {
  return uv.accept(this.handle, client.handle);
};
Stream.prototype.write = function write(data, callback) {
  return new Req(uv.write.call(this, this.handle, data, callback));
};
Stream.prototype.shutdown = function shutdown(callback) {
  return new Req(uv.shutdown.call(this, this.handle, callback));
};

exports.Tcp = Tcp;
function Tcp() {
  this.handle = uv.new_tcp.call(this);
}
Tcp.prototype.__proto__ = Stream.prototype;
Tcp.prototype.bind = function bind(host, port) {
  return uv.tcp_bind(this.handle, host, port);
};
Tcp.prototype.connect = function connect(host, port, callback) {
  return new Req(uv.tcp_connect.call(this, this.handle, host, port, callback));
};

exports.Req = Req;
function Req(req) {
  this.req = req;
}
Req.prototype.cancel = function cancel() {
  return uv.cancel(this.req);
};
