FROM ubuntu:latest

COPY . /code/
WORKDIR /code/

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
        libcpprest-dev &&\
    apt-get remove -y gcc-9 &&\
    apt autoremove -y &&\
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 20 &&\
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 20 &&\
    mkdir /code/build/

RUN ./getstuff.sh

WORKDIR /code/build/

RUN scan-build cmake .. && scan-build make
