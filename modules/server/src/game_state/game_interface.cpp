#include <server/game/game_state/game_interface.h>

namespace server
{
    GameInterface::ptr_t GameInterface::make(const std::string &game_id,
                                             const std::vector<shared::CardBase::id_t> &play_cards,
                                             const std::vector<Player::id_t> &player_ids)
    {
        return ptr_t(new GameInterface(game_id, play_cards, player_ids));
    }

    GameInterface::response_t GameInterface::receive_action(std::unique_ptr<shared::ActionDecision> action_decision,
                                                            const std::optional<std::string> &in_response_to,
                                                            const Player::id_t &affected_player_id)
    {
        return in_response_to.has_value()
                ? handle_action(std::move(action_decision), affected_player_id)
                : handle_response(std::move(action_decision), affected_player_id, in_response_to.value());
    }

    GameInterface::response_t GameInterface::handle_action(std::unique_ptr<shared::ActionDecision> action_decision,
                                                           const Player::id_t &affected_player_id)
    {
#define HANDLE_ACTION(type)                                                                                            \
    if ( dynamic_cast<shared::type *>(action_decision.get()) ) {                                                       \
        return type##_handler(std::unique_ptr<shared::type>(static_cast<shared::type *>(action_decision.release())),   \
                              affected_player_id);                                                                     \
    }

        HANDLE_ACTION(PlayActionCardDecision);
        HANDLE_ACTION(BuyCardDecision);
        HANDLE_ACTION(EndTurnDecision);
        HANDLE_ACTION(ChooseNCardsFromHandDecision);

        // return failure
        return response_t(new shared::ResultResponseMessage("game_id", "msg_id", false));
    }

    GameInterface::response_t GameInterface::handle_response(std::unique_ptr<shared::ActionDecision> action_decision,
                                                             const std::string &in_response_to,
                                                             const Player::id_t &affected_player_id)
    {
#define HANDLE_RESPONSE(type)                                                                                          \
    if ( dynamic_cast<shared::type *>(action_decision.get()) ) {                                                       \
        return type##_response_handler(                                                                                \
                std::unique_ptr<shared::type>(static_cast<shared::type *>(action_decision.release())),                 \
                affected_player_id, in_response_to);                                                                   \
    }

        HANDLE_RESPONSE(PlayActionCardDecision);
        HANDLE_RESPONSE(BuyCardDecision);
        HANDLE_RESPONSE(EndTurnDecision);
        HANDLE_RESPONSE(ChooseNCardsFromHandDecision);

        // return failure
        return response_t(new shared::ResultResponseMessage("game_id", "msg_id", false));
    }

} // namespace server