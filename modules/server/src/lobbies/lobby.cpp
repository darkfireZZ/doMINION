
#include <server/lobbies/lobby.h>
#include <shared/game/game_state/board_base.h>
#include <shared/utils/assert.h>
#include <shared/utils/logger.h>

namespace server
{
    Lobby::Lobby(const Player::id_t &game_master, const std::string &lobby_id) :
        game_interface(nullptr), game_master(game_master), lobby_id(lobby_id)
    {
        LOG(INFO) << "Lobby constructor called with lobby_id: " << lobby_id;
        players.push_back(game_master);
    };

    void Lobby::handleMessage(MessageInterface &message_interface,
                              std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        // NOLINTBEGIN(bugprone-macro-parentheses)
#define HANDLE(message_type, handler_func)                                                                             \
    if ( typeid(&(*message)) == typeid(shared::message_type) ) {                                                       \
        LOG(INFO) << "Trying to handle: " << #message_type;                                                            \
        std::unique_ptr<shared::message_type> casted_message(static_cast<shared::message_type *>(message.release()));  \
        handler_func(message_interface, std::move(casted_message));                                                    \
        return;                                                                                                        \
    }
        // NOLINTEND(bugprone-macro-parentheses)

        // handle messages the lobby is responsible for
        HANDLE(JoinLobbyRequestMessage, join);
        HANDLE(StartGameRequestMessage, startGame);
        HANDLE(GameStateRequestMessage, getGameState);

        const auto &requestor_id = message->player_id; // TODO: is message::player_id always same as action::player_id ?
        if ( !gameRunning() ) {
            LOG(ERROR) << "Tried to join a game that has already started!";
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, message->message_id,
                                                                  "Game has not started yet!");
            throw std::runtime_error("unreachable code");
        }

        // Check if player is in the lobby
        if ( !playerInLobby(requestor_id) ) {
            LOG(DEBUG) << "Received Action and Player is not in the requested lobby. Lobby ID: " << lobby_id
                       << " , Player ID: " << requestor_id << " , Message ID: " << message->message_id;
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, message->message_id,
                                                                  "Player is notin the lobby");
            return;
        }

        // TODO: SEND THIS, MAYBE CHANGE TO ARRAY?
        auto response = game_interface->handleMessage(message);
        auto order_msg = std::make_unique<shared::ActionOrderMessage>(lobby_id, std::move(response));
        message_interface.sendMessage(std::move(order_msg), "add player id here");
    }

    void Lobby::getGameState(MessageInterface &message_interface,
                             std::unique_ptr<shared::GameStateRequestMessage> request)
    {
        LOG(ERROR) << "Not implemented yet";
        throw std::runtime_error("not implemented yet");

        const auto &player_id = request->player_id;
        if ( !gameRunning() ) {
            LOG(WARN) << "Tried to join a game that has already started!";
            message_interface.send<shared::ResultResponseMessage>(player_id, lobby_id, false, request->message_id,
                                                                  "Game has already started");
            return; // we do nothing in this case
        }
    }

    void Lobby::join(MessageInterface &message_interface, std::unique_ptr<shared::JoinLobbyRequestMessage> request)
    {
        const auto &requestor_id = request->player_id;
        LOG(INFO) << "Lobby::join called with Lobby ID: " << lobby_id << " and Player ID: " << requestor_id;

        if ( gameRunning() ) {
            LOG(WARN) << "Tried to join a game that has already started!";
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Game has already started");
            return; // we do nothing in this case
        }

        // Check if player is already in the lobby
        if ( playerInLobby(requestor_id) ) {
            LOG(DEBUG) << "Player is already in the lobby. Lobby ID: " << lobby_id << " , Player ID: " << requestor_id;
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Player is already in the lobby");
            return;
        }

        if ( players.size() >= shared::board_config::MAX_PLAYER_COUNT ) {
            LOG(DEBUG) << "Lobby is full. Lobby ID: " << lobby_id << " , Player ID: " << requestor_id
                       << " , Max players: " << shared::board_config::MAX_PLAYER_COUNT;
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Lobby is full");
            return;
        }

        // Add player to the lobby
        players.push_back(requestor_id);

        // Send JoinLobbyBroadcast to all players
        message_interface.broadcast<shared::JoinLobbyBroadcastMessage>(players, lobby_id, players);
        message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, true, request->message_id);
    };

    // PRE: selected_cards are validated in message parsing
    void Lobby::startGame(MessageInterface &message_interface, std::unique_ptr<shared::StartGameRequestMessage> request)
    {
        const auto &requestor_id = request->player_id;
        LOG(INFO) << "Lobby::start_game called with Lobby ID: " << lobby_id << " and Player ID: " << requestor_id;

        if ( gameRunning() ) {
            LOG(WARN) << "Tried to join a game that has already started!";
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Game has already started");
            return; // we do nothing in this case
        }

        // Check if gamemaster is starting the game
        if ( requestor_id != game_master ) {
            LOG(DEBUG) << "Lobby::start_game is called by someone differnt than game master. Lobby ID: " << lobby_id
                       << " , Player ID: " << requestor_id << " , Game Master ID: " << game_master;
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Only the game master can start the game");
            return;
        }

        if ( players.size() < shared::board_config::MIN_PLAYER_COUNT ||
             players.size() > shared::board_config::MAX_PLAYER_COUNT ) {
            LOG(DEBUG) << "Lobby::start_game is called with not enough or to many players. Lobby ID: " << lobby_id
                       << " , Player ID: " << requestor_id << " , Number of players: " << players.size();
            message_interface.send<shared::ResultResponseMessage>(requestor_id, lobby_id, false, request->message_id,
                                                                  "Invalid player count to start game");
            return;
        }

        // Create new game interface
        game_interface = GameInterface::make(lobby_id, request->selected_cards, players);

        // send messages
        LOG(INFO) << "Sending StartGameBroadcastMessage in Lobby ID: " << lobby_id;
        message_interface.broadcast<shared::StartGameBroadcastMessage>(players, lobby_id);

        std::for_each(players.begin(), players.end(),
                      [&](const auto &player_id)
                      {
                          LOG(INFO) << "Sending GameStateMessage in Lobby ID: " << lobby_id
                                    << " to Player ID: " << player_id;
                          message_interface.send<shared::GameStateMessage>(player_id, lobby_id,
                                                                           game_interface->getGameState(player_id));
                      });
    }

    void Lobby::receiveAction(MessageInterface &message_interface,
                              std::unique_ptr<shared::ActionDecisionMessage> action)
    {
        LOG(INFO) << "Lobby::receive_action called with Lobby ID: " << lobby_id
                  << " and Player ID: " << action->player_id;
        // Check if game has started
        if ( !game_interface ) {
            LOG(ERROR) << "Game interface in Lobby::receive_action is nullptr. The game hasn't started yet.";
            throw std::runtime_error("Game interface is nullptr");
        }

        // Check if player is in the lobby
        Player::id_t player_id = action->player_id;
        if ( !playerInLobby(player_id) ) {
            LOG(DEBUG) << "Received Action and Player is not in the requested lobby. Lobby ID: " << lobby_id
                       << " , Player ID: " << player_id << " , Message ID: " << action->message_id;
            message_interface.send<shared::ResultResponseMessage>(player_id, lobby_id, false, action->message_id,
                                                                  "Player is notin the lobby");
            return;
        }

        // GameInterface::response_t response_order = game_interface->handleMessage(std::move(action->decision),
        // action->message_id, player_id);
        // std::unique_ptr<shared::ActionOrderMessage> response_msg =
        // std::make_unique<shared::ActionOrderMessage>(lobby_id, std::move(response_order));
        // message_interface.sendMessage(std::move(response_msg), player_id);
    }
} // namespace server
