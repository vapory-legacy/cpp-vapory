FROM fedora:25

RUN dnf -qy install git cmake
RUN git clone --recursive https://github.com/vaporyco/cpp-vapory --branch build-on-linux --single-branch --depth 2
RUN /cpp-vapory/scripts/install_deps.sh

RUN cd /tmp && cmake /cpp-vapory
RUN cd /tmp && make -j8 && make install && ldconfig

ENTRYPOINT ["/usr/local/bin/vap"]
