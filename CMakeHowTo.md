# FCPPGL - Sample project - CMake build guide

## WINDOWS (MinGW-w64)

To build the sample project, you'll need:

- MinGW-w64 8.1.0 (posix threads)
- CMake 3.9 (or higher)

After setting the respective environmental variables, go into the current directory through the console and type the following commands:
```
cmake -S ./ -B ./bin -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -Wno-dev
cmake --build ./bin/
```

Wait for the application to be built on your Windows system.

## LINUX

To build the sample project, you'll need:

- xorg-dev package (X11)
- CMake 3.9 (or higher)

To install the X11 package, type the following command:
```
sudo apt-get  install xorg-dev
```

After installing X11, go into the current directory through the console and type the following commands:
```
cmake -S ./ -B ./bin -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -Wno-dev
cmake --build ./bin/
```

Wait for the application to be built on your Linux system.
