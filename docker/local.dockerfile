FROM registry.gitlab.com/attu/cxxjson

ARG GROUP
ARG USER

RUN pacman -Syy --noconfirm \
 && pacman -S --noconfirm cgdb \
 && pacman -Scc --noconfirm \
 && groupadd --gid ${GROUP} user \
 && useradd --uid ${USER} \
            --create-home \
            --gid user \
            --shell /bin/bash \
            user

VOLUME /home/user/cxxjson
USER user
WORKDIR /home/user/cxxjson
