#include "GeneralHandlers.h"
#include <Networking/MessageHandler.h>
#include <Networking/NetworkPacket.h>
#include <Networking/NetworkClient.h>
#include <Networking/PacketUtils.h>
#include <Networking/AddressType.h>
#include "../../../Utils/ServiceLocator.h"
#include "../../../ECS/Components/Network/ConnectionSingleton.h"

namespace Client
{
    void GeneralHandlers::Setup(MessageHandler* messageHandler)
    {
        messageHandler->SetMessageHandler(Opcode::CMSG_CONNECTED, { ConnectionStatus::AUTH_SUCCESS, 0, GeneralHandlers::HandleConnected });
    }

    bool GeneralHandlers::HandleConnected(std::shared_ptr<NetworkClient> client, NetworkPacket* packet)
    {
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<128>();
        buffer->Put(Opcode::SMSG_CONNECTED);
        buffer->PutU16(0);
        client->Send(buffer.get());

        client->SetStatus(ConnectionStatus::CONNECTED);
        return true;
    }
}