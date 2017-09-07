#####THIS IS THE UDP CLIENT TOOL THAT WORKS WITH THIS UDP SERVER. ITS FROM `COMPOSITE` CODE-BASE.
#####THIS MODIFIED VERSION WORKS MAINLY WITH THIS SERVER AND SHOULD BE **RUN ON THE HOST PC** WHEN YOU'VE UDP SERVER RUNNING ON THE TARGET.

### TO RUN
```
./udpcli <server_ip> 9998 0 <sleep_val> 9999

<server_ip> is 111.111.111.1 for VM1 and currently only VM1 implements udp server.
9998 - fixed server incoming port. client to write to this port.
0    - fixed payload size which is default to 16bytes (seq number + timestamp)
<sleep_val> number of iterations to sleep for. very crude to use. 0 is to flood as fast as the client can.
9999 - fixed outgoing server port. client to read from this port.
```
