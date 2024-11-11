#pragma once

#include <string>
#include <vector>

namespace shared
{

    // Representing card types like this allows us to combine them using bitwise operations.
    // Example: `ACTION | ATTACK` will create a card that is both an action and an attack.
    enum CardType
    {
        ACTION = 0b1,
        ATTACK = 0b10,
        REACTION = 0b100,
        TREASURE = 0b1000,
        VICTORY = 0b10000,
        CURSE = 0b100000
    };

    class CardBase
    {
    public:
        using id_t = std::string;

        CardBase(id_t id, CardType type, unsigned int cost) : id(id), type(type), cost(cost) {}
        bool isAction() const;
        bool isAttack() const;
        bool isTreasure() const;
        bool isReaction() const;
        bool isVictory() const;
        bool isCurse() const;
        unsigned int getCost();
        CardType getType();
        id_t getId();
        virtual std::string toString() = 0;

    protected:
        id_t id;
        const CardType type;
        const unsigned int cost;
    };

    class PlayerBase
    {
    public:
        using id_t = std::string;
        id_t getId() const;
        // TODO: initialize victory_points, available_actions, available_buys, available_treasure, current_card,
        // discard_pile, draw_pile_size
        PlayerBase(id_t player_id) : player_id(player_id) {}

    protected:
        bool operator==(const PlayerBase &other) const;

        id_t player_id;
        unsigned int victory_points;
        std::vector<CardBase::id_t> played_cards;
        std::vector<CardBase::id_t> gained_cards;
        unsigned int available_actions;
        unsigned int available_buys;
        unsigned int available_treasure;
        CardBase::id_t current_card;
        std::pair<CardBase::id_t, unsigned int> discard_pile;
        unsigned int draw_pile_size;
    };

    class ReducedEnemy : public PlayerBase
    {
    public:
        ReducedEnemy(id_t player_id, unsigned int hand_size) : PlayerBase(player_id), hand_size(hand_size) {}
        bool operator==(const ReducedEnemy &other) const;

    protected:
        unsigned int hand_size;
    };

    class ReducedPlayer : public PlayerBase
    {
    public:
        ReducedPlayer(id_t player_id, std::vector<CardBase::id_t> hand_cards) :
            PlayerBase(player_id), hand_cards(hand_cards)
        {}
        bool operator==(const ReducedPlayer &other) const;

    protected:
        std::vector<CardBase::id_t> hand_cards;
    };

    struct Pile
    {
    public:
        Pile(CardBase::id_t card, unsigned int count) : card(card), count(count) {}
        bool operator==(const Pile &other) const;

        // TODO: Test these functions
        std::string to_json();
        static Pile *from_json(const std::string &json);

        CardBase::id_t card;
        unsigned int count;
    };

    class Board
    {
    public:
        /**
         * @brief Construct a new Board object
         *
         * @param kingdom_cards The kingdom cards that are available in this game (chosen by the game master).
         * They must be exactly 10 cards.
         */
        Board(const std::vector<CardBase::id_t> &kingdom_cards, unsigned int num_players);
        bool operator==(const Board &other) const;

        std::vector<Pile> kingdom_cards;
        std::vector<Pile> victory_cards;
        std::vector<Pile> treasure_cards;
        std::vector<CardBase::id_t> trash;
    };

    class ReducedGameState
    {
    public:
        ReducedGameState(Board board, ReducedPlayer player, std::vector<ReducedEnemy> enemies,
                         PlayerBase::id_t current_player) :
            board(board), player(player), enemies(enemies), current_player(current_player)
        {}
        bool operator==(const ReducedGameState &other) const;

        Board board;
        ReducedPlayer player;
        std::vector<ReducedEnemy> enemies;
        PlayerBase::id_t current_player;
    };
} // namespace shared
