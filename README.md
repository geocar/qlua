# Pre-Flight

## Building on OSX

Linking with m64/c.o requires GCC

    brew install gcc lua rlwrap

## Building on Linux

Default is to build lua using l64/c.o

    apt-get install build-essential liblua5.1-dev

but kdb is 32-bit, so to get *it* working you'll need:

    dpkg --add-architecture i386
    apt-get update
    apt-get install -yy --no-install-recommends ia32-libs gcc rlwrap

# Testing

Start up kdb in a window:

    rlwrap ~/q/?32/q

and load the server:

    \l test.q

Run the lua-side tests in another window:

    make test

