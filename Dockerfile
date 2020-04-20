FROM ubuntu:latest
WORKDIR /home/projects
ENV LANG C.UTF-8

RUN apt-get update && apt-get install -y \
  vim \
  gcc \
  g++ \
  make \
  git \
  binutils \
  libc6-dev
