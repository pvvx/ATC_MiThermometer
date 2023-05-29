FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Amsterdam

# Update package manager and install other package dependencies.
# Remove the apt cache when done since it's useless for Docker and only increases image size
RUN apt-get -y update && \
    apt-get install -y \
    curl \
    xz-utils \
    git \
    dpkg-dev \
    wget \
    libssl-dev \
    libncurses5 libncursesw5 \
    build-essential \
    python3 \
    python-pip \
    && rm -rf /var/lib/apt/lists*