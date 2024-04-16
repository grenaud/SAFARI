Full build instructions are available in the README.

### Prerequisites: Linux
We recommend gcc-4.9 or newer. vg has been shown to build with clang (with some trouble). It has not been tested significantly with other compilers (e.g. icpc).

You will need to install the following dependencies:
```
sudo apt-get install build-essential git cmake pkg-config libncurses-dev libbz2-dev  \
                     protobuf-compiler libprotoc-dev libjansson-dev automake libtool \
                     jq bc rs curl unzip redland-utils librdf-dev bison flex gawk \
                     lzma-dev liblzma-dev liblz4-dev libffi-dev
```
### Prerequisites: Mac OS
You'll need a modern gcc (4.9 or greater). Clang is not officially supported due to openmp oddities with it.

We also recommend either MacPorts or HomeBrew for package management.

You'll need to install the following dependencies, **if using MacPorts**:

```
sudo port install gcc7 libtool \
     jansson jq cmake pkgconfig \
     autoconf automake libtool \
     coreutils samtools redland \
     bison gperftools md5sha1sum \
     rasqal gmake autogen cairo clang-3.8
```

***if using HomeBrew***, you'll also need to modify some environment variables:
```
brew install automake libtool jq \
     jansson coreutils gcc49 samtools \
     pkg-config cmake raptor bison lz4 xz

export PATH="/usr/local/opt/coreutils/libexec/gnubin:/usr/local/bin:$PATH"

# Force use of new version of bison
brew link bison --force

# Use glibtool/ize
export LIBTOOL=glibtool
export LIBTOOLIZE=glibtoolize
# Make symlinks to use gxx-4.9 instead of builtin gxx (CC and CXX not yet fully honored)
ln -s gcc-4.9 /usr/local/bin/gcc
ln -s g++-4.9 /usr/local/bin/g++

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH;
export LIBRARY_PATH=$LD_LIBRARY_PATH;
```

### Building vg

    git clone --recursive https://github.com/vgteam/vg
    cd vg
    source source_me.sh
    make -j 4

### Docker

     docker pull quay.io/vgteam/vg
     docker run -it vgteam/vg