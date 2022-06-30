# DPeerNode

DPeerNode is a kit of tools for creating peer2peer applications. There is tools for creating p2p connections, clients interface,
creating shadow channels (TCP/UDP) between clients, creating data stream with any user behaivor, for any data types.

DPeerNode provide media system tool:
1. media streams (audio/video)
2. audio/video calls
3. audio/video/photo messages
4. low-level tools to create own media system and streams
Compile DPeerNode with uncommented DPN_INCLUDE_FFMPEG_FOR_MEDIA to use ffmpeg in DPeerNode or
comment this macro before compiling to replace ffmpeg media core to abstract tool which user can use to define media core behaivor

DPeerNode provide file system tool:
1. file streams (TCP&UDP)
2. file catalogs with sync catalogs mechanism
3. p2p file transmiting
4. distribute file transmitining between channels and trheads

DPeerNode provide propagation tool:
Each propagation scheme is a line or tree of IO nodes.
Data in schemes propagate from entry point to end point (or end points in tree scheme).
Each IO node is abstract tool and DPeerNode provide set of implementations of this tool, but
user can create own IO nodes to include it to scheme.
Each scheme is a child of DPN::Thread::Unit so it simple to put it to thread units pool.

DPeerNode provide crypto module and specific IO implemetations. Crypto IO node can be set in any propagation schemes
to encrypt data for next nodes.

DPeerNode provide network tools for share client lists between other clients to make possible create net(s).
DPeerNode provide network tools for use some peer nodes as proxy-node to make proxy-connections.
DPeerNode provide text message module to exchange text messages and create group chats.
