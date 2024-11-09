
#include <rapidjson/document.h>
#include <shared/game_state.h>
#include <shared/utils/json.h>

using namespace rapidjson;
using namespace shared;

namespace shared
{
    bool PlayerBase::operator==(const PlayerBase &other) const
    {
        return this->id == other.id && this->victory_points == other.victory_points &&
                this->played_cards == other.played_cards && this->gained_cards == other.gained_cards &&
                this->available_actions == other.available_actions && this->available_buys == other.available_buys &&
                this->available_treasure == other.available_treasure && this->current_card == other.current_card &&
                this->discard_pile == other.discard_pile && this->draw_pile_size == other.draw_pile_size;
    }

    bool ReducedEnemy::operator==(const ReducedEnemy &other) const
    {
        return PlayerBase::operator==(other) && this->hand_size == other.hand_size;
    }

    bool ReducedPlayer::operator==(const ReducedPlayer &other) const
    {
        return PlayerBase::operator==(other) && this->hand_cards == other.hand_cards;
    }

    bool Pile::operator==(const Pile &other) const { return this->card == other.card && this->count == other.count; }

    std::string Pile::to_json()
    {
        Document doc;
        doc.SetObject();

        ADD_STRING_MEMBER(card.c_str(), card);
        ADD_UINT_MEMBER(count, count);

        return document_to_string(doc);
    }

    Pile *Pile::from_json(const std::string &json)
    {
        Document doc;
        doc.Parse(json.c_str());

        if ( doc.HasParseError() ) {
            return nullptr;
        }

        CardBase::id_t card;
        GET_STRING_MEMBER(card, doc, "card");
        unsigned int count;
        GET_UINT_MEMBER(count, doc, "count");

        return new Pile(card, count);
    }

    bool Board::operator==(const Board &other) const
    {
        return this->victory_cards == other.victory_cards && this->treasure_cards == other.treasure_cards &&
                this->kingdom_cards == other.kingdom_cards && this->trash == other.trash;
    }

    bool ReducedGameState::operator==(const ReducedGameState &other) const
    {
        return this->board == other.board && this->player == other.player && this->enemies == other.enemies &&
                this->active_player == other.active_player;
    }
} // namespace shared
