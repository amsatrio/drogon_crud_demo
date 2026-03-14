# --- Stage 1: Build Stage ---
FROM ubuntu:22.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and Drogon + SQLite dependencies
RUN apt-get update && apt-get install -y \
    git gcc g++ cmake make \
    libjsoncpp-dev uuid-dev zlib1g-dev libssl-dev libsqlite3-dev

# Build Drogon from source
WORKDIR /tmp
RUN git clone https://github.com/drogonframework/drogon.git && \
    cd drogon && git submodule update --init && \
    mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=OFF && \
    make -j$(nproc) install

# Build your app
WORKDIR /app
COPY . .

# --- Stage 2: Runtime Stage (Distroless) ---
# 'cc' flavor includes libstdc++ and glibc
FROM gcr.io/distroless/cc-debian12

WORKDIR /app

# Copy the binary and config
COPY --from=builder /app/build/drogon_crud_demo .
COPY --from=builder /app/config.yaml .
COPY --from=builder /app/config.json .

# Copy necessary shared libraries that aren't in distroless
COPY --from=builder /usr/lib/libdrogon.so* /usr/lib/
COPY --from=builder /usr/lib64/libdrogon.so* /usr/lib64/
COPY --from=builder /usr/local/lib/libdrogon.so* /usr/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libsqlite3.so* /usr/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libjsoncpp.so* /usr/lib/

# Create a data directory for the SQLite file
# Distroless runs as non-root by default if you use :nonroot, 
# but for simple file writes, we'll stay as root for this example.
VOLUME ["/app/data"]

EXPOSE 8080
ENTRYPOINT ["./drogon_crud_demo"]