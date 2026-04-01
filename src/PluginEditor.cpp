#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    numCoefficientsSliderAttachment(p.parameters, "numCoefficients", numCoefficientsSlider),
    whisperButtonAttachment(p.parameters, "whisperFlag", whisperButton)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(numCoefficientsSlider);
    numCoefficientsSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    numCoefficientsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 15);
    numCoefficientsSlider.addListener(this);

    addAndMakeVisible (numCoefficientsSliderLabel);
    numCoefficientsSliderLabel.setText ("Order", juce::dontSendNotification);
    numCoefficientsSliderLabel.attachToComponent (&numCoefficientsSlider, true); 

    addAndMakeVisible(whisperButton);
    whisperButton.setButtonText("Whisper");
    whisperButton.addListener(this);

    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) 
{
}

void AudioPluginAudioProcessorEditor::buttonClicked(juce::Button* button)
{

}

void AudioPluginAudioProcessorEditor::buttonStateChanged(juce::Button *button)
{

}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    // processorRef.parameters.get
}

void AudioPluginAudioProcessorEditor::resized()
{
    int sliderBuffer = 120;
    numCoefficientsSlider.setBounds(sliderBuffer, 20, getWidth() - sliderBuffer - 10, 100);

    whisperButton.setBounds(100, 100, 100, 100);
}
