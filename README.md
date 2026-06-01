# VulkanKV

A simple in-memory key-value store written in C using TCP sockets.

This project was built for learning low-level networking, socket programming, memory management, and basic database concepts.

## Features

- TCP server
- In-memory key-value storage
- SET command
- GET command
- DEL command
- PING command
- Basic error handling

## Build

Compile with GCC:

```bash
gcc main.c -o vulkankv
```

## Run

```bash
./vulkankv
```

The server listens on:

```text
127.0.0.1:8080
```

## Connect

Using netcat:

```bash
nc localhost 8080
```

## Commands

### PING

```text
PING
```

Response:

```text
PONG
```

### SET

Store a key-value pair.

```text
SET name gustavo
```

Response:

```text
OK
```

### GET

Retrieve a value by key.

```text
GET name
```

Response:

```text
gustavo
```

If the key does not exist:

```text
NOT FOUND
```

### DEL

Delete a key.

```text
DEL name
```

Response:

```text
OK
```

If the key does not exist:

```text
NOT FOUND
```

## Current Limitations

- Single-threaded
- In-memory only (no persistence)
- Maximum of 100 key-value pairs
- One client handled at a time
- No hash table implementation yet
- Simple text protocol

## Learning Goals

This project is intended to explore:

- TCP networking
- POSIX sockets
- Command parsing
- In-memory data structures
- Database fundamentals
- Systems programming in C

## Future Improvements

- Hash table storage
- Multiple client support
- Persistence
- Better protocol handling
- Project modularization
- Unit tests
