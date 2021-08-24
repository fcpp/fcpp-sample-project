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

The next sections contain the setup instructions for the various supported OSs, based on the CMake build system.
For backward compatibility (and faster testing), the Bazel build system is also supported but not recommended: in particular, the OpenGL graphical user interface is not available with Bazel. In order to use Bazel instead of CMake for building, substitute `./make.sh bazel` for `./make.sh` in the commands below.

**Warning:** the graphical simulations are based on OpenGL, and common Virtual Machine software (e.g., VirtualBox) has faulty support for OpenGL. Thus, running the graphical experiments in a VM is not supported: it may work for you, but it is not recommended. Batch simulations should work within VMs, and a Docker container to this aim is provided for convenience.

### Vagrant

Download Vagrant from [https://www.vagrantup.com](https://www.vagrantup.com), then type the following commands in a terminal to enter the Vagrant VM:
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

### Docker

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
but the actual path may vary depending on your installation.

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


## Execution

In order to execute the simulations, type one of the following command in a terminal:
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

Executing a graphical simulation will open a window displaying the simulation scenario, initially still: you can start running the simulation by pressing `P` (current simulated time is displayed in the bottom-left corner). While the simulation is running, network statistics will be periodically printed in the console. You can interact with the simulation through the following keys:
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
