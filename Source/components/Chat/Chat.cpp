#include "Chat.h"

Chat::Chat()
{
    transcriptViewport.setViewedComponent(&transcriptContent, false);
    addAndMakeVisible(transcriptViewport);
    addMessage("Chat started. Type a message and click Send.", false);

    chatInput.setMultiLine(false);
    chatInput.setTextToShowWhenEmpty("Type message...", juce::Colours::grey);
    chatInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    chatInput.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(chatInput);

    submitButton.setButtonText("Send");
    submitButton.onClick = [this] { handleSubmit(); };
    addAndMakeVisible(submitButton);

    loadingLabel.setText("", juce::dontSendNotification);
    loadingLabel.setJustificationType(juce::Justification::centredLeft);
    loadingLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(loadingLabel);
}

void Chat::handleSubmit()
{
    const juce::String userText = chatInput.getText().trim();

    if (userText.isNotEmpty())
    {
        addMessage(userText, true);

        // Hook for external response logic
        sendRequest(userText);
        chatInput.clear();
    }
}

void Chat::resized() {
    auto area = getLocalBounds().reduced(10);
    // Input row height fixed; transcript takes the rest and grows when window grows
    const int inputRowHeight = 60;
    transcriptViewport.setBounds(area.removeFromTop(juce::jmax(80, getHeight() - inputRowHeight - 20)));
    
    loadingLabel.setBounds(area.removeFromTop(20));
    auto buttonArea = area.removeFromRight(80); // Space for submit button
    submitButton.setBounds(buttonArea);
    
    chatInput.setBounds(area);
}


void Chat::sendRequest(const juce::String& userMessage)
{
    // 1. Define the API endpoint and get API key from environment
    juce::URL url("https://GATWAY_URL/effects-advice");
    juce::String apiKey = ""; // Replace with your key if necessary

    // 2. Create the JSON request body required by the API
    juce::var requestBodyJson;
    auto* requestObject = new juce::DynamicObject();
    requestObject->setProperty("prompt", userMessage);
    requestBodyJson = requestObject;
    juce::String jsonString = juce::JSON::toString(requestBodyJson);

    setLoading(true);
    // addMessage("Generating...", false);

    // 3. Launch the network request on a separate thread
    juce::Thread::launch([this, url, apiKey, jsonString]() {
        // Use JUCE's HTTP API (suppress deprecation warning until newer API is stable)
        auto postUrl = url.withPOSTData(jsonString);
        
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
        if (auto stream = postUrl.createInputStream(false, nullptr, nullptr, 
            "Content-Type: application/json\r\nx-api-key: " + apiKey + "\r\n", 15000))
        #pragma clang diagnostic pop
        {
            const juce::String response = stream->readEntireStreamAsString();
            
            // Switch back to the main message thread to update the UI
            juce::MessageManager::callAsync([this, response]() {
                handleResponse(response);
            });
        }
        else
        {
            juce::MessageManager::callAsync([this]() {
                handleResponse("{\"error\": \"Could not connect to the API server. Please check your internet connection and API configuration.\"}");
            });
        }
    });
}

void Chat::handleResponse(const juce::String& response)
{
    chatInput.clear(); // Clear input after trying to send

    setLoading(false);

    if (response.isEmpty())
    {
        addMessage("Error - No response from server.", false);
        return;
    }

    // Parse JSON {"advice": "..."} if possible
    juce::var parsed = juce::JSON::parse(response);
    if (parsed.isObject())
    {
        auto* obj = parsed.getDynamicObject();
        if (obj != nullptr && obj->hasProperty("advice"))
        {
            juce::String advice = obj->getProperty("advice").toString();
            addMessage(advice, false);
            return;
        }
    }

    // Fallback to raw response text
    addMessage(response, false);
}

void Chat::setLoading(bool shouldLoad)
{
    isLoading = shouldLoad;
    loadingLabel.setText(shouldLoad ? "Thinking..." : "", juce::dontSendNotification);
    submitButton.setEnabled(!shouldLoad);
}

void Chat::addMessage(const juce::String& text, bool isUser)
{
    // Bubble container
    struct Bubble : public juce::Component
    {
        Bubble(const juce::String& t, bool u) : message(t), isUser(u) {}
        void paint(juce::Graphics& g) override
        {
            auto r = getLocalBounds().toFloat();
            auto bg = (isUser ? juce::Colours::cornflowerblue : juce::Colours::darkslategrey).withAlpha(0.9f);
            g.setColour(bg);
            g.fillRoundedRectangle(r.reduced(2.0f), 10.0f);
            g.setColour(juce::Colours::white.withAlpha(0.12f));
            g.drawRoundedRectangle(r.reduced(2.0f), 10.0f, 1.0f);
        }
        juce::String message;
        bool isUser;
        juce::Label label;
    };

    auto* bubble = new Bubble(text, isUser);
    bubble->setInterceptsMouseClicks(false, false);
    bubble->label.setText(text, juce::dontSendNotification);
    bubble->label.setJustificationType(juce::Justification::centredLeft);
    bubble->label.setColour(juce::Label::textColourId, juce::Colours::white);
    bubble->label.setFont(juce::Font(14.0f));
    bubble->label.setInterceptsMouseClicks(false, false);
    bubble->addAndMakeVisible(bubble->label);

    bubbles.add(bubble);
    transcriptContent.addAndMakeVisible(bubble);

    const int padding = 8;
    const int contentWidth = transcriptViewport.getWidth();
    const int bubbleWidth = juce::jmax(220, contentWidth - 40);
    const int textWidth = bubbleWidth - 20;

    // Simple text height estimate
    int lines = juce::jmax(1, (int)std::ceil(text.length() / 28.0));
    int textH = lines * (int)std::round(bubble->label.getFont().getHeight() + 4.0f);
    int bubbleHeight = textH + 16;

    int y = padding;
    for (auto* c : bubbles)
        if (c != bubble)
            y = juce::jmax(y, c->getBottom() + padding);

    int x = padding;
    if (isUser)
        x = contentWidth - bubbleWidth - padding;

    bubble->setBounds(x, y, bubbleWidth, bubbleHeight);
    bubble->label.setBounds(10, 8, textWidth, textH);

    transcriptContent.setSize(contentWidth, bubble->getBottom() + padding);
    transcriptViewport.setViewPosition(0, juce::jmax(0, transcriptContent.getHeight() - transcriptViewport.getHeight()));
}

