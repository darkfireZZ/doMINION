// reduced_game_state_test.cpp

#include <gtest/gtest.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <shared/utils/test_helpers.h>

TEST(ReducedGameStateTest, ParameterizedConstructor)
{
    // Create a list of ReducedEnemies
    shared::PlayerBase enemy1("enemy1");
    shared::PlayerBase enemy2("enemy2");
    std::vector<shared::ReducedEnemy::ptr_t> enemies;
    enemies.emplace_back(shared::ReducedEnemy::make(enemy1, 0));
    enemies.emplace_back(shared::ReducedEnemy::make(enemy2, 0));

    // Set the active player ID
    shared::PlayerBase::id_t active_player = "player";
    shared::PlayerBase player(active_player);
    shared::ReducedPlayer::ptr_t reduced_player = shared::ReducedPlayer::make(player, {});

    // Create a Board object
    auto kingdom_cards = get_valid_kingdom_cards();
    shared::Board::ptr_t board = shared::Board::make(kingdom_cards, 2);

    // Create the ReducedGameState
    shared::ReducedGameState game_state(board, std::move(reduced_player), std::move(enemies), active_player);

    // Check that the members are initialized correctly
    EXPECT_EQ(game_state.active_player, active_player);

    // Since Board and ReducedPlayer are complex objects, we would need to have appropriate comparison operators
    // For now, we can check IDs or sizes where applicable

    // Check player ID
    EXPECT_EQ(game_state.reduced_player->getId(), "player");

    // Check enemies
    ASSERT_EQ(game_state.reduced_enemies.size(), 2);
    EXPECT_EQ(game_state.reduced_enemies[0]->getId(), "enemy1");
    EXPECT_EQ(game_state.reduced_enemies[1]->getId(), "enemy2");
}
