# main.py
# This version uses the Gemini 2.0 Flash model as requested.

import functions_framework
import vertexai
from vertexai.generative_models import GenerativeModel, Part
import json
import base64

# --- Configuration ---
GCP_PROJECT_ID = "effect-sequencer"
GCP_REGION = "us-central1"

# Initialize Vertex AI SDK
vertexai.init(project=GCP_PROJECT_ID, location=GCP_REGION)

# Load the Gemini 2.0 Flash model
# This is a modern, fast, and cost-effective model.
model = GenerativeModel("gemini-2.0-flash-001")

@functions_framework.http
def get_audio_parameters(request):
    """
    HTTP Cloud Function to analyze and describe an audio clip.
    """
    headers = {
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'POST, OPTIONS',
        'Access-Control-Allow-Headers': 'Content-Type, x-api-key',
        'Access-Control-Max-Age': '3600'
    }

    if request.method == 'OPTIONS':
        return ('', 204, headers)

    # --- Main logic ---
    try:
        request_json = request.get_json(silent=True)

        if not request_json or not all(k in request_json for k in ['audio_data', 'mime_type']):
            error_msg = "Invalid request. 'audio_data' and 'mime_type' fields are required."
            return (json.dumps({"error": error_msg}), 400, headers)

        base64_audio = request_json['audio_data']
        mime_type = request_json['mime_type']

        audio_bytes = base64.b64decode(base64_audio)

        # --- 1. Construct the Descriptive Prompt ---
        # This prompt now asks the model for a textual description of the audio.
        meta_prompt = """
        You are an expert audio analyst. Listen to the provided audio clip carefully.
        
        Your task is to describe the sonic characteristics of the audio.
        
        Respond with a JSON object containing your analysis. The JSON object should have two keys:
        1. "description": A one-sentence summary of the sound.
        2. "characteristics": A list of three to five key descriptive words or phrases (e.g., "Deep sub bass", "Aggressive mid-range", "Crisp high-end", "Heavily compressed", "Reverberant space").
        
        Only return the raw JSON object. Do not add any other text or explanation.
        """

        # --- 2. Call the Gemini API with Multimodal Input ---
        response = model.generate_content([
            meta_prompt,
            Part.from_data(data=audio_bytes, mime_type=mime_type)
        ])
        
        raw_model_output = response.text
        return (raw_model_output, 200, headers)
        # --- 3. Server-Side JSON Validation (Best Practice) ---
        # try:
        #     parsed_json = json.loads(raw_model_output)
        #     validated_output = json.dumps(parsed_json)
        #     final_headers = headers.copy()
        #     final_headers['Content-Type'] = 'application/json'
        #     return (validated_output, 200, final_headers)
        # except (json.JSONDecodeError, TypeError):
        #     error_message = json.dumps({"error": "Model returned invalid JSON."})
        #     return (error_message, 500, headers)

    except Exception as e:
        error_message = json.dumps({"error": f"An internal error occurred: {str(e)}"})
        # Reset content type for JSON error response
        headers['Content-Type'] = 'application/json'
        return (error_message, 500, headers)

@functions_framework.http
def get_effects_advice(request):
    """
    HTTP Cloud Function to give brief audio-effects advice based on a short prompt.
    """
    headers = {
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'POST, OPTIONS',
        'Access-Control-Allow-Headers': 'Content-Type, x-api-key',
        'Access-Control-Max-Age': '3600'
    }

    if request.method == 'OPTIONS':
        return ('', 204, headers)

    try:
        request_json = request.get_json(silent=True)
        if not request_json or 'prompt' not in request_json:
            return (json.dumps({"error": "Invalid request. 'prompt' is required."}), 400, headers)

        user_prompt = request_json['prompt']

        system_prompt = (
            "You are a concise audio-effects assistant. Provide brief, practical advice on effects "
            "(EQ, compression, reverb, delay, saturation, modulation). Keep responses under 60 words, "
            "use imperative phrasing, and avoid preambles. If the user asks about your identity, purpose, "
            "or capabilities, reply with a single-sentence introduction of your role as an audio-effects assistant."
        )

        response = model.generate_content([
            system_prompt + "\nUser: " + user_prompt + "\nResponse:"])

        advice = response.text.strip() if hasattr(response, 'text') else ''
        # Enforce brevity if model returns longer text
        if len(advice) > 480:
            advice = advice[:477] + '...'

        final_headers = headers.copy()
        final_headers['Content-Type'] = 'application/json'
        return (json.dumps({"advice": advice}), 200, final_headers)

    except Exception as e:
        error_message = json.dumps({"error": f"An internal error occurred: {str(e)}"})
        headers['Content-Type'] = 'application/json'
        return (error_message, 500, headers)