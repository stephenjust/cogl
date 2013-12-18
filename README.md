cogl (Raspberry Pi)
====

This clone of cogl is intended for development of a Raspberry Pi backend.

Building:
Run the following commands to build cogl for Raspberry Pi
./autogen.sh
./configure --prefix=/usr --enable-gles2=yes --enable-cogl-gles2=yes enable-rpi-egl-platform=yes

Progress:
- [x] Create build configuration for Raspberry Pi
- [ ] Clone an existing winsys to create a stub winsys for Raspberry Pi
- [ ] Modify winsys to create an egl window in the framebuffer
- [ ] ...
