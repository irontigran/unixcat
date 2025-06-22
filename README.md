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

Check the man page and `--help` output for full documentation.

# Examples

Not all of these features are available on all systems.
Run `ucat --help` to see what's available on your installation.

**Listen on a socket; when a peer connects, print out their credentials once, then only the data they send afterwards:**

```
ucat -lR once /tmp/sock
```

**Connect to a socket; pass the bash file descriptor and also attach your credentials to every message:**

```
ucat -f /bin/bash -S always /tmp/sock
```

**Connect to a socket; pretend that you are root (requires sudo privileges):**

```
ucat --uid 0 --gid 0 /tmp/sock
```

**Listen on an abstract address using a datagram socket; print the SELinux context of the peer:**

```
ucat -lu --security @tmpsock
```

# Features and Testing Status

|                   | Linux              | FreeBSD            | NetBSD             | OpenBSD            | DragonFlyBSD       | MacOS              |
| ----------------- | :-----:            | :-----:            | :----:             | :-----:            | :----------:       | :---:              |
| Stream sockets    | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Datagram          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Seqpacket         | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x:                |
| Abstract addrs    | :heavy_check_mark: | :x:                | :x:                | :x:                | :x:                | :x:                |
| Pass fds          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Receive creds     | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x:                | :x:                | :x:                |
| Send creds        | :heavy_check_mark: | :heavy_check_mark: | :x:                | :x:                | :x:                | :x:                |
| Modify creds      | :heavy_check_mark: | :x:                | :x:                | :x:                | :x:                | :x:                |
| SELinux contexts  | :heavy_check_mark: | :x:                | :x:                | :x:                | :x:                | :x:                |
| Testing           | CI                 | Manually           | Manually           | -                  | -                  | CI                 |
