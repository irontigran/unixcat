# ucat

ucat (unixcat) is a tiny networking utility that allows you to talk to unix domain sockets from command line.
It is meant to be reminiscent of using netcat.
Though both netcat and socat can do basic communication with a unix domain socket, unixcat allows you to exercise some of the more obscure features of unix domain sockets:

- Send ancillary messages, including passing file descriptors, sending and receiving credentials, and providing SELinux security labels.
- Use sequenced-packet sockets as well as stream and datagram sockets.
- Connect and listen to abstract socket addresses.

# Quickstart

unixcat is an autotools project, so you can download the tarball and run the standard commands to configure and intstall it.

```
./configure
make install
```

unixcat will only compile in the features that are available on your system.

Run `ucat -l /tmp/sock` to listen on `/tmp/sock` and `ucat /tmp/sock` to connect to `/tmp/sock`.

Check the man page and `--help` output for more.
