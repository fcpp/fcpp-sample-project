# FCPP Sample Project

Sample project provided with the FCPP distribution, designed to help setup of FCPP-based projects.
This project consists of both *batch* and *graphical* simulations.

The first is a translation in FCPP of the experiments in [this repository](https://bitbucket.org/Harniver/aamas19-summarising), presented at [AAMAS 2019](http://aamas2019.encs.concordia.ca), which compare the performance of existing self-stabilising collection algorithms. This translation has been presented and evaluated at [ACSOS 2020](https://conf.researchr.org/home/acsos-2020) through [this paper](http://giorgio.audrito.info/static/fcpp.pdf).

The second is a graphical implementation of two paradigmatic aggregate computing routines: in *spreading collection* nodes compute distances from a leader, gathering the maximal distance towards it, which then broadcasts the obtained diameter estimate back to the network; in *channel broadcast* two appointed devices communicate by selecting an elliptical area connecting them. 

All commands below are assumed to be issued from the cloned git repository folder.
For any issues with reproducing the experiments, please contact [Giorgio Audrito](mailto:giorgio.audrito@unito.it).

## References

- FCPP main website: [https://fcpp.github.io](https://fcpp.github.io).
- FCPP documentation: [http://fcpp-doc.surge.sh](http://fcpp-doc.surge.sh).
- FCPP sources: [https://github.com/fcpp/fcpp](https://github.com/fcpp/fcpp).

## Batch Simulation

The *collection compare* batch simulation is preferably executed through the [Bazel](https://bazel.build) build system, and can be either done in a virtual environment for an easier setup, or in the native OS for increased performance. Execution instructions follow for the various supported mediums.

### Vagrant

Download Vagrant from [https://www.vagrantup.com](https://www.vagrantup.com), then type the following commands in a terminal:
```
vagrant up
vagrant ssh
cd fcpp
./make.sh run -O collection_compare
```
Then you should get output about building the experiments and running them (in the Vagrant virtual machine). After that you can exit and stop the virtual machine through:
```
exit
vagrant halt
```

### Docker

Download Docker from [https://www.docker.com](https://www.docker.com), then download the Docker container from GitHub by typing the following command in a terminal:
```
docker pull docker.pkg.github.com/fcpp/fcpp/container:1.0
```
Alternatively, you can build the container yourself with the following command:
```
docker build -t docker.pkg.github.com/fcpp/fcpp/container:1.0 .
```
Once you have the Docker container locally available, type the following commands:
```
docker run -it --volume $PWD:/fcpp --workdir /fcpp docker.pkg.github.com/fcpp/fcpp/container:1.0 bash
./make.sh run -O collection_compare
```
Then you should get output about building the experiments and running them (in the Docker container). After that you can exit and stop the container through:
```
exit
```

### Custom Build

In order to get started on your machine you need the following installed:

- [Bazel](https://bazel.build) (tested with version 2.1.0)
- [GCC](https://gcc.gnu.org) (tested with version 9.2.0) or [Clang](https://clang.llvm.org) (tested with Apple Clang 12.0.5)

Once you have them installed, you should be able to run `./make.sh run -O collection_compare`, getting output about building the experiments and running them.

## Graphical Simulation

The OpenGL-based graphical simulations can only be built through the [CMake](https://cmake.org) build system in the native OS. Common Virtual Machine software (e.g., VirtualBox) has faulty support for OpenGL, hence running the graphical experiments in a VM is not supported: it may work for you, but it is not recommended. Execution instructions follow for the various supported OSs, followed by a description of the two demo scenarios and their user interface.

### Windows

Pre-requisites:
- [Git Bash](https://gitforwindows.org) (for issuing unix-style commands)
- [MinGW-w64 builds 8.1.0](http://mingw-w64.org/doku.php/download/mingw-builds)
- [CMake 3.9](https://cmake.org) (or higher)
- [Asymptote](http://asymptote.sourceforge.io) (for building the plots)

During CMake installation, make sure you select to add `cmake` to the `PATH` (at least for the current user).
During MinGW installation, make sure you select "posix" threads (should be the default) and not "win32" threads. After installing MinGW, you need to add its path to the environment variable `PATH`. The default path should be:
```
C:\Program Files (x86)\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin
```
but the actual path may vary depending on your installation. Then, type the following command in a terminal:
```
> ./make.sh gui windows [target...]
```
where `[target...]` are among the two described in the *Demo Scenarios* subsection (if present). You should see output about building the executables, then the graphical simulation should pop up.

### Linux

Pre-requisites:
- Xorg-dev package (X11)
- G++ 9 (or higher)
- CMake 3.9 (or higher)
- Asymptote (for building the plots)

To install these packages in Ubuntu, type the following command:
```
sudo apt-get install xorg-dev g++ cmake asymptote
```
Then, type the following command in a terminal:
```
> ./make.sh gui unix [target...]
```
where `[target...]` are among the two described in the *Demo Scenarios* subsection (if present). You should see output about building the executables, then the graphical simulation should pop up.

### MacOS

Pre-requisites:
- Xcode Command Line Tools
- CMake 3.9 (or higher)
- [Asymptote](http://asymptote.sourceforge.io) (for building the plots)

To install them, assuming you have the [brew](https://brew.sh) package manager, type the following commands:
```
xcode-select --install
brew install cmake asymptote
```
Then, type the following command in a terminal:
```
> ./make.sh gui unix [target...]
```
where `[target...]` are among the two described in the *Demo Scenarios* subsection (if present). You should see output about building the executables, then the graphical simulation should pop up.

### Demo Scenarios

The installation instructions above build two demo scenarios in the `bin/` directory:
- Channel Broadcast (executable and target `channel_broadcast`)
- Spreading Collection (executable and target `spreading_collection`)

To launch a scenario manually, move to the `bin` directory and run its executable. This will open a window displaying the simulation scenario, initially still: you can start running the simulation by pressing `P` (current simulated time is displayed in the bottom-left corner). While the simulation is running, network statistics will be periodically printed in the console. You can interact with the simulation through the following keys:
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
