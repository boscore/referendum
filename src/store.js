import Vue from 'vue'
import Vuex from 'vuex'
import axios from 'axios'
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
    proxies: null
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
    }
  },
  actions: {
    setScatter ({ commit }, payload) {
      commit('setScatter', { scatter: payload.scatter })
    },
    setCurrentProposal ({ commit }, payload) {
      commit('setCurrentProposal', { proposal: payload.proposal })
    },
    async getProposals ({ commit, dispatch }, payload) {
      // axios.defaults.headers.common['Orign'] = 'https://s3.amazonaws.com'
      const res = await axios.get(API_URL.API_POLL_TALLY)
      if (res.status === 200) {
        Object.keys(res.data).forEach(key => {
          try {
            if (res.data[key].proposal.proposal_json) {
              res.data[key].proposal.proposal_json = JSON.parse(res.data[key].proposal.proposal_json)
            } else {
              res.data[key].proposal.proposal_json = {
                type: '',
                content: ''
              }
            }
          } catch (e) {
            console.log(e)
          }
        })
        if (payload && payload.hasOwnProperty('proposalName')) {
          dispatch('setCurrentProposal', { proposal: res.data[payload.proposalName] })
        }
        commit('setProposals', { proposals: res.data })
      }
    },
    async getAccounts ({ commit, dispatch }, payload) {
      const res = await axios.get(API_URL.API_GET_ALL_ACCOUNTS)
      if (res.status === 200) {
        commit('setAccounts', { accounts: res.data })
        localStorage.setItem('accounts', JSON.stringify(res.data))
      }
    },
    async getProxies ({ commit, dispatch }, payload) {
      const res = await axios.get(API_URL.API_GET_ALL_PROXIES)
      if (res.status === 200) {
        commit('setProxies', { proxies: res.data })
        localStorage.setItem('proxies', JSON.stringify(res.data))
      }
    },
    async getVotes ({ commit, dispatch }, payload) {
      const res = await axios.get(API_URL.API_GET_ALL_VOTES)
      if (res.status === 200) {
        res.data.forEach(vote => {
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
        localStorage.setItem('votes', JSON.stringify(res.data))
        commit('setVotes', { votes: res.data })
      }
    }
  }
})
