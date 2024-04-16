Some of the solutions to common vg problems, and techniques for solving uncommon problems, are listed here.

## Build Problems

### My vg build fails because of something about protobuf.

This can be caused by not having checked out the submodules when you checked out vg. Go to the directory where you checked out vg and do:

```
git submodule update --init --recursive
```

Then try building again.

If that doesn't help, try going into the Protobuf directory and building it separately from vg:

```
cd deps/protobuf
make
```

If that doesn't work, the problem is likely with Protobuf and not vg itself, although to fix it vg may need to be updated to a newer version of Protobuf.

### My vg build fails because of something about a regex and/or an LRUCache, with a bunch of template errors.

or

### My vg build fails and "4.8" is present in the error log.

This can be caused by using an old compiler and/or C++ standard library. The minimum version of GCC that can build vg is 4.9. Make sure you have GCC 4.9 installed, and that running `g++ --version` reports version 4.9 or greater. Also make sure that GCC is not somehow picking up the headers from a wrong, older C++ standard library. Adjust the environment variables `PATH`, `C_INCLUDE_PATH`, `CPLUS_INCLUDE_PATH`, `LIBRARY_PATH`, `LD_LIBRARY_PATH`, and `LD_RUN_PATH` as necessary to select the correct version of the compiler and standard library, if you have had to install either of them yourself.

## Runtime Problems

### My `vg` run is slow/stuck!

Some classes of bugs manifest as a `vg` command never finishing. Debugging these problems can be greatly expedited by obtaining a backtrace from the stuck process, to show where it is stuck.

To get a debug trace from a running `vg`, first you need to know the
PID (process ID) of the vg you want to debug. You can get that with
`pgrep vg` if there is only one vg running. Otherwise you can do `ps
xau | grep $USER | grep vg` and sort out which running vg you want,
and then take the number in the second column.

Then once you have the PID, you start up gdb with `gdb`. Then, if your
PID is "1234", you do `attach 1234` inside of gdb to attach to the
running process (which will pause it). Then you can do `bt` in gdb to
get a backtrace (which may be multiple screens long and make you hit
enter to see them all).

If your backtrace doesn't look informative (for example, if it's just
sitting in "futex wait" or something similar), it could be you are on
a thread that isn't doing anything interesting and some other thread
is really the hold-up. In that case, do `info threads` and find the
number of a thread that isn't just sitting around waiting. (Often you
will see all the threads sitting in the same wait function except for
one that is toiling away at some impossible problem). Then once you
know the number of the thread you want to look at (say it is 10), do
`thread 10` to switch to that thread, and then `bt` again to get a
backtrace for that thread.

When you are done with gdb, do `quit` and let the vg process continue on.

Then you can open a Github issue for the slow snarl finding and attach
the backtrace(s).