FROM ubuntu:16.04

RUN apt update && \
    apt upgrade -y && \
    apt install -y \
    g++ 

WORKDIR /app
COPY hello_world.cpp .
RUN g++ hello_world.cpp -o hello_world

ENTRYPOINT ["./hello_world"]
 
