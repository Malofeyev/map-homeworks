FROM alpine:3.21.3

RUN apk add --no-cache \
clang \
g++

WORKDIR /app
COPY 2.cpp .

RUN g++ -o app_gcc 2.cpp
RUN clang++ -o app_clang 2.cpp
ENTRYPOINT ["sh", "-c", "echo 'Running gcc' && ./app_gcc && echo 'Running clang' && ./app_clang"]

