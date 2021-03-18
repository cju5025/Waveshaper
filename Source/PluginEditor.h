#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class Waveshaper01AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Waveshaper01AudioProcessorEditor (Waveshaper01AudioProcessor&);
    ~Waveshaper01AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    Waveshaper01AudioProcessor& audioProcessor;
    
    ScopedPointer<Slider> driveKnob;
    ScopedPointer<Slider> rangeKnob;
    ScopedPointer<Slider> blendKnob;
    ScopedPointer<Slider> volumeKnob;
    
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> rangeAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> blendAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Waveshaper01AudioProcessorEditor)
};
