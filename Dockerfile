FROM ubuntu:latest

RUN DEBIAN_FRONTEND=noninteractive apt-get update &&\
    DEBIAN_FRONTEND=noninteractive apt-get install -y\
        wget\
        subversion\
        gcc-10\
        g++-10\
        clang-tools\
        git\
        cmake\
        ninja-build\
        cppcheck\
        openssl\
        libssl-dev\
        libunwind-dev\
        libzmq5\
        libzmq5-dev\
        libcpprest-dev\
        libboost-iostreams-dev\
        libboost-test-dev\
        libboost-log-dev &&\
    apt-get remove -y gcc-9 &&\
    apt autoremove -y

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 20

COPY . /code/
WORKDIR /code/

RUN ./getstuff.sh

RUN mkdir /code/build/
WORKDIR /code/build/

RUN scan-build cmake ..
RUN scan-build make -j10
