# bos-poll

## Project setup
```
npm install
```

### Compiles and hot-reloads for development
```
npm run serve
```

### Compiles and minifies for production
```
npm run build
```

### Run your tests
```
npm run test
```

### Lints and fixes files
```
npm run lint
```

### Run your unit tests
```
npm run test:unit
```

### Network configuration
configurations of this project are for BOS testnent 
You need to set network configuration as BOS mainnet before you deploy
```
// set in src/assets/constants.js
// blockchain network, please set as BOS mainnet information
NETWORK = {
  blockchain: 'eos',
  protocol: 'https',
  host: 'bostest.api.blockgo.vip',
  port: 443,
  chainId: '33cc2426f1b258ef8c798c34c0360b31732ea27a2d7e35a65797850a86d1ba85'
}

// blockchain api node, please set as BOS mainnet node
NODE_ENDPOINT = 'https://bostest.api.blockgo.vip'

// Backend server address, please set as your backend url
BACKEND_URL = ''

// data from tally https://github.com/EOS-Nation/bos-referendum-tally, please change to BOS mainnet tally
API_POLL_TALLY: 'https://s3.amazonaws.com/bostest.referendum/referendum/tallies/latest.json',
API_GET_ALL_VOTES: 'https://s3.amazonaws.com/bostest.referendum/eosio.forum/vote/latest.json',
API_GET_ALL_ACCOUNTS: 'https://s3.amazonaws.com/bostest.referendum/referendum/accounts/latest.json',
API_GET_ALL_PROXIES: 'https://s3.amazonaws.com/bostest.referendum/referendum/proxies/latest.json'

```

### Customize configuration
See [Configuration Reference](https://cli.vuejs.org/config/).
