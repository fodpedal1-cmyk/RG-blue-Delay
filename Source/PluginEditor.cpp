#include "PluginEditor.h"
#include "PluginProcessor.h"

namespace
{
    class PedalLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(
            juce::Graphics& g,
            int x,
            int y,
            int width,
            int height,
            float sliderPos,
            float rotaryStartAngle,
            float rotaryEndAngle,
            juce::Slider&) override
        {
            auto area = juce::Rectangle<float>(
                (float)x,
                (float)y,
                (float)width,
                (float)height).reduced(8.0f);

            auto size = juce::jmin(
                area.getWidth(),
                area.getHeight());

            auto centre = area.getCentre();
            auto radius = size * 0.40f;

            // Shadow
            g.setColour(juce::Colours::black.withAlpha(0.55f));
            g.fillEllipse(
                centre.x - radius + 3.0f,
                centre.y - radius + 5.0f,
                radius * 2.0f,
                radius * 2.0f);

            // Outer metal ring
            g.setColour(juce::Colour(35, 37, 39));
            g.fillEllipse(
                centre.x - radius,
                centre.y - radius,
                radius * 2.0f,
                radius * 2.0f);

            g.setColour(juce::Colour(115, 120, 125));
            g.drawEllipse(
                centre.x - radius,
                centre.y - radius,
                radius * 2.0f,
                radius * 2.0f,
                2.0f);

            // Knob body
            auto knobRadius = radius * 0.82f;

            juce::ColourGradient metal(
                juce::Colour(210, 214, 218),
                centre.x,
                centre.y - knobRadius,
                juce::Colour(55, 58, 62),
                centre.x,
                centre.y + knobRadius,
                false);

            g.setGradientFill(metal);

            g.fillEllipse(
                centre.x - knobRadius,
                centre.y - knobRadius,
                knobRadius * 2.0f,
                knobRadius * 2.0f);

            g.setColour(juce::Colour(225, 228, 230));
            g.drawEllipse(
                centre.x - knobRadius,
                centre.y - knobRadius,
                knobRadius * 2.0f,
                knobRadius * 2.0f,
                1.5f);

            // Position marker
            auto angle =
                rotaryStartAngle +
                sliderPos *
                (rotaryEndAngle - rotaryStartAngle);

            auto markerLength = knobRadius * 0.62f;

            juce::Point<float> p1(
                centre.x + std::cos(angle) * knobRadius * 0.12f,
                centre.y + std::sin(angle) * knobRadius * 0.12f);

            juce::Point<float> p2(
                centre.x + std::cos(angle) * markerLength,
                centre.y + std::sin(angle) * markerLength);

            g.setColour(juce::Colours::black);
            g.drawLine(
                juce::Line<float>(p1, p2),
                4.0f);

            // Centre cap
            g.setColour(juce::Colour(30, 32, 34));
            g.fillEllipse(
                centre.x - 7.0f,
                centre.y - 7.0f,
                14.0f,
                14.0f);
        }
    };

    PedalLookAndFeel pedalLookAndFeel;
}

RGBlueDelayAudioProcessorEditor::RGBlueDelayAudioProcessorEditor(
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

RGBlueDelayAudioProcessorEditor::~RGBlueDelayAudioProcessorEditor()
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
        juce::MathConstants<float>::pi * 1.25f,
        juce::MathConstants<float>::pi * 2.75f,
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
    auto bounds = getLocalBounds().toFloat();

    // Main 125B enclosure
    juce::ColourGradient body(
        juce::Colour(45, 48, 52),
        0.0f,
        0.0f,
        juce::Colour(20, 22, 25),
        500.0f,
        640.0f,
        false);

    g.setGradientFill(body);

    g.fillRoundedRectangle(
        bounds.reduced(7.0f),
        18.0f);

    // Metal edge
    g.setColour(juce::Colour(115, 120, 125));
    g.drawRoundedRectangle(
        bounds.reduced(7.0f),
        18.0f,
        2.0f);

    // Corner screws
    g.setColour(juce::Colour(160, 163, 165));

    const float screws[][2] =
    {
        {25.0f, 25.0f},
        {475.0f, 25.0f},
        {25.0f, 615.0f},
        {475.0f, 615.0f}
    };

    for (auto& s : screws)
    {
        g.fillEllipse(
            s[0] - 5.0f,
            s[1] - 5.0f,
            10.0f,
            10.0f);

        g.setColour(juce::Colour(65, 68, 70));

        g.drawLine(
            s[0] - 3.0f,
            s[1] - 3.0f,
            s[0] + 3.0f,
            s[1] + 3.0f,
            1.2f);

        g.setColour(juce::Colour(160, 163, 165));
    }

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(27.0f)
                .withStyle("bold")));

    g.drawFittedText(
        "RG BLUE DELAY",
        juce::Rectangle<int>(40, 35, 420, 35),
        juce::Justification::centred,
        1);

    g.setColour(juce::Colour(165, 170, 175));
    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(11.0f)));

    g.drawFittedText(
        "ANALOG DELAY",
        juce::Rectangle<int>(40, 70, 420, 20),
        juce::Justification::centred,
        1);

    // Panel line
    g.setColour(juce::Colour(90, 94, 98));

    g.drawLine(
        40.0f,
        98.0f,
        460.0f,
        98.0f,
        1.0f);

    // LED state
    bool bypassed =
        audioProcessor.parameters
            .getRawParameterValue("BYPASS")
            ->load() > 0.5f;

    if (!bypassed)
    {
        g.setColour(
            juce::Colours::deepskyblue.withAlpha(0.18f));

        g.fillEllipse(
            75.0f,
            487.0f,
            48.0f,
            48.0f);

        g.setColour(juce::Colours::deepskyblue);

        g.fillEllipse(
            88.0f,
            500.0f,
            22.0f,
            22.0f);
    }
    else
    {
        g.setColour(juce::Colour(35, 36, 38));

        g.fillEllipse(
            88.0f,
            500.0f,
            22.0f,
            22.0f);
    }

    // Footswitch housing
    g.setColour(juce::Colour(14, 15, 16));

    g.fillRoundedRectangle(
        160.0f,
        485.0f,
        180.0f,
        95.0f,
        12.0f);

    g.setColour(juce::Colour(80, 83, 86));

    g.drawRoundedRectangle(
        160.0f,
        485.0f,
        180.0f,
        95.0f,
        12.0f,
        2.0f);

    // 3PDT-style metal switch
    g.setColour(juce::Colour(145, 148, 150));

    g.fillRoundedRectangle(
        205.0f,
        503.0f,
        90.0f,
        58.0f,
        8.0f);

    g.setColour(juce::Colours::black);

    g.drawRoundedRectangle(
        205.0f,
        503.0f,
        90.0f,
        58.0f,
        8.0f,
        2.0f);

    // Three contacts
    g.setColour(juce::Colour(35, 37, 39));

    g.fillEllipse(218.0f, 523.0f, 14.0f, 14.0f);
    g.fillEllipse(243.0f, 523.0f, 14.0f, 14.0f);
    g.fillEllipse(268.0f, 523.0f, 14.0f, 14.0f);

    // ON / BYPASS
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
    g.setColour(juce::Colour(125, 128, 132));

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(10.0f)));

    g.drawFittedText(
        "RG ELECTRONICS",
        juce::Rectangle<int>(40, 605, 420, 20),
        juce::Justification::centred,
        1);
}

void RGBlueDelayAudioProcessorEditor::resized()
{
    delaySlider.setBounds(
        45, 115, 180, 165);

    repeatSlider.setBounds(
        275, 115, 180, 165);

    mixSlider.setBounds(
        160, 300, 180, 165);

    delayLabel.setBounds(
        70, 260, 130, 25);

    repeatLabel.setBounds(
        300, 260, 130, 25);

    mixLabel.setBounds(
        185, 445, 130, 25);

    footswitchButton.setBounds(
        160, 485, 180, 95);
}
