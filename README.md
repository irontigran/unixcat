# ucat

ucat (unixcat) is a tiny networking utility that allows you to talk to unix domain sockets from command line.
It is meant to be reminiscent of using netcat.
In fact, both netcat and socat can do basic communication with a unix domain socket.

But ucat can be used to work with some of the more obscure features of domain sockets:

- Ancillary messages
- Sequenced packets
- Abstract sockets
