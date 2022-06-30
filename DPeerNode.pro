CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DPN_Buffers.cpp \
    DPN_Catalog.cpp \
    DPN_Channel.cpp \
    DPN_ClientCore.cpp \
    DPN_ClientInterface.cpp \
    DPN_ClientUnderLayer.cpp \
    DPN_Core.cpp \
    DPN_DataStream.cpp \
    DPN_FileSystem.cpp \
    DPN_FileSystemProcessors.cpp \
    DPN_IO.cpp \
    DPN_MainChannel.cpp \
    DPN_MediaMaster.cpp \
    DPN_MediaSystem.cpp \
    DPN_MediaSystemProcessors.cpp \
    DPN_Modules.cpp \
    DPN_ModulesIncluder.cpp \
    DPN_Network.cpp \
    DPN_NodeConnector.cpp \
    DPN_PacketType.cpp \
    DPN_Processors.cpp \
    DPN_Propagation.cpp \
    DPN_ThreadBridge.cpp \
    DPN_ThreadMaster.cpp \
    DPN_ThreadUnit.cpp \
    DPN_TransmitProcessor.cpp \
    DPN_TransmitTools.cpp \
    DPN_TransportHandler.cpp \
    DPN_Util.cpp \
    DPeerNode.cpp \
    __dpeernode_global.cpp

HEADERS += \
    DPN_Buffers.h \
    DPN_Catalog.h \
    DPN_Channel.h \
    DPN_ClientCore.h \
    DPN_ClientInterface.h \
    DPN_ClientUnderLayer.h \
    DPN_Core.h \
    DPN_DataStream.h \
    DPN_FileSystem.h \
    DPN_FileSystemProcessors.h \
    DPN_IO.h \
    DPN_MainChannel.h \
    DPN_MediaMaster.h \
    DPN_MediaSystem.h \
    DPN_MediaSystemProcessors.h \
    DPN_Modules.h \
    DPN_ModulesIncluder.h \
    DPN_Network.h \
    DPN_NodeConnector.h \
    DPN_PacketType.h \
    DPN_Processors.h \
    DPN_Propagation.h \
    DPN_ThreadBridge.h \
    DPN_ThreadMaster.h \
    DPN_ThreadUnit.h \
    DPN_TransmitProcessor.h \
    DPN_TransmitTools.h \
    DPN_TransportHandler.h \
    DPN_Util.h \
    DPeerNode.h \
    __dpeernode_global.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += C:/DothProject/DStreamLight/
INCLUDEPATH += F:/FFMPEG_LAST/include

INCLUDEPATH += C:/DothProject/DLogs/
INCLUDEPATH += C:/DothProject/DTL/
INCLUDEPATH += C:/DothProject/DXT/



