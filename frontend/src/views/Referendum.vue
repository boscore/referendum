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
              class="search-input"
            >
              <i @click="searchBy = searchText" slot="suffix" class="el-input__icon el-icon-search"></i>
            </el-input>

          </div>
        </div>
        <el-tabs v-model="activeTab">
          <!-- 我的提案 -->
          <el-tab-pane label="My proposals" name="proposals">
            <div v-loading="actionLoading" class="card" style="margin: 10px 0px">
              <div v-if="scatter">
                <div v-if="!scatter.identity">
                  <p>A Scatter account is required</p>
                  <div class="button" @click="getIdentity">Pair Scatter</div>
                </div>
                <div class="proposal-table" v-else>
                  <p class="account-name">Account: {{scatter.identity.accounts[0].name}}
                    <span v-if="$store.state.isPC" class="button" @click="forgetIdentity">Remove Identity</span>
                    <router-link v-if="$store.state.isPC" :to="{path: '/create_proposal'}">
                      <span class="button">Create Proposal</span>
                    </router-link>
                  </p>
                  <div style="overflow: auto">
                  <el-table style="min-width: 600px"  :data="myProposals" empty-text="No records found" :default-sort="{prop:'proposal_name', order:'ascending'}">
                    <el-table-column sortable label="Proposal" prop="proposal_name"></el-table-column>
                    <el-table-column sortable label="Created" >
                       <template slot-scope="scope">
                         <span>{{$util.dateConvert(scope.row.created_at)}}</span>
                       </template>
                    </el-table-column>
                    <!-- <el-table-column sortable label="Expire">
                      <template slot-scope="scope">
                         <span>{{$util.dateConvert(scope.row.expires_at)}}</span>
                       </template>
                    </el-table-column> -->
                    <!-- <el-table-column>
                      <template slot-scope="scope">
                        <el-button v-if="!isExpired(scope.row.expires_at)" type="danger" @click="expireProp(scope.row.proposal_name)">Expire</el-button>
                        <label v-else>Expired</label>
                      </template>
                    </el-table-column> -->
                    <el-table-column>
                      <template slot-scope="scope">
                        <el-dropdown trigger="click">
                          <span>
                            <i style="font-size: 20px" class="el-icon-setting"></i>
                          </span>
                          <el-dropdown-menu slot="dropdown">
                            <!-- <el-dropdown-item type="primary" @click="console.log('extend')">
                              <p  @click="openPicker(scope.row.proposal_name)">Extend</p>
                            </el-dropdown-item> -->
                            <el-dropdown-item type="primary" >
                              <p @click="cancelProp(scope.row.proposal_name)">Cancel </p>
                            </el-dropdown-item>
                            <el-dropdown-item v-if="scope.row.shouldReview" type="primary" @click="applyReview(scope.row.proposal_name)">
                              Apply for Review
                            </el-dropdown-item>
                            <el-dropdown-item v-if="scope.row.approved_by_BET" type="primary" @click="claimRewards()">Claim Rewards</el-dropdown-item>
                          </el-dropdown-menu>
                        </el-dropdown>
                        <!-- <label v-else>Expired</label> -->
                      </template>
                    </el-table-column>
                  </el-table>
                  </div>
                </div>
              </div>
              <div v-else>
                <p>Scatter is required!</p>
                <a target="blank" href="https://get-scatter.com/">
                 <div class="button">Get Scatter</div>
                </a>
              </div>
            </div>
            <!-- <mt-datetime-picker
              ref="picker"
              type="datetime"
              :startDate="new Date()"
              cancelText="Cancel"
              confirmText="Extend"
              @confirm="setExtendTime"
            ></mt-datetime-picker>
            <el-dialog
              title="Extend"
              v-loading="actionLoading"
              :visible.sync="extendVisible"
              width="30%"
            >
              <label>New expiry(UTC):
                <el-date-picker
                  type="datetime"
                  value-format="yyyy-MM-ddThh:mm:ss"
                  placeholder="extend date"
                  v-model="extendTime">
                </el-date-picker>
              </label>
              <span slot="footer" class="dialog-footer">
                <el-button @click="extendVisible = false">Cancel</el-button>
                <el-button type="primary" @click="() => {
                  extendProp()
                  extendVisible = false
                  }">Confirm</el-button>
              </span>
            </el-dialog> -->
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
                  <p class="account-name">Account: {{scatter.identity.accounts[0].name}}
                     <span v-if="$store.state.isPC" style="margin: 0 10px" class="button" @click="forgetIdentity">Remove Identity</span></p>
                  <el-table :data="myVotes" empty-text="No records found" :default-sort="{prop:'proposal_name', order:'ascending'}">
                    <el-table-column sortable label="Proposal" prop="proposal_name"></el-table-column>
                    <el-table-column sortable label="Result" prop="result"></el-table-column>
                    <el-table-column sortable label="Voted">
                      <template slot-scope="scope">
                         <span>{{$util.dateConvert(scope.row.updated_at)}}</span>
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
          <el-tab-pane v-if="$store.state.isPC" label="How to vote" name="tutorial">
            <div class="card tutorial">
              <p>1. Set Scatter networks</p>
              <img src="@/assets/images/tutorial-1.png"/>
              <p>2. Add your key</p>
              <img src="@/assets/images/tutorial-2.png"/>
              <p>3. Then link your account</p>
              <img src="@/assets/images/tutorial-3.png"/>
              <p>4. Login and send your vote to a proposal </p>
              <img src="@/assets/images/tutorial-4.png"/>
            </div>
          </el-tab-pane>
        </el-tabs>
        <div class="clear-float">
            <h1 class="title" style="float:left">Discover Polls</h1>
            <div style="float:right">
              <el-select class="select-button" v-model="sortBy">
                <el-option
                  v-for="item in sortOptions"
                  :key="item.value"
                  :label="item.label"
                  :value="item.value"
                ></el-option>
              </el-select>
              <el-select class="select-button" v-model="filterBy" multiple collapse-tags placeholder="Filter">
                <el-option
                  v-for="item in filterOptions"
                  :key="item.value"
                  :label="item.label"
                  :value="item.value">
                </el-option>
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
              :votes="prop.stats.staked"
              :staked="prop.stats.staked.total"
              :expired="false"
              class="prop-card"></PropCard>
          </div>
        </div>
      </el-main>
    </el-container>
  </div>
</template>

<script>
// @ is an alias to /src
import { MessageBox as MbMessageBox } from 'mint-ui'
import { MessageBox } from 'element-ui'
import Eos from 'eosjs'
import { NETWORK, API_URL, EOSFORUM } from '@/assets/constants.js'
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
      extendTime: null,
      extendPropName: '',
      extendVisible: false,
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
        }
        // {
        //   value: 'ExpiresFirst',
        //   label: 'Expires First'
        // },
        // {
        //   value: 'ExpiresLast',
        //   label: 'Expires Last'
        // }
      ],
      filterOptions: [
        {
          value: 'poll',
          label: 'Poll'
        },
        {
          value: 'referendum',
          label: 'Referendum'
        }
        // {
        //   value: 'approved',
        //   label: 'Approved'
        // },
        // {
        //   value: 'expired',
        //   label: 'Expired'
        // },
        // {
        //   value: 'ongoing',
        //   label: 'Ongoing'
        // }
      ],
      filterBy: ['poll', 'referendum'],
      sortBy: 'MostVoted',
      searchText: '',
      searchBy: ''
    }
  },
  async mounted () {
    await this.$store.dispatch('getProposals')
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
            ongoing: true
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
            proposal.shouldReview = this.proposals[key].approved_by_vote && !this.proposals[key].review
            myProposals.push(proposal)
          }
        })
      }
      return myProposals
    }
  },
  methods: {
    alert (title, msg) {
      if (this.$store.state.isPC) {
        MessageBox.alert(msg, title, {
          confirmButtonText: 'OK'
        })
      } else {
        MbMessageBox.alert(msg, title, {
          confirmButtonText: 'OK'
        })
      }
    },
    applyReview (proposal) {
      fetch(API_URL.API_APPLY_REVIEW)
        .then(res => res.json())
        .then(res => {
          this.alert('Success', 'Apply for review success')
          // Message({
          //   showClose: true,
          //   type: 'success',
          //   message: 'Apply for review success'
          // })
        })
        .catch(e => {
          let error = this.$util.errorFormat(e)
          this.alert('Error', 'Error: ' + error.message)
          // Message({
          //   showClose: true,
          //   type: 'error',
          //   message: 'Error: ' + e.message
          // })
          console.log(e)
        })
    },
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
          this.alert('Success', 'Claim success')
          // Message({
          //   showClose: true,
          //   type: 'success',
          //   message: 'Claim success'
          // })
        })
        .catch(e => {
          let error = this.$util.errorFormat(e)
          this.alert('Error', 'Claim ERROR:' + error.message)
          // Message({
          //   showClose: true,
          //   type: 'error',
          //   message: 'Claim ERROR: ' + e.message
          // })
          console.log(e)
        })
    },
    cancelProp (proposal) {
      this.actionLoading = true
      const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      const transactionOptions = {
        actions: [{
          account: EOSFORUM,
          name: 'cancel',
          authorization: [{
            actor: account.name,
            permission: account.authority
          }],
          data: {
            proposer: account.name,
            proposal_name: proposal
          }
        }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
        .then(res => {
          this.actionLoading = false
          this.alert('Success', `Extend ${this.extendPropName}`)
          // Message({
          //   showClose: true,
          //   type: 'success',
          //   message: `Extend ${this.extendPropName}`
          // })
        }).catch(e => {
          this.actionLoading = false
          let error = this.$util.errorFormat(e)
          this.alert('Error', 'Extend ERROR:' + error.message)
          // Message({
          //   showClose: true,
          //   type: 'error',
          //   message: 'Extend ERROR:' + e.message
          // })
          console.log(e)
          // MessageBox.alert(e, 'ERROR', {
          //   confirmButtonText: 'OK'
          // })
        })
    },
    // expireProp (proposal) {
    //   this.actionLoading = true
    //   const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
    //   const transactionOptions = {
    //     actions: [{
    //       account: 'eosio.forum',
    //       name: 'expire',
    //       authorization: [{
    //         actor: account.name,
    //         permission: account.authority
    //       }],
    //       data: { proposal_name: proposal }
    //     }]
    //   }
    //   this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
    //     .then(res => {
    //       this.actionLoading = false
    //       this.alert('Success', `Expired ${proposal}`)
    //       // Message({
    //       //   showClose: true,
    //       //   type: 'success',
    //       //   message: `Expired ${proposal}`
    //       // })
    //     }).catch(e => {
    //       this.actionLoading = false
    //       let error = this.$util.errorFormat(e)
    //       this.alert('Error', 'Expired ERROR:' + error.message)
    //       // Message({
    //       //   showClose: true,
    //       //   type: 'error',
    //       //   message: 'Expired ERROR:' + e.message
    //       // })
    //       console.log(e)
    //     })
    // },
    openPicker (proposal) {
      this.extendPropName = proposal
      if (this.$store.state.isPC) {
        this.extendVisible = true
      } else {
        this.$refs['picker'].open()
      }
    },
    extendProp () {
      this.actionLoading = true
      const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      const transactionOptions = {
        actions: [{
          account: 'eosforumdapp',
          name: 'extend',
          authorization: [{
            actor: account.name,
            permission: account.authority
          }],
          data: {
            proposer: 'icelandtest3',
            proposal_name: 'icelandtestz',
            expires_at: this.extendTime
          }
        }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
        .then(res => {
          this.actionLoading = false
          this.alert('Success', `Extend ${this.extendPropName}`)
          // Message({
          //   showClose: true,
          //   type: 'success',
          //   message: `Extend ${this.extendPropName}`
          // })
        }).catch(e => {
          this.actionLoading = false
          let error = this.$util.errorFormat(e)
          this.alert('Error', 'Extend ERROR:' + error.message)
          // Message({
          //   showClose: true,
          //   type: 'error',
          //   message: 'Extend ERROR:' + e.message
          // })
          console.log(e)
          // MessageBox.alert(e, 'ERROR', {
          //   confirmButtonText: 'OK'
          // })
        })
    },
    setExtendTime (time) {
      function formatNumber (n) {
        if (n < 10) {
          return '0' + n
        }
        return n
      }
      this.extendTime = `${time.getFullYear()}-${formatNumber(time.getMonth() + 1)}-${formatNumber(time.getDate())}T${formatNumber(time.getHours())}:${formatNumber(time.getMinutes())}:${formatNumber(time.getSeconds())}`
      this.extendProp()
    },
    isExpired (exporiesAt) {
      let now = new Date().getTime() + (new Date().getTimezoneOffset() * 60 * 1000)
      let expiry = new Date(exporiesAt).getTime()
      if (expiry < now) {
        return true
      }
      return false
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
  },
  watch: {
    $route () {
      this.$store.dispatch('getProposals')
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
.prop-card
  margin 25px
.card
  font-size 18px
  color #606266
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
.tutorial
  text-align left
  img
    display block
    width 80%
    margin auto
.search-input
  width 200px
.select-button
  margin 0 5px
@media only screen and (max-width 700px)
  .card
    padding 10px
  .tutorial
    img
      width 100%
  #search-bar
    width 100%
  .search-input
    width 100%
    margin-bottom 10px
  .select-button
    width 50%
    margin 0 0 5px 0
@media only screen and (max-width 450px)
  .prop-card
    margin 25px 0
  .prop-list
    justify-content center
</style>
