FROM base/archlinux

ADD https://github.com/Yelp/dumb-init/releases/download/v1.2.1/dumb-init_1.2.1_amd64 /usr/bin/dumb-init
RUN chmod +x /usr/bin/dumb-init
ENTRYPOINT ["/usr/bin/dumb-init", "--"]

ADD https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=benchmark /tmp/benchmark/PKGBUILD
ADD https://aur.archlinux.org/cgit/aur.git/plain/PKGBUILD?h=libc%2b%2b /tmp/libc++/PKGBUILD

RUN pacman -Syy --noconfirm \
 && pacman -S --noconfirm \
        clang \
        cmake \
        fakeroot \
        gcc \
        gmock \
        gtest \
        libunwind \
        make \
        ninja \
        python \
        python-pip \
 && pacman -Scc --noconfirm \
 && pip install gcovr \
 && useradd \
      --system \
      --no-create-home \
      --shell /usr/bin/bash \
      builder \
 && chown -R builder /tmp/benchmark \
 && cd /tmp/benchmark \
 && su builder -c 'makepkg --nocheck' \
 && pacman --noconfirm -U \
      /tmp/benchmark/*.pkg.tar.xz \
 && rm -rf /tmp/benchmark \
 && chown -R builder /tmp/libc++ \
 && cd /tmp/libc++ \
 && su builder -c 'makepkg --nocheck --skippgpcheck' \
 && pacman --noconfirm -U \
      /tmp/libc++/*.pkg.tar.xz \
 && rm -rf /tmp/libc++ \
 && pacman -Rns --noconfirm \
      cmake \
      fakeroot \
      gmock \
      gtest \
      libunwind \
      make \
      ninja \
 && userdel builder

WORKDIR /
USER root
