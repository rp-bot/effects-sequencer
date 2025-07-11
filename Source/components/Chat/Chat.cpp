#include "Chat.h"

Chat::Chat()
{
    chatDisplay.setMultiLine(true);
    chatDisplay.setReadOnly(true);
    chatDisplay.setScrollbarsShown(true);
    chatDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey);
    chatDisplay.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(chatDisplay);
    chatDisplay.insertTextAtCaret("Chat started. Type a message and click Send.\n");

    chatInput.setMultiLine(false);
    chatInput.setTextToShowWhenEmpty("Type message...", juce::Colours::grey);
    chatInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    chatInput.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(chatInput);

    submitButton.setButtonText("Send");
    submitButton.onClick = [this] { handleSubmit(); };
    addAndMakeVisible(submitButton);
}

void Chat::handleSubmit()
{
    const juce::String userText = chatInput.getText().trim();

    if (userText.isNotEmpty())
    {
        chatDisplay.moveCaretToEnd();
        chatDisplay.insertTextAtCaret("You: " + userText + "\n");

        // Hook for external response logic
        sendRequest(userText);
        // chatInput.clear();
    }
}

void Chat::resized() {
    auto area = getLocalBounds().reduced(10);
    chatDisplay.setBounds(area.removeFromTop(getHeight() - 60));
    
    auto buttonArea = area.removeFromRight(80); // Space for submit button
    submitButton.setBounds(buttonArea);
    
    chatInput.setBounds(area);
}


void Chat::sendRequest(const juce::String& base64AudioData)
{
    // 1. Define the API endpoint and get API key from environment
    juce::URL url("GATE_WAY_URL");
    juce::String apiKey = "API_KEY"; // Replace with your key if necessary

    // 2. Create the JSON request body
    juce::var requestBodyJson;
    auto* requestObject = new juce::DynamicObject();
    requestObject->setProperty("mime_type", "audio/octet-stream");
    requestObject->setProperty("audio_data", base64AudioData);
    requestBodyJson = requestObject;
    juce::String jsonString = juce::JSON::toString(requestBodyJson);

    chatDisplay.moveCaretToEnd();
    chatDisplay.insertTextAtCaret("Bot: Sending audio data...\n");

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
                handleResponse("Error: Could not connect to the API server. Please check your internet connection and API configuration.");
            });
        }
    });
}

void Chat::handleResponse(const juce::String& response)
{
    chatInput.clear(); // Clear input after trying to send
    chatDisplay.moveCaretToEnd(); // Ensure caret is at the end before new messages

    if (response.isEmpty())
    {
        chatDisplay.insertTextAtCaret("Bot: Error - No response from server.\n");
        return;
    }

    // Simply display the raw response for now
    chatDisplay.insertTextAtCaret("Bot: " + response + "\n");
}

