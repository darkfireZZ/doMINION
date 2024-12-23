#pragma once

#include "image_panel.h"

#include <shared/game/game_state/reduced_game_state.h>

#include <wx/wx.h>


namespace client
{
    /**
     * @brief A utility panel that displays a pile of cards
     */
    class SingleCardPanel : public wxPanel
    {
    public:
        SingleCardPanel(wxWindow *parent, const std::string &card_name, wxSize size = wxDefaultSize, int padding = 0);

        template <typename Functor>
        void makeClickable(const wxEventTypeTag<wxMouseEvent> click_type, Functor f);

        const std::string getCardName() const { return CardName; }

        void setBorderColor(const wxColour &color);

    private:
        shared::CardBase::id_t CardName;
        ImagePanel *Image;
    };

    template <typename Functor>
    void SingleCardPanel::makeClickable(const wxEventTypeTag<wxMouseEvent> click_type, Functor f)
    {
        Image->Bind(click_type, f);
    }
} // namespace client
