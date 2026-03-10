FROM swift:5.9

RUN chmod 666 /etc/passwd && \
    mkdir -p /home/user && chmod 777 /home/user && \
    chmod -R 777 /root && \
    mkdir -p /.cache/clang/ModuleCache && \
    mkdir -p /.cache/org.swift.swiftpm && \
    chmod -R 777 /.cache && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        ruby-full git build-essential libsqlite3-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    gem install jazzy && \
    git clone --depth 1 --branch 0.35.0 https://github.com/jpsim/SourceKitten.git /tmp/sourcekitten && \
    cd /tmp/sourcekitten && \
    swift build -c release && \
    JAZZY_SOURCEKITTEN=$(find /var/lib/gems -name sourcekitten -path "*/jazzy-*/bin/sourcekitten" | head -1) && \
    cp .build/release/sourcekitten "$JAZZY_SOURCEKITTEN" && \
    rm -rf /tmp/sourcekitten

COPY --chmod=755 docker-entrypoint.sh /docker-entrypoint.sh

WORKDIR /srv

COPY --chmod=755 generate-docs.sh /generate-docs.sh

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["/generate-docs.sh"]
