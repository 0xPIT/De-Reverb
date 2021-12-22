//
//  SimpleMeter.h
//
//  Copyright © 2019 Eric Tarr. All rights reserved.
//  Code may not be distributed without permission
//
//  Based on Inter-App Audio Plugin (https://docs.juce.com/master/tutorial_plugin_examples.html )
//
//  This class creates a meter component which draws a value between 0 and 1

#pragma once

class SimpleMeter : public Component, private Timer
{
public:
    SimpleMeter()
    {
        startTimerHz(30);
    }
    
    void paint(Graphics& g) override
    {
        auto area = g.getClipBounds();
        auto unfilledHeight = area.getHeight() * (1.0 - level);
        auto unfilledWidth = area.getWidth() * (1.0 - level);

        g.fillAll(Colours::transparentBlack);
        g.setColour(getLookAndFeel().findColour(Slider::thumbColourId));
        g.fillRoundedRectangle(area.toFloat(), 6.0);
        
        switch (configuration) {
            case HORIZONTAL:
            case GAINREDUCTION:
                g.reduceClipRegion(area.getX(), area.getY(), area.getWidth() - (int)unfilledWidth, area.getHeight());
                break;
                
            case VERTICAL:
            default:
                g.reduceClipRegion(area.getX(), area.getY(), area.getWidth(), (int)unfilledHeight);
                break;
        }

        g.setColour(getLookAndFeel().findColour(Slider::trackColourId));
        g.fillRoundedRectangle(area.toFloat(), 6.0);
    }
    
    void resized() override
    {
    }
    
    void update(float newLevel)
    {
        maxLevel = jmin(newLevel, 1.0f);
    }
    
    enum Configuration {
        HORIZONTAL,
        VERTICAL,
        GAINREDUCTION
    };
    
    Configuration configuration = VERTICAL;
    
private:
    void timerCallback() override
    {
        auto callbackLevel = maxLevel.load();
        
        if (callbackLevel > 0.000001) {
            level = callbackLevel;
        }
        else {
            level = 0; // Round down to zero, if the value is very small
        }
        
        repaint();
    }
    
    std::atomic<float> maxLevel { 0.0f };
    float level = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleMeter)
};
