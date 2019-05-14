<template>
  <div class="home">
    <el-container>
      <el-main>
        <div class="clear-float">
          <h1 class="title" style="float:left">Your Opinion Matters</h1>
          <div id="search-bar">
            <el-input
              clearable
              v-model="searchText"
              @blur="searchBy = searchText"
              style="width: 200px"
            >
              <i @click="searchBy = searchText" slot="suffix" class="el-input__icon el-icon-search"></i>
            </el-input>
            <label>
              Filters:
            <el-select  v-model="filterBy" multiple collapse-tags placeholder="Filter">
              <el-option
                v-for="item in filterOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value">
              </el-option>
            </el-select>
            </label>
          </div>
        </div>
        <el-tabs v-model="activeTab">
          <!-- <el-tab-pane label="Search" name="search"></el-tab-pane> -->
          <!-- 我的提案 -->
          <el-tab-pane label="My proposals" name="proposals">
            <div v-loading="actionLoading" class="card" style="margin: 10px 0px">
              <div v-if="scatter">
                <div v-if="!scatter.identity">
                  <p>A Scatter account is required</p>
                  <div class="button" @click="getIdentity">Pair Scatter</div>
                </div>
                <div class="proposal-table" v-else>
                  <p class="account-name">Accout: {{scatter.identity.accounts[0].name}}
                    <span class="button" @click="forgetIdentity">Remove Identity</span>
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
                    <el-table-column>
                      <template slot-scope="scope">
                        <el-button v-if="scope.row.approved_by_BET" type="primary" @click="claimRewards()">Claim Rewards</el-button>
                        <!-- <label v-else>Expired</label> -->
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
            <div class="card" style="margin: 10px 0px">
              <div v-if="scatter">
                <div v-if="!scatter.identity">
                  <p>A Scatter account is required</p>
                  <div class="button" @click="getIdentity">Pair Scatter</div>
                </div>
                <div class="proposal-table" v-else>
                  <p class="account-name">Account: {{scatter.identity.accounts[0].name}} <span style="margin: 0 10px" class="button" @click="forgetIdentity">Remove Identity</span></p>
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
          <!-- 提案流程 -->
          <el-tab-pane label="Proposal process" name="process">
            <div class="card">
              <h2>Fund payment process points</h2>
              <ul id="fund-process">
                <li>
                  The community chooses suitable proposals for fund support by referendum.
                </li>
                <li>
                  The amount of a single proposal incentives cannot exceed 1 Million BOS. Generally, after the deadline expires, the proposal incentives are automatically paid.
                </li>
                <li>
                  10 days before the deadline, if the BOS Executive Team believes that the task has not been effectively executed, it has the right to temporarily freeze the payment of funds and submit the payment proposal to BP vote. BOSCore Executive Team checkpoints:
                  <ul style="list-style-type:lower-alpha">
                    <li>
                      Testcase coverage, whether the test passed?
                    </li>
                    <li>
                      Whether the coding quality is up to standard?
                    </li>
                    <li>
                      Whether the specified functions in the proposal are fully implemented?
                    </li>
                    <li>
                      Is there a security bug?
                    </li>
                  </ul>
                </li>
                <li>
                  If the fund payment is frozen, the BOS independent auditor needs to publish the review within 7 days, make independent judgment in the contract, and issue a link to the relevant investigation report.
                </li>
                <li>
                  Within two weeks after the auditor’s comments, BP needs to make a decision. If more than 2/3+1 active BPs agree to continue to pay the incentives, it will continue to pay 90% of the incentives; Otherwise the payment will be rejected.
                </li>
                <li>
                  The 10% of the incentives for the proposal are equally divided between the auditors who agree with the majority of the BPs and the BPs who voted.
                </li>
              </ul>
              <img style="width:100%" src="@/assets/proposal_flow.png" />
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
    </el-container>
  </div>
</template>

<script>
// @ is an alias to /src
import { Message } from 'element-ui'
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
      actionLoading: false,
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
        // {
        //   value: 'approved',
        //   label: 'Approved'
        // },
        {
          value: 'expired',
          label: 'Expired'
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
      // filter approved proposals WIP
      let propList = []
      if (this.proposals) {
        Object.keys(this.proposals).forEach(key => {
          let flags = { // types of proposal
            poll: false,
            referendum: false,
            approved: false,
            expired: false,
            ongoing: false
          }
          this.filterBy.forEach(filter => {
            if (filter === 'poll') { // 暂时把不是referendum的认为是poll
              if (this.proposals[key].proposal.proposal_json.type && this.proposals[key].proposal.proposal_json.type.search('referendum') === -1) {
                flags.poll = true
              }
            }
            if (filter === 'referendum') {
              if (this.proposals[key].proposal.proposal_json.type && this.proposals[key].proposal.proposal_json.type.search('referendum') !== -1) {
                flags.referendum = true
              }
            }
            if (filter === 'expired') {
              if (this.isExpired(this.proposals[key].proposal.expires_at)) {
                flags.expired = true
              }
            }
            if (filter === 'ongoing') {
              if (!this.isExpired(this.proposals[key].proposal.expires_at)) {
                flags.ongoing = true
              }
            }
          })
          let flag = (flags.poll || flags.referendum) && (flags.expired || flags.ongoing)
          if (flag) {
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
            let proposal = { ...this.proposals[key].proposal }
            proposal.approved_by_BET = this.proposals[key].approved_by_BET
            myProposals.push(proposal)
          }
        })
      }
      return myProposals
    }
  },
  methods: {
    claimRewards () {
      const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      const transactionOptions = {
        actions: [{
          account: 'escrow.bos',
          name: 'claim',
          authorization: [{
            actor: account.name,
            permission: account.authority
          }],
          data: { escrow_name: account.name }
        }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
        .then(res => {
          Message({
            showClose: true,
            type: 'success',
            message: 'Claim success'
          })
        })
        .catch(e => {
          Message({
            showClose: true,
            type: 'error',
            message: 'Expired ERROR' + String(e)
          })
        })
    },
    expireProp (proposal) {
      this.actionLoading = true
      const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      const transactionOptions = {
        actions: [{
          account: 'eosio.forum',
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
          this.actionLoading = false
          Message({
            showClose: true,
            type: 'success',
            message: `Expired ${proposal}`
          })
        }).catch(e => {
          this.actionLoading = false
          Message({
            showClose: true,
            type: 'error',
            message: 'Expired ERROR' + String(e)
          })
          // MessageBox.alert(e, 'ERROR', {
          //   confirmButtonText: 'OK'
          // })
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
  font-size 18px
  color #606266
  padding 22px 34px
  background: #FCFDFF;
  box-shadow: 0 2px 4px 0 #B0D9FF;
  border-radius: 8px;
  margin-bottom 22px
.button
  margin 5px
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
#fund-process
  list-style-type:decimal;
  text-align:left
  li
    margin-top: 20px;
    margin-bottom: 20px;
#search-bar
  float right
  display flex
  flex-wrap wrap
  justify-content flex-end

</style>
