A simple "data hub" system based on IP sockets.

# How it works

After a successful `make`, three executables are built: `hub`, `push`, `pull`.

* `hub` is the server-like thing. Better start it first.

* `push` is designed to keep pushing data to `hub`. Start it as long as you know where to push.
  (Actually it just sends data out to a remote, and doesn't care if anyone is there to receive.)

* `pull` is designed to, well, pull data from `hub`, if available.

When the whole system is ready, `hub` will re-send all data from `push` to `pull`.
As an example, run the following commands each in a new terminal of the same machine:

    $ hub 18000 18001

    $ push 127.0.0.1 18000

    $ pull 127.0.0.1 18001

# The idea

`hub` is what matters. `pull` and `push` can be any client that is able to communicate using sockets.

# TODO

1. Make `hub` a more flexible server. Now it can only serve one puller in its lifetime.