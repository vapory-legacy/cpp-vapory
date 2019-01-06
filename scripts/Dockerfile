# For running this container as non-root user, see
# https://github.com/vaporyco/cpp-vapory/blob/develop/scripts/docker-vap
# or call `docker run` like this:
#
##    mkdir -p ~/.vapory ~/.web3
##    docker run --rm -it \
##      -p 127.0.0.1:8545:8545 \
##      -p 0.0.0.0:30303:30303 \
##      -v ~/.vapory:/.vapory -v ~/.web3:/.web3 \
##      -e HOME=/ --user $(id -u):$(id -g) vapory/client-cpp

FROM alpine

RUN apk add --no-cache \
        libstdc++ \
        gmp \
        libmicrohttpd \
        leveldb --repository http://dl-cdn.alpinelinux.org/alpine/edge/testing/ \
    && apk add --no-cache --virtual .build-deps \
        git \
        cmake \
        g++ \
        make \
        linux-headers libmicrohttpd-dev \
        leveldb-dev --repository http://dl-cdn.alpinelinux.org/alpine/edge/testing/ \
    && sed -i -E -e 's|#warning|//#warning|' /usr/include/sys/poll.h \
    && git clone --recursive https://github.com/vaporyco/cpp-vapory --branch develop --single-branch --depth 1 \
    && mkdir /build && cd /build \
    && cmake /cpp-vapory -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTOOLS=Off -DTESTS=Off \
    && make vap \
    && make install \
    && cd / && rm /build -rf \
    && rm /cpp-vapory -rf \
    && apk del .build-deps \
    && rm /var/cache/apk/* -f

EXPOSE 8545 30303

ENTRYPOINT [ "/usr/local/bin/vap" ]
CMD [ "-j" ]
