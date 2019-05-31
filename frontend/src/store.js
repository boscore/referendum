import Vue from 'vue'
import Vuex from 'vuex'
import { API_URL } from '@/assets/constants.js'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    language: 'en',
    scatter: null,
    currentProposal: null,
    accounts: null,
    votes: null,
    proposals: null,
    proxies: null,
    screenWidth: -1
  },
  mutations: {
    setScatter (state, payload) {
      state.scatter = payload.scatter
    },
    setCurrentProposal (state, payload) {
      state.currentProposal = payload.proposal
    },
    setProposals (state, payload) {
      state.proposals = payload.proposals
    },
    setAccounts (state, payload) {
      state.accounts = payload.accounts
    },
    setProxies (state, payload) {
      state.proxies = payload.proxies
    },
    setVotes (state, payload) {
      state.votes = payload.votes
    },
    setScreenWidth (state, payload) {
      state.screenWidth = payload.screenWidth
    }
  },
  actions: {
    setScreenWidth ({ commit }, payload) {
      commit('setScreenWidth', { screenWidth: payload.screenWidth })
    },
    setScatter ({ commit }, payload) {
      commit('setScatter', { scatter: payload.scatter })
    },
    setCurrentProposal ({ commit }, payload) {
      commit('setCurrentProposal', { proposal: payload.proposal })
    },
    getProposals ({ commit, dispatch }, payload) {
      // axios.defaults.headers.common['Origin'] = 'https://s3.amazonaws.com'
      fetch(API_URL.API_GET_ALL_PROPOSALS)
        .then(res => {
          if (res.status !== 200) {
            console.log(res.statusText)
          }
          return res.json()
        })
        .then(res => {
          Object.keys(res).forEach(key => {
            try {
              if (res[key].proposal.proposal_json) {
                res[key].proposal.proposal_json = JSON.parse(res[key].proposal.proposal_json)
              } else {
                res[key].proposal.proposal_json = {
                  type: '',
                  content: ''
                }
              }
            } catch (e) {
              console.log('invalid proposal_json')
              res[key].proposal.proposal_json = {
                type: '',
                content: ''
              }
            }
          })
          if (payload && payload.hasOwnProperty('proposalName')) {
            dispatch('setCurrentProposal', { proposal: res[payload.proposalName] })
          }
          commit('setProposals', { proposals: res })
        })
    },
    getAccounts ({ commit, dispatch }, payload) {
      fetch(API_URL.API_GET_ALL_ACCOUNTS)
        .then(res => {
          if (res.status !== 200) {
            console.log(res.statusText)
          }
          return res.json()
        })
        .then(res => {
          commit('setAccounts', { accounts: res })
        })
    },
    getProxies ({ commit, dispatch }, payload) {
      fetch(API_URL.API_GET_ALL_PROXIES)
        .then(res => {
          if (res.status !== 200) {
            console.log(res.statusText)
          }
          return res.json()
        })
        .then(res => {
          commit('setProxies', { proxies: res })
        })
    },
    getVotes ({ commit, dispatch }, payload) {
      fetch(API_URL.API_GET_ALL_VOTES)
        .then(res => {
          if (res.status !== 200) {
            console.log(res.statusText)
          }
          return res.json()
        })
        .then(res => {
          res.forEach(vote => {
            if (vote.vote_json) {
              try {
                vote.vote_json = JSON.parse(vote.vote_json)
              } catch (e) {
                console.log('invalid vote_json')
              }
            } else {
              vote.vote_json = null
            }
          })
          commit('setVotes', { votes: res })
        })
    }
  }
})
