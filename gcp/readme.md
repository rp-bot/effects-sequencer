## 1. Local Environment & Project Setup

These commands configure your local machine to interact with your Google Cloud project.

```bash
# Log in to your Google Account
gcloud auth login

# Set your active project (replace your-gcp-project-id)
gcloud config set project your-gcp-project-id

# Enable required Google Cloud services
gcloud services enable cloudfunctions.googleapis.com
gcloud services enable run.googleapis.com
gcloud services enable aiplatform.googleapis.com
gcloud services enable iam.googleapis.com
gcloud services enable cloudbuild.googleapis.com
```

## 2. Source Code

Create a folder on your local machine and add the following two files:

* **main.py**
    Contains the Python code for the Cloud Function. The final working version uses the `gemini-2.0-flash-001` model.

* **requirements.txt**
    Lists the necessary Python libraries:

    ```txt
    functions-framework
    google-cloud-aiplatform
    ```


## 3. Cloud IAM Permissions

Grant the necessary permissions for the Cloud Build service and the Cloud Function. These are one-time setup steps per project.

```bash
# Grant Cloud Build the “Cloud Functions Admin” role
gcloud projects add-iam-policy-binding your-gcp-project-id \
  --member="serviceAccount:PROJECT_NUMBER@cloudbuild.gserviceaccount.com" \
  --role="roles/cloudfunctions.admin"

# Grant Cloud Build the “Service Account User” role
gcloud projects add-iam-policy-binding your-gcp-project-id \
  --member="serviceAccount:PROJECT_NUMBER@cloudbuild.gserviceaccount.com" \
  --role="roles/iam.serviceAccountUser"

# Grant the Cloud Function’s service account the “Vertex AI User” role
gcloud projects add-iam-policy-binding your-gcp-project-id \
  --member="serviceAccount:PROJECT_NUMBER-compute@developer.gserviceaccount.com" \
  --role="roles/aiplatform.user"
```

> **Note:** Replace `your-gcp-project-id` and `PROJECT_NUMBER` with your actual GCP project ID and project number.

## 4. Deployment

Navigate into your source code folder and deploy the function:

```bash
gcloud functions deploy audio-agent-v1 \
  --gen2 \
  --runtime=python311 \
  --region=us-central1 \
  --source=. \
  --entry-point=get_audio_parameters \
  --trigger-http \
  --allow-unauthenticated
```

## 5. Testing

1. **Retrieve the function URL**
   After a successful deployment, note the URL printed in the output.

2. **Test with `curl`**
   Replace `YOUR_FUNCTION_URL` with the actual URL:

   ```bash
   curl -X POST \
     -H "Content-Type: application/json" \
     -d '{"prompt": "a bright, plucky sound"}' \
     YOUR_FUNCTION_URL
   ```

3. **Expected Output**

   ```json
   {
     "cutoff": 85,
     "resonance": 70
   }
   ```
