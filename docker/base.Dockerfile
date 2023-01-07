FROM ubuntu:22.04

RUN apt-get update && apt-get install -y build-essential cmake g++ wget unzip openmpi-bin openmpi-doc libopenmpi-dev openssh-server

RUN useradd -m -d /home/mpiuser -s /bin/bash mpiuser
RUN #echo 'mpiuser:mpiuser' | chpasswd

RUN service ssh start

RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip
RUN unzip opencv.zip

RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.tar.bz2
RUN tar -xf boost_1_81_0.tar.bz2

WORKDIR /build
RUN cmake  ../opencv-4.x
RUN cmake --build .
RUN make install

WORKDIR /boost_1_81_0
RUN ./bootstrap.sh
RUN echo "using mpi ;" >> project-config.jam
RUN ./b2
RUN ./b2 install

ENV PATH="${PATH}:/boost_1_81_0"
ENV LD_LIBRARY_PATH="/usr/local/lib"

