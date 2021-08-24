FROM debian:bullseye
MAINTAINER giorgio.audrito@gmail.com
ARG DEBIAN_FRONTEND=noninteractive

# Step 2: install packages needed for ./make.sh all
RUN apt-get -qq update &&\
    apt-get -qq -y install doxygen texlive texlive-font-utils cmake ninja-build gcc g++ &&\
    apt-get -qq clean all

# Step 3: install packages needed for ./make.sh run
RUN apt-get -qq -y install git less procps asymptote htop &&\
    apt-get -qq clean all

# Step 4: install packages needed for bazel
RUN apt-get -qq -y install apt-transport-https curl gnupg git less procps asymptote htop &&\
    apt-get -qq clean all &&\
    curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > /etc/apt/trusted.gpg.d/bazel.gpg &&\
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list

# Step 5: install bazel itself
RUN apt-get -qq -y install bazel-bootstrap &&\
    apt-get -qq clean all
