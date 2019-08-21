from google.cloud import translate
from google.oauth2 import service_account
import os
import markdown

def translate_text(text, target='en',title=False):
    text = markdown.markdown(text, output_format='html5', extensions=['extra']) if title == False else text
    credentials_path = os.path.dirname(os.path.realpath(__file__)) + os.sep + 'GOOGLE_APPLICATION_CREDENTIALS.json'
    credentials = service_account.Credentials.from_service_account_file(credentials_path)
    translate_client = translate.Client(
        credentials=credentials)
    result = translate_client.translate(
        text, target_language=target, format_ = 'html')
    # print(result['translatedText'])
    return result['translatedText']


if __name__ == '__main__':
   p = translate_text(
       '## 提案背景：\n \n作为EOS的最强侧链BOSCore凭借IBC跨链转账和3秒LIB的特性以及数字资产自由港的愿景，在行业内已享有不小的知名度。但是BOSCore生态推广长久以来面临几个问题。\n \n')
   print(p)
