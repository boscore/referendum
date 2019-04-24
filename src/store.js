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
    proposals: null
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
    }
  }
})
