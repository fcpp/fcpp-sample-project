# FCPP Sample Projects

Sample projects provided with the FCPP distribution, designed to provide guidance for the setup of new FCPP-based projects for various execution paradigms. The repository contains three sample projects:

- **Spreading-collection**. This project shows how a single aggregate program can be setup for being run under different execution paradigms without modifications. It implements a simple composition of spreading and collection blocks, to dynamically calculate the diameter of a network. This project consists of four files:
    - `lib/spreading_collection.hpp` which contains the aggregate program and general setup;
    - `run/spreading_collection_gui.cpp` which executes the program interactively with a GUI;
    - `run/spreading_collection_run.cpp` wich executes the program non-interactively in the command line;
    - `run/spreading_collection_batch.cpp` with executes the program on a batch of scenarios, producing summarising plots.

- **Collection-compare**. This project shows a non-interactive command line-based setup, and is a translation into FCPP of the experiments in [this repository](https://bitbucket.org/Harniver/aamas19-summarising), presented at [AAMAS 2019](http://aamas2019.encs.concordia.ca), which compare the performance of existing self-stabilising collection algorithms. This translation has been presented and evaluated at [ACSOS 2020](https://conf.researchr.org/home/acsos-2020) through [this paper](http://giorgio.audrito.info/static/fcpp.pdf).

- **Channel-broadcast**. This project shows a graphical interactive setup, and implements a paradigmatic aggregate computing routine: two appointed devices communicate through broadcast in a selected elliptical area connecting them. 

All commands below are assumed to be issued from the cloned git repository folder.
For any issues with reproducing the experiments, please contact [Giorgio Audrito](mailto:giorgio.audrito@unito.it).


## References

- FCPP main website: [https://fcpp.github.io](https://fcpp.github.io).
- FCPP documentation: [http://fcpp-doc.surge.sh](http://fcpp-doc.surge.sh).
- FCPP sources: [https://github.com/fcpp/fcpp](https://github.com/fcpp/fcpp).


## Setup

The next sections contain the setup instructions based on the CMake build system for the various supported OSs and virtual containers. Jump to the section dedicated to your system of choice and ignore the others.
For backward compatibility (and faster testing), the [Bazel](https://bazel.build) build system is also supported but not recommended: in particular, the OpenGL graphical user interface is not available with Bazel. In order to use Bazel instead of CMake for building, you have to install it and then substitute `./make.sh bazel` for `./make.sh` in the commands of the "Execution" section.

### Windows

Pre-requisites:
- [Git Bash](https://gitforwindows.org) (for issuing unix-style commands)
- [MinGW-w64](http://mingw-w64.org) (GCC 11.2.0 or higher)
- [CMake 3.18](https://cmake.org) (or higher)
- [Asymptote](http://asymptote.sourceforge.io) (for building the plots)

It is recommended to install MinGW-w64 and CMake through [MSYS2](https://www.msys2.org/) in order to get the latest version of MinGW-w64's GCC and CMake. To do so:
- Download and install MSYS2.
- Run "MSYS2 MSYS" from the start menu; a terminal will appear.
- Run `pacman -Syu`; a restart of all MSYS2 processes is required at the end of the update.
- Run "MSYS2 MSYS" again, and run `pacman -Su`.
- Run `pacman -S --needed base-devel mingw-w64-x86_64-toolchain` to install the MinGW-w64 toolchain.
- Run `pacman -S mingw-w64-x86_64-cmake` to install CMake.
- Run `pacman -S mingw-w64-x86_64-make` to install MinGW-w64's make tool (used by CMake).

After the installation of these packages, make sure to add their path to the `PATH` environment variable (e.g., by editing the `.bashrc` file in your home). They should reside in MSYS2's installation folder as such:
```
C:\msys64\mingw64\bin
```
but the actual path may vary depending on your installation (GCC's and CMake's binaries are already in `PATH` if you execute the "MSYS2 MinGW x64" shortcut from the start menu). Then, you should be able to build the whole library with CMake through:
```
./make.sh gui windows
```

### Linux

Pre-requisites:
- Xorg-dev package (X11)
- G++ 9 (or higher)
- CMake 3.18 (or higher)
- Asymptote (for building the plots)

To install these packages in Ubuntu, type the following command:
```
sudo apt-get install xorg-dev g++ cmake asymptote
```
In Fedora, the `xorg-dev` package is not available. Instead, install the packages:
```
libX11-devel libXinerama-devel.x86_6 libXcursor-devel.x86_64 libXi-devel.x86_64 libXrandr-devel.x86_64 mesa-libGL-devel.x86_64
```

### MacOS

Pre-requisites:
- Xcode Command Line Tools
- CMake 3.18 (or higher)
- Asymptote (for building the plots)

To install them, assuming you have the [brew](https://brew.sh) package manager, type the following commands:
```
xcode-select --install
brew install cmake asymptote
```

### Docker container

**Warning:** the graphical simulations are based on OpenGL, which is **not** available in the Docker container. Use this system for batch simulations only.

Download Docker from [https://www.docker.com](https://www.docker.com), then you can download the Docker container from GitHub by typing the following command in a terminal:
```
docker pull docker.pkg.github.com/fcpp/fcpp/container:1.0
```
Alternatively, you can build the container yourself with the following command:
```
docker build -t docker.pkg.github.com/fcpp/fcpp/container:1.0 .
```
Once you have the Docker container locally available, type the following command to enter the container:
```
docker run -it --volume $PWD:/fcpp --workdir /fcpp docker.pkg.github.com/fcpp/fcpp/container:1.0 bash
```
and the following command to exit it:
```
exit
```
In order to properly link the executables in Docker, you may need to add the `-pthread` option (substitute `-O` for `-O -pthread` below).

### Vagrant container

**Warning:** the graphical simulations are based on OpenGL, which is **not** available in the Vagrant container. Use this system for batch simulations only.

Download Vagrant from [https://www.vagrantup.com](https://www.vagrantup.com) and VirtualBox from [https://www.virtualbox.org](https://www.virtualbox.org), then type the following commands in a terminal to enter the Vagrant container:
```
vagrant up
vagrant ssh
cd fcpp
```
and the following commands to exit it:
```
exit
vagrant halt
```

### Virtual Machines

If you use a VM with a graphical interface, refer to the section for the operating system installed on it.

**Warning:** the graphical simulations are based on OpenGL, and common Virtual Machine software (e.g., VirtualBox) has faulty support for OpenGL. If you rely on a virtual machine for graphical simulations, it might work provided that you select hardware virtualization (as opposed to software virtualization). However, it is recommended to use the native OS whenever possible.


## Execution

In order to execute the simulations, type the following command in a terminal:
```
> ./make.sh [gui] run -O [targets...]
```
The `gui` option, if present, enables the graphical user interface (has no effect on command-line targets). The possible targets are:
- `spreading_collection_gui` (with GUI)
- `spreading_collection_run`
- `spreading_collection_batch` (produces plots)
- `collection_compare`
- `channel_broadcast` (with GUI, produces plots)
- `all` (for running all of the above)
You can also type part of a target and the script will execute every possible expansion (e.g., `comp` would expand to `collection_compare`).

Running the above command, you should see output about building the executables and running them, graphical simulations should pop up (if there are any in the targets), PDF plots should be produces in the `plot/` directory (if any are produced by the targets), and the textual output will be saved in the `output/` directory.

### Graphical User Interface

Executing a graphical simulation will open a window displaying the simulation scenario, initially still: you can start running the simulation by pressing `P` (current simulated time is displayed in the bottom-left corner). While the simulation is running, network statistics may be periodically printed in the console, and be possibly aggregated in form of an Asymptote plot at simulation end. You can interact with the simulation through the following keys:

- `Esc` to end the simulation
- `P` to stop/resume
- `O`/`I` to speed-up/slow-down simulated time
- `L` to show/hide connection links between nodes
- `G` to show/hide the grid on the reference plane and node pins
- `M` enables/disables the marker for selecting nodes
- `left-click` on a selected node to open a window with node details
- `C` resets the camera to the starting position
- `Q`,`W`,`E`,`A`,`S`,`D` to move the simulation area along orthogonal axes
- `right-click`+`mouse drag` to rotate the camera
- `mouse scroll` for zooming in and out
- `left-shift` added to the camera commands above for precision control
- any other key will show/hide a legenda displaying this list

Hovering on a node will also display its UID in the top-left corner.
