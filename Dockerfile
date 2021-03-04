FROM ubuntu:latest

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y gcc-10 g++-10 git cmake
RUN DEBIAN_FRONTEND=noninteractive apt install -y libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev libwebsocketpp-dev openssl libssl-dev
RUN apt-get remove -y gcc-9
RUN apt-get install -y libboost-iostreams-dev libboost-test-dev
RUN apt-get install -y libboost-log-dev
RUN apt-get install -y ninja-build
RUN apt-get install -y pkgconf
RUN apt-get install -y libz-dev
RUN apt-get install -y libunwind-dev
RUN apt-get install -y libcpprest-dev
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 20
COPY . /code
RUN mkdir /code/build/
WORKDIR /code/build/
RUN cmake ..
RUN make -j10
