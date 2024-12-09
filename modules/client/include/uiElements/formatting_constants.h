#pragma once
#include <wx/wx.h>

namespace client
{
    namespace formatting_constants
    {
        // Text formatting constants
        const wxFont DEFAULT_FONT = wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        // Panel background colours
        const wxColour ENEMY_BACKGROUND = wxColour(255, 200, 200);
        const wxColour ACTIVE_ENEMY_BACKGROUND = wxColour(255, 200, 150);
        const wxColour ACTIVE_PLAYER_INFO_BACKGROUND = wxColour(200, 240, 200);
        const wxColour PASSIVE_PLAYER_INFO_BACKGROUND = ACTIVE_ENEMY_BACKGROUND; // wxColour(200, 220, 240);

        // Panel formatting constants
        #ifdef __APPLE__
        const wxSize DEFAULT_BOARD_PILE_SIZE = wxSize(100, 150);
        #else
        const wxSize DEFAULT_BOARD_PILE_SIZE = wxSize(80, 120);
        #endif
        const wxSize DEFAULT_PLAYED_CARD_SIZE = wxSize(60, 90);
        const wxSize DEFAULT_CARD_SELECTION_SIZE = wxSize(160, 240);

        // Border colours
        const wxColour PLAYABLE_HAND_CARD_BORDER = wxColour(0, 130, 0);
    } // namespace formatting_constants
} // namespace client
