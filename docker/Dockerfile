FROM base/archlinux

ADD https://github.com/Yelp/dumb-init/releases/download/v1.2.1/dumb-init_1.2.1_amd64 /usr/bin/dumb-init
RUN chmod +x /usr/bin/dumb-init
ENTRYPOINT ["/usr/bin/dumb-init", "--"]

RUN pacman -Syy --noconfirm \
 && pacman -S --noconfirm \
        cgdb \
        gcc \
        make \
        python \
        python-pip \
 && pacman -Scc --noconfirm

ADD https://github.com/gcovr/gcovr/archive/master.tar.gz /tmp/gcovr.tar.gz
RUN mkdir /tmp/gcovr -p \
 && tar -C /tmp/gcovr -xzf /tmp/gcovr.tar.gz --strip-components 1 \
 && pip install /tmp/gcovr \
 && rm -rf /tmp/gcovr*

