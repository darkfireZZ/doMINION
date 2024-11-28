#include <shared/utils/logger.h>
#include <uiElements/enemy_panel.h>
#include <uiElements/formatting_constants.h>
#include <uiElements/pile_panel.h>
#include <uiElements/single_card_panel.h>

#include <wx/dcbuffer.h>
#include <wx/gbsizer.h>

namespace client
{
    EnemyPanel::EnemyPanel(wxWindow *parent, reduced::Enemy &enemy) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 80))
    {
        LOG(INFO) << "Ctor EnemyPanel";
        // Set a light red background color
        this->SetBackgroundStyle(wxBG_STYLE_PAINT);
        this->Bind(wxEVT_PAINT, &EnemyPanel::onPaint, this);

        // Outer sizer to add horizontal padding
        wxBoxSizer *outerSizer = new wxBoxSizer(wxHORIZONTAL);

        // Inner sizer for content
        wxBoxSizer *innerSizer = new wxBoxSizer(wxHORIZONTAL);


        /* ===========display draw pile=========== */
        wxBoxSizer *drawPileSizer = new wxBoxSizer(wxVERTICAL);

        // Text for the title
        wxStaticText *DrawPileText =
                new wxStaticText(this, wxID_ANY, "Draw Pile", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);

        // The pile itself
        shared::Pile Draw_Pile("Card_back", enemy.getDrawPileSize());
        auto height = 30;
        PilePanel *Draw_Pile_panel = new PilePanel(this, Draw_Pile, wxSize(height / 3 * 2, height));

        // add new items to the sizer
        drawPileSizer->Add(DrawPileText, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        drawPileSizer->Add(Draw_Pile_panel, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        innerSizer->Add(drawPileSizer, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));


        /* ===========display player id=========== */
        // new sizer for displaying the name and the hand cards vertically
        auto *centerSizer = new wxGridSizer(2, 1, 1, 2);

        // TODO: display name in a bigger and bold font
        wxStaticText *PlayerId =
                new wxStaticText(this, wxID_ANY, enemy.getId(), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);

        // new sizer for the hand cards
        auto *handCardSizer = new wxGridSizer(1, enemy.getHandSize(), 1, 1);
        auto hand_card_heigth = 30;
        auto hand_card_width = hand_card_heigth / 3 * 2;
        for ( unsigned int i = 0; i < enemy.getHandSize(); i++ ) {
            SingleCardPanel *Card = new SingleCardPanel(this, "Card_back", wxSize(hand_card_width, hand_card_heigth));
            handCardSizer->Add(Card, 0, wxALL, 2);
        }

        // add new items to the sizer
        centerSizer->Add(PlayerId, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        centerSizer->Add(handCardSizer, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        innerSizer->Add(centerSizer, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));

        /* ===========display discard pile=========== */
        wxBoxSizer *discardPileSizer = new wxBoxSizer(wxVERTICAL);

        // Text for the title
        wxStaticText *discardPileText =
                new wxStaticText(this, wxID_ANY, "Discard Pile", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);

        // The pile itself
        shared::Pile Discard_Pile("Card_back", enemy.getDiscardPileSize());
        PilePanel *Discard_Pile_panel = new PilePanel(this, Discard_Pile, wxSize(height / 3 * 2, height));

        // add new items to the sizer
        discardPileSizer->Add(discardPileText, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        discardPileSizer->Add(Discard_Pile_panel, wxSizerFlags().Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL));
        innerSizer->Add(discardPileSizer, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL));


        // finally add the inner sizer to the outer sizer
        outerSizer->Add(innerSizer, wxSizerFlags(1).Expand().Border(wxALL, 1));

        this->SetSizer(outerSizer);
    }

    // generated by AI
    void EnemyPanel::onPaint(wxPaintEvent &event)
    {
        wxAutoBufferedPaintDC dc(this); // Use double-buffered drawing to prevent flickering
        dc.Clear(); // Clear the panel

        // Get the panel dimensions
        wxSize size = this->GetSize();

        // Create a rounded rectangle
        wxBrush brush(formatting_constants::ENEMY_BACKGROUND);
        dc.SetBrush(brush);
        dc.SetPen(*wxTRANSPARENT_PEN); // No border
        dc.DrawRoundedRectangle(0, 0, size.GetWidth(), size.GetHeight(), 10); // Radius of 10 for rounded corners

        // Continue normal event processing
        event.Skip();
    }
} // namespace client
