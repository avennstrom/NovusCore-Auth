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

    bool GeneralHandlers::HandleConnected(std::shared_ptr<NetworkClient> client, std::shared_ptr<NetworkPacket>& packet)
    {
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<128>();
        buffer->Put(Opcode::SMSG_CONNECTED);
        buffer->PutU16(0);
        client->Send(buffer);

        client->SetStatus(ConnectionStatus::CONNECTED);
        return true;
    }
    bool GeneralHandlers::HandleRequestRealmlist(std::shared_ptr<NetworkClient> client, std::shared_ptr<NetworkPacket>& packet)
    {
        entt::registry* registry = ServiceLocator::GetRegistry();
        RealmlistCacheSingleton& realmlistCacheSingleton = registry->ctx<RealmlistCacheSingleton>();

        // Send Realmlist
        client->Send(realmlistCacheSingleton.realmlistBuffer);

        return true;
    }
    bool GeneralHandlers::HandleSelectRealm(std::shared_ptr<NetworkClient> client, std::shared_ptr<NetworkPacket>& packet)
    {
        entt::registry* registry = ServiceLocator::GetRegistry();
        RealmlistCacheSingleton& realmlistCacheSingleton = registry->ctx<RealmlistCacheSingleton>();

        u8 realmIndex = 0;
        if (!packet->payload->GetU8(realmIndex))
            return false;

        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<128>();

        auto itr = realmlistCacheSingleton.realmlistMap.find(realmIndex);
        if (itr == realmlistCacheSingleton.realmlistMap.end())
        {
            // Realm does not exist, send back "SMSG_SEND_ADDRESS" to tell the client theres no available server

            if (!PacketUtils::Write_SMSG_SEND_ADDRESS(buffer, 0))
                return false;
            
            client->Send(buffer);
        }
        else
        {
            if (!PacketUtils::Write_MSG_REQUEST_ADDRESS(buffer, AddressType::REALM, client->GetEntity(), &realmIndex, 1))
                return false;

            auto& connection = registry->ctx<ConnectionSingleton>();
            connection.networkClient->Send(buffer);
        }

        return true;
    }
}