NOTE: For all windows builds, you need to open the correct VisualStudio
command prompt for 32/64-bit builds.


OpenSSL 1.1.1h
==============================================================================
Use the below instructions to deploy your own openssl libraries.

PLAT is one of the below:
darwin-x86
darwin-x86_64
linux-x86
linux-x86_64
windows-x86
windows-x86_64
perl Configure VC-WIN64A no-deprecated threads no-comp no-ssl no-bf no-camellia no-cast no-chacha no-md4 no-rc4 no-whirlpool no-rmd160
# Use these options for Linux, Darwin and Windows
BUILD_OPTS=no-deprecated threads no-comp no-ssl no-bf no-camellia \
no-cast no-chacha no-md4 no-rc4 no-whirlpool no-rmd160

# add this to CFLAGS and LDFLAGS for Darwin
MACVER="-mmacosx-version-min=10.4"

Linux or Darwin
-------------------------------
* x86_64 build
Darwin: CFLAGS="$MACVER" LDFLAGS="$MACVER" ./config $BUILD_OPTS
Linux:  ./config $BUILD_OPTS
make
make test

* x86 build (add in cflags and ldflags like above for darwin)
KERNEL_BITS=32 ./config $BUILD_OPTS
make
make test

* copy libraries (includes symlinks)
cp libcrypto.[so|dylib]* libssl.[so|dylib]* $LIBPCLDIR/libs/$PLAT

* copy include
cp -RL include/openssl $LIBPCLDIR/libs/$PLAT/include


Windows
-------------------------------
NOTE: some options have broken some tests.

*) x86_64 build
perl Configure VC-WIN64A $BUILD_OPTS
nmake
nmake test

*) x86 build
perl Configure VC-WIN32 $BUILD_OPTS
nmake
nmake test


libpq 9.6.3
==============================================================================
This is currently only needed by the Device Messaging Service (DMS), which
only runs on Ubuntu 16.04. This is only for phase3 server components.

Linux:
We have to manually compile libpq 9.6.3 so we can link it with our version
of openssl, which differs from the packaged openssl on Ubuntu 16.04.

*) download 9.6.3 source
wget https://ftp.postgresql.org/pub/source/v9.6.3/postgresql-9.6.3.tar.gz

*) tar xvzf postgresql-9.6.3.tar.gz

*) cd postgresql-9.6.3

*) configure the project (link with phase3 libs, not system)
PHASE3DIR=/home/andrew/phase3
LDFLAGS="-L$PHASE3DIR/libs/linux-x86_64" ./configure \
	--with-includes=$PHASE3DIR/libs/linux-x86_64/include \
	--with-libraries=$PHASE3DIR/libs/linux-x86_64 --with-openssl

*) cd src/interfaces/libpq

*) make

*) copy all libpq.so* (symlinks as well) => $PHASE3DIR/libs/$PLAT
*) copy libpq-fe.h and libpq-events.h => $PHASE3DIR/libs/$PLAT/include


zlib 1.2.11
==============================================================================
Used by phase3 applications and required by OpenSSL, cURL and PostgreSQL.

Linux:
*) download 1.2.11 source
wget https://zlib.net/zlib-1.2.11.tar.gz

*) tar xvzf zlib-1.2.11.tar.gz

*) cd zlib-1.2.11

*) ./configure && make

*) copy all libz.so* (symlinks as well) => $PHASE3DIR/libs/$PLAT

*) copy zlib.h and zconf.h => $PHASE3DIR/libs/$PLAT/include

Windows:
todo....
