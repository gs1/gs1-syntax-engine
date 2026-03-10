#!/bin/bash

set -euo pipefail

if ! getent passwd "$(id -u)" >/dev/null 2>&1; then
    echo "user:x:$(id -u):$(id -g):Dynamic User:/home/user:/bin/sh" >> /etc/passwd
fi

exec "$@"
