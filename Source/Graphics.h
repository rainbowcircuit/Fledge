#pragma once
#include <JuceHeader.h>
#include <cmath>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

//Takuma your waveforms look like a butt

class OperatorDisplayGraphics : public juce::Component, juce::AudioProcessorParameter::Listener, juce::AsyncUpdater, juce::Timer
{
public:
    OperatorDisplayGraphics(FledgeAudioProcessor& p) : audioProcessor(p)
    {
        startTimerHz(30);
        
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->addListener(this);
        }
    }
    
    ~OperatorDisplayGraphics()
    {
        const auto params = audioProcessor.getParameters();
        for (auto param : params){
            param->removeListener(this);
        }
    }
    
    void setIndex(int index)
    {
        this->index = index;
    }
    
    void paint(juce::Graphics &g) override
    {
        bounds = getLocalBounds().toFloat();
        bounds.reduce(5, 5);
        juce::Path bgFill;
        bgFill.addRoundedRectangle(bounds, 5.0f);
        g.setColour(juce::Colour(12, 10, 11));

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth();
        float height = bounds.getHeight();

        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.setColour(Colors::mainColors[index]);
        
        juce::Path macroX, macroY;
        float sliderWidth = width * 0.1f;
        float sliderHeight = height * 0.05f;

        float macroYPosition = juce::jlimit(y, y + height - sliderWidth - sliderHeight, macroPosition.y);
        macroY.addRoundedRectangle(x, macroYPosition, sliderHeight, sliderWidth, 2);
        g.fillPath(macroY);

        float macroXPosition = juce::jlimit(x + sliderHeight, x + width - sliderWidth, macroPosition.x);
        macroX.addRoundedRectangle(macroXPosition, height * 0.95f, sliderWidth, sliderHeight, 2);
        g.fillPath(macroX);
        
        juce::Path waveform = waveformPath(g, x + width * 0.1f,
                                           y,
                                           width * 0.9f,
                                           height * 0.9f,
                                           ratio, amplitude);
        
        g.strokePath(waveform, strokeType);
        
    }
    
    juce::Path waveformPath(juce::Graphics &g, float x, float y, float width, float height, float freq, float amp)
    {
        juce::Path graphicPath;
        graphicPath.startNewSubPath(x, y + height/2);
        int domainResolution = 128;
        float widthIncrement = width/domainResolution;
        for (int i = 0; i < domainResolution; i++)
        {
            float sin = 1.0f * std::sin((i/40.7f) * freq * 2.0f);
            graphicPath.lineTo(x + widthIncrement * i, (y + height/2) + (height * sin/2) * amp);
        }
    
        graphicPath = graphicPath.createPathWithRoundedCorners(4.0f);
        return graphicPath;
    }
    
    void resized() override {}
    
    
    void setRatioAndAmplitude(float ratio, float fixed, float amplitude, bool isRatio)
    {
        this->ratio = ratio;
        this->fixed = fixed;
        this->amplitude = amplitude/100.0f;
        repaint();
    }
    
    void mouseDrag(const juce::MouseEvent& m) override
    {
        auto mouse = m.getPosition().toFloat();
        macroPosition = mouse;
        setParameter(mouse.x, mouse.y);
        repaint();
    }
    
    void setParameter(float x, float y)
    {
        float ratio = juce::jlimit(0.0f, 1.0f, x/100.0f);
        audioProcessor.apvts.getParameter("ratio" + juce::String(index))->setValueNotifyingHost(ratio);
        
        float amplitude = juce::jlimit(0.0f, 1.0f, 1.0f - (y/100.0f));
        audioProcessor.apvts.getParameter("amplitude" + juce::String(index))->setValueNotifyingHost(amplitude);
    }
    
    void timerCallback() override
    {
        auto bounds = getLocalBounds().toFloat();
        auto mouse = getMouseXYRelative().toFloat();
        
        if (bounds.contains(mouse))
        {
            setMouseCursor(juce::MouseCursor::UpDownLeftRightResizeCursor);
        }
    }
    
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        newValueAtomic.store(newValue);
        parameterIndexAtomic.store(parameterIndex);
        triggerAsyncUpdate();
    }
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {}
    
    void handleAsyncUpdate() override
    {
        float newValue = newValueAtomic.load();
        int parameterIndex = parameterIndexAtomic.load();
        juce::String newParameterID;
        
        if (auto* param = dynamic_cast<juce::AudioProcessorParameterWithID*>(audioProcessor.getParameters()[parameterIndex]))
        {
            if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
            {
                newParameterID = param->paramID;
            }
        }
        
        if (newParameterID == "ratio" + juce::String(index))
        {
            macroPosition.x = newValue * bounds.getWidth();
        } else if (newParameterID == "amplitude" + juce::String(index))
        {
            macroPosition.y = (1.0f - newValue) * bounds.getHeight();
        }
    }


private:
    juce::Rectangle<float> bounds;
    juce::Point<float> macroPosition;
    
    std::atomic<float> newValueAtomic;
    std::atomic<int> parameterIndexAtomic;
    
    int index;
    float ratio, fixed, amplitude;
    
    FledgeAudioProcessor& audioProcessor;
};



class EnvelopeDisplayGraphics : public juce::Component, juce::Timer
{
public:
    EnvelopeDisplayGraphics(FledgeAudioProcessor &p, int index) : audioProcessor(p)
    {
        this->index = index;
        startTimerHz(60);
    }

    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() * 0.9f;
        float height = bounds.getHeight() * 0.9f;
        float widthMargin = bounds.getWidth() * 0.05f;
        float heightMargin = bounds.getHeight() * 0.05f;
        
        calculateSegment();

        // adjusted foreground
        drawSegment(g, x + widthMargin, y + heightMargin, width, height);
        
        for (int i = 0; i < 5; i++)
        {
            pointsGlobalAdjusted[i].drawHandles(g);
        }
    }
    
    void resized() override
    {
        calculateSegment();
    }
    
    float calculateScaledPercentage(float segment, float total)
    {
        if (total == 0.0f)
            return 0.0f; // avoid divide-by-zero

        float proportion = (segment / total);
        float scale = 80.0f * 0.01f;

        return proportion * scale;
    }

    void calculateSegment()
    {
        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() * 0.9f;
        float height = bounds.getHeight() * 0.9f;
        float widthMargin = bounds.getWidth() * 0.05f;
        float heightMargin = bounds.getHeight() * 0.05f;

        pointsGlobalAdjusted[0].coords = { x + widthMargin,
            y + height + heightMargin }; // Bottom (0)
        
        pointsGlobalAdjusted[1].coords = { x + widthMargin + width * attackPct,
            y + heightMargin }; // Top (1.0)
        
        pointsGlobalAdjusted[2].coords = { x + widthMargin + width * (attackPct + decayPct),
            y + heightMargin + height * (1.0f - sustain) }; // Sustain level
        
        pointsGlobalAdjusted[3].coords = { x + widthMargin + width * (attackPct + decayPct + sustainPct),
            y + heightMargin + height * (1.0f - sustain) }; // Same sustain level
        
        pointsGlobalAdjusted[4].coords = { x + widthMargin + width * (attackPct + decayPct + sustainPct + releasePct),
            y + height + heightMargin };
        repaint();
    }
    
  
    void setEnvelope(float attack, float decay, float sustain, float release, float attackAdj, float decayAdj, float sustainAdj, float releaseAdj)
    {
        this->attack = attack;
        this->decay = decay;
        this->sustain = sustain / 100.0f;
        this->release = release;
        
        float attackAdjusted = attack * std::pow(2.0f, attackAdj / 100.0f);
        float decayAdjusted = decay * std::pow(2.0f, decayAdj / 100.0f);
        float releaseAdjusted = release * std::pow(2.0f, releaseAdj / 100.0f);
        sustainLevelAdjusted = (sustain / 100.0) * std::pow(2.0f, sustainAdj / 100.0f);
        sustainLevelAdjusted = juce::jlimit(0.0f, 1.0f, sustainLevelAdjusted);
        
        // Sustain always takes 25% of width
        sustainPct = 0.25f;
        
        // Calculate A+D+R proportions for remaining 75%
        float adrSum = attack + decay + release;
        float adrAdjSum = attackAdjusted * decayAdjusted * releaseAdjusted;

        if (adrSum > 0.0f) {
            attackPct  = (attack / adrSum) * 0.75f;
            decayPct   = (decay / adrSum) * 0.75f;
            releasePct = (release / adrSum) * 0.75f;
            
        } else {
            attackPct  = 0.25f;
            decayPct   = 0.25f;
            releasePct = 0.25f;
        }
        
        if (adrAdjSum > 0.0f){
            attackAdjPct = (attackAdjusted / adrAdjSum) * 0.75f;
            decayAdjPct = (decayAdjusted / adrAdjSum) * 0.75f;
            releaseAdjPct = (releaseAdjusted / adrAdjSum) * 0.75f;
            
        } else {
            attackAdjPct = 0.25f;
            decayAdjPct = 0.25f;
            releaseAdjPct = 0.25f;
        }
        calculateSegment();
    }
    
    void drawSegment(juce::Graphics &g, float x, float y, float width, float height)
    {
        auto points = pointsGlobalAdjusted;
        
        juce::Path envelopePath;
        envelopePath.startNewSubPath(points[0].coords);
        envelopePath.lineTo(points[1].coords);
        envelopePath.cubicTo(points[1].coords.x, points[2].coords.y,
                             points[1].coords.x + width * decayPct * 0.5f, points[2].coords.y,
                             points[2].coords.x, points[2].coords.y);
        envelopePath.lineTo(points[3].coords);
        envelopePath.cubicTo(points[3].coords.x, points[4].coords.y,
                             points[3].coords.x + width * releasePct * 0.5f, points[4].coords.y,
                             points[4].coords.x, points[4].coords.y);
        
        g.setColour(Colors::mainColors[index].withAlpha((float)0.15f));
        g.fillPath(envelopePath);
        
        g.setColour(Colors::mainColors[index]);
        juce::PathStrokeType strokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(envelopePath, strokeType);
        
        
    }

    void timerCallback() override
    {
        auto mouse = getMouseXYRelative().toFloat();
        for (int i = 1; i < 5; i++) // ignore init segment
        {
            pointsGlobalAdjusted[i].isMouseOver = pointsGlobalAdjusted[i].isOver(mouse);
        }
    }
    
    void mouseDown(const juce::MouseEvent &m) override
    {
        auto mouse = m.getPosition().toFloat();
        for (int i = 1; i < 5; i++) // ignore init segment
        {
            if (pointsGlobalAdjusted[i].isOver(mouse))
            {
                dragIndex = i;
                dragStartPoint = mouse;
                
                auto segmentParameterID = (getSegmentParamID(*dragIndex));
                initialParamValue = audioProcessor.apvts.getParameter(segmentParameterID)->getValue();
            }
        }
    }
    
    void mouseDrag(const juce::MouseEvent &m) override
    {
        auto mousePoint = m.getPosition().toFloat();
        float sensitivity = 0.01f;

        if (dragIndex.has_value())
        {
            float deltaX = mousePoint.x - dragStartPoint.x;
            float newValueX = juce::jlimit(0.0f, 1.0f, initialParamValue + (deltaX * sensitivity));

            float deltaY = mousePoint.y - dragStartPoint.y;
            float newValueY = juce::jlimit(0.0f, 1.0f, initialParamValue + (-deltaY * sensitivity));

            if (*dragIndex == 1) {
                setEnvelopeParam(1, newValueX);

            } else if (*dragIndex == 2){
                setEnvelopeParam(2, newValueX);

            } else if (*dragIndex == 3){
                setEnvelopeParam(3, newValueY);

            } else if (*dragIndex == 4){
                setEnvelopeParam(4, newValueX);
            }
            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent &m) override
    {
        dragIndex.reset();
    }
    
    juce::String getSegmentParamID(int segmentDragged)
    {
        juce::String segmentParameterID;
        if (segmentDragged == 1){
            segmentParameterID = "attack" + juce::String(index);
            
        } else if (segmentDragged == 2){
            segmentParameterID = "decay" + juce::String(index);
            
        } else if (segmentDragged == 3){
            segmentParameterID = "sustain" + juce::String(index);
        
        } else if (segmentDragged == 4){
            segmentParameterID = "release" + juce::String(index);

        }
    
        return segmentParameterID;
    }
    
    void setEnvelopeParam(int segmentDragged, float adjustAmount)
    {
        auto segmentParameterID = getSegmentParamID(segmentDragged);
        auto paramRange = audioProcessor.apvts.getParameterRange(segmentParameterID);
        
        audioProcessor.apvts.getParameter(segmentParameterID)->setValueNotifyingHost(adjustAmount);
        
    }

    
private:
    int index;
    float attackPct, decayPct, releasePct, sustainPct;
    float attack, decay, sustain, release;
    float attackAdjPct, decayAdjPct, releaseAdjPct;
    float sustainLevelAdjusted;
    
    juce::Point<float> dragStartPoint;
    float initialParamValue;
    
    struct Handle
    {
        juce::Point<float> coords;
        bool isMouseOver = false;
        bool xyAdjust;
        
        bool isOver(juce::Point<float>& m)
        {
            juce::Rectangle point(coords.x - 6.0f, coords.y - 6.0f, 12.0f, 12.0f);
            return (point.contains(m));
        }
        
        void drawHandles(juce::Graphics &g)
        {
            juce::Path handlePath;
            float size = isMouseOver ? 8.0f : 6.0f;
            juce::Colour color = isMouseOver ? juce::Colour(200, 200, 200) : juce::Colour(150, 150, 150);

            handlePath.addRoundedRectangle(coords.x - size/2, coords.y - size/2, size, size, size/4);
            g.setColour(color);
            g.strokePath(handlePath, juce::PathStrokeType(1.0f));
        }
    };
    
    std::array<Handle, 5> pointsNoAdjusted;
    std::array<Handle, 5> pointsGlobalAdjusted;

    std::optional<int> dragIndex;
    FledgeAudioProcessor& audioProcessor;
};


class WaveformDisplayGraphics : public juce::Component, juce::Timer
{
public:
    WaveformDisplayGraphics()
    {
        calculateEnvelopeSegments();
        
        for (int i = 0; i < 4; i++) {
            for (int k = 0; k < 72; k++) {
                ampSmooth[i][k].reset(5);
            }
        }
        
        startTimerHz(60);
    }
    
    void paint(juce::Graphics &g) override
    {
        auto bounds = getLocalBounds().toFloat();
        float twopi = juce::MathConstants<float>::twoPi;
        float x = bounds.getX();
        float y = bounds.getY();
        float height = bounds.getHeight();
        float graphicWidth = bounds.getWidth() * 0.9f;
        float graphicHeight = bounds.getHeight() * 0.85f;
        float widthMargin = bounds.getWidth() * 0.05f;
        float heightMargin = bounds.getHeight() * 0.075f;

        float phaseScale = domainResolution/twopi;

        juce::Path graphicLines;
        float widthIncrement = graphicWidth/domainResolution;
        float heightIncrement = graphicHeight/envelopeSegments;

        for (int j = 0; j < envelopeSegments; j++){
            int k = envelopeSegments - j;
            float amp3 = ampSmooth[3][k].getNextValue();
            float amp2 = ampSmooth[2][k].getNextValue();
            float amp1 = ampSmooth[1][k].getNextValue();
            float amp0 = ampSmooth[0][k].getNextValue();
            
            float heightScaled = y + heightMargin + heightIncrement * j;
            
            float sin0Phase = fmodf(((0/phaseScale) * op[0].ratio * 2.0f), twopi);
            float sin = amp0 * fastSin.sin(sin0Phase);

            graphicLines.startNewSubPath(x + widthMargin, (heightScaled + height/envelopeSegments) + sin * height * 0.005f);
            for (int i = 1; i <= domainResolution; i++)
            {
                float op3Sin = fastSin.sin(fmodf(((i/phaseScale) * op[3].ratio)
                                                 + (op[3].phase * twopi)
                                                 + ((op0Phase * op[3].gain[0])
                                                 + (op1Phase * op[3].gain[1])
                                                 + (op2Phase * op[3].gain[2])
                                                 + (op3Phase * op[3].gain[3])) * 8.0f, twopi));

                op3Phase = amp3 * op[3].amplitude * op3Sin;

                float op2Sin = fastSin.sin(fmodf(((i/phaseScale) * op[2].ratio)
                                                 + (op[2].phase * twopi)
                                                 + ((op0Phase * op[2].gain[0])
                                                 + (op1Phase * op[2].gain[1])
                                                 + (op2Phase * op[2].gain[2])
                                                 + (op3Phase * op[2].gain[3])) * 8.0f, twopi));

                op2Phase = amp2 * op[2].amplitude * op2Sin;

                float op1Sin = fastSin.sin(fmodf(((i/phaseScale) * op[1].ratio)
                                                 + (op[1].phase * twopi)
                                                 + ((op0Phase * op[1].gain[0])
                                                 + (op1Phase * op[1].gain[1])
                                                 + (op2Phase * op[1].gain[2])
                                                 + (op3Phase * op[1].gain[3])) * 8.0f, twopi));

                op1Phase = amp1 * op[1].amplitude * op1Sin;

                float op0Sin = fastSin.sin(fmodf(((i/phaseScale) * op[0].ratio)
                                                 + (op[0].phase * twopi)
                                                 + ((op0Phase * op[0].gain[0])
                                                 + (op1Phase * op[0].gain[1])
                                                 + (op2Phase * op[0].gain[2])
                                                 + (op3Phase * op[0].gain[3])) * 8.0f, twopi));
                                                 
                op0Phase = amp0 * op[0].amplitude * op0Sin;

                float outputPhase = ((op0Phase * outputGain[0])
                            + (op1Phase * outputGain[1])
                            + (op2Phase * outputGain[2])
                            + (op3Phase * outputGain[3]));

                graphicLines.lineTo(x + widthMargin + widthIncrement * i,
                                    (heightScaled + height/envelopeSegments) + outputPhase * height * 0.05f);
             
            }
        }
        
        graphicLines = graphicLines.createPathWithRoundedCorners(20.0f);
        g.setColour(Colors::mainColors[0]);
        juce::PathStrokeType strokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.strokePath(graphicLines, strokeType);
    }
    
    void setGainCoefficients(int index, int gainIndex, int outputGainIndex)
    {
        outputGain = toBinary4(outputGainIndex);

        switch(index){
            case 0:
                op[0].gain = toBinary4(gainIndex);
                break;
            case 1:
                op[1].gain = toBinary4(gainIndex);
                break;
            case 2:
                op[2].gain = toBinary4(gainIndex);
                break;
            case 3:
                op[3].gain = toBinary4(gainIndex);
                break;
        }
        repaint();
    }
    
    inline std::array<float, 4> toBinary4(int input)
   {
       std::array<float, 4> bits;
       for (int i = 0; i < 4; ++i)
           bits[i] = (input >> i) & 1;
       return bits;
   }

    void resized() override {};
    
    void setEnvelope(int index, float attack, float decay, float sustain, float release)
    {
        op[index].attack = attack;
        op[index].decay = decay;
        op[index].sustain = sustain/100.0f;
        op[index].release = release;
        calculateEnvelopeSegments();
        calculateAmplutude();
        repaint();
    }
    
    void setFMParameter(int index, float ratio, float fixed, bool isRatio, float amplitude, float phase)
    {
        op[index].ratio = ratio;
        op[index].fixed = fixed;
        op[index].amplitude = amplitude/100.0f;
        op[index].isRatio = isRatio;
        op[index].phase = phase/100.0f;
        calculateAmplutude();
        repaint();
    }

    void calculateEnvelopeSegments()
    {
        for (int index = 0; index < 4; index++)
        {
            float attackDecayTime = (op[index].attack + op[index].decay);
            op[index].attackSegment = (op[index].attack/attackDecayTime) * 36;
            op[index].decaySegment = (op[index].decay/attackDecayTime) * 36;
            
            float releaseClamped = juce::jlimit(0.0f, 20.0f, op[index].release);
            op[index].releaseSegment = std::pow(releaseClamped/20.0f, 0.5f) * 24;
        }
    }
    
    void calculateAmplutude()
    {
        for (int j = 0; j < envelopeSegments; j++){
            int k = envelopeSegments - j;
            ampSmooth[3][k].setTargetValue(op[3].generateAmplitude(k));
            ampSmooth[2][k].setTargetValue(op[2].generateAmplitude(k));
            ampSmooth[1][k].setTargetValue(op[1].generateAmplitude(k));
            ampSmooth[0][k].setTargetValue(op[0].generateAmplitude(k));
        }
    }
    
    void timerCallback() override
    {
        repaint();
    }

private:
    int domainResolution = 128;
    int envelopeSegments = 72;
    float op0Phase, op1Phase, op2Phase, op3Phase;
    std::vector<float> segmentAmplitude;
    std::array<std::array<juce::SmoothedValue<float>, 72>, 4> ampSmooth;
    juce::dsp::FastMathApproximations fastSin;
    struct operatorValues
    {
        float ratio, fixed, amplitude, phase;
        bool isRatio;
        float attack = 3000.0f, decay = 1000.0f, sustain = 1.0f, release = 5000.0f;
        float attackSegment, decaySegment, sustainSegment = 12, releaseSegment;
        std::array<float, 4> gain = { 0.0f, 0.0f, 0.0f, 0.0f };

        float generateAmplitude(float segmentIndex)
        {
            float amplitude = 0.0f;
            
            if (attackSegment > 0 && segmentIndex <= attackSegment) // attack portion
            {
                amplitude = (1.0 / attackSegment) * segmentIndex;
                
            } else if (segmentIndex > attackSegment && segmentIndex <= attackSegment + decaySegment)
            {
                int decayIndex = segmentIndex - attackSegment;
                float decayProgress = decayIndex / decaySegment;
                amplitude = 1.0f + decayProgress * (sustain - 1.0f);
                
            } else if (segmentIndex > attackSegment + decaySegment && segmentIndex <= attackSegment + decaySegment + sustainSegment) {
                amplitude = sustain;

            } else if (segmentIndex > attackSegment + decaySegment + sustainSegment && segmentIndex <= attackSegment + decaySegment + sustainSegment + releaseSegment)
                // release portion
            {
                int releaseIndex = segmentIndex - (attackSegment + decaySegment + sustainSegment);
                float releaseProgress = releaseIndex / releaseSegment;
                amplitude = sustain * (1.0f - releaseProgress);

            } else {
                amplitude = 0.0f;
            }
            return amplitude;
        }
    };
    
    std::array<float, 4> outputGain = { 1.0f, 0.0f, 0.0f, 0.0f };
    std::array<operatorValues, 4> op;
};
