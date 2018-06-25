FROM base/archlinux

ADD https://github.com/Yelp/dumb-init/releases/download/v1.2.1/dumb-init_1.2.1_amd64 /usr/bin/dumb-init
RUN chmod +x /usr/bin/dumb-init
ENTRYPOINT ["/usr/bin/dumb-init", "--"]

RUN pacman -Syy --noconfirm \
 && pacman -S --noconfirm \
        cmake \
        fakeroot \
        gcc \
        gmock \
        gtest \
        make \
        python \
        python-pip \
 && pacman -Scc --noconfirm \
 && useradd \
      --system \
      --no-create-home \
      --shell /usr/bin/bash \
      builder

ADD https://github.com/gcovr/gcovr/archive/master.tar.gz /tmp/gcovr.tar.gz
RUN mkdir /tmp/gcovr -p \
 && tar -C /tmp/gcovr -xzf /tmp/gcovr.tar.gz --strip-components 1 \
 && pip install /tmp/gcovr \
 && rm -rf /tmp/gcovr*

ADD https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=benchmark /tmp/benchmark/PKGBUILD
RUN chown -R builder /tmp/benchmark

USER builder
WORKDIR /tmp/benchmark

RUN makepkg

WORKDIR /
USER root

RUN pacman --noconfirm -U \
      /tmp/benchmark/benchmark-1.4.1-1-any.pkg.tar.xz \
 && rm -rf /tmp/benchmark
