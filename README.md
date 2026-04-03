# Chaos Proxy

This project is a custom-built HTTP proxy written in C++ using `epoll` for efficient I/O multiplexing.  
It is designed as a foundation for a **chaos testing tool**, capable of simulating unreliable network conditions between clients and backend servers.

## Current Status

⚠️ **Work in progress**  
At this stage, only the **core proxy functionality** has been implemented.

- Handles multiple frontend clients using `epoll`
- Establishes a corresponding backend connection per client
- Forwards data bidirectionally
- Uses non-blocking sockets and event-driven architecture

## Planned Chaos Features

The proxy is being extended with a "chaos engine" to inject faults into live traffic, including:

- **Byte corruption** — randomly mutating bytes in transmitted data
- **Chunk drop** — dropping parts of requests or responses
- **Artificial delays** — introducing latency between reads/writes
- **Connection termination** — closing connections unexpectedly

These features aim to simulate real-world network instability and test backend resilience.

***The proxy listens internally on port 5050*

## Usage

```bash
./proxy -b <backend_ip> -p <backend_port>
