# FCPP Sample Project - Comparison of Collection Algorithms

Sample project provided with the FCPP distribution, designed to help setup of FCPP-based projects.

The project is a transation in FCPP of the experiments in [this repository](https://bitbucket.org/Harniver/aamas19-summarising), presented at [AAMAS 2019 - 18th International Conference on Autonomous Agents and Multiagent Systems](http://aamas2019.encs.concordia.ca), which compare the performance of existing self-stabilising collection algorithms.

This FCPP translation has been presented at [ACSOS 2020 - 1st IEEE International Conference on Autonomic Computing and Self-Organizing Systems](https://conf.researchr.org/home/acsos-2020) through [this paper](http://giorgio.audrito.info/static/fcpp.pdf). For any issues with reproducing the experiments, please contact [Giorgio Audrito](mailto:giorgio.audrito@unito.it).

## Getting Started

### References

- FCPP main website: [https://fcpp.github.io](https://fcpp.github.io).
- FCPP documentation: [http://fcpp-doc.surge.sh](http://fcpp-doc.surge.sh).
- FCPP sources: [https://github.com/fcpp/fcpp](https://github.com/fcpp/fcpp).

### Vagrant

Download Vagrant from [https://www.vagrantup.com](https://www.vagrantup.com), then type the following commands in a terminal:
```
vagrant up
vagrant ssh
cd fcpp
./make.sh run -O runner
```
Then you should get output about building the experiments and running them (in the Vagrant virtual machine). After that you can exit and stop the virtual machine through:
```
exit
vagrant halt
```

### Docker

Download Docker from [https://www.docker.com](https://www.docker.com), then type the following commands in a terminal:
```
docker build -t fcpp .
docker run -it --volume $PWD:/fcpp --workdir /fcpp fcpp bash
./make.sh run -O runner
```
Then you should get output about building the experiments and running them (in the Docker container). After that you can exit and stop the container through:
```
exit
```

### Custom Build

In order to get started on your machine you need the following installed:

- [Bazel](https://bazel.build) (tested with version 2.1.0)
- [GCC](https://gcc.gnu.org) (tested with version 9.3.0)

Once you have them installed, you should be able to run `./make.sh run -O runner`, getting output about building the experiments and running them.
