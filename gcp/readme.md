## 1. Local Environment & Project Setup

These commands configure your local machine to interact with your Google Cloud project.

---

### **Part 1: Initial Project & Function Setup**

#### **1.1: Configure Local Environment**

These commands configure your `gcloud` CLI to work with your project.

```bash
# Log in to your Google Account
gcloud auth login

# Set your active project (replace with your actual Project ID)
gcloud config set project effect-sequencer

# Enable all required Google Cloud services
gcloud services enable cloudfunctions.googleapis.com \
  run.googleapis.com \
  aiplatform.googleapis.com \
  iam.googleapis.com \
  cloudbuild.googleapis.com \
  apigateway.googleapis.com
```

#### **1.2: Set Up Source Code**

Create a new folder and add the following three files.

- **`main.py`** (The Cloud Function code)

  - This file contains the logic for the Cloud Function, using the `gemini-2.0-flash-001` model.

- **`requirements.txt`** (Python dependencies)

  ```text
  functions-framework
  google-cloud-aiplatform
  ```

- **`api-config.yaml`** (API Gateway configuration)
  Refer to the file for more context.
  > **Action:** Before proceeding, you must get the function URL from step 1.4 and paste it into the `address:` field in this file.

#### **1.3: Grant Initial IAM Permissions**

These permissions allow Cloud Build to create the function and the function to call Vertex AI.

```bash
# Get your project number for the following commands
# (Replace PROJECT_NUMBER in the commands below with the output)
gcloud projects describe effect-sequencer --format="value(projectNumber)"

# Grant Cloud Build the ability to build and manage the function
gcloud projects add-iam-policy-binding effect-sequencer \
  --member="serviceAccount:PROJECT_NUMBER@cloudbuild.gserviceaccount.com" \
  --role="roles/cloudfunctions.admin"

gcloud projects add-iam-policy-binding effect-sequencer \
  --member="serviceAccount:PROJECT_NUMBER@cloudbuild.gserviceaccount.com" \
  --role="roles/iam.serviceAccountUser"

# Grant the function's runtime account permission to call the AI model
gcloud projects add-iam-policy-binding effect-sequencer \
  --member="serviceAccount:PROJECT_NUMBER-compute@developer.gserviceaccount.com" \
  --role="roles/aiplatform.user"
```

#### **1.4: Deploy the Cloud Function**

This deploys the function with a temporary public URL so we can add it to the `api-config.yaml`.

```bash
gcloud functions deploy get_audio_parameters \
  --gen2 \
  --runtime=python311 \
  --region=us-central1 \
  --source=. \
  --entry-point=get_audio_parameters \
  --trigger-http \
  --allow-unauthenticated
```

> **Action:** After deployment, copy the `url` from the output and paste it into the `api-config.yaml` file (Step 1.2).

---

### **Part 2: API Gateway and Security Hardening**

#### **2.1: Lock Down the Cloud Function**

Now we secure the function so _only_ the API Gateway can call it.

```bash
# Get your project number if you don't have it
gcloud projects describe effect-sequencer --format="value(projectNumber)"

# Grant the official API Gateway service account the "Cloud Functions Invoker" role
# (Replace PROJECT_NUMBER with your actual project number)
gcloud functions add-invoker-policy-binding get_audio_parameters \
  --member="serviceAccount:service-PROJECT_NUMBER@gcp-sa-apigateway.iam.gserviceaccount.com" \
  --region=us-central1

# Remove public access from the Cloud Function
gcloud functions remove-invoker-policy-binding get_audio_parameters \
  --member="allUsers" \
  --region=us-central1
```

#### **2.2: Deploy the API Gateway**

With the function secured and the config file updated, we deploy the gateway.

```bash
# Create the parent API resource
gcloud api-gateway apis create audio-agent-api --project=effect-sequencer

# Create the initial API config using the updated YAML file
gcloud api-gateway api-configs create v1-config \
  --api=audio-agent-api --openapi-spec=api-config.yaml \
  --project=effect-sequencer

# Create the gateway itself
gcloud api-gateway gateways create audio-agent-gateway \
  --api=audio-agent-api --api-config=v1-config \
  --location=us-central1 --project=effect-sequencer
```

#### **2.3: Enable the Gateway's Managed Service**

This crucial step "switches on" the API key validation service for your gateway.

```bash
# Find the managed service name for your gateway
gcloud api-gateway apis describe audio-agent-api --project=effect-sequencer

# Enable the service (replace MANAGED_SERVICE_NAME with the value from the command above)
gcloud services enable MANAGED_SERVICE_NAME
```

---

### **Part 3: Final Testing**

#### **3.1: Create and Restrict an API Key**

1.  Navigate to the [Credentials page](https://console.cloud.google.com/apis/credentials) in the Google Cloud Console.
2.  Click **"+ CREATE CREDENTIALS"** > **"API key"**.
3.  Copy the new key.
4.  Edit the key and under **"API restrictions"**, restrict it to the **"API Gateway API"**.

#### **3.2: Test the Secure Endpoint**

Use the Gateway's URL and your new API key to test the final, secure endpoint.

```bash
# Get the gateway URL (look for defaultHostname)
gcloud api-gateway gateways describe audio-agent-gateway --location=us-central1

# Test with curl (replace YOUR_GATEWAY_URL and YOUR_API_KEY)
curl -X POST \
  -H "Content-Type: application/json" \
  -H "x-api-key: YOUR_API_KEY" \
  -d '{"prompt": "a warm, gentle pad sound with a slow attack"}' \
  https://YOUR_GATEWAY_URL/get-parameters
```

A successful JSON response confirms the entire system is deployed correctly and securely.

```json
{
  "cutoff": 60,
  "resonance": 20
}
```
