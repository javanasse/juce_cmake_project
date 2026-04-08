#pragma once

#include "PluginProcessor.hpp"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, 
                                                public juce::Slider::Listener, 
                                                public juce::Button::Listener,
                                                public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void sliderValueChanged(juce::Slider* slider) override;

    void buttonClicked(juce::Button* button) override;
    void buttonStateChanged(juce::Button *button) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::Slider numCoefficientsSlider;
    juce::Label numCoefficientsSliderLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment numCoefficientsSliderAttachment;

    juce::ToggleButton whisperButton;
    juce::Label whisperButtonLabel;
    juce::AudioProcessorValueTreeState::ButtonAttachment whisperButtonAttachment;

    juce::Rectangle<int> spectrogramBounds;
    juce::Path spectrogramPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
