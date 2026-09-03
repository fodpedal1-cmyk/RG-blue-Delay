#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================
// CONSTRUCTOR
//==============================================================

RGBlueDelayAudioProcessorEditor::RGBlueDelayAudioProcessorEditor(
    RGBlueDelayAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p)
{
    //==========================================================
    // DELAY
    //==========================================================

    setupKnob(delaySlider, delayLabel, "DELAY");

    delayAttachment =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.parameters,
                "DELAY",
                delaySlider);

    //==========================================================
    // REPEAT
    //==========================================================

    setupKnob(repeatSlider, repeatLabel, "REPEAT");

    repeatAttachment =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.parameters,
                "REPEAT",
                repeatSlider);

    //==========================================================
    // MIX
    //==========================================================

    setupKnob(mixSlider, mixLabel, "MIX");

    mixAttachment =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.parameters,
                "MIX",
                mixSlider);

    //==========================================================
    // FOOTSWITCH
    //==========================================================

    footswitchButton.setClickingTogglesState(true);
    footswitchButton.setButtonText("");
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

    //==========================================================
    // WINDOW SIZE
    //==========================================================

    setSize(500, 640);
}

//==============================================================
// DESTRUCTOR
//==============================================================

RGBlueDelayAudioProcessorEditor::~RGBlueDelayAudioProcessorEditor()
{
}

//==============================================================
// KNOB SETUP
//==============================================================

void RGBlueDelayAudioProcessorEditor::setupKnob(
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& labelText)
{
    slider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag);

    slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false,
        90,
        22);

    slider.setColour(
        juce::Slider::rotarySliderFillColourId,
        juce::Colour(0xff111111));

    slider.setColour(
        juce::Slider::rotarySliderOutlineColourId,
        juce::Colour(0xff050505));

    slider.setColour(
        juce::Slider::thumbColourId,
        juce::Colours::white);

    slider.setColour(
        juce::Slider::textBoxTextColourId,
        juce::Colours::white);

    slider.setColour(
        juce::Slider::textBoxBackgroundColourId,
        juce::Colour(0xff111111));

    slider.setColour(
        juce::Slider::textBoxOutlineColourId,
        juce::Colours::transparentBlack);

    slider.setRange(0.0, 1.0, 0.001);
    slider.setValue(0.5);

    addAndMakeVisible(slider);

    label.setText(
        labelText,
        juce::dontSendNotification);

    label.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(17.0f)
                .withStyle("bold")));

    label.setColour(
        juce::Label::textColourId,
        juce::Colours::white);

    label.setJustificationType(
        juce::Justification::centred);

    addAndMakeVisible(label);
}

//==============================================================
// PAINT
//==============================================================

void RGBlueDelayAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    //==========================================================
    // PEDAL BODY
    //==========================================================

    g.setColour(juce::Colour(0xff151515));
    g.fillRoundedRectangle(
        bounds.reduced(8.0f),
        18.0f);

    //==========================================================
    // OUTER EDGE
    //==========================================================

    g.setColour(juce::Colour(0xff303030));
    g.drawRoundedRectangle(
        bounds.reduced(8.0f),
        18.0f,
        3.0f);

    //==========================================================
    // INNER PANEL
    //==========================================================

    g.setColour(juce::Colour(0xff1c1c1c));
    g.drawRoundedRectangle(
        bounds.reduced(17.0f),
        13.0f,
        1.0f);

    //==========================================================
    // TITLE
    //==========================================================

    g.setColour(juce::Colours::white);

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(29.0f)
                .withStyle("bold")));

    g.drawFittedText(
        "RG BLUE DELAY",
        juce::Rectangle<int>(
            30,
            28,
            getWidth() - 60,
            45),
        juce::Justification::centred,
        1);

    //==========================================================
    // SUBTITLE
    //==========================================================

    g.setFont(
        juce::Font(
            juce::FontOptions()
                .withHeight(11.0f)));

    g.setColour(juce::Colour(0xff999999));

    g.drawFittedText(
        "ANALOG-STYLE DELAY",
        juce::Rectangle<int>(
            30,
            70,
            getWidth() - 60,
            20),
        juce::Justification::centred,
        1);

    //==========================================================
    // DIVIDER
    //==========================================================

    g.setColour(juce::Colour
