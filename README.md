# MQTT_simulator
## Demonstration project aiming to show the main concepts behind MQTT protocol and basic (simplified) network communication schemes

### Introduction (from WikipediA)
Message Queuing Telemetry Transport (MQTT) is a lightweight, publish-subscribe network protocol used to send messages between devices

It usually runs over TCP/IP, but any network protocol that provides ordered, lossless, bi-directional connections can support MQTT

It is designed for connections with remote locations where a "small code footprint" is required or network bandwidth is limited


### Main Idea
The core of this project consists of two separate programs, called broker and client (explained below), associated with the two parts involved in the MQTT protocol

These programs are (as for now) to be run on the same directory, since they communicate with each other through an automatically created register file ("network")

It is this file that both, brokers and clients (and eventually routers), write to and read from, transfering packets, each of which consists of a single text line


### MQTT Client
An object of class "client" represents an abstraction of an MQTT client, able to communicate with brokers through the network using packets

A client session can be started using the "client" executable located on the exe directory


### MQTT Broker
An object of class "broker" represents an abstraction of an MQTT broker, able to communicate with clients through the network using packets

A broker session can be started using the "broker" executable located on the exe directory


### Sessions
Broker and client (and eventually router) sessions provide some basic user interface to control the corresonding device

A system help is automatically shown whenever a session is started (when the corresponding executable is run), explaining the supported control commands

Sessions use multithread processing to control packets input/output and user interface in paralell 


### Packets
A packet is a list of values for predifined parameters, namely: 

time_stamp       -> UnixTime corresponding to the instant the packet is sent

sender_address   -> address of the device (broker or client) sending the packet

receiver_address -> address of the device (broker or client) receiving the packet

type             -> type of the packet (details below)

payload          -> actual information intended to be transmited (status code, encryption pre-key, topic name, entry pair, type of device joining or leaving the network, etc...)

signature        -> number validating the authenticity of the packet (some sort of simple hash code for the decoded payload)


A packet can be of any of the following types:

join                         -> device joining the network

left                         -> device leaving the network

creq (connection request)    -> client requesting connection to a broker

cres (connection response)   -> broker responding to a connection request

sreq (subscription request)  -> client requesting substription to a broker on some topic

sres (subscription response) -> broker responding to a subscription request

publ (publish)               -> device publishing data entry pair (details below)

ping                         -> device sending ping

disc (disconnection)         -> device notifying connection termitated



### Entry
An instance of type "entry" is a tuple of the form {topic, value} representing an update to that topic state


### Connection
An instance of type "connection" represents an abstraction of an stablished connection to another device

It stores the following data:

address     -> (local or global) address of the device with which the connection is stablished

status_code -> status of the connection (200 meaning OK)

time_stamp  -> UnixTime corresponding to the instant the connection was stablished

key         -> key (or pre-key at the initial Diffie-Hellman key generation process) used to encrypt/decrypt packets

description -> human readable description of the connection

last_ping   -> UnixTime corresponding to the last ping received from the connected device


### Subscription
An instance of type "subscription" represents an abstraction of an stablished subscription from a client to a broker

It stores the following data:

topic       -> topic of the subscription

address     -> (local or global) address of the device with which the subscription is stablished

status_code -> status of the subscription (200 meaning OK)

time_stamp  -> UnixTime corresponding to the instant the subscription was stablished

description -> human readable description of the subscription


### Encryption
Payloads are encrypted using a simple scheme of character shifting 

The key used to encrypt/decrypt packets sent between two devices is generated when the connection is stablished

A simpliflied version of the Diffie-Hellman key exchange protocol is used for this purpose

This key is stored in the "connection" instance created when the connection is stablished





### Multi-Network
So far, every device on this virtual network (every client and broker session) had to be running on the same directory, in order to access the communication file

This was later extended by creating two additional programs, namely: a router and a master router (explained below)


### Router
An object of class "router" represents an abstraction of a network router, able to redirect packets in and out of the network it is conncted to

This way, several virtual networks can be connected to each other, forming sort of a virtual internet

This is achieved by the router by simultaneously communicating with its virtual network (through the "network" file) and the master router (through real network interfaces)

An active router is constantly checking for packets both, on "network" to be uploaded ("post" method) to the master router, and on the master router buffer to be downloaded ("get" method) to the local network

A simple form of addresses table is implemented to deal with local and global addresses

A router session can be started using the "router" executable located on the exe directory


### Master Router
The master router is a set of containers and ever running APIFunctions on the WolframCloud Servers that can be used to interconnect routers and, through them, local virtual networks

Packets to be delivered to each router are buffered as an association from network gate (keys) to list of packets (values), when uploaded

Delivered packets are automatically deleted, and the maximum number of packets stored for a router at any moment is limited

Routers are automatically disconnected after a time without sending requests (taken as pings)






