export const COLOR = {}
export const NETWORK = {
  blockchain: 'eos',
  protocol: 'https',
  host: 'bostest.api.blockgo.vip',
  port: 443,
  chainId: '33cc2426f1b258ef8c798c34c0360b31732ea27a2d7e35a65797850a86d1ba85'
}
export const NODE_ENDPOINT = 'https://bostest.api.blockgo.vip'
// export const NETWORK = {
//   blockchain: 'eos',
//   protocol: 'https',
//   host: 'nodes.get-scatter.com',
//   port: 443,
//   chainId: 'aca376f206b8fc25a6ed44dbdc66547c36c6c33e3a119ffbeaef943642f0e906'
// }
const BACKEND_URL = `https://boswps.eosplay.me`
export const API_URL = {
  API_GET_PRODUCERS: BACKEND_URL + '/getBPs',
  API_GET_ALL_PROPOSALS: BACKEND_URL + '/getAllProposals',
  API_GET_PROPOSAL: BACKEND_URL + '/getProposal',
  API_POLL_TALLY: 'https://s3.amazonaws.com/bostest.referendum/referendum/tallies/latest.json',
  API_GET_ALL_VOTES: 'https://s3.amazonaws.com/bostest.referendum/eosio.forum/vote/latest.json',
  API_GET_ALL_ACCOUNTS: 'https://s3.amazonaws.com/bostest.referendum/referendum/accounts/latest.json',
  API_GET_ALL_PROXIES: 'https://s3.amazonaws.com/bostest.referendum/referendum/proxies/latest.json'
  // API_GET_ALL_ACCOUNTS: 'https://s3.amazonaws.com/api.eosvotes.io/eosvotes/accounts/latest.json',
  // API_GET_ALL_VOTES: 'https://s3.amazonaws.com/api.eosvotes.io/eosio.forum/vote/latest.json',
  // API_POLL_TALLY: 'https://s3.amazonaws.com/api.eosvotes.io/eosvotes/tallies/latest.json',
  // API_GET_ALL_PROXIES: 'https://s3.amazonaws.com/api.eosvotes.io/eosvotes/proxies/latest.json'
}
