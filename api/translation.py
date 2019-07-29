from google.cloud import translate

def translate_text(text, target='en'):
    translate_client = translate.Client()
    result = translate_client.translate(text, target_language=target)

    return result['translatedText']
