#pragma once

#include <map>
#include <memory>
#include <string>

#include <shared/message_types.h>

#include <server/game/game_state/game_state.h>
#include <server/message_interface.h>

namespace server
{

    /**
     * @brief A lobby is a container for a game that is being played.
     *
     * A lobby is created by a game master, who is the first player to join the lobby.
     * The game master can start the game when they want to.
     */
    class Lobby
    {
    public:
        /**
         * @brief Create a new game lobby.
         *
         * @param game_master The player who created the lobby.
         */
        Lobby(shared::PlayerBase::id_t game_master, std::string lobby_id);
        /**
         * @brief Add a player to the lobby.
         */
        void join(MessageInterface &message_interface, shared::JoinLobbyRequestMessage request);
        void start_game(MessageInterface &message_interface, shared::StartGameRequestMessage request);
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);

        shared::ReducedGameState get_game_state(shared::PlayerBase::id_t player) const;
        std::vector<shared::PlayerBase::id_t> get_players() const { return players; }


        shared::PlayerBase::id_t get_game_master() const { return game_master; };

    private:
        std::unique_ptr<GameState> game_state;
        shared::PlayerBase::id_t game_master;

        std::vector<shared::PlayerBase::id_t> players;

        std::string lobby_id;
    };

    /**
     * @brief The lobby manager contains all the running games / lobbies.
     *
     * The lobby manager is responsible for creating, joining and starting games.
     * It also receives actions from players and passes them on to the correct game.
     */
    class LobbyManager
    {
    public:
        // TODO: The message interface should not be passed to the constructor, but to the methods that need it.
        // Also, the message interface should definitely not be passed as a raw pointer.
        LobbyManager(MessageInterface *message_interface) : message_interface(message_interface) {};
        /*message_interface(std::make_unique<MessageInterface>(message_interface))*/

        void create_lobby(shared::CreateLobbyRequestMessage request);
        void join_lobby(shared::JoinLobbyRequestMessage request);
        void start_game(shared::StartGameRequestMessage request);
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);
        shared::ReducedGameState get_game_state(std::string game_id, shared::PlayerBase::id_t player);
        const std::map<std::string, std::unique_ptr<Lobby>> *get_games() { return &games; };

    private:
        std::map<std::string, std::unique_ptr<Lobby>> games;
        MessageInterface *message_interface; // THIS MUST BE UNIQUE
    };
} // namespace server
