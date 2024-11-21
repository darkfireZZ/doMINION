#include <server/lobbies/lobby.h>
#include <server/lobbies/lobby_manager.h>
#include <server/network/message_interface.h>
#include <shared/message_types.h>
#include <shared/utils/uuid_generator.h>
#include <typeinfo>

#include <gmock/gmock.h>
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Truly;

/**
 * @brief Mock implementation of the MessageInterface
 *
 * @details Used for testing the game logic without actually sending messages to the client
 */
class MockMessageInterface : public server::MessageInterface
{
public:
    // Mock the send_message method, assuming it takes these parameters
    MOCK_METHOD(void, send_message,
                (std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t &player_id),
                (override));
};

MATCHER(IsCreateLobbyResponseMessage, "Checks if the message is CreateLobbyResponseMessage")
{
    auto create_lobby_response_msg = dynamic_cast<shared::CreateLobbyResponseMessage *>(arg.get());
    return create_lobby_response_msg != nullptr;
}

MATCHER(IsSuccessMessage, "Checks if the message is a success message")
{
    auto result_msg = dynamic_cast<shared::ResultResponseMessage *>(arg.get());
    return (result_msg != nullptr) && result_msg->success;
}

MATCHER(IsFailureMessage, "Checks if the message is a failure message")
{
    auto result_msg = dynamic_cast<shared::ResultResponseMessage *>(arg.get());
    return (result_msg != nullptr) && !result_msg->success;
}

MATCHER(IsJoinLobbyBroadcastMessage, "Checks if the message is JoinLobbyBroadcastMessage")
{
    auto join_lobby_broadcast_msg = dynamic_cast<shared::JoinLobbyBroadcastMessage *>(arg.get());
    return join_lobby_broadcast_msg != nullptr;
}

TEST(ServerLibraryTest, CreateLobby)
{
    std::shared_ptr<MockMessageInterface> message_interface = std::make_shared<MockMessageInterface>();
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";

    auto request1 =
            std::make_unique<shared::CreateLobbyRequestMessage>("123", uuid_generator::generate_uuid_v4(), player_1);
    auto request2 =
            std::make_unique<shared::CreateLobbyRequestMessage>("123", uuid_generator::generate_uuid_v4(), player_2);

    // All expected function calls of send_message
    {
        InSequence s;
        EXPECT_CALL(*message_interface, send_message(IsCreateLobbyResponseMessage(), _))
                .Times(1); // Success for first time creating lobby
        EXPECT_CALL(*message_interface, send_message(IsFailureMessage(), _))
                .Times(1); // Error for second time creating lobby
    }

    const auto *games = lobby_manager.get_games();
    ASSERT_EQ(games->empty(), true) << "LobbyManager should be empty at the beginning";

    lobby_manager.create_lobby(std::move(request1));

    ASSERT_EQ(games->size(), 1) << "LobbyManager should contain one lobby after creating one";
    ASSERT_EQ(games->find("123") != games->end(), true) << "Lobby with id 123 should exist";
    ASSERT_EQ(games->at("123")->get_game_master(), player_1) << "Game master should be player_1";
    ASSERT_EQ(games->at("123")->get_players().size(), 1) << "There should be one player in the lobby";

    // No new lobby should be created, because game with id 123 already exists
    lobby_manager.create_lobby(std::move(request2));
    ASSERT_EQ(games->size(), 1);
    // Check if the lobby with id really 123 exists
    ASSERT_EQ(games->find("123") != games->end(), true);
    // Check if the game_master didn't change
    ASSERT_EQ(games->at("123")->get_game_master(), player_1);
    ASSERT_EQ(games->at("123")->get_players().size(), 1);
}

// TODO: Implement tests for the following methods
TEST(ServerLibraryTest, JoinLobby)
{
    std::shared_ptr<MockMessageInterface> message_interface = std::make_shared<MockMessageInterface>();
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";
    shared::PlayerBase::id_t player_3 = "Paul";
    shared::PlayerBase::id_t player_4 = "John";
    shared::PlayerBase::id_t player_5 = "George";

    auto request1 =
            std::make_unique<shared::CreateLobbyRequestMessage>("123", uuid_generator::generate_uuid_v4(), player_1);
    auto request2 =
            std::make_unique<shared::JoinLobbyRequestMessage>("123", uuid_generator::generate_uuid_v4(), player_2);
    auto request2_again =
            std::make_unique<shared::JoinLobbyRequestMessage>("123", uuid_generator::generate_uuid_v4(), player_2);
    auto false_request3 =
            std::make_unique<shared::JoinLobbyRequestMessage>("222", uuid_generator::generate_uuid_v4(), player_3);
    auto corrected_request3 =
            std::make_unique<shared::JoinLobbyRequestMessage>("123", uuid_generator::generate_uuid_v4(), player_3);
    auto request4 =
            std::make_unique<shared::JoinLobbyRequestMessage>("123", uuid_generator::generate_uuid_v4(), player_4);
    auto request5 =
            std::make_unique<shared::JoinLobbyRequestMessage>("123", uuid_generator::generate_uuid_v4(), player_5);

    const auto *games = lobby_manager.get_games();

    lobby_manager.create_lobby(std::move(request1));

    // All expected function calls of send_message
    {
        InSequence s;
        // request 2
        EXPECT_CALL(*message_interface, send_message(IsJoinLobbyBroadcastMessage(), player_1)).Times(1);
        EXPECT_CALL(*message_interface, send_message(IsSuccessMessage(), player_2)).Times(1);
        // request 2 again
        EXPECT_CALL(*message_interface, send_message(IsFailureMessage(), player_2)).Times(1);
        // false_request 3
        EXPECT_CALL(*message_interface, send_message(IsFailureMessage(), player_3)).Times(1);
        // corrected_request 3 (1x ResultResponseMessage + 2x JoinLobbyBroadcastMessage)
        // + request 4 (1x ResultResponseMessage + 3x JoinLobbyBroadcastMessage)
        EXPECT_CALL(*message_interface, send_message(_, _)).Times(7);
        // request 5
        EXPECT_CALL(*message_interface, send_message(IsFailureMessage(), player_5)).Times(1);
    }
    lobby_manager.join_lobby(std::move(request2));
    ASSERT_EQ(games->at("123")->get_players().size(), 2) << "There should be two players in the lobby";
    ASSERT_EQ(games->at("123")->get_players().at(1), player_2) << "Player 2 should be in the lobby";

    // Player 2 should not be added again
    lobby_manager.join_lobby(std::move(request2_again));
    ASSERT_EQ(games->at("123")->get_players().size(), 2) << "There should still be two players in the lobby";

    // Player 3 should not be able to join the lobby with id 123
    lobby_manager.join_lobby(std::move(false_request3));
    ASSERT_EQ(games->at("123")->get_players().size(), 2) << "There should still be two players in the lobby";

    // Player 3 should be able to join the lobby with id 123
    lobby_manager.join_lobby(std::move(corrected_request3));

    // Player 4 should be able to join the lobby
    lobby_manager.join_lobby(std::move(request4));

    // Player 5 should not be able to join because the lobby is full
    lobby_manager.join_lobby(std::move(request5));
    ASSERT_EQ(games->at("123")->get_players().size(), 4) << "There should still be four players in the lobby";
}

TEST(ServerLibraryTest, StartGame)
{
    std::shared_ptr<MockMessageInterface> message_interface = std::make_shared<MockMessageInterface>();
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";

    auto request1 = std::make_unique<shared::CreateLobbyRequestMessage>("123", "101", player_1);
    auto request2 = std::make_unique<shared::JoinLobbyRequestMessage>("123", "102", player_2);

    std::vector<shared::CardBase::id_t> selected_cards = {"Moat",   "Village", "Woodcutter", "Workshop", "Militia",
                                                          "Cellar", "Market",  "Mine",       "Smithy",   "Remodel"};

    // Start game request with wrong game_id
    auto request3 = std::make_unique<shared::StartGameRequestMessage>("abc", "103", player_1, selected_cards);

    // Start game request not as game_master
    auto request4 = std::make_unique<shared::StartGameRequestMessage>("123", "104", player_2, selected_cards);

    // Start game request as game_master
    auto request5 = std::make_unique<shared::StartGameRequestMessage>("123", "105", player_1, selected_cards);

    lobby_manager.create_lobby(std::move(request1));
    lobby_manager.join_lobby(std::move(request2));

    // All expected function calls of send_message
    {
        InSequence s;
        // request3
        EXPECT_CALL(*message_interface, send_message(IsFailureMessage(), player_1)).Times(1);
        // request4
        EXPECT_CALL(*message_interface, send_message(IsFailureMessage(), player_2)).Times(1);
        // request5
        EXPECT_CALL(*message_interface, send_message(_, _)).Times(4);
    }

    lobby_manager.start_game(std::move(request3));
    lobby_manager.start_game(std::move(request4));
    lobby_manager.start_game(std::move(request5));
    // TODO: Check if the game started correctly
}
