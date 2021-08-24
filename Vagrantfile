# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "debian/bullseye64"

  config.vm.synced_folder ".", "/home/vagrant/fcpp"

  config.vm.provision "shell", inline: <<-SHELL
    apt-get -qq update
    apt-get -qq -y install doxygen texlive texlive-font-utils cmake ninja-build gcc g++
    apt-get -qq -y install git less procps asymptote htop
    apt-get -qq -y install apt-transport-https curl gnupg git less procps asymptote htop
    curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > /etc/apt/trusted.gpg.d/bazel.gpg
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
    apt-get -qq -y install bazel-bootstrap
    apt-get -qq clean all
  SHELL
end
