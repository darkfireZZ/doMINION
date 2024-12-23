#include <gtest/gtest.h>
#include <shared/utils/logger.h>

#include <shared/message_types.h>
#include <shared/player_result.h>
#include <shared/utils/test_helpers.h>

using namespace shared;

// ======= SERVER TO CLIENT MESSAGES ======= //

TEST(SharedLibraryTest, GameStateMessageTwoWayConversion)
{

    const std::vector<CardBase::id_t> kingdom_cards = getValidKingdomCards();
    const unsigned int num_players = 2;
    Board::ptr_t board = Board::make(kingdom_cards, num_players);

    reduced::Player::ptr_t player =
            reduced::Player::make(PlayerBase("Felix"), {"Copper", "Copper", "Estate", "Chapel", "Silver"});
    std::vector<reduced::Enemy::ptr_t> enemies;
    enemies.push_back(reduced::Enemy::make(PlayerBase("Marius"), 5));

    const std::string active_player = "Felix";

    const GamePhase game_phase = GamePhase::ACTION_PHASE;

    std::unique_ptr<reduced::GameState> game_state = std::make_unique<reduced::GameState>(
            std::move(board), std::move(player), std::move(enemies), active_player, game_phase);
    GameStateMessage original_message("123", std::move(game_state), "789", "456");

    std::string json = original_message.toJson();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::fromJson(json);

    std::unique_ptr<GameStateMessage> parsed_message(dynamic_cast<GameStateMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, CreateLobbyResponseMessageTwoWayConversion)
{
    CreateLobbyResponseMessage original_message("123", std::nullopt);

    std::string json = original_message.toJson();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::fromJson(json);

    std::unique_ptr<CreateLobbyResponseMessage> parsed_message(
            dynamic_cast<CreateLobbyResponseMessage *>(base_message.release()));
    LOG(DEBUG) << "original_message id: " << original_message.message_id;
    LOG(DEBUG) << "parsed_message id: " << parsed_message->message_id;
    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, JoinLobbyBroadcastMessageTwoWayConversion)
{
    JoinLobbyBroadcastMessage original_message("123", {"player_1", "player_2"}, "456");

    std::string json = original_message.toJson();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::fromJson(json);

    std::unique_ptr<JoinLobbyBroadcastMessage> parsed_message(
            dynamic_cast<JoinLobbyBroadcastMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, StartGameBroadcastMessageTwoWayConversion)
{
    StartGameBroadcastMessage original_message("123");

    std::string json = original_message.toJson();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::fromJson(json);

    std::unique_ptr<StartGameBroadcastMessage> parsed_message(
            dynamic_cast<StartGameBroadcastMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, EndGameBroadcastMessageTwoWayConversion)
{

    std::vector<PlayerResult> results = {{"player1", 10}, {"player2", 20}, {"player3", 30}};
    EndGameBroadcastMessage original_message("123", results);

    std::string json = original_message.toJson();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::fromJson(json);

    std::unique_ptr<EndGameBroadcastMessage> parsed_message(
            dynamic_cast<EndGameBroadcastMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, ResultResponseMessageTwoWayConversion)
{
    bool success = true;
    std::string in_response_to = "hui";
    std::string additional_information = "hey";
    ResultResponseMessage original_message("123", success, in_response_to, additional_information);

    std::string json = original_message.toJson();

    std::unique_ptr<ServerToClientMessage> base_message;
    base_message = ServerToClientMessage::fromJson(json);

    std::unique_ptr<ResultResponseMessage> parsed_message(
            dynamic_cast<ResultResponseMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}


TEST(SharedLibraryTest, ActionOrderMessageTwoWayConversion)
{
    std::unique_ptr<ActionOrder> order = std::make_unique<ChooseFromStagedOrder>(
            1, 1, shared::ChooseFromOrder::AllowedChoice::DISCARD, std::vector<shared::CardBase::id_t>(1, "a card"));
    ActionOrderMessage original_message("123", std::move(order), test_helper::getReducedGameStatePtr(4));

    std::string json = original_message.toJson();

    std::unique_ptr<ServerToClientMessage> base_message;

    base_message = ServerToClientMessage::fromJson(json);

    std::unique_ptr<ActionOrderMessage> parsed_message(dynamic_cast<ActionOrderMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

// ======= CLIENT TO SERVER MESSAGES ======= //

TEST(SharedLibraryTest, GameStateRequestMessageTwoWayConversion)
{
    GameStateRequestMessage original_message("123", "player1");

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<GameStateRequestMessage> parsed_message(
            dynamic_cast<GameStateRequestMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, CreateLobbyRequestMessageTwoWayConversion)
{
    CreateLobbyRequestMessage original_message("123", "player1");

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<CreateLobbyRequestMessage> parsed_message(
            dynamic_cast<CreateLobbyRequestMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, JoinLobbyRequestMessageTwoWayConversion)
{
    JoinLobbyRequestMessage original_message("123", "player1");

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<JoinLobbyRequestMessage> parsed_message(
            dynamic_cast<JoinLobbyRequestMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, StartGameRequestMessageTwoWayConversion)
{
    std::vector<std::string> cards = {"village",    "Smithy",  "Market", "Council_Room", "Festival",
                                      "Laboratory", "Library", "Mine",   "Witch",        "Artisan"};
    StartGameRequestMessage original_message("123", "player1", cards);

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<StartGameRequestMessage> parsed_message(
            dynamic_cast<StartGameRequestMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionPlayActionCard)
{
    std::unique_ptr<ActionDecision> decision = std::make_unique<PlayActionCardDecision>("Village");
    ActionDecisionMessage original_message("123", "player1", std::move(decision), "789");

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionBuyCard)
{
    ActionDecisionMessage original_message("123", "player1", std::make_unique<BuyCardDecision>("copper"));

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionEndActionPhase)
{
    ActionDecisionMessage original_message("123", "player1", std::make_unique<EndActionPhaseDecision>(), "789");

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionEndTurn)
{
    ActionDecisionMessage original_message("123", "player1", std::make_unique<EndTurnDecision>(), "789");

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}

TEST(SharedLibraryTest, ActionDecisionMessageTwoWayConversionChooseNCardsFromHand)
{
    std::unique_ptr<ActionDecision> decision = std::make_unique<GainFromBoardDecision>("a_card");
    ActionDecisionMessage original_message("123", "player1", std::move(decision));

    std::string json = original_message.toJson();

    std::unique_ptr<ClientToServerMessage> base_message;
    base_message = ClientToServerMessage::fromJson(json);

    std::unique_ptr<ActionDecisionMessage> parsed_message(
            dynamic_cast<ActionDecisionMessage *>(base_message.release()));

    ASSERT_NE(parsed_message, nullptr);
    ASSERT_EQ(*parsed_message, original_message);
}
