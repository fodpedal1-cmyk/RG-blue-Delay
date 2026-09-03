#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <cmath>

namespace
{
class RealisticLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(
        juce::Graphics& g,
        int x,
        int y,
        int w,
        int h,
        float value,
        float startAngle,
        float endAngle,
        juce::Slider&) override
    {
        auto r = juce::Rectangle<float>(
            (float)x, (float)y, (float)w, (float)h);

        auto c = r.getCentre();
        float radius = juce::jmin(r.getWidth(), r.getHeight()) * 0.39f;

        // Shadow
        g.setColour(juce::Colours::black.withAlpha(0.65f));
        g.fillEllipse(
            c.x - radius + 4.0f,
            c.y - radius + 6.0f,
            radius * 2.0f,
            radius * 2.0f);

        // Outer metal ring
        g.setColour(juce::Colour(22, 24, 26));
        g.fillEllipse(
            c.x - radius,
            c.y - radius,
            radius * 2.0f,
            radius * 2.0f);

        g.setColour(juce::Colour(135, 139, 142));
        g.drawEllipse(
            c.x - radius,
            c.y - radius,
            radius * 2.0f,
            radius * 2.0f,
            2.0f);

        // Metallic knob
        float kr = radius * 0.83f;

        juce::ColourGradient metal(
            juce::Colour(235, 237, 238),
            c.x,
            c.y - kr,
            juce::Colour(52, 55, 58),
            c.x,
            c.y + kr,
            false);

        g.setGradientFill(metal);
        g.fillEllipse(
            c.x - kr,
            c.y - kr,
            kr * 2.0f,
            kr * 2.0f);

        g.setColour(juce::Colour(205, 208, 210));
        g.drawEllipse(
            c.x - kr,
            c.y - kr,
            kr * 2.0f,
            kr * 2.0f,
            1.5f);

        // Knob grip lines
        g.setColour(juce::Colours::black.withAlpha(0.35f));

        for (int i = 0; i < 24; ++i)
        {
            float a = juce::MathConstants<float>::twoPi
                    * (float)i / 24.0f;

            float x1 = c.x + std::cos(a) * kr * 0.88f;
            float y1 = c.y + std::sin(a) * kr * 0.88f;
            float x2 = c.x + std::cos(a) * kr * 0.97f;
            float y2 = c.y + std::sin(a) * kr * 0.97f;

            g.drawLine(x1, y1, x2, y2, 1.0f);
        }

        // Position marker
        float angle =
            startAngle + value * (endAngle - startAngle);

        float marker = kr * 0.68f;

        float mx =
            c.x + std::cos(angle) * marker;

        float my =
            c.y + std::sin(angle) * marker;

        g.setColour(juce::Colours::white);
        g.drawLine(
            c.x,
            c.y,
            mx,
            my,
            3.5f);

        // Centre cap
        g.setColour(juce::Colour(30, 32, 34));
        g.fillEllipse(
            c.x - 7.0f,
            c.y - 7.0f,
            14.0f,
            14.0f);
    }
};

RealisticLookAndFeel pedalLookAndFeel;
}

RGBlueDelayAudioProcessorEditor::
RGBlueDelayAudioProcessorEditor(
    RGBlueDelayAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p)
{
    setupKnob(delaySlider, delayLabel, "DELAY");
    setupKnob(repeatSlider, repeatLabel, "REPEAT");
    setupKnob(mixSlider, mixLabel, "MIX");

    delayAttachment =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.parameters,
                "DELAY",
                delaySlider);

    repeatAttachment =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.parameters,
                "REPEAT",
                repeatSlider);

    mixAttachment =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.parameters,
                "MIX",
                mixSlider);

    footswitchButton.setClickingTogglesState(true);

    footswitchButton.setColour(
        juce::TextButton::buttonColourId,
        juce::Colours::transparentBlack);

    footswitchButton.setColour(
        juce::TextButton::buttonOnColourId,
        juce::Colours::transparentBlack);

    footswitchButton.setColour(
        juce::TextButton::textColourOffId,
        juce::Colours::transparentBlack);

    footswitchButton.setColour(
        juce::TextButton::textColourOnId,
        juce::Colours::transparentBlack);

    addAndMakeVisible(footswitchButton);

    bypassAttachment =
        std::make_unique<
            juce::AudioProcessorValueTreeState::ButtonAttachment>(
                audioProcessor.parameters,
                "BYPASS",
                footswitchButton);

    setLookAndFeel(&pedalLookAndFeel);

    setSize(500, 640);
}

RGBlueDelayAudioProcessorEditor::
~RGBlueDelayAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void RGBlueDelayAudioProcessorEditor::setupKnob(
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& text)
{
    slider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    slider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0);

    slider.setRange(0.0, 1.0, 0.001);

    slider.setRotaryParameters(
        1.25f * juce::MathConstants<float>::pi,
        2.75f * juce::MathConstants<float>::pi,
        true);

    slider.setLookAndFeel(&pedalLookAndFeel);

    addAndMakeVisible(slider);

    label.setText(
        text,
        juce::dontSendNotification);

    label.setJustificationType(
        juce::Justification::centred);

    label.setColour(
        juce::Label::textColourId,
        juce::Colours::white);

    label.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(15.0f)
                .withStyle("bold")));

    addAndMakeVisible(label);
}

void RGBlueDelayAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    // 125B metal enclosure
    juce::ColourGradient body(
        juce::Colour(32, 44, 55),
        0.0f,
        0.0f,
        juce::Colour(8, 13, 18),
        500.0f,
        640.0f,
        false);

    g.setGradientFill(body);

    g.fillRoundedRectangle(
        b.reduced(7.0f),
        18.0f);

    // Metallic rim
    g.setColour(juce::Colour(125, 130, 134));
    g.drawRoundedRectangle(
        b.reduced(7.0f),
        18.0f,
        2.0f);

    // Inner rim
    g.setColour(juce::Colour(12, 16, 20));
    g.drawRoundedRectangle(
        b.reduced(13.0f),
        14.0f,
        1.0f);

    // Corner screws
    const float sx[] = {27.0f, 473.0f, 27.0f, 473.0f};
    const float sy[] = {27.0f, 27.0f, 613.0f, 613.0f};

    for (int i = 0; i < 4; ++i)
    {
        g.setColour(juce::Colour(165, 168, 170));

        g.fillEllipse(
            sx[i] - 5.0f,
            sy[i] - 5.0f,
            10.0f,
            10.0f);

        g.setColour(juce::Colour(55, 58, 60));

        g.drawLine(
            sx[i] - 3.0f,
            sy[i] - 3.0f,
            sx[i] + 3.0f,
            sy[i] + 3.0f,
            1.2f);
    }

    // Brand
    g.setColour(juce::Colours::white);

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(27.0f)
                .withStyle("bold")));

    g.drawFittedText(
        "RG BLUE DELAY",
        juce::Rectangle<int>(50, 32, 400, 35),
        juce::Justification::centred,
        1);

    g.setColour(juce::Colour(170, 190, 205));

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(11.0f)));

    g.drawFittedText(
        "ANALOG STYLE DELAY",
        juce::Rectangle<int>(50, 67, 400, 20),
        juce::Justification::centred,
        1);

    // Top divider
    g.setColour(juce::Colour(100, 110, 118));

    g.drawLine(
        45.0f,
        96.0f,
        455.0f,
        96.0f,
        1.0f);

    // LED
    bool bypassed =
        audioProcessor.parameters
            .getRawParameterValue("BYPASS")
            ->load() > 0.5f;

    if (!bypassed)
    {
        g.setColour(
            juce::Colours::deepskyblue.withAlpha(0.20f));

        g.fillEllipse(
            69.0f,
            485.0f,
            54.0f,
            54.0f);

        g.setColour(
            juce::Colours::deepskyblue);

        g.fillEllipse(
            84.0f,
            500.0f,
            24.0f,
            24.0f);
    }
    else
    {
        g.setColour(juce::Colour(30, 32, 34));

        g.fillEllipse(
            84.0f,
            500.0f,
            24.0f,
            24.0f);
    }

    // Footswitch plate
    g.setColour(juce::Colour(10, 11, 12));

    g.fillRoundedRectangle(
        155.0f,
        480.0f,
        190.0f,
        105.0f,
        13.0f);

    g.setColour(juce::Colour(105, 108, 110));

    g.drawRoundedRectangle(
        155.0f,
        480.0f,
        190.0f,
        105.0f,
        13.0f,
        2.0f);

    // Realistic switch cap
    juce::ColourGradient switchMetal(
        juce::Colour(220, 222, 223),
        250.0f,
        500.0f,
        juce::Colour(75, 78, 80),
        250.0f,
        560.0f,
        false);

    g.setGradientFill(switchMetal);

    g.fillRoundedRectangle(
        204.0f,
        500.0f,
        92.0f,
        62.0f,
        9.0f);

    g.setColour(juce::Colour(25, 27, 29));

    g.drawRoundedRectangle(
        204.0f,
        500.0f,
        92.0f,
        62.0f,
        9.0f,
        2.0f);

    // 3PDT detail
    g.setColour(juce::Colour(35, 37, 39));

    g.fillEllipse(218.0f, 523.0f, 14.0f, 14.0f);
    g.fillEllipse(243.0f, 523.0f, 14.0f, 14.0f);
    g.fillEllipse(268.0f, 523.0f, 14.0f, 14.0f);

    // Status
    g.setColour(juce::Colours::white);

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(13.0f)
                .withStyle("bold")));

    g.drawFittedText(
        bypassed ? "BYPASS" : "ON",
        juce::Rectangle<int>(350, 510, 80, 25),
        juce::Justification::centred,
        1);

    // Footer
    g.setColour(juce::Colour(130, 140, 148));

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(10.0f)));

    g.drawFittedText(
        "RG ELECTRONICS",
        juce::Rectangle<int>(50, 605, 400, 18),
        juce::Justification::centred,
        1);
}

void RGBlueDelayAudioProcessorEditor::resized()
{
    delaySlider.setBounds(
        45, 110, 180, 165);

    repeatSlider.setBounds(
        275, 110, 180, 165);

    mixSlider.setBounds(
        160, 295, 180, 165);

    delayLabel.setBounds(
        70, 258, 130, 25);

    repeatLabel.setBounds(
        300, 258, 130, 25);

    mixLabel.setBounds(
        185, 442, 130, 25);

    footswitchButton.setBounds(
        155, 480, 190, 105);
}
