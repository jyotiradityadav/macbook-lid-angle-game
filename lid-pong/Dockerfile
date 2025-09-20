# Lid Pong - Docker Container
# Multi-stage build for optimized deployment

# Build stage
FROM ubuntu:22.04 as builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    pkg-config \
    libglfw3-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    xorg-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY src/ ./src/
COPY mac-angle/ ./mac-angle/
COPY Makefile ./

# Build the application
RUN make

# Runtime stage
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libglfw3 \
    libgl1-mesa-glx \
    libglu1-mesa \
    xvfb \
    x11vnc \
    fluxbox \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -s /bin/bash lidpong

# Set working directory
WORKDIR /home/lidpong

# Copy built application from builder stage
COPY --from=builder /app/lid-pong ./
COPY --chown=lidpong:lidpong README.md ./

# Create startup script
RUN echo '#!/bin/bash\n\
# Start virtual display\n\
Xvfb :1 -screen 0 1024x768x24 &\n\
export DISPLAY=:1\n\
\n\
# Start window manager\n\
fluxbox &\n\
\n\
# Start VNC server for remote access\n\
x11vnc -display :1 -nopw -listen localhost -xkb -ncache 10 -ncache_cr -forever &\n\
\n\
echo "Lid Pong Docker Container"\n\
echo "========================"\n\
echo "VNC server running on port 5900"\n\
echo "Connect with VNC viewer to localhost:5900"\n\
echo ""\n\
echo "Note: Lid angle sensor will not work in Docker."\n\
echo "Use keyboard controls (UP/DOWN arrows) instead."\n\
echo ""\n\
\n\
# Run the game\n\
./lid-pong\n\
' > start.sh && chmod +x start.sh

# Switch to non-root user
USER lidpong

# Expose VNC port
EXPOSE 5900

# Set entrypoint
ENTRYPOINT ["./start.sh"]

# Labels
LABEL maintainer="Lid Pong Team"
LABEL description="Lid Pong - MacBook Lid Angle Game in Docker"
LABEL version="1.0.0"
