cogl 1.18
=========

This clone of the cogl repository is to demonstrate cogl functioning on the Raspberry Pi. At this point, the code is still experimental, however it can display some simple cogl demos and clutter demos.

Building Cogl
-------------

Generate Makefiles using the following:

```
./autogen.sh --prefix=/usr --enable-gles2=yes --enable-cogl-gles2=yes --enable-rpi-egl-platform=yes
make
sudo make install
```

The system cogl libraries are installed in /usr/lib/arm-linux-gnuabihf. You may need to move these elsewhere for GCC to find your libraries.

Note that the cogl build will fail at building tests-conformance. These are not being built with the -lGLESv2 flag so they cannot find some methods. The Broadcom libraries move some libraries between libEGL and libGLESv2 in their implementation.

Using Clutter
-------------

To use clutter, you must compile it from source. The Raspbian packages are not built with support for an EGL backend.

You can get clutter 1.14 using git:

```
git checkout https://github.com/GNOME/clutter -b clutter-1.14
```

Build clutter 1.14 with the following:

```
./autogen.sh --prefix=/usr --enable-x11-backend=yes --enable-egl-backend=yes --enable-evdev-input=yes --enable-docs=no
make
sudo make install
```

You can use this combination of clutter and cogl to use the sample programs located at https://github.com/clutter-project/toys. These demo programs appear to run relatively smoothly at 1920x1080. There is likely more room for optimization within cogl to speed things up.
