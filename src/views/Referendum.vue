<template>
  <div class="home">
    <el-container>
      <el-main style="padding-left:50px;padding-right:50px">
        <div class="clear-float">
          <h1 class="title" style="float:left">Your Opinion Matters</h1>
          <div style="float:right">
            <el-input
              clearable
              v-model="searchText"
              @blur="searchBy = searchText"
              style="width: 200px"
            >
              <i @click="searchBy = searchText" slot="suffix" class="el-input__icon el-icon-search"></i>
            </el-input>
            <label>
              Filter
            </label>
            <el-select  v-model="filterBy" multiple collapse-tags placeholder="Filter">
              <el-option
                v-for="item in filterOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value">
              </el-option>
            </el-select>
          </div>
        </div>
        <el-tabs v-model="activeTab">
          <!-- <el-tab-pane label="Search" name="search"></el-tab-pane> -->
          <!-- 我的提案 -->
          <el-tab-pane label="My proposals" name="proposals">
            <div class="card" style="margin: 10px 25px">
              <div v-if="scatter">
                <div v-if="!scatter.identity">
                  <p>A Scatter account is required</p>
                  <div class="button" @click="getIdentity">Pair Scatter</div>
                </div>
                <div class="proposal-table" v-else>
                  <p class="account-name">Accout: {{scatter.identity.accounts[0].name}}
                    <span style="margin: 0 10px" class="button" @click="forgetIdentity">Remove Identity</span>
                    <router-link :to="{path: '/create_proposal'}">
                      <span class="button">Create Proposal</span>
                    </router-link>
                  </p>
                  <el-table  :data="myProposals" empty-text="No records found" :default-sort="{prop:'proposal_name', order:'ascending'}">
                    <el-table-column sortable label="Proposal" prop="proposal_name"></el-table-column>
                    <el-table-column sortable label="Created" prop="created_at"></el-table-column>
                    <el-table-column sortable label="Expire" prop="expires_at"></el-table-column>
                    <el-table-column>
                      <template slot-scope="scope">
                        <el-button v-if="!isExpired(scope.row.expires_at)" type="danger" @click="expireProp(scope.row.proposal_name)">Expire</el-button>
                        <label v-else>Expired</label>
                      </template>
                    </el-table-column>
                  </el-table>
                </div>
              </div>
              <div v-else>
                <p>Scatter is required!</p>
                <a target="blank" href="https://get-scatter.com/">
                 <div class="button">Get Scatter</div>
                </a>
              </div>
            </div>
          </el-tab-pane>
          <!-- 我的投票 -->
          <el-tab-pane label="My votes" name="votes">
            <div class="card" style="margin: 10px 25px">
              <div v-if="scatter">
                <div v-if="!scatter.identity">
                  <p>A Scatter account is required</p>
                  <div class="button" @click="getIdentity">Pair Scatter</div>
                </div>
                <div class="proposal-table" v-else>
                  <p class="account-name">Accout: {{scatter.identity.accounts[0].name}} <span style="margin: 0 10px" class="button" @click="forgetIdentity">Remove Identity</span></p>
                  <el-table :data="myVotes" empty-text="No records found" :default-sort="{prop:'proposal_name', order:'ascending'}">
                    <el-table-column sortable label="Proposal" prop="proposal_name"></el-table-column>
                    <el-table-column sortable label="Result" prop="result"></el-table-column>
                    <el-table-column sortable label="Voted" prop="updated_at"></el-table-column>
                  </el-table>
                </div>
              </div>
              <div v-else>
                <p>Scatter is required!</p>
                <a target="blank" href="https://get-scatter.com/">
                 <div class="button">Get Scatter</div>
                </a>
              </div>
            </div>
          </el-tab-pane>
        </el-tabs>
        <div class="clear-float">
            <h1 class="title" style="float:left">Discover Polls</h1>
            <div style="float:right">
              sort results by
              <el-select v-model="sortBy">
                <el-option
                  v-for="item in sortOptions"
                  :key="item.value"
                  :label="item.label"
                  :value="item.value"
                ></el-option>
              </el-select>
            </div>
          </div>
        <div class="prop-list" v-loading="!proposals">
          <div
            style="cursor: pointer"
            v-for="(prop, index) in propList"
            :key="index"
            @click="turnDetail(prop)"
          >
            <PropCard
              :type="prop.proposal.proposal_json.type || 'unknown'"
              :title="prop.proposal.title"
              :desc="prop.proposal.proposal_json.content || ''"
              :votes="prop.stats.votes"
              :staked="prop.stats.staked.total"
              :style="{margin: '25px'}"></PropCard>
          </div>
        </div>
      </el-main>
      <el-footer>
      </el-footer>
    </el-container>
  </div>
</template>

<script>
// @ is an alias to /src
import { MessageBox } from 'element-ui'
import Eos from 'eosjs'
import { NETWORK } from '@/assets/constants.js'
import PropCard from '@/components/PropCard.vue'
export default {
  name: 'Referendum',
  components: {
    PropCard
  },
  data () {
    return {
      activeTab: 'proposals',
      sortOptions: [
        {
          value: 'MostVoted',
          label: 'Most Voted'
        },
        {
          value: 'NewestFirst',
          label: 'Newest First'
        },
        {
          value: 'OldestFirst',
          label: 'Oldest First'
        },
        {
          value: 'ExpiresFirst',
          label: 'Expires First'
        },
        {
          value: 'ExpiresLast',
          label: 'Expires Last'
        }
      ],
      filterOptions: [
        {
          value: 'poll',
          label: 'Poll'
        },
        {
          value: 'referendum',
          label: 'Referendum'
        },
        {
          value: 'approved',
          label: 'Approved'
        },
        {
          value: 'rejected',
          label: 'Rejected'
        },
        {
          value: 'ongoing',
          label: 'Ongoing'
        }
      ],
      filterBy: ['poll', 'referendum', 'approved', 'rejected', 'ongoing'],
      sortBy: 'MostVoted',
      searchText: '',
      searchBy: ''
    }
  },
  async created () {
    await this.getProposals()
  },
  computed: {
    scatter () {
      return this.$store.state.scatter
    },
    eos () {
      if (this.scatter && this.scatter.identity) {
        const eosOptions = { expireInSeconds: 60 }
        const eos = this.scatter.eos(NETWORK, Eos, eosOptions)
        return eos
      }
      return null
    },
    proposals () {
      return this.$store.state.proposals
    },
    propList () {
      let propList = []
      if (this.proposals) {
        Object.keys(this.proposals).forEach(key => {
          let flags = Array(5).fill(false)
          this.filterBy.forEach(filter => {
            if (filter === 'poll') { // 暂时把不是referendum的认为是poll
              if (this.proposals[key].proposal.proposal_json.type && this.proposals[key].proposal.proposal_json.type.search('referendum') === -1) {
                flags[0] = true
              }
            }
            if (filter === 'referendum') {
              if (this.proposals[key].proposal.proposal_json.type && this.proposals[key].proposal.proposal_json.type.search('referendum') !== -1) {
                flags[1] = true
              }
            }
          })
          for (let i = 0; i < flags.length; i++) {
            if (flags[i]) {
              if (this.searchBy === '') {
                propList.push(this.proposals[key])
              } else { // 根据关键字搜索
                let regexp = new RegExp(this.searchBy, 'i')
                if (regexp.test(this.proposals[key].id) ||
                  regexp.test(this.proposals[key].proposal.title) ||
                  (this.proposals[key].proposal.proposal_json.content && regexp.test(this.proposals[key].proposal.proposal_json.content))) {
                  propList.push(this.proposals[key])
                }
              }
              break
            }
          }
        })
      }
      if (this.sortBy === 'MostVoted') {
        propList.sort((a, b) => {
          return b.stats.staked.total - a.stats.staked.total
        })
      } else if (this.sortBy === 'NewestFirst') {
        propList.sort((a, b) => {
          let aDate = new Date(a.proposal.created_at).getTime()
          let bDate = new Date(b.proposal.created_at).getTime()
          return bDate - aDate
        })
      } else if (this.sortBy === 'OldestFirst') {
        propList.sort((a, b) => {
          let aDate = new Date(a.proposal.created_at).getTime()
          let bDate = new Date(b.proposal.created_at).getTime()
          return aDate - bDate
        })
      } else if (this.sortBy === 'ExpiresFirst') {
        propList.sort((a, b) => {
          let aDate = new Date(a.proposal.expires_at).getTime()
          let bDate = new Date(b.proposal.expires_at).getTime()
          return aDate - bDate
        })
      } else if (this.sortBy === 'ExpiresLast') {
        propList.sort((a, b) => {
          let aDate = new Date(a.proposal.expires_at).getTime()
          let bDate = new Date(b.proposal.expires_at).getTime()
          return bDate - aDate
        })
      }
      return propList
    },
    myVotes () {
      let myVotes = null
      if (this.$store.state.accounts && this.scatter && this.scatter.identity) {
        myVotes = this.$store.state.accounts[this.scatter.identity.accounts[0].name]
        if (myVotes) {
          let list = []
          Object.keys(myVotes.votes).forEach(key => {
            let vote = { ...myVotes.votes[key] }
            if (vote.vote === 1) {
              vote.result = 'YES'
            } else if (vote.vote === 0) {
              vote.result = 'NO'
            } else {
              vote.result = 'ABSTAIN'
            }
            list.push(vote)
          })
          return list
        }
      }
      return []
    },
    myProposals () {
      let myProposals = []
      if (this.proposals && this.scatter && this.scatter.identity) {
        Object.keys(this.proposals).forEach(key => {
          if (this.proposals[key].proposal.proposer === this.scatter.identity.accounts[0].name) {
            myProposals.push(this.proposals[key].proposal)
          }
        })
      }
      return myProposals
    }
  },
  methods: {
    expireProp (proposal) {
      const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      const transactionOptions = {
        actions: [{
          account: 'bosforumdapp',
          name: 'expire',
          authorization: [{
            actor: account.name,
            permission: account.authority
          }],
          data: { proposal_name: proposal }
        }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
        .then(res => {
          MessageBox.alert(`Expired ${proposal}`, '', {
            confirmButtonText: 'OK'
          })
        }).catch(e => {
          MessageBox.alert(e, 'ERROR', {
            confirmButtonText: 'OK'
          })
        })
    },
    isExpired (exporiesAt) {
      let now = new Date().getTime() + (new Date().getTimezoneOffset() * 60 * 1000)
      let expiry = new Date(exporiesAt).getTime()
      if (expiry < now) {
        return true
      }
      return false
    },
    getProposals () {
      this.$store.dispatch('getProposals')
    },
    getIdentity () { // scatter认证
      const requiredFields = {
        accounts: [ NETWORK ]
      }
      this.scatter.getIdentity(requiredFields).then(() => {
        // console.log(this.scatter.identity)
        this.$store.dispatch('setScatter', { scatter: this.scatter })
      })
    },
    turnDetail (prop) {
      if (window.localStorage) {
        localStorage.setItem('proposalName', prop.proposal.proposal_name)
      }
      this.$store.dispatch('setCurrentProposal', { proposal: prop })
      this.$router.push({ path: '/poll_detail?proposal=' + prop.proposal.proposal_name })
    },
    forgetIdentity () {
      this.scatter.forgetIdentity()
    }
  }
}
</script>

<style lang="stylus" scoped>
.title
  font-family PingFangSC-Semibold
  font-size 20px
  color #507DFE
  letter-spacing 0
  text-align center
  margin-top 0
  margin-bottom 22px
  text-align left
.prop-list
  min-height 100px
  display flex
  flex-wrap wrap
  justify-content flex-start
.card
  font-family: Roboto-Regular
  padding 22px 34px
  background: #FCFDFF;
  box-shadow: 0 2px 4px 0 #B0D9FF;
  border-radius: 8px;
  margin-bottom 22px
.button
  display inline-block
  background: #527FFF
  cursor pointer
  height 32px
  padding 5px 10px
  box-sizing border-box
  border-radius 32px
  font-family: Roboto-Bold;
  font-size: 16px;
  line-height 22px
  color: #FFFFFF;
  letter-spacing: 0;
  text-align: center;
.account-name
    font-family Roboto-Bold
    text-align left
    font-size 20px
    color #8290aa
</style>
