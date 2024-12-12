#include <panels/card_selection_panel.h>
#include <uiElements/popup.h>

#include <game_controller.h>
#include <shared/game/cards/card_base.h>
#include <shared/game/cards/card_factory.h>
#include <shared/utils/logger.h>
#include <uiElements/formatting_constants.h>
#include <uiElements/image_panel.h>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/wx.h>
#include "dominion.h"
#include "uiElements/text_panel.h"

namespace client
{
    // NOLINTBEGIN(bugprone-suspicious-enum-usage)
    CardSelectionPanel::CardSelectionPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 1024))
    {
        TextPanel *Title = new TextPanel(this, wxID_ANY, "Select 10 cards", TextFormat::TITLE);
        wxBoxSizer *VerticalSizer = new wxBoxSizer(wxVERTICAL);
        VerticalSizer->Add(Title, 0, wxALIGN_CENTER | wxTOP, 15);

        const shared::CardFactory::map_t &all_cards = shared::CardFactory::getAll();
        for ( const auto &card : all_cards ) {
            if ( card.second->isKingdom() ) {
                // This is a really hacky way of ignoring the God Mode card, but
                // it is good enough for now
                if ( !wxGetApp().isDebugMode() && card.first == "God_Mode" ) {
                    continue;
                }

                selectedCards[card.first] = false;
            }
        }

        VerticalSizer->Add(createCardSelection(), 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);

        wxGridSizer *GridSizer = new wxGridSizer(1, 4, 0, 100);
        // show number of selected cards
        SelectedCardCountPanel =
                new TextPanel(this, wxID_ANY, "Selected Cards: " + std::to_string(selectedCardCount), TextFormat::BOLD);

        GridSizer->Add(SelectedCardCountPanel, 0, wxALIGN_LEFT | wxALL, 5);

        // make the auto selction fill button
        wxButton *AutoSelectButton = new wxButton(this, wxID_ANY, "Select random");

        AutoSelectButton->Bind(wxEVT_BUTTON, [this](const wxCommandEvent &) { makeAutoSelection(); });

        GridSizer->Add(AutoSelectButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        // make the clear selection button
        wxButton *ClearSelectionButton = new wxButton(this, wxID_ANY, "Clear selection");

        ClearSelectionButton->Bind(wxEVT_BUTTON, [this](const wxCommandEvent &) { clearSelection(); });

        GridSizer->Add(ClearSelectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        // make the start game button
        StartButton = new wxButton(this, wxID_ANY, "Start Game");

        StartButton->Bind(wxEVT_BUTTON,
                          [this](const wxCommandEvent &) { wxGetApp().getController().startGame(selectedCards); });

        // Disable the start button by default
        StartButton->Enable(false);
        StartButton->SetToolTip("Select 10 kingdom cards to start the game");

        GridSizer->Add(StartButton, 0, wxALIGN_RIGHT | wxALL, 5);

        VerticalSizer->Add(GridSizer, 0, wxEXPAND | wxALL, 5);
        SetBackgroundColour(formatting_constants::DEFAULT_PANEL_BACKGROUND);
        this->SetSizerAndFit(VerticalSizer);
    }

    wxScrolledWindow *CardSelectionPanel::createCardSelection()
    {
        // Create the scrolled window
        wxScrolledWindow *scrolledWindow =
                new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(800, 600), wxVSCROLL | wxHSCROLL);
        scrolledWindow->SetScrollRate(10, 15); // Set scroll rate in pixels

        // generated by AI
        // Load the background image
        wxBitmap backgroundBitmap;
        wxString background_file = wxString("assets") + wxFileName::GetPathSeparator() + "wooden_background.jpg";
        if ( wxFileExists(background_file) ) {
            backgroundBitmap = wxBitmap(background_file, wxBITMAP_TYPE_ANY);
        } else {
            LOG(WARN) << "Could not load background image: " << background_file;

            wxImage brownImage(800, 600); // Size of the fallback image
            brownImage.SetRGB(wxRect(0, 0, 800, 600), 230, 155, 0); // RGB for brown (139, 69, 19)
            backgroundBitmap = wxBitmap(brownImage);
        }

        // Bind the OnPaint event to draw the background
        scrolledWindow->Bind(wxEVT_PAINT,
                             [scrolledWindow, backgroundBitmap](wxPaintEvent &event)
                             {
                                 wxPaintDC dc(scrolledWindow);

                                 // Draw the background image, scaled to the window size
                                 if ( backgroundBitmap.IsOk() ) {
                                     wxSize size = scrolledWindow->GetSize();
                                     wxImage scaledImage =
                                             backgroundBitmap.ConvertToImage().Scale(size.GetWidth(), size.GetHeight());
                                     dc.DrawBitmap(wxBitmap(scaledImage), 0, 0, false);
                                 }

                                 // Proceed with default paint behavior
                                 event.Skip();
                             });
        // end generated by AI


        wxBoxSizer *ParentSizer = new wxBoxSizer(wxVERTICAL);

        wxGridSizer *CardSelection = new wxGridSizer(0, 4, 15, 15);

        // Add the cards to the sizer
        for ( auto card : selectedCards ) {
            SingleCardPanel *CardPanel = new SingleCardPanel(scrolledWindow, card.first,
                                                             formatting_constants::DEFAULT_CARD_SELECTION_SIZE, 5);
            CardPanel->setBorderColor(wxNullColour);
            makeSelectable(CardPanel);
            cardPanels.push_back(CardPanel);
            CardSelection->Add(CardPanel, 0, wxALIGN_CENTER);
        }

        ParentSizer->Add(CardSelection, 1, wxEXPAND | wxALL, 20);
        scrolledWindow->SetSizer(ParentSizer);
        scrolledWindow->FitInside(); // Ensure virtual size fits the content

        return scrolledWindow;
    }

    void CardSelectionPanel::makeAutoSelection()
    {
        // Select random action cards until 10 are selected
        if ( selectedCardCount >= 10 ) {
            return;
        }
        while ( selectedCardCount < 10 ) {
            while ( true ) {
                // Generate random index between 0 and the number of cards
                int random_index = rand() % cardPanels.size();
                if ( !selectedCards[cardPanels.at(random_index)->getCardName()] ) {
                    clickOnSelectableCard(cardPanels.at(random_index));
                    break;
                }
            }
        }
    }

    void CardSelectionPanel::clearSelection()
    {
        for ( auto *card : cardPanels ) {
            if ( selectedCards[card->getCardName()] ) {
                clickOnSelectableCard(card);
            }
        }
    }

    void CardSelectionPanel::makeSelectable(SingleCardPanel *card_panel)
    {
        // This display a little text box when hovering over the card
        card_panel->SetToolTip("Select card");
        card_panel->SetCursor(wxCursor(wxCURSOR_HAND));
        card_panel->makeClickable(wxEVT_LEFT_UP,
                                  [this, card_panel](wxMouseEvent & /*event*/) { clickOnSelectableCard(card_panel); });
        card_panel->makeClickable(wxEVT_RIGHT_UP,
                                  [card_panel](wxMouseEvent & /*event*/)
                                  { showCardPopup(card_panel, card_panel->getCardName()); });
    }

    void CardSelectionPanel::clickOnSelectableCard(SingleCardPanel *card_panel)
    {
        this->switchCardSelectionState(card_panel->getCardName());
        LOG(INFO) << "Card " << card_panel->getCardName()
                  << " clicked, new selection state: " << selectedCards[card_panel->getCardName()];

        // Change the border color of the card depending of the selection state
        wxColour new_border_colour = selectedCards[card_panel->getCardName()]
                ? formatting_constants::SELECTED_CARD_BACKGROUND
                : wxNullColour;
        card_panel->setBorderColor(new_border_colour);

        // Update the selected card count
        SelectedCardCountPanel->SetLabel("Selected Cards: " + std::to_string(selectedCardCount));
        SelectedCardCountPanel->GetParent()->Layout(); // Ensure the layout is updated

        // Update the start game button state
        StartButton->Enable(selectedCardCount == 10);
        (selectedCardCount == 10) ? StartButton->SetToolTip("Start the game")
                                  : StartButton->SetToolTip("Select 10 kingdom cards to start the game");
    }
    // NOLINTEND(bugprone-suspicious-enum-usage)
} // namespace client
