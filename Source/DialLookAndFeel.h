/*
  ==============================================================================

    DialLookAndFeel.h
    Created: 2 Jul 2025 7:31:43am
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

enum class DialLAF { RoundDial, GainSlider, HorizontalSlider, VerticalSlider};

class DialLookAndFeel : public juce::LookAndFeel_V4
{
public:
    
    DialLookAndFeel(DialLAF l) : lookAndFeel(l)
    {
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxTextColourId, juce::Colour(200, 200, 200));
    }
    
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override
    {
        bool hover = slider.isMouseOver();
        
        switch(lookAndFeel){
            case DialLAF::RoundDial:
            {
                drawRoundDial(g, x, y, width, sliderPosProportional);
                break;
            }
            case DialLAF::GainSlider:
            {
                break;
            }
            case DialLAF::HorizontalSlider:
            {
                drawHorizontalSlider(g, x, y, width, height, sliderPosProportional, hover);
                break;
            }
            case DialLAF::VerticalSlider:
            {
                drawVerticalSlider(g, x, y, width, height, sliderPosProportional, hover);
                break;
            }

        }
    }
    
    void setIndex(int index)
    {
        this->index = index;
    }
    
    void drawRoundDial(juce::Graphics &g, float x, float y, float size, float position)
    {
        //==============================================================================

        float pi = juce::MathConstants<float>::pi;
        float dialStart = 1.25f * pi;
        float dialEnd = 2.75f * pi;
        float sliderPositionScaled = 2.0f + (1.0f - position);
        float dialPositionInRadians = dialStart + sliderPositionScaled * (dialEnd - dialStart);
        

        juce::Path dialBodyPath, dialDotPath, dialOutlinePath, dialSelectPath, tensionLeftPath, tensionRightPath;
        
        float dialOutlineRadius = (size * 0.8f)/2;
        float dialBodyRadius = (size * 0.65f)/2;
        float dialDotRadius = (size * 0.45f)/2;

        dialOutlinePath.addCentredArc(x + size/2, x + size/2,
                                      dialOutlineRadius, dialOutlineRadius,
                                      0.0f, dialStart, dialEnd, true);
        g.setColour(Colors::mainColors[0]); //

        juce::PathStrokeType strokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(dialOutlinePath, juce::PathStrokeType(strokeType));

        //==============================================================================
        // dial body
        
        dialBodyPath.addCentredArc(x + size/2, y + size/2,
                                   dialBodyRadius, dialBodyRadius,
                                   0.0f, 0.0f, 6.28f, true);
        g.setColour(Colors::mainColors[0]); //
        g.strokePath(dialBodyPath, juce::PathStrokeType(strokeType));
        g.setColour(Colors::mainColors[0].withAlpha((float)0.35f)); //
        g.fillPath(dialBodyPath);

        //==============================================================================
        // dial dot
        
        juce::Point<float> outlineCoords = {x + size/2 + std::sin(dialPositionInRadians) * dialDotRadius,
            x + size/2 + std::cos(dialPositionInRadians) * dialDotRadius};

        dialDotPath.addCentredArc(outlineCoords.x, outlineCoords.y,
                                  1.5, 1.5, 0.0f, 0.0f, pi * 2, true);
        g.setColour(juce::Colour(200, 200, 200));
        g.fillPath(dialDotPath);
    }
    
    void drawHorizontalSlider(juce::Graphics& g, float x, float y, float width, float height, float pos, bool isHovered)
    {
        juce::Rectangle<float> slider = { (x + width - sliderSize) * pos,
            y,
            sliderSize,
            height };

        juce::Path sliderPath;
        sliderPath.addRoundedRectangle(slider, sliderSize * 0.15f);
        
        auto fillColor = isHovered ? Colors::mainHoverColors[index] : Colors::mainColors[index];
        g.setColour(fillColor);
        g.fillPath(sliderPath);
    }
    
    void drawVerticalSlider(juce::Graphics& g, float x, float y, float width, float height, float pos, bool isHovered)
    {
        juce::Rectangle<float> slider = { x,
            (y + height - sliderSize) * (1.0f - pos),
            width,
            sliderSize };
                
        juce::Path sliderPath;
        sliderPath.addRoundedRectangle(slider, sliderSize * 0.15f);
        
        auto fillColor = isHovered ? Colors::mainHoverColors[index] : Colors::mainColors[index];
        g.setColour(fillColor);
        g.fillPath(sliderPath);
    }
        
    void setSliderSize(float sliderSize)
    {
        this->sliderSize = sliderSize;
    }

    void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        g.setColour(juce::Colours::transparentBlack);
        juce::Path bgPath;
        bgPath.addRoundedRectangle(textEditor.getLocalBounds().reduced(4.0f, 0.0f), 3.0f);
        g.fillPath(bgPath);
    }

    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        auto label = LookAndFeel_V4::createSliderTextBox(slider);
        
        label->setFont(juce::FontOptions(12.0f, juce::Font::plain));
        label->setJustificationType(juce::Justification::centred);
        label->setColour(juce::Label::textColourId, juce::Colour(200, 200, 200));

        label->onEditorShow = [label]()
        {
            if(auto* editor = label->getCurrentTextEditor())
            {
                editor->setJustification(juce::Justification::centred);
            }
        };
        return label;
    }
    
private:
    DialLAF lookAndFeel;
    int index;
    float sliderSize;
};


class EditableTextBoxSlider : public juce::Component, juce::Timer, juce::AudioProcessorParameter::Listener, juce::AsyncUpdater, juce::Label::Listener
{
public:
    EditableTextBoxSlider(FledgeAudioProcessor& p, juce::String parameterID, juce::String parameterSuffix) : audioProcessor(p)
    {
        this->parameterID = parameterID;
        this->parameterSuffix = parameterSuffix;
        
        addAndMakeVisible(textBox);
        textBox.setEditable(false, false, false);
        textBox.setInterceptsMouseClicks(false, false);
        textBox.setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
        
        // initialize displayed value
        auto value = audioProcessor.params->apvts.getRawParameterValue(parameterID)->load();
        juce::String formattedValue = juce::String(value, numDecimals) + parameterSuffix;
        textBox.setText(formattedValue, juce::dontSendNotification);
        textBox.addListener(this);
        
        // initialize parameter ranges
        juce::NormalisableRange range = audioProcessor.params->apvts.getParameterRange(parameterID);
        rangeStart = range.start;
        rangeEnd = range.end;
        
        // add listener
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->addListener(this);
        }
        
        // start timer
        startTimerHz(30);
    }
    
    ~EditableTextBoxSlider()
    {
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->removeListener(this);
        }
    }
    
    void paint(juce::Graphics& g) override {}
    
    void resized() override {
        auto bounds = getLocalBounds();
        textBox.setBounds(bounds);
        
    }
        
    void mouseDown(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
        
        initialParamValue = audioProcessor.params->apvts.getParameter(parameterID)->getValue();
    }

    void mouseDrag(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        float deltaY = mousePoint.y - dragStartPoint.y;
        
        float sensitivity = 0.005f;
        float newValue = juce::jlimit(0.0f, 1.0f, initialParamValue + (-deltaY * sensitivity));
        textValueToParamValue(newValue);
    }
    
    void mouseUp(const juce::MouseEvent& m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        dragStartPoint.y = mousePoint.y;
    }
    
    void mouseDoubleClick(const juce::MouseEvent& m) override
    {
        textBox.setInterceptsMouseClicks(true, true);
        textBox.setEditable(true, true, false);
        textBox.showEditor();
        textBox.grabKeyboardFocus();
    }
    
    void labelTextChanged(juce::Label* l) override
    {
        auto value = l->getText().getFloatValue();
        float valueLimited = juce::jlimit(rangeStart, rangeEnd, value);
        
        l->setText(juce::String(valueLimited, numDecimals), juce::dontSendNotification);
        textBox.setInterceptsMouseClicks(false, false);
        
        float normalized = (valueLimited - rangeStart) / (rangeEnd - rangeStart);
        textValueToParamValue(normalized);
        repaint();
    }
    
    void editorHidden(juce::Label *, juce::TextEditor &) override
    {
        textBox.setInterceptsMouseClicks(false, false);
    }
    
    void textValueToParamValue(float value)
    {
        value = juce::jlimit(0.0f, 1.0f, value);
        audioProcessor.params->apvts.getParameter(parameterID)->setValueNotifyingHost(value);
    }
        
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        const juce::SpinLock::ScopedLockType lock(pendingLock);
        pendingChanges.emplace_back(parameterIndex, newValue);
        triggerAsyncUpdate();
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    
    void handleAsyncUpdate() override
    {
        std::vector<std::pair<int, float>> updatesCopy;

        {
            const juce::SpinLock::ScopedLockType lock(pendingLock);
            updatesCopy.swap(pendingChanges); // safely move all pending updates
        }

        for (const auto& [parameterIndex, newValue] : updatesCopy)
        {
            juce::String newParameterID;
            float scaledValue;
            
            if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[parameterIndex]))
            {
                if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
                {
                    scaledValue = rangedParam->convertFrom0to1(newValue);
                    newParameterID = param->paramID;
                }
            }
            
            if (newParameterID == parameterID)
            {
                juce::String formattedValue = juce::String(scaledValue, numDecimals) + parameterSuffix;
                textBox.setText(formattedValue, juce::dontSendNotification);
            }
        }
    }

    void setFontSize(float size)
    {
        textBox.setFont(juce::FontOptions(size, juce::Font::plain));
    }
    
    void setNumDecimals(int numDecimals)
    {
        this->numDecimals = numDecimals;
    }
    
    void timerCallback() override
    {
        auto bounds = getLocalBounds().toFloat();
        auto mouse = getMouseXYRelative().toFloat();
        if (bounds.contains(mouse))
        {
            setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
        }
    }
    
private:
    float initialParamValue;
    float rangeStart, rangeEnd;
    
    std::vector<std::pair<int, float>> pendingChanges;
    juce::SpinLock pendingLock;

    juce::Point<float> dragStartPoint;
    juce::Label textBox;
    int numDecimals = 1;
    juce::String parameterID, parameterSuffix = "";
    
    FledgeAudioProcessor& audioProcessor;
};
