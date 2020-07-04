#pragma once
#include <memory>

class MessageHandler;
class NetworkClient;
struct NetworkPacket;
namespace Client
{
    class GeneralHandlers
    {
    public:
        static void Setup(MessageHandler*);
        static bool HandleConnected(std::shared_ptr<NetworkClient>, NetworkPacket*);
        static bool HandleRequestRealmlist(std::shared_ptr<NetworkClient>, NetworkPacket*);
        static bool HandleSelectRealm(std::shared_ptr<NetworkClient>, NetworkPacket*);
    };
}