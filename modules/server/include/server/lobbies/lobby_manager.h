
#pragma once

#include <map>
#include <memory>
#include <string>

#include <server/lobbies/lobby.h>
#include <server/network/message_interface.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <shared/message_types.h>

namespace server
{
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

        /**
         * @brief Create a new lobby manager.
         *
         * @param message_interface The message interface to send messages to the players.
         */
        LobbyManager(std::shared_ptr<MessageInterface> message_interface) : message_interface(message_interface){};
        /*message_interface(std::make_unique<MessageInterface>(message_interface))*/

        /**
         * @brief Create a new lobby.
         * This will create a new lobby and add it to the list of games. The game master will be added to the lobby.
         *
         * @param request The CreateLobbyRequestMessage to create the lobby with.
         */
        void create_lobby(std::unique_ptr<shared::CreateLobbyRequestMessage> request);

        /**
         * @brief Join a lobby.
         * This will add a player to the lobby with the given lobby id.
         *
         * @param request The JoinLobbyRequestMessage to join the lobby with.
         */
        void join_lobby(std::unique_ptr<shared::JoinLobbyRequestMessage> request);

        /**
         * @brief Start a game.
         * This will start the game in the lobby with the given lobby id.
         *
         * @param request The StartGameRequestMessage to start the game with.
         *
         * @pre The request contains 10 cards.
         */
        void start_game(std::unique_ptr<shared::StartGameRequestMessage> request);

        /**
         * @brief Receive an action from a player and handle it correctly.
         * This will be passed on to the correct lobby.
         *
         * @param action The ActionDecisionMessage to handle.
         *
         * @pre Valid ActionDecisionMessage.
         */
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action);

        /**
         * @brief Get the games that are currently running.
         *
         * @return A const reference to the map of lobby ids.
         */
        const std::map<std::string, std::shared_ptr<Lobby>> *get_games() { return &games; };

    private:
        std::map<std::string, std::shared_ptr<Lobby>> games;
        std::shared_ptr<MessageInterface> message_interface;

        /**
         * @brief Check if a lobby exists.
         *
         * @param lobby_id The id of the lobby to check.
         *
         * @return True if the lobby exists, false otherwise.
         */
        bool lobby_exists(std::string lobby_id);
    };
} // namespace server