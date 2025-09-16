#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), distortionControls(p.apvts), reverbControls(p.apvts), mainControls(p.apvts)
{
    // Layout constants
    constexpr int moduleMinWidth = 380;
    constexpr int moduleMinHeight = 160;
    constexpr int mainMinHeight = 110;
    constexpr int margin = 10;
    constexpr int swapRowHeight = 30;

    addAndMakeVisible(distortionControls);
    addAndMakeVisible(reverbControls);
    addAndMakeVisible(mainControls);
    addAndMakeVisible(swapButton);
    swapAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.apvts, "chainOrderSwap", swapButton);
    swapButton.onClick = [this]() { updateOrderUI(); };
    addAndMakeVisible(chatComponent);
    setResizable(true, true);
    if (auto* constrainer = getConstrainer())
    {
        const int minW = 2 * margin + 3 * moduleMinWidth + 12; // small padding between columns
        const int minH = 2 * margin + swapRowHeight + moduleMinHeight + mainMinHeight + 60;
        constrainer->setSizeLimits(minW, minH, 4000, 3000);
    }
    setSize(1000, 600);


}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);
    // Draw signal flow arrow between the two effect modules
    bool swap = false;
    if (auto* proc = dynamic_cast<AudioPluginAudioProcessor*>(getAudioProcessor()))
    {
        if (auto* p = proc->apvts.getRawParameterValue("chainOrderSwap"))
            swap = p->load() > 0.5f;
    }

    auto distBounds = distortionControls.getBounds();
    auto revBounds = reverbControls.getBounds();
    auto mainBounds = mainControls.getBounds();

    // Compute start/end on the horizontal centers at the facing edges
    juce::Point<float> start, end;
    if (!swap)
    {
        start = juce::Point<float>((float)distBounds.getRight(), distBounds.getCentreY());
        end   = juce::Point<float>((float)revBounds.getX(),    revBounds.getCentreY());
    }
    else
    {
        start = juce::Point<float>((float)revBounds.getRight(), revBounds.getCentreY());
        end   = juce::Point<float>((float)distBounds.getX(),    distBounds.getCentreY());
    }

    // Only draw if there is space between modules
    if (end.x > start.x + 6.0f || start.x > end.x + 6.0f)
    {
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawLine({ start, end }, 2.0f);

        // Arrowhead at end
        const float dx = end.x - start.x;
        const float dy = end.y - start.y;
        const float len = std::sqrt(dx * dx + dy * dy);
        if (len > 0.001f)
        {
            const float ux = dx / len;
            const float uy = dy / len;
            const float headLen = 12.0f;
            const float headWidth = 8.0f;
            juce::Point<float> tip = end;
            juce::Point<float> base = { end.x - ux * headLen, end.y - uy * headLen };
            // Perpendicular
            const float px = -uy;
            const float py = ux;
            juce::Point<float> left  = { base.x + px * (headWidth * 0.5f), base.y + py * (headWidth * 0.5f) };
            juce::Point<float> right = { base.x - px * (headWidth * 0.5f), base.y - py * (headWidth * 0.5f) };
            juce::Path head;
            head.addTriangle(tip, left, right);
            g.fillPath(head);
        }
    }

    // Draw arrow from downstream module to MainControls (wet path terminus)
    juce::Point<float> mstart, mend;
    if (!swap)
    {
        // Dist -> Rev -> Main
        mstart = { (float)revBounds.getRight(), revBounds.getCentreY() };
    }
    else
    {
        // Rev -> Dist -> Main
        mstart = { (float)distBounds.getRight(), distBounds.getCentreY() };
    }
    mend = { (float)mainBounds.getX(), mainBounds.getCentreY() };

    if (mend.x > mstart.x + 6.0f)
    {
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawLine({ mstart, mend }, 2.0f);

        const float dx = mend.x - mstart.x;
        const float dy = mend.y - mstart.y;
        const float len = std::sqrt(dx * dx + dy * dy);
        if (len > 0.001f)
        {
            const float ux = dx / len;
            const float uy = dy / len;
            const float headLen = 12.0f;
            const float headWidth = 8.0f;
            juce::Point<float> tip = mend;
            juce::Point<float> base = { mend.x - ux * headLen, mend.y - uy * headLen };
            const float px = -uy;
            const float py = ux;
            juce::Point<float> left  = { base.x + px * (headWidth * 0.5f), base.y + py * (headWidth * 0.5f) };
            juce::Point<float> right = { base.x - px * (headWidth * 0.5f), base.y - py * (headWidth * 0.5f) };
            juce::Path head;
            head.addTriangle(tip, left, right);
            g.fillPath(head);
        }
    }
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // const int sliderWidth = 80;
    // const int sliderHeight = 100;
    // const int margin = 30;

    constexpr int moduleMinHeight = 160;
    auto area = getLocalBounds().reduced(10);
    const int topRowHeight = 240; // fixed to avoid empty space growth
    auto topRow = area.removeFromTop(topRowHeight);
    swapButton.setBounds(topRow.removeFromTop(30).removeFromLeft(320).reduced(4));

    bool swap = false;
    if (auto* proc = dynamic_cast<AudioPluginAudioProcessor*>(getAudioProcessor()))
    {
        if (auto* p = proc->apvts.getRawParameterValue("chainOrderSwap"))
            swap = p->load() > 0.5f;
    }

    // Three columns with explicit gaps: left, middle; right is compact square from right edge
    auto contentRow = topRow; // remaining after swapButton
    const int columnGap = 16;
    int square = juce::jmin(contentRow.getHeight() - 8, contentRow.getWidth() / 3);
    auto right = contentRow.removeFromRight(square).reduced(4);
    // insert gap before right edge content
    contentRow.removeFromRight(columnGap);
    // split remaining for left/middle with a visible gap
    int halfWidth = (contentRow.getWidth() - columnGap) / 2;
    auto left = contentRow.removeFromLeft(halfWidth).reduced(4);
    contentRow.removeFromLeft(columnGap);
    auto middle = contentRow.reduced(4);

    if (!swap)
    {
        distortionControls.setBounds(left);
        reverbControls.setBounds(middle);
    }
    else
    {
        reverbControls.setBounds(left);
        distortionControls.setBounds(middle);
    }
    mainControls.setBounds(right);
    // Let chat occupy all remaining space and its input stick to the bottom
    chatComponent.setBounds(area);

}

void AudioPluginAudioProcessorEditor::updateOrderUI()
{
    swapButton.setButtonText("Click to swap");
    resized();
    repaint();
}
