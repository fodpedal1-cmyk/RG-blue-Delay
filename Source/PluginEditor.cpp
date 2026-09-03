#include "PluginEditor.h"
#include "PluginProcessor.h"

RGBlueDelayAudioProcessorEditor::RGBlueDelayAudioProcessorEditor(
    RGBlueDelayAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p)
{
    setupKnob(delaySlider, delayLabel, "DELAY");
    setupKnob(repeatSlider, repeatLabel, "REPEAT");
    setupKnob(mixSlider, mixLabel, "MIX");

    delayAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.parameters, "DELAY", delaySlider);

    repeatAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.parameters, "REPEAT", repeatSlider);

    mixAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.parameters, "MIX", mixSlider);

    footswitchButton.setClickingTogglesState(true);
    footswitchButton.setColour(
        juce::TextButton::buttonColourId,
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

    setSize(500, 640);
}

RGBlueDelayAudioProcessorEditor::~RGBlueDelayAudioProcessorEditor()
{
}

void RGBlueDelayAudioProcessorEditor::setupKnob(
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false,
        90,
        22);

    slider.setRange(0.0, 1.0, 0.001);
    slider.setColour(
        juce::Slider::rotarySliderFillColourId,
        juce::Colours::lightgrey);
    slider.setColour(
        juce::Slider::rotarySliderOutlineColourId,
        juce::Colours::black);
    slider.setColour(
        juce::Slider::thumbColourId,
        juce::Colours::white);
    slider.setColour(
        juce::Slider::textBoxTextColourId,
        juce::Colours::white);
    slider.setColour(
        juce::Slider::textBoxBackgroundColourId,
        juce::Colours::black);
    slider.setColour(
        juce::Slider::textBoxOutlineColourId,
        juce::Colours::grey);

    addAndMakeVisible(slider);

    label.setText(
        labelText,
        juce::dontSendNotification);

    label.setJustificationType(juce::Justification::centred);
    label.setColour(
        juce::Label::textColourId,
        juce::Colours::white);
    label.setFont(
        juce::Font(juce::FontOptions()
                       .withHeight(16.0f)
                       .withStyle("bold")));

    addAndMakeVisible(label);
}

void RGBlueDelayAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Pedal body
    g.setColour(juce::Colour(28, 30, 32));
    g.fillRoundedRectangle(
        bounds.reduced(8.0f),
        18.0f);

    // Border
    g.setColour(juce::Colour(75, 78, 82));
    g.drawRoundedRectangle(
        bounds.reduced(8.0f),
        18.0f,
        3.0f);

    // Header
    g.setColour(juce::Colours::white);
    g.setFont(
        juce::Font(juce::FontOptions()
                       .withHeight(27.0f)
                       .withStyle("bold")));

    g.drawFittedText(
        "RG BLUE DELAY",
        juce::Rectangle<int>(30, 25, 440, 35),
        juce::Justification::centred,
        1);

    g.setFont(
        juce::Font(juce::FontOptions()
                       .withHeight(13.0f)));

    g.setColour(juce::Colour(150, 155, 160));

    g.drawFittedText(
        "ANALOG-STYLE DELAY",
        juce::Rectangle<int>(30, 58, 440, 22),
        juce::Justification::centred,
        1);

    // Divider
    g.setColour(juce::Colour(90, 93, 96));
    g.drawLine(
        35.0f,
        92.0f,
        465.0f,
        92.0f,
        1.0f);

    // Footswitch plate
    g.setColour(juce::Colour(18, 19, 20));
    g.fillRoundedRectangle(
        145.0f,
        500.0f,
        210.0f,
        78.0f,
        10.0f);

    g.setColour(juce::Colour(80, 82, 85));
    g.drawRoundedRectangle(
        145.0f,
        500.0f,
        210.0f,
        78.0f,
        10.0f,
        2.0f);

    // 3PDT-style footswitch
    g.setColour(juce::Colour(115, 118, 120));
    g.fillRoundedRectangle(
        205.0f,
        515.0f,
        90.0f,
        48.0f,
        7.0f);

    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(
        205.0f,
        515.0f,
        90.0f,
        48.0f,
        7.0f,
        2.0f);

    // Three switch circles
    g.setColour(juce::Colour(35, 36, 38));

    g.fillEllipse(218.0f, 528.0f, 14.0f, 14.0f);
    g.fillEllipse(243.0f, 528.0f, 14.0f, 14.0f);
    g.fillEllipse(268.0f, 528.0f, 14.0f, 14.0f);

    // Blue LED
    bool bypassed =
        audioProcessor.parameters
            .getRawParameterValue("BYPASS")
            ->load() > 0.5f;

    if (!bypassed)
    {
        g.setColour(juce::Colours::deepskyblue);
        g.fillEllipse(
            104.0f,
            523.0f,
            18.0f,
            18.0f);

        g.setColour(
            juce::Colours::deepskyblue.withAlpha(0.25f));

        g.fillEllipse(
            96.0f,
            515.0f,
            34.0f,
            34.0f);
    }
    else
    {
        g.setColour(juce::Colour(35, 35, 35));
        g.fillEllipse(
            104.0f,
            523.0f,
            18.0f,
            18.0f);
    }

    // Status text
    g.setColour(juce::Colours::white);
    g.setFont(
        juce::Font(juce::FontOptions()
                       .withHeight(14.0f)
                       .withStyle("bold")));

    g.drawFittedText(
        bypassed ? "BYPASS" : "ON",
        juce::Rectangle<int>(365, 525, 75, 20),
        juce::Justification::centred,
        1);

    // Footer
    g.setColour(juce::Colour(130, 133, 136));
    g.setFont(
        juce::Font(juce::FontOptions()
                       .withHeight(11.0f)));

    g.drawFittedText(
        "RG ELECTRONICS",
        juce::Rectangle<int>(30, 605, 440, 20),
        juce::Justification::centred,
        1);
}

void RGBlueDelayAudioProcessorEditor::resized()
{
    delaySlider.setBounds(45, 115, 180, 165);
    repeatSlider.setBounds(275, 115, 180, 165);
    mixSlider.setBounds(160, 300, 180, 165);

    delayLabel.setBounds(75, 265, 120, 25);
    repeatLabel.setBounds(305, 265, 120, 25);
    mixLabel.setBounds(190, 450, 120, 25);

    footswitchButton.setBounds(
        145,
        500,
        210,
        78);
}
