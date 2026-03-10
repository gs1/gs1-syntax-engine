FROM mcr.microsoft.com/dotnet/sdk@sha256:f2f0cb3af991eb6959c8a20551b0152f10cce61354c089dd863a7b72c0f00fea

ARG DOCFX_VERSION=2.78.4

ENV DOTNET_CLI_HOME=/root
ENV PATH="${PATH}:/root/.dotnet/tools"

RUN chmod 666 /etc/passwd && \
    mkdir -p /home/user && chmod 777 /home/user && \
    chmod -R 777 /root && \
    dotnet tool update -g docfx --version "${DOCFX_VERSION}"

COPY --chmod=755 docker-entrypoint.sh /docker-entrypoint.sh

WORKDIR /srv

ENTRYPOINT ["/docker-entrypoint.sh"]
CMD ["docfx"]
