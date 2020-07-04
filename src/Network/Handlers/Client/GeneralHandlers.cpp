#include "GeneralHandlers.h"
#include <Networking/MessageHandler.h>
#include <Networking/NetworkPacket.h>
#include <Networking/NetworkClient.h>
#include <Networking/PacketUtils.h>
#include <Networking/AddressType.h>
#include "../../../Utils/ServiceLocator.h"
#include "../../../ECS/Components/Singletons/RealmlistCacheSingleton.h"
#include "../../../ECS/Components/Network/ConnectionSingleton.h"

namespace Client
{
    void GeneralHandlers::Setup(MessageHandler* messageHandler)
    {
        messageHandler->SetMessageHandler(Opcode::CMSG_CONNECTED, { ConnectionStatus::AUTH_SUCCESS, 0, GeneralHandlers::HandleConnected });
        messageHandler->SetMessageHandler(Opcode::CMSG_REQUEST_REALMLIST, { ConnectionStatus::CONNECTED, 0, GeneralHandlers::HandleRequestRealmlist });
        messageHandler->SetMessageHandler(Opcode::CMSG_SELECT_REALM, { ConnectionStatus::CONNECTED, 1, GeneralHandlers::HandleSelectRealm });
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
    bool GeneralHandlers::HandleRequestRealmlist(std::shared_ptr<NetworkClient> client, NetworkPacket* packet)
    {
        entt::registry* registry = ServiceLocator::GetRegistry();
        RealmlistCacheSingleton& realmlistCacheSingleton = registry->ctx<RealmlistCacheSingleton>();

        // Send Realmlist
        client->Send(realmlistCacheSingleton.realmlistBuffer.get());


        return true;
    }
    bool GeneralHandlers::HandleSelectRealm(std::shared_ptr<NetworkClient> client, NetworkPacket* packet)
    {
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<128>();
        buffer->Put(Opcode::SMSG_CONNECTED);
        buffer->PutU16(0);
        client->Send(buffer.get());

        client->SetStatus(ConnectionStatus::CONNECTED);
        return true;
    }
}