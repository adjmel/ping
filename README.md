# ft_ping

## Overview
ft_ping is a custom implementation of the `ping` command based on `inetutils-2.0`. It allows users to test the accessibility of a remote machine over an IP network and measure the round-trip time (RTT) for data packets.

## Features
- Send ICMP echo requests and receive replies.
- Measure round-trip time (RTT).
- Display packet loss statistics.
- Support for verbose output (`-v` option).
- Proper handling of IPv4 addresses and FQDNs without reverse DNS resolution.
- Error handling and edge case management.

## Definitions
### Ping Command
- The `ping` command tests connectivity between two network devices.
- Measures round-trip time (RTT) and reliability.
- Can be used for network troubleshooting and diagnostics.
- It operates by sending ICMP Echo Request packets and waiting for Echo Replies.
- Vulnerable to DoS attacks such as "ping flood".

### Key Concepts
- **IP Network**: A network using the Internet Protocol (IP) for communication.
- **Round-Trip Time (RTT)**: Time taken for a packet to travel to the destination and back.
- **Network Protocol**: Rules that define communication between devices.
- **ICMP Protocol**: Used for sending error messages and operational information.
- **TTL (Time-To-Live)**: Limits the lifespan of a packet in a network.
- **Socket**: Endpoint for sending and receiving data in a network.

## Usage
### Running the program
The program must be run as root:
```sh
sudo ./ft_ping 192.168.0.1
sudo ./ft_ping google.com
sudo ./ft_ping -v 8.8.8.8
```

### Expected Behavior
- Resolves hostnames to IP addresses for sending packets but does not perform reverse DNS lookup on responses.
- Supports verbose mode (`-v`) for detailed packet analysis.
- Provides statistics after execution (packet loss, min/avg/max RTT, etc.).

## Testing
### Basic Tests
```sh
./ft_ping 127.0.0.1  # Localhost test
./ft_ping 8.8.8.8    # Google public DNS
./ft_ping google.com # Domain resolution test
./ft_ping 256.256.256.256  # Invalid IP error
./ft_ping -?  # Display help
```

### Edge Cases
```sh
./ft_ping 10.255.255.1  # Packet loss scenario
./ft_ping 11111111      # Invalid IP handling
./ft_ping -v -?         # Verbose and help combined
./ft_ping 192.168.1.256 # Invalid subnet address
```

## Installation
### Compilation
Ensure you have the necessary dependencies installed, then run:
```sh
make
```
This will produce the `ft_ping` executable.

### Running on Linux
If compiling on Linux, make sure to link with the math library:
```sh
$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lm
```
If needed, install `inetutils-2.0` for reference:
```sh
wget https://ftp.gnu.org/gnu/inetutils/inetutils-2.0.tar.gz
tar -xvzf inetutils-2.0.tar.gz
cd inetutils-2.0
./configure --prefix=/opt/inetutils-2.0
make -j$(nproc)
sudo make install
```

(modifications in progress)
