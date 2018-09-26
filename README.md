# simple-echo-server-C
Simple TCP echo server with multi-connection support.

Compile and run

- Server
```
gcc -lpthread echo_server_tcp.c && ./a.out
```

Client
```
gcc tcp_client.c -o client && ./client
```
or
```
nc localhost 12345
```
