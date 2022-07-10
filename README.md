# DPeerNode

## About
DPeerNode is a kit of tools for creating peer2peer applications. There is tools for creating p2p connections, clients interface,
creating shadow channels (TCP/UDP) between clients, creating data stream with any user behaivor, for any data types.

## Modules

Modules in DPeerNode is implementations of abstract `DPN::AbstractModule` and using for provide specific methods and interfaces 
over channels ( `DPN::Channel` namespace ), clients data ( see `DPN::Client` namespace ) etc.

## Packets
For transmiting data and information between peers (clients) DPeerNode use single main TCP channel
to send special packets. Each packet has own type `DPN::PacketType`

## Packet processors
To do some actions before, after and during clients dialog - DPeerNode use packet processors ( `DPN::PacketProcessor` ).

## Threads
DPeerNode use thread system which penetrate many DPeerNode classes ( see `DPN::Thread::ThreadUser` ).
For one DPeerNode user should exist one ThreadUser which sharing to many classes down. 
Each task you want to run in thread should inherit by `DPN::Thread::AbstractThreadUnit`. Also see `DPN::Thread::ThreadUnit` and
`DPN::Thread::ThreadUnitWrapper`. 

## IO node
DPeerNode use abstract `DPN::IO::IONode` to build any propagation schemes with `DPN::IO::IONode` implementations.
Each IONode can be include to scheme in any place ( like entry-point or middle node or end-point ) to 
get data from previos IONode ( entry-point node will generate data ), process it and propagate to 
next IONode or several IONodes

## Propagation schemes

Each propagation scheme is a line or tree of IO nodes.
Data in schemes propagate from entry point to end point (or end points in tree scheme).
Each IO node is abstract tool and DPeerNode provide set of implementations of this tool, but
user can create own IO nodes to include it to scheme.
Each scheme is a child of DPN::Thread::Unit so it simple to put it to thread units pool.

## Network Module
Network module provide interfaces to:
- Create virtual servers to implement boradcast, multicast, unicast TCP/UDP channels between clients
- Use peer node as proxy to implement proxy connection between clients
- Create shadow TCP/UDP direct connections between clients to reserve it for any modules
- Sharing local environment to each client to let them expand their peer-space

## Media Module
Media module provide interfaces to create special media schemes. For example you can build tree-like scheme like this:

```
                              [Saver]
                  [Encoder] ->
                              [Sender]
[CaptureMaster] ->            

                  [Render]
                  
                  [Drawer] -> [Encoder] -> [Sender]
                  
[CaptureMaster] - IONode using to capture raw audio/video frames from device or mem
[Encoder]       - IONode using to encode raw frames with audio/video codecs (like H264)
[Render]        - IONode using to render raw frames with any user tool
[Drawer]        - IONode by user using to draw on raw frames (for example: time, logo or AR things)
[Saver]         - IONode using to save encoded audio/video frames to file (make usual audio/video files like mp3 or mkv)
                    or save to mem ( for example: save audio piece to mem to use this like audio-message to send after )
[Sender]        - IONode using to send encoded audio/video frames throuh TCP/UDP channel
```
With media module user can:
- Create media schemes to use it for make audio/video calls
- Create media schemes to make audio/video/photo messages
- Create group media chats ( with `DPN::Network::Server` )

Compile DPeerNode with uncommented `DPN_INCLUDE_FFMPEG_FOR_MEDIA` to use ffmpeg in DPeerNode or
comment this macro before compiling to replace ffmpeg media core to abstract tool which user can use to define media core behaivor

## File Module

File module is:
- File catalogs
- Sync catalog between clients or clients group
- Transmit files directly between clients through special TCP/UDP channels
- Checking file's hash sums on both peers to make sure file transmited correctly
- Checking file catalog's versions on both peers to make sure file request is actual
- Consider download parts of file to request only missing parts
- File transmiting can be distribure between channels and threads
- P2P file transmiting

## Chat Module
Chat module provide special interfaces to:
- Create peer2peer chat with chat things like a chat legend or messages status ( `sent`, `delivired`, `failed`, `viewed` )
- Create group chat using `DPN::Network::Server`


## Crypto
Special `DPN::IO::IONode` inplementation which provide fast symmetric encryption in propagation schemes.
Such io node can be placed in scheme to encode data stream before put it to TCP/UDP channel.

For secure transporting symmetric key to peer DPeerNode provide asymetric encryption tool ( raw state now ).
Possible asymteric encryption implementations:
- OpenSSL library to use RSA algorithm ( direct including to project or asbtract interface to implement it external )
- Own RSA implementation with any keys lenght (in process; see DRSA project)
- Own asymetric algorytm 

