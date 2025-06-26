# main.py
# This version uses the Gemini 2.0 Flash model as requested.

import functions_framework
import vertexai
# We import GenerativeModel for the Gemini family of models
from vertexai.generative_models import GenerativeModel
import json

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
    HTTP Cloud Function to translate a sound description into parameters.
    """
    headers = {
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'POST, OPTIONS',
        'Access-Control-Allow-Headers': 'Content-Type, x-api-key', # Allow the API key header
        'Access-Control-Max-Age': '3600'
    }

    if request.method == 'OPTIONS':
        return ('', 204, headers)

    # --- Main logic ---
    try:
        request_json = request.get_json(silent=True)

        if not request_json or 'prompt' not in request_json:
            return (json.dumps({"error": "Invalid request. 'prompt' field is missing."}), 400, headers)

        user_prompt = request_json['prompt']

        # --- V1 Meta-Prompt ---
        meta_prompt = f"""
        You are a synthesizer programming assistant.
        Your task is to take a user's description of a sound and return a JSON object with parameter values.

        The user wants this sound: "{user_prompt}"

        Respond with a JSON object that has two keys: 'cutoff' (a value from 0 to 100) and 'resonance' (a value from 0 to 100).
        Do not add any other text or explanation. Only return the JSON.
        """

        # Call the Gemini API using the generate_content method
        response = model.generate_content(meta_prompt)
        
        raw_model_output = response.text
        
        final_headers = headers.copy()
        final_headers['Content-Type'] = 'application/json'

        return (raw_model_output, 200, final_headers)

    except Exception as e:
        error_message = json.dumps({"error": f"An internal error occurred: {str(e)}"})
        return (error_message, 500, headers)

