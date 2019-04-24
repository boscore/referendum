export const COLOR = {}
export const NETWORK = {
  blockchain: 'eos',
  protocol: 'https',
  host: 'nodes.get-scatter.com',
  port: 443,
  chainId: 'aca376f206b8fc25a6ed44dbdc66547c36c6c33e3a119ffbeaef943642f0e906'
}

export const API_URL = {
  API_POLL_TALLY: 'https://s3.amazonaws.com/api.eosvotes.io/eosvotes/tallies/latest.json',
  // 'https://s3.amazonaws.com/bostest.referendum/referendum/tallies/latest.json',
  API_GET_ALL_VOTES: 'https://s3.amazonaws.com/api.eosvotes.io/eosio.forum/vote/latest.json',
  // 'https://s3.amazonaws.com/bostest.referendum/bosforumapp/vote/latest.json',
  API_GET_ALL_VOTERS: 'https://s3.amazonaws.com/api.eosvotes.io/eosio/voters/latest.json'
  // 'https://s3.amazonaws.com/bostest.referendum/eosio/voters/latest.json'
}
