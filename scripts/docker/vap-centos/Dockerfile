FROM centos:7

RUN yum -q -y install git
RUN git clone --recursive https://github.com/vaporyco/cpp-vapory --branch build-on-linux --single-branch --depth 8
RUN /cpp-vapory/scripts/install_cmake.sh --prefix /usr
RUN /cpp-vapory/scripts/install_deps.sh
RUN yum -y install boost-devel

RUN cd /tmp && cmake /cpp-vapory
RUN cd /tmp && make -j8 && make install && ldconfig

ENTRYPOINT ["/usr/local/bin/vap"]
