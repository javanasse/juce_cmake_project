#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       ),
    parameters(*this, nullptr, juce::Identifier("IAS_LPC"), 
        {
            std::make_unique<juce::AudioParameterInt>(
                "numCoefficients",
                "Num Coefficients",
                1, 
                70,
                8
            ),
            std::make_unique<juce::AudioParameterBool>(
                "whisperFlag",
                "Whisper",
                false,
                juce::AudioParameterBoolAttributes().withStringFromValueFunction ([] (auto x, auto) { return x ? "On" : "Off"; })
                                                        .withLabel ("enabled")
            )
        }
    )
{
    // initialize parameters
    numCoefficientsParameter = parameters.getRawParameterValue("numCoefficients");
    whisperFlagParameter = parameters.getRawParameterValue("whisperFlag");

    // initialize lpc data
    this->lpc_instance = lpc_create();

    // intialize audio buffer
    inputBuffer.setSize(1, bufferSize);
    outputBuffer.setSize(1, bufferSize);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    outputAnalyser.stopThread (1000);
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    outputAnalyser.setupAnalyser (int (sampleRate), float (sampleRate));
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    outputAnalyser.stopThread (1000);
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // clear unused input channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // get parameters for block
    const uint numCoefficients = static_cast<uint>(*numCoefficientsParameter);
    bool whisperFlag = static_cast<bool>(*whisperFlagParameter);

    // loop over samples
    for (size_t sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        inputBuffer.setSample(0, writePosition, buffer.getSample(0, sample));
        
        // process inputBuffer when full
        if (writePosition == 0)
        {
            // lpc data
            float coefficients[numCoefficients];
            float power;
            float pitch;
            int glottal = 0;

            lpc_analyze(this->lpc_instance, 
                inputBuffer.getWritePointer(0), 
                inputBuffer.getNumSamples(), 
                coefficients, 
                numCoefficients, 
                &power, 
                &pitch
            );

            if (whisperFlag)
            {
                pitch = 0.0f;
            }
            // else
            // {
            //     // juce::MidiMessageMetadata firstMessage = *midiMessages.begin();
            // }

            // clear will be the window of life
            lpc_synthesize(this->lpc_instance, 
                outputBuffer.getWritePointer(0), 
                outputBuffer.getNumSamples(), 
                coefficients, 
                numCoefficients, 
                power, 
                pitch, 
                glottal
            );
        }

        // write output
        buffer.setSample(0, sample, outputBuffer.getSample(0, writePosition));

        // increment write position
        writePosition = (writePosition + 1) % bufferSize;
    }


    
    outputAnalyser.addAudioData (buffer, 0, getTotalNumOutputChannels());
    
    
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

void AudioPluginAudioProcessor::createAnalyserPlot (juce::Path& p, const juce::Rectangle<int> bounds, float minFreq)
{
        outputAnalyser.createPath (p, bounds.toFloat(), minFreq);
}

bool AudioPluginAudioProcessor::checkForNewAnalyserData()
{
    return outputAnalyser.checkForNewData();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
