from google.cloud import translate
from google.oauth2 import service_account
import os

def translate_text(text, target='en'):
    credentials_path = os.path.dirname(os.path.realpath(__file__)) + os.sep + 'GOOGLE_APPLICATION_CREDENTIALS.json'
    credentials = service_account.Credentials.from_service_account_file(credentials_path)
    translate_client = translate.Client(credentials=credentials)
    result = translate_client.translate(text, target_language=target)

    return result['translatedText']
