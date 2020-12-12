# OpenSSL Library {#openssl}
@brief Provides cryptography and SSL support.

PCL uses OpenSSL as the cryptography and SSL library for SSL sockets, PRNG, Message Digests and 
Encryption. It is built to dynamically link with the OpenSSL shared objects on the system. 

PCL does not distribute any resources for OpenSSL. For Unix machines, openssl is almost
always installed. You may need to install the development package. For windows, there are
instructions provided below on how to install it.

Note for Unix machines, make sure that openssl header files are discoverable by GCC/Clang for
building PCL. Also ensure that the shared objects can be found at link and runtime.

### Windows OpenSSL Install
The best way to install OpenSSL on Windows for building and runtime, is to install the prebuilt
OpenSSL Windows package. This can be found here https://slproweb.com/products/Win32OpenSSL.html.
You want to install the "Win64 OpenSSL vX.X.X", where version is the current one listed. The
description makes it clear that this is for development: "Recommended for software developers by
the creators of OpenSSL". If you want to build 32-bit, there is an installer listed there as
well. Do not install the "light" version!

This will install OpenSSL at C:\\Program Files\\OpenSSL-Win64. You will need to adjust your
Environment Variables so cl.exe can find the headers and and link.exe can find the import
libraries. In addition, you also want DLLs to be found at runtime.

#### OpenSSL Environment Variables
To add environment variables so the windows compiler and linker can find development resources
and the DLLs can be found at runtime, follow the below steps:

* From Start Search, type "env" and you should see "Edit the system environment variables"
* Click the "Environment Variables" button
* Under "User variables" add or append the below (they may or may not exist)
    * INCLUDE -> C:\\Program Files\\OpenSSL-Win64\\include (compile)
    * LIB -> C:\\Program Files\\OpenSSL-Win64\\lib (link)
    * PATH -> C:\\Program Files\\OpenSSL-Win64 (runtime)

### OpenSSL Packaging for Distribution
PCL provides no instructions on how to distribute OpenSSL DLLs with your application. That is
completely application-specific. PCL suggests installing the openssl DLLs, found in
C:\\Program Files\\OpenSSL-Win64 directory, alongside your application EXE. Windows always
searches for DLLs in the same directry as the executable file. Where ever your application
installs them, make sure the DLLs can be found at runtime.
