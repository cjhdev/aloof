Aloof Protocol
==============

Aloof is a general purpose application protocol for connecting MCUs to 
MCUs and PCs over your choice of transport layer.

It has the following characteristics:

- client/server pattern
    - clients send commands
    - servers send responses to commands
    - servers send unsolicited alerts

- commands read/write primitive data types to objects
    - primitive types include integers, booleans, floats, strings, and blobs
    - composite types are not supported (they can be passed to the application as blobs)
    - objects are identified by integers 
    
- hello message able to be sent by either client or server
    - hello is answered by hello-hello
    - passes protocol version, version specific feature support bit-string, and station mode (client or server)
    - this allows different versions to co-exist
    
- error response
    - provides 1:1 feedback when a protocol level error is encountered (e.g. unknown command)

## Messages and Flow
