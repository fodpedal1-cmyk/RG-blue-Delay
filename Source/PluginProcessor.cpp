#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================
// RG BLUE DELAY
// Component-inspired PT2399 / Deep Blue style delay
// Controls:
// DELAY  = 60 - 634 ms
// REPEAT = feedback
// MIX    = wet/dry
// BYPASS = footswitch
//==============================================================

namespace
{
    constexpr float minDelayMs = 60.0f;
    constexpr float maxDelayMs = 634.0f;

    constexpr float maxFeedback = 0.92f;

    // Component-inspired filter targets.
    //
    // These are DSP equivalents of the important audio-path
    // resistor/capacitor networks used in the reference circuit.
    //
    // 5.1k + 22nF  -> approximately 1.42 kHz
    // 2k  + 47nF  -> approximately 1.69 kHz
    //
    // The delay repeats therefore become progressively darker.

    constexpr float feedbackHighPassHz = 70.0f;
    constexpr float feedbackLowPassHz  = 7200.0f;

    constexpr float inputHighPassHz = 25.0f;
    constexpr float inputLowPassHz  = 16000.0f;
}

//==============================================================
// CONSTRUCTOR
//==============================================================

RGBlueDelayAudioProcessor::RGBlueDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
        BusesProperties()
            .withInput(
                "Input",
                juce::AudioChannelSet::stereo(),
                true)
            .withOutput(
                "Output",
                juce::AudioChannelSet::stereo(),
                true)),
#endif

      parameters(
          *this,
          nullptr,
          "PARAMETERS",
          createParameterLayout())
{
}

//==============================================================
// DESTRUCTOR
//==============================================================

RGBlueDelayAudioProcessor::~RGBlueDelayAudioProcessor()
{
}

//==============================================================
// PARAMETER LAYOUT
//==============================================================

juce::AudioProcessorValueTreeState::ParameterLayout
RGBlueDelayAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(
        std::make_unique<juce::AudioParameterFloat>(
            "DELAY",
            "Delay",
            juce::NormalisableRange<float>(
                minDelayMs,
                maxDelayMs,
                0.01f),
            300.0f));

    params.push_back(
        std::make_unique<juce::AudioParameterFloat>(
            "REPEAT",
            "Repeat",
            juce::NormalisableRange<float>(
                0.0f,
                maxFeedback,
                0.001f),
            0.45f));

    params.push_back(
        std::make_unique<juce::AudioParameterFloat>(
            "MIX",
            "Mix",
            juce::NormalisableRange<float>(
                0.0f,
                1.0f,
                0.001f),
            0.32f));

    params.push_back(
        std::make_unique<juce::AudioParameterBool>(
            "BYPASS",
            "Bypass",
            false));

    return { params.begin(), params.end() };
}

//==============================================================
// PREPARE TO PLAY
//==============================================================

void RGBlueDelayAudioProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    //==========================================================
    // Delay memory
    //==========================================================

    const auto maximumDelaySamples =
        static_cast<int>(
            std::ceil(
                (maxDelayMs / 1000.0f)
                * static_cast<float>(sampleRate)))
        + samplesPerBlock
        + 8;

    delayBufferSize =
        juce::jmax(
            maximumDelaySamples,
            32);

    delayBuffer.setSize(
        2,
        delayBufferSize);

    delayBuffer.clear();

    writePosition = 0;

    currentDelaySamples =
        static_cast<float>(
            300.0
            / 1000.0
            * sampleRate);

    //==========================================================
    // Parameter smoothing
    //==========================================================

    delaySmoothed.reset(sampleRate, 0.025);
    feedbackSmoothed.reset(sampleRate, 0.025);
    mixSmoothed.reset(sampleRate, 0.025);
    bypassSmoothed.reset(sampleRate, 0.010);

    delaySmoothed.setCurrent
