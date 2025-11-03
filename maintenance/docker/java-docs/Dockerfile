FROM debian:trixie-slim

RUN chmod 666 /etc/passwd && \
    mkdir -p /home/user && chmod 777 /home/user && \
    chmod -R 777 /root && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends default-jdk-headless && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

COPY --chmod=755 docker-entrypoint.sh /docker-entrypoint.sh

WORKDIR /srv

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["javadoc", "-sourcepath", "../../../src/java", "-d", "../../../docs/java", "-notimestamp", "org.gs1.gs1encoders"]
