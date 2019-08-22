from google.cloud import translate
from google.oauth2 import service_account
import os
import markdown

def translate_text(text, target='en',title=False):
    try:
        text = markdown.markdown(text, output_format='html5', extensions=['extra']) if title == False else text
        credentials_path = os.path.dirname(os.path.realpath(__file__)) + os.sep + 'GOOGLE_APPLICATION_CREDENTIALS.json'
        credentials = service_account.Credentials.from_service_account_file(credentials_path)
        translate_client = translate.Client(
            credentials=credentials)
        result = translate_client.translate(
            text, target_language=target, format_ = 'html')
        return result['translatedText']
    except Exception as err:
        print(err)


if __name__ == '__main__':
   p = translate_text("# DecentralBank BOS Proposal: Bitcoin Light Client We propose to build a Bitcoin Light Client directly onto the BOS chain. This will allow BOS smart contracts to listen for Bitcoin payments and trigger actions based on BTC payments trustlessly on-chain. ## Technical Details All the block headers from the Bitcoin chain will be synced to a BOS smart contract. These block headers can be used to produce Merkle proofs that a transaction has been included in a Bitcoin block. When a valid Merkle proof is provided for a transaction, that transaction's details are registered in the smart contract, and other smart contracts will be able to take action based on that payment. ## Milestones & Funding * Sync Block Headers (end of July) * Merkle proof verification for transactions (mid-September) For each phase of development, we are requesting 200,000 BOS in funding (total 400,000 BOS). This funding will go toward bringing an additional developer onto the project. ## Use Cases * Trustlessly listen for a BTC transaction to swap BOS for BTC * Dapps can accept Bitcoin as payment for services. For example, a betting dapp can accept Bitcoin as a payment and run an action when the payment has finalized without relying on a trusted source. ## Team DecentralBank is developing a blockchain for algorithmic stablecoins and is focused on building core infrastructure for EOSIO blockchains. The co-founders are Kedar Iyer and Sam Kazemian, and the team includes Travis Moore, the CTO of Everipedia. Together, these 3 were the developers behind the Everipedia protocol and website. ", target='ja')
   print(p)
