#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr float minDelayMs = 60.0f;
    constexpr float maxDelayMs = 634.0f;

    constexpr float maxFeedback = 0.92f;

    // Circuit-inspired filtering
    constexpr float inputHighPassHz = 25.0f;
    constexpr float inputLowPassHz = 16000.0f;

    // 5.1k + 22nF style feedback high-pass region
    constexpr float feedbackHighPassHz = 70.0f;

    // 2k + 47nF style darker repeat low-pass region
    constexpr float feedbackLowPassHz = 7200.0f;

    // Final output smoothing
    constexpr float outputLowPassHz = 12000.0f;
}

//==============================================================
// CONSTRUCTOR
//==============================================================

RGBlueDelayAudioProcessor::RGBlueDelayAudioProcessor()
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
// PARAMETERS
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
                0.1f),
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
// PREPARE
//==============================================================

void RGBlueDelayAudioProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    currentSampleRate = sampleRate;

    const int maximumDelaySamples =
        static_cast<int>(
            std::ceil(
                sampleRate *
                (maxDelayMs / 1000.0)));

    delayBufferSize =
        maximumDelaySamples + 4;

    delayBuffer.setSize(
        getTotalNumOutputChannels(),
        delayBufferSize);

    delayBuffer.clear();

    writePosition = 0;

    const float initialDelay =
        parameters
            .getRawParameterValue("DELAY")
            ->load();

    const float initialFeedback =
        parameters
            .getRawParameterValue("REPEAT")
            ->load();

    const float initialMix =
        parameters
            .getRawParameterValue("MIX")
            ->load();

    const float initialBypass =
        parameters
            .getRawParameterValue("BYPASS")
            ->load();

    delaySmoothed.reset(
        sampleRate,
        0.025);

    feedbackSmoothed.reset(
        sampleRate,
        0.025);

    mixSmoothed.reset(
        sampleRate,
        0.025);

    bypassSmoothed.reset(
        sampleRate,
        0.010);

    // JUCE 8
