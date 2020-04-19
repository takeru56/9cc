FROM ubuntu:latest
MAINTAINER takeru
WORKDIR /home/projects

RUN apt-get update && apt-get install -y \
  vim \
  gcc \
  g++ \
  make \
  git \
  binutils \
  libc6-dev
