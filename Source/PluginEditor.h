#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================
// RG BLUE DELAY EDITOR
//==============================================================

class RGBlueDelayAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit RGBlueDelayAudioProcessorEditor(
        RGBlueDelayAudioProcessor&);

    ~RGBlueDelayAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    RGBlueDelayAudioProcessor& audioProcessor;

    //==========================================================
    // KNOBS
    //==========================================================

    juce::Slider delaySlider;
    juce::Slider repeatSlider;
    juce::Slider mixSlider;

    juce::Label delayLabel;
    juce::Label repeatLabel;
    juce::Label mixLabel;

    //==========================================================
    // FOOTSWITCH
    //==========================================================

    juce::TextButton footswitchButton;

    //==========================================================
    // PARAMETER ATTACHMENTS
    //==========================================================

    std::unique_ptr<
        juce::AudioProcessorValueTreeState::SliderAttachment>
        delayAttachment;

    std::unique_ptr<
        juce::AudioProcessorValueTreeState::SliderAttachment>
        repeatAttachment;

    std::unique_ptr<
        juce::AudioProcessorValueTreeState::SliderAttachment>
        mixAttachment;

    std::unique_ptr<
        juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassAttachment;

    //==========================================================
    // SETUP
    //==========================================================

    void setupKnob(
        juce::Slider& slider,
        juce::Label& label,
        const juce::String& labelText);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        RGBlueDelayAudioProcessorEditor)
};
