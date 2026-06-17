FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && apt-get install -y g++ cmake

COPY . .

RUN ./build.sh

ENTRYPOINT [ "./build/lisp" ]
