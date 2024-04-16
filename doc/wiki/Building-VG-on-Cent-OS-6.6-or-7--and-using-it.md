#  Cent-OS 7 

Main task is to get an uptodate gcc on your platform.
You can ask a sys admin to do so but building gcc is not to bad

Once you have the new gcc just have it in your path before the existing one.
e.g. I have gcc 5.3.0 build and installed into /root/gcc
so my path is ```export PATH="/usr/local/bin:$PATH"```

Then you need to make sure VG uses the right glibc/libstc++.so

```export LD_PRELOAD="/usr/local/lib64/libstdc++.so"```

Make sure you install some basic tools if they are not already installed. You'll need the `protobuf` and `jansson` development libraries installed on your server. Additionally, to run the tests, you will need `jq`, `bc` and `rs`.

```
sudo yum install gcc gcc-c++ glibc-devel make git cmake pkgconfig ncurses-devel bzip2-libs  \
                protobuf-compiler protobuf protobuf-compiler protobuf-devel automake libtool \
                bc rs curl zip redland-devel bison flex gawk \                     
```
You will need rpm files to install `liblzma-dev` `lzma-dev` `liblz4-dev` `libjansson-dev` and `jq`.

```
# wget http://ftp.tu-chemnitz.de/pub/linux/dag/redhat/el7/en/x86_64/rpmforge/RPMS/lzma-libs-4.32.7-1.el7.rf.x86_64.rpm
# rpm -Uvh lzma-libs-4.32.7-1.el7.rf.x86_64.rpm
# wget http://ftp.tu-chemnitz.de/pub/linux/dag/redhat/el7/en/x86_64/rpmforge/RPMS/lzma-4.32.7-1.el7.rf.x86_64.rpm
# rpm -Uvh lzma-4.32.7-1.el7.rf.x86_64.rpm
# wget http://ftp.tu-chemnitz.de/pub/linux/dag/redhat/el7/en/x86_64/rpmforge/RPMS/lzma-devel-4.32.7-1.el7.rf.x86_64.rpm
# rpm -Uvh lzma-devel-4.32.7-1.el7.rf.x86_64.rpm
# sudo yum install lzma-devel
# sudo yum install lz4
#wget http://springdale.math.ias.edu/data/puias/7/x86_64/os/Addons/Packages/lz4-devel-r131-1.el7.x86_64.rpm
# rpm -Uvh lz4-devel-r131-1.el7.x86_64.rpm
# sudo yum install lz4-devel
```
For `jansson`
```
# wget http://www.digip.org/jansson/releases/jansson-2.7.tar.gz
# tar -zxvf jansson-2.7.tar.gz
# cd jansson-2.7
# ./configure -–prefix=/usr/
# make clean
# make && make install
# ldconfig
```
For `jq`
```
# wget https://github.com/stedolan/jq/releases/download/jq-1.5/jq-linux64 -O jq
# chmod +x jq
# mv jq /usr/local/bin
```

Now, obtain the repo and its submodules:

```git clone --recursive https://github.com/vgteam/vg.git```
Then build with ```. ./source_me.sh && make static```, and run with ```./bin/vg```.



# Cent-OS 6.6

Warning: this is a major hackatack, using this in production ends badly and quickly...

Cent-OS 6.6 is an old piece of code, but it is what is installed on many a cluster. The first problem is the VG does not compile with the ancient GCC version installed on it.

#Dependencies

## GCC

Install a new GCC, but not to new as VG itself is not liked by GCC 5.3.0. i.e. many tests fail on this version.

## LibRDF

We need librdf version 2 but that is not available on cent-os 6.6. So build one from source.

## Things from yum

yum install ncurses-devel zlib-devel libbsd libbsd-devel samtools jq bzip2-devel

# Compiling VG

Use your new gcc, by making sure its first in the path.
Make sure the build lib64 is first in your LD_LIBRARY_PATH env.
Do the same for raptor2 adding it after GCC libs.
Make sure raptor2 is included correctly or just be evil and copy the things into vg/lib and vg/include if you can't be bothered.

## XG
XG now depends on DYNAMIC make sure you get it. Also do not build it static. Your base system does not have static pthread or zlib. You will waste hours trying to figure out why libz.so can't be found when its there. Well XG want libz.a.... Hack the makefile to not build static things.

## VG
```bash
. ./source_me.sh; make && make test
```

Cry for all the failing tests...

## Getting rs

rs is a command that is apt-get able on ubuntu/debian but seems to miss a package on fedora/cent-os.

You can get the source at
https://www.mirbsd.org/MirOS/dist/mir/rs/
and compile it with
```bash
cd rs
gcc -lbsd -Wall -Wextra -Wformat -DUSE_LIBBSD rs.c 
```
Then your tests should passs

