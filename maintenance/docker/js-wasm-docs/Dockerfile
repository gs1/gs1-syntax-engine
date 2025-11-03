FROM debian:trixie-slim

ARG NODE_VERSION=25.1.0
ARG NVM_VERSION=0.40.3

RUN chmod 666 /etc/passwd && \
    mkdir -p /home/user && chmod 777 /home/user && \
    chmod -R 777 /root && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        curl ca-certificates bash libatomic1 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ENV NVM_DIR=/root/.nvm

RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v$NVM_VERSION/install.sh | bash && \
    . "$NVM_DIR/nvm.sh" && \
    nvm install $NODE_VERSION && \
    npm i -g jsdoc

ENV PATH="${PATH}:${NVM_DIR}/versions/node/v${NODE_VERSION}/bin"

COPY --chmod=755 docker-entrypoint.sh /docker-entrypoint.sh

WORKDIR /srv

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["jsdoc","-c","jsdoc.json"]
