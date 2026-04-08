FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg:latest AS builder

WORKDIR /app
COPY CMakeLists.txt ./
COPY src ./src
COPY config ./config

RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg:latest

WORKDIR /app
COPY --from=builder /app/build/medical-api-gateway ./
COPY config/ ./config/

EXPOSE 8080

HEALTHCHECK --interval=10s --timeout=5s --retries=3 \
    CMD curl -f http://localhost:8080/users/admin_user || exit 1

ENTRYPOINT ["/app/medical-api-gateway"]
