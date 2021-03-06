#ifndef RWE_UISTAGEDBUTTON_H
#define RWE_UISTAGEDBUTTON_H

#include <rwe/observable/Subject.h>
#include <rwe/ui/UiComponent.h>
#include <vector>

namespace rwe
{
    class UiStagedButton : public UiComponent
    {
    private:
        std::shared_ptr<SpriteSeries> spriteSeries;
        std::vector<std::string> labels;
        std::shared_ptr<SpriteSeries> labelFont;

        /** True if the button is currently pressed down. */
        bool pressed{false};

        /**
         * True if the button is "armed".
         * The button is armed if the mouse cursor was pressed down inside of it
         * and has not yet been released.
         */
        bool armed{false};

        unsigned int currentStage{0};

        Subject<ButtonClickEvent> clickSubject;

    public:
        UiStagedButton(
            int posX,
            int posY,
            unsigned int sizeX,
            unsigned int sizeY,
            std::shared_ptr<SpriteSeries> _spriteSeries,
            std::vector<std::string> _labels,
            std::shared_ptr<SpriteSeries> _labelFont);

        void render(UiRenderService& graphics) const override;

        void mouseDown(MouseButtonEvent /*event*/) override;

        void mouseUp(MouseButtonEvent event) override;

        void mouseEnter() override;

        void mouseLeave() override;

        void unfocus() override;

        void keyDown(KeyEvent event) override;

        Observable<ButtonClickEvent>& onClick();

        void setStage(unsigned int newStage);

        bool autoChangeStage{true};

    private:
        void activateButton(const ButtonClickEvent& event);
    };
}

#endif
