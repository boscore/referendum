<template>
  <div v-loading="!proposal" class="poll-detail">
    <el-container>
      <el-main class="header">
        <div id="back-button" @click="$router.push({path: '/'})">
          <i class="el-icon-arrow-left"></i>View All Polls
        </div>
        <h1>{{proposal.proposal.title}}
        </h1>
        <h2 v-if="CDToAuditor > 0" style="color: #E74C3C">The countdown to auditors giving opinions ends in {{CDToAuditor}} days</h2>
        <h2 v-if="CDToBP > 0" style="color: #E74C3C">The countdown to BPs voting ends in {{CDToBP}} days</h2>
        <p>
          <span>{{`${proposal.proposal.proposal_name} by ${proposal.proposal.proposer} `}}</span>
          <span style="margin: 0 5px">{{$util.dateConvert(proposal.proposal.expires_at)}} </span>
          <span>{{proposal.proposal.proposal_json.type || 'unknown'}} </span>
        </p>
        <div style="margin-bottom:30px">
          <div
            class="radio-button"
            @click="activeButton = 'desc'"
            :class="{'radio-button-active': activeButton === 'desc'}">
            Description
          </div>
          <div
            v-if="votes.length"
            class="radio-button"
            @click="turnTo('stats')"
            :class="{'radio-button-active': activeButton === 'stats'}">
            Stats
          </div>
          <div
            class="radio-button"
            @click="activeButton = 'voters'"
            :class="{'radio-button-active': activeButton === 'voters'}">
            Voters
          </div>
          <div
            class="radio-button"
            @click="turnTo('comments')"
            :class="{'radio-button-active': activeButton === 'comments'}">
            Comments
          </div>
        </div>
        <div style="overflow:auto;border-radius: 8px;margin-bottom: 22px">
        <el-steps style="background: #fff;min-width:500px;margin-bottom: 0"  class="card" :active="activeStep" simple finish-status="success" process-status="finish">
          <el-step title="Vote"></el-step>
          <el-step title="Develop"></el-step>
          <el-step title="Review"></el-step>
          <el-step title="Finish"></el-step>
        </el-steps>
        </div>
      <el-container v-loading="propLoading">
        <el-main class="main">
          <div v-if="activeButton !=='voters'" v-html="content" ref="desc" class="card prop-content">
          </div>
          <!-- <div class="card">
            <div class="radio-button" :class="{'radio-button-active': true}">English</div>
            <div class="radio-button" :class="{'radio-button-active': false}">中文</div>
          </div> -->
          <div v-else class="card" ref="voters">
            <h2>Voters</h2>
            <el-table :data="showVoters" :default-sort="{prop: 'staked', order: 'descending'}">
              <el-table-column sortable label="Name" prop="voter"></el-table-column>
              <el-table-column sortable label="Votes" prop="staked"></el-table-column>
              <el-table-column sortable label="Type" prop="type"></el-table-column>
              <el-table-column sortable label="Vote" prop="result"></el-table-column>
            </el-table>
            <div v-if="showVotersNum < votes.length">
              <div class="button" style="margin: 20px auto;padding: 5px 20px" @click="showMoreVoters">Load more voters</div>
            </div>
          </div>
          <div
            style="text-align: center; overflow: auto"
            v-if="votes.length"
            v-loading="chartLoading"
            class="card"
            ref="stats">
            <h3>Votes by vote size</h3>
          <div style="min-width:600px;height:500px">

            <IEcharts
              ref="chart"

              :option="chartOption"
            ></IEcharts>
            </div>
          </div>

          <div class="card" ref="comments">
            <h2>Auditor comments {{auditorComm.length}}</h2>
            <Comment v-for="(comment, index) in auditorComm" :key="index" v-bind="comment"></Comment>
          </div>

          <div class="card">
            <h2>BP comments {{BPComm.length}}</h2>
            <Comment v-for="(comment, index) in BPComm" :key="index" v-bind="comment"></Comment>
          </div>

          <div class="card">
            <h2>Other comments {{otherComm.length}}</h2>
            <Comment v-for="(comment, index) in otherComm" :key="index" v-bind="comment"></Comment>
          </div>
        </el-main>
        <el-aside :width="asideWidth">
          <div class="card" id="poll-status">
            <h2>Poll Status</h2>
            <el-progress :stroke-width="10" class="pass-percent" :percentage="agreePercent"></el-progress>
            <el-progress :stroke-width="10" class="dissent-percent" :percentage="rejectPercent"></el-progress>
            <el-progress :stroke-width="10" class="abstain-percent" :percentage="abstainPercent"></el-progress>
            <p>{{$util.toThousands((this.proposal.stats.staked.total / 10000).toFixed(0))}} BOS voted</p>
            <div class="scatter-panel">
              <div v-if="scatter">
                <div v-if="isExpired(proposal.proposal.expires_at)">
                  This proposal is expired
                </div>
                <div v-else-if="!scatter.identity" @click="getIdentity" class="button">
                  Link Scatter to vote
                </div>
                <div v-else>
                  <div style="magin-bottom:10px">
                  <el-radio-group v-model="voteActionParams.vote">
                    <el-radio :label="1">YES</el-radio>
                    <el-radio :label="0">NO</el-radio>
                    <el-radio :label="255">ABSTAIN</el-radio>
                  </el-radio-group>
                  </div>
                  <div v-if="isAuditor || isBP">
                    <p>Please write your opinion</p>
                    <el-input
                      type="textarea"
                      :rows="4"
                      v-model="myComment"
                    ></el-input>
                  </div>
                  <div v-else style="margin:5px 0">
                    <el-checkbox v-model="writeComment">
                      Post a public comment (optional)
                    </el-checkbox>
                    <div v-if="writeComment">
                      <el-input
                        type="textarea"
                        :rows="4"
                        v-model="myComment"
                      ></el-input>
                      <p>Select Yes/No to cast your vote and make your comment public on the EOS blockchain. Your comment and vote will be recorded on-chain for ever, if you want to change your comment please vote again and our algorithm will attempt to just show your latest comment.</p>
                    </div>
                  </div>

                  <div v-if="myVote">You voted {{myVote.result}}</div>
                  <div style="display: flex; justify-content:flex-start;margin-top:10px">
                    <div @click="sendVote" class="button" style="margin-right: 20px;width:80px">
                      Vote
                    </div>
                    <div v-if="myVote" class="button" @click="sendUnvote" style="background: red;margin-right: 20px;width:80px">Unvote</div>
                  </div>
                </div>
              </div>
              <a v-else target="blank" href="https://get-scatter.com/">
                <div class="button">
                  Get Scatter to vote
                </div>
              </a>
            </div>
            <hr style="border: none; border-bottom:2px solid #D8D8D8;" />
            <div>
              <h3 v-if="this.proposal.approved_by_vote">Meet the conditions {{this.proposal.meet_conditions_days}} Days</h3>
              <p>{{this.proposal ? this.proposal.stats.votes.accounts : 0}} accounts</p>
              <p>{{this.proposal ? this.calcDays(this.proposal.proposal.created_at, new Date().toString()) : 0}} days since poll started</p>
              <p>{{(this.proposal.stats.staked.total / 100 / this.proposal.stats.currency_supply).toFixed(2)}}% participation</p>
            </div>
          </div>
          <div class="card">
            <h2>The conditions for the approved proposal</h2>
            <p>1. The votes from token holders is not less than 40% of BP votes from token holders when the proposal was initiated.</p>
            <p>2. The ratio of approved votes/disapproved is greater than 1.5.</p>
            <p>3. The above conditions last for 20 days.</p>
          </div>
          <h2 v-if="relatedPolls.length">
            Related Polls
          </h2>
          <div id="related-polls">
            <div
              v-for="(prop, index) in relatedPolls"
              @click="turnDetail(prop)"
              :key="index"
              style="margin-bottom: 30px; cursor: pointer"
            >
            <PropCard
              :type="prop.proposal.proposal_json.type || 'unknown'"
              :title="prop.proposal.title"
              :desc="prop.proposal.proposal_json.content || ''"
              :votes="prop.stats.votes"
              :staked="prop.stats.staked.total"
              >
            </PropCard>
          </div>
          </div>
        </el-aside>
      </el-container>
      </el-main>
    </el-container>
  </div>
</template>

<script>
import marked from 'marked'
import Eos from 'eosjs'
import { Message } from 'element-ui'
import { NETWORK, API_URL, NODE_ENDPOINT } from '@/assets/constants.js'
import IEcharts from 'vue-echarts-v3/src/lite.js'
import 'echarts/lib/chart/pie'
import 'echarts/lib/component/tooltip'
import 'echarts/lib/component/title'
import 'echarts/lib/component/grid'
import PropCard from '@/components/PropCard.vue'
import Comment from '@/components/Comment.vue'
export default {
  name: 'PollDetail',
  components: {
    PropCard,
    IEcharts,
    Comment
  },
  computed: {
    account () {
      if (this.scatter && this.scatter.identity) {
        return this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      }
      return null
    },
    activeStep () {
      if (this.proposal) {
        if (this.proposal.approved_by_vote) {
          if (this.proposal.finish) {
            // final approved
            return 4
          } else if (this.proposal.review) {
            // voting by BPs
            return 2
          }
          // reviewing
          return 1
        }
      }
      return 0
    },
    asideWidth () {
      if (this.screenWidth < 821) {
        return '100%'
      }
      return '350px'
    },
    chartLoading () {
      if (this.votes) {
        return false
      } else {
        return true
      }
    },
    chartOption () {
      let data = []
      if (this.votes) {
        this.votes.forEach(vote => {
          let label = true
          if (vote.vote !== 1 && vote.vote !== 0) {
            return
          }
          if (data.length > 20) {
            if (this.proposal.stats.staked.total !== 0 && (vote.staked * 10000 / this.proposal.stats.staked.total) > 0.002) {
              return
            }
            label = false
          }
          data.push({
            value: vote.staked,
            name: vote.voter,
            itemStyle: {
              color: (vote.vote === 1) ? 'rgb(97, 169, 19)' : 'rgb(217, 83, 79)'
            },
            label: {
              show: label
            },
            labelLine: {
              show: label
            }
          })
        })
      }
      return {
        // grid: {
        //   left: 200,
        //   right: 100,
        //   top: 1000,
        //   containLabel: true
        // },
        tooltip: {
          trigger: 'item',
          formatter: '{b} : {c} BOS ({d}%)'
        },
        series: [
          {
            type: 'pie',
            radius: '65%',
            center: ['50%', '50%'],
            selectedMode: 'single',
            data: data,
            label: {
              normal: {
                textStyle: {
                  fontSize: 18,
                  color: '#235894'
                }
              }
            },
            itemStyle: {
              normal: {
                borderWidth: 1,
                borderColor: '#fff'
              }
            }
          }
        ]
      }
    },
    votes () { // votes for this proposal
      let allVotes = this.$store.state.votes
      let allAccounts = this.$store.state.accounts
      let allProxies = this.$store.state.proxies
      if (allVotes && allAccounts && allProxies) {
        if (typeof allVotes === 'string') {
          allVotes = JSON.parse(allVotes)
        }
        let votes = []
        allVotes.forEach(vote => {
          if (vote.proposal_name === this.proposalName) {
            if (vote.vote === 1) {
              vote.result = 'YES'
            } else if (vote.vote === 0) {
              vote.result = 'NO'
            } else {
              vote.result = 'ABSTAIN'
            }
            if (allAccounts[vote.voter]) {
              vote.type = 'Voter'
              vote.staked = Number((allAccounts[vote.voter].staked / 10000).toFixed(0))
            } else if (allProxies[vote.voter]) {
              vote.type = 'Proxy'
              vote.staked = Number((allProxies[vote.voter].votes[this.proposalName].staked_proxy / 10000).toFixed(0))
            } else {
              vote.type = 'Voter'
              vote.staked = 0
            }
            // sort votes
            if (votes.length === 0 || vote.staked <= votes[votes.length - 1].staked) {
              votes.push(vote)
            } else {
              for (let i = votes.length - 1; i >= 0; i--) {
                if (vote.staked < votes[i].staked) {
                  votes.splice(i + 1, 0, vote) // 插在该元素后面
                  break
                } else if (i === 0) {
                  votes.splice(0, 0, vote)
                }
              }
            }
          }
        })
        return votes
      } else {
        return []
      }
    },
    auditorComm () {
      let comm = []
      this.votes.forEach(vote => {
        if (vote.vote_json && vote.vote_json.comment) {
          let comment = {
            avatar: '',
            name: vote.voter,
            time: vote.updated_at,
            comment: vote.vote_json.comment
          }
          let isAuditor = this.auditorsList.find(auditor => {
            return Boolean(auditor.auditor_name === vote.voter)
          })
          if (isAuditor) {
            comm.push(comment)
          }
        }
      })
      return comm
    },
    BPComm () {
      let comm = []
      this.votes.forEach(vote => {
        if (vote.vote_json && vote.vote_json.comment) {
          let comment = {
            avatar: '',
            name: vote.voter,
            time: vote.updated_at,
            comment: vote.vote_json.comment
          }
          let isBP = this.producers.find(producer => {
            return Boolean(producer.owner === vote.voter)
          })
          if (isBP) {
            comm.push(comment)
          }
        }
      })
      return comm
    },
    otherComm () {
      let comm = []
      this.votes.forEach(vote => {
        if (vote.vote_json && vote.vote_json.comment) {
          let comment = {
            avatar: '',
            name: vote.voter,
            time: vote.updated_at,
            comment: vote.vote_json.comment
          }
          let isAuditor = this.auditorsList.find(auditor => {
            return Boolean(auditor.auditor_name === vote.voter)
          })
          let isBP = this.producers.find(producer => {
            return Boolean(producer.owner === vote.voter)
          })
          if (!isAuditor && !isBP) {
            comm.push(comment)
          }
        }
      })
      return comm
    },
    showVoters () {
      return this.votes.slice(0, this.showVotersNum)
    },
    myVote () {
      let myVotes = null
      if (this.$store.state.accounts && this.scatter && this.scatter.identity) {
        myVotes = this.$store.state.accounts[this.scatter.identity.accounts[0].name]
        if (myVotes && myVotes.votes[this.proposalName]) {
          let vote = { ...myVotes.votes[this.proposalName] }
          if (vote.vote === 1) {
            vote.result = 'YES'
          } else if (vote.vote === 0) {
            vote.result = 'NO'
          } else {
            vote.result = 'ABSTAIN'
          }
          return vote
        }
      }
      return null
    },
    proposalName () {
      return this.$route.query.proposal || localStorage.getItem('proposalName')
    },
    relatedPolls () {
      let related = []
      const proposals = this.$store.state.proposals
      if (proposals && this.proposal) {
        Object.keys(proposals).forEach(key => {
          if (proposals[key].proposal.proposer === this.proposal.proposal.proposer &&
          proposals[key].proposal.proposal_name !== this.proposal.proposal.proposal_name) {
            if (related.length < 2) {
              if (!this.$util.isExpired(proposals[key].proposal.expires_at)) {
                related.push(proposals[key])
              }
            }
          }
        })
      }
      return related
    },
    content () {
      if (this.proposal) {
        return marked(this.proposal.proposal.proposal_json.content, { sanitize: true })
      } else {
        return 'no content'
      }
    },
    abstainPercent () {
      return Number((100 - this.agreePercent - this.rejectPercent).toFixed(1))
    },
    agreePercent () {
      if (!this.proposal || this.proposal.stats.staked.total === 0 || !this.proposal.stats.staked[1]) {
        return 0
      } else {
        return Number((100 * this.proposal.stats.staked[1] / this.proposal.stats.staked.total).toFixed(1))
      }
    },
    rejectPercent () {
      if (!this.proposal || this.proposal.stats.staked.total === 0 || !this.proposal.stats.staked[0]) {
        return 0
      } else {
        return Number((100 * this.proposal.stats.staked[0] / this.proposal.stats.staked.total).toFixed(1))
      }
    },
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
    isAuditor () {
      return this.auditorsList.find(auditor => {
        return auditor.auditor_name === this.account.name
      })
    },
    isBP () {
      return this.auditorsList.find(auditor => {
        return auditor.auditor_name === this.account.name
      })
    },
    finalApproved () {
      if (this.proposal) {
        if (this.proposal.approved_by_BPs || this.proposal.approved_by_BET) {
          return 1
        } else if (this.proposal.approved_by_BPs_date !== 'None' && !this.approved_by_BPs_date) {
          return 0
        }
      }
      return -1
    }
  },
  data () {
    return {
      title: 'Should EOS tokens sent to eosio.ramfee and eosio.names accounts in the future be allocated to REX?',
      activeButton: 'desc',
      auditorsList: [],
      CDToBP: -1,
      CDToAuditor: -1,
      voteActionParams: {
        voter: '',
        proposal_name: '',
        vote: -1,
        vote_json: ''
      },
      myComment: '',
      producers: [],
      proposal: {
        approved_by_BET: false,
        approved_by_BPs: false,
        approved_by_BPs_date: 'None',
        approved_by_vote: false,
        approved_by_vote_date: 'None',
        id: '',
        meet_conditions_days: 0,
        proposal: {
          expires_at: '',
          created_at: '',
          title: '',
          proposer: '',
          proposal_name: '',
          proposal_json: {
            type: '',
            content: ''
          }
        },
        reviewed_by_BET_date: 'None',
        stats: {
          votes: {},
          accounts: {},
          staked: {
            total: 0
          }
        }
      },
      propLoading: true,
      writeComment: false,
      showVotersNum: 30,
      screenWidth: document.body.clientWidth
    }
  },
  created () {
    this.getProposal()
  },
  mounted () {
    window.addEventListener('resize', () => {
      this.$refs['chart'].resize()
      this.screenWidth = document.body.clientWidth
    })
    this.getProducers()
    this.getAuditors()
  },
  methods: {
    getProposal () {
      fetch(API_URL.API_GET_PROPOSAL + '/' + this.proposalName)
        .then(res => {
          if (res.status !== 200) {
            console.log(res.statusText)
          }
          return res.json()
        })
        .then(res => {
          this.propLoading = false
          this.proposal = res
          if (this.proposal.approved_by_vote && !this.proposal.approved_by_BET && this.proposal.reviewed_by_BET_date && this.proposal.reviewed_by_BET_date !== 'None') {
            const start = new Date(this.proposal.reviewed_by_BET_date).getTime()
            const end = new Date().getTime()
            const d = Math.floor((end - start) / 1000 / 60 / 60 / 24)
            if (d >= 7) {
              this.CDToBP = 14 - d + 7
            } else {
              this.CDToAuditor = 7 - d
            }
          }
          try {
            this.proposal.proposal.proposal_json = JSON.parse(this.proposal.proposal.proposal_json)
          } catch (e) {
            console.log(e)
          }
        }).catch(e => {
          this.propLoading = false
          Message({
            showClose: true,
            message: 'Get Proposal ERROR:' + e.message,
            type: 'error'
          })
          console.log(e)
        })
    },
    getProducers () {
      fetch(API_URL.API_GET_PRODUCERS)
        .then(res => {
          if (res.status !== 200) {
            console.log(res.statusText)
          }

          return res.json()
        })
        .then(res => {
          this.producers = res.producer
        }).catch(e => {
          Message({
            showClose: true,
            message: 'Get Producers ERROR\n' + String(e),
            type: 'error'
          })
          console.log(e)
        // MessageBox.alert(e, 'Get Producers ERROR', {
        //   confirmButtonText: 'OK'
        // })
        })
    },
    getAuditors () {
      // const tableOptions = new FormData()
      // tableOptions.append('scope', 'auditor.bos')
      // tableOptions.append('code', 'auditor.bos')
      // tableOptions.append('table', 'auditors')
      // tableOptions.append('json', true)
      const tableOptions = {
        'scope': 'auditor.bos',
        'code': 'auditor.bos',
        'table': 'auditors',
        'json': true
      }
      fetch(NODE_ENDPOINT + '/v1/chain/get_table_rows', {
        method: 'POST',
        body: JSON.stringify(tableOptions),
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'
        }
      }).then(res => res.json())
        .then(res => {
          this.auditorsList = res.rows
        }).catch(e => {
        // MessageBox.alert(e, 'Get Auditors ERROR', {
        //   confirmButtonText: 'OK'
        // })
          Message({
            showClose: true,
            message: 'Get Auditors ERROR\n' + String(e),
            type: 'error'
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
    getIdentity () {
      const requiredFields = {
        accounts: [ NETWORK ]
      }
      this.scatter.getIdentity(requiredFields).then(() => {
        // console.log(this.scatter.identity)
        this.$store.dispatch('setScatter', { scatter: this.scatter })
      })
    },
    calcDays (start, end) {
      let startDay = new Date(start)
      let endDay = new Date(end)
      return ((endDay.getTime() - startDay.getTime()) / 1000 / 3600 / 24).toFixed(0)
    },
    calcPercent (numerator, denominator) {
      if (!denominator === 0) {
        return 0
      } else {
        return Number((numerator * 100 / denominator).toFixed(2))
      }
    },
    sendVote () {
      if (this.voteActionParams.vote === -1) {
        // MessageBox.alert('Please choose your vote', '', {
        //   confirmButtonText: 'OK'
        // })
        Message({
          showClose: true,
          message: 'Please choose your vote',
          type: 'warning'
        })
      } else if (this.myComment === '' && (this.isAuditor || this.isBP)) {
        Message({
          showClose: true,
          message: 'Please write your opinion of this proposal',
          type: 'warning'
        })
      } else {
        this.voteActionParams.voter = this.account.name
        this.voteActionParams.proposal_name = this.proposalName
        if (this.myComment !== '' && (this.writeComment || this.isAuditor || this.isBP)) {
          this.voteActionParams.vote_json = JSON.stringify({ comment: this.myComment })
        }
        const transactionOptions = {
          actions: [{
            account: 'eosio.forum',
            name: 'vote',
            authorization: [{
              actor: this.account.name,
              permission: this.account.authority
            }],
            data: { ...this.voteActionParams }
          }]
        }
        this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
          .then(res => {
            Message({
              message: `Your vote  has been cast on ${this.proposalName}, data will be updated some time later`,
              type: 'success'
            })
            this.$store.dispatch('getAccounts')
            this.$store.dispatch('getVotes')
            this.$store.dispatch('getProxies')
            // MessageBox.alert(`Your vote has been cast on ${this.proposalName}`, '', {
            //   confirmButtonText: 'OK'
            // })
          }).catch(e => {
            Message({
              showClose: true,
              message: 'Vote ERROR:' + e.message,
              type: 'error'
            })
            console.log(e)
            // MessageBox.alert(JSON.parse(e).error.name, 'ERROR', {
            //   confirmButtonText: 'OK'
            // })
          })
      }
    },
    sendUnvote () {
      const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      const actionParams = {
        voter: account.name,
        proposal_name: this.proposalName
      }
      const transactionOptions = {
        actions: [{
          account: 'eosio.forum',
          name: 'unvote',
          authorization: [{
            actor: account.name,
            permission: account.authority
          }],
          data: { ...actionParams }
        }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
        .then(res => {
          Message({
            message: `Your unvote on ${this.proposalName} was successful, data will be updated some time later`,
            type: 'success'
          })
          this.$store.dispatch('getAccounts')
          this.$store.dispatch('getVotes')
          this.$store.dispatch('getProxies')
          // MessageBox.alert(`Your unvote on ${this.proposalName} was successful, data will be updated some time later`, '', {
          //   confirmButtonText: 'OK'
          // })
        }).catch(e => {
          Message({
            showClose: true,
            message: 'Unvote ERROR: ' + e.message,
            type: 'error'
          })
          console.log(e)
          // MessageBox.alert(JSON.parse(e).error.name, 'ERROR', {
          //   confirmButtonText: 'OK'
          // })
        })
    },
    turnTo (target) {
      // 跳转到某个card
      this.activeButton = target
      this.$refs[target].scrollIntoView()
    },
    turnDetail (prop) {
      // 进入另一个prop
      if (window.localStorage) {
        localStorage.setItem('proposalName', prop.proposal.proposal_name)
      }
      this.$store.dispatch('setCurrentProposal', { proposal: prop })
      this.propLoading = true

      this.$router.push({ path: '/poll_detail', query: { proposal: prop.proposal.proposal_name } })
      this.getProposal()
    },
    showMoreVoters () {
      this.showVotersNum += 30
      if (this.showVotersNum > this.votes.length) {
        this.showVotersNum = this.votes.length
      }
    }
  },
  watch: {
    $route () {
      this.getProposal()
      this.$store.dispatch('getAccounts')
      this.$store.dispatch('getVotes')
      this.$store.dispatch('getProxies')
    }
  }
}
</script>

<style lang="stylus">
@media only screen and (max-width 840px)
  .poll-detail
    .el-container
      flex-wrap wrap
      // flex-direction column-reverse
    .el-aside
      width 100%
#poll-status
  .el-progress__text
    font-family Roboto-Bold
    font-size 11px
    letter-spacing 0
    text-align center
  .pass-percent
    .el-progress__text
      color #30D094
    .el-progress-bar__inner
      background-image linear-gradient(270deg, #41B976 0%, #2CD69B 100%)
  .dissent-percent
    .el-progress__text
      color #F46666
    .el-progress-bar__inner
      background-image linear-gradient(269deg, #F06262 0%, #FF7171 100%)
  .abstain-percent
    .el-progress__text
      color #F4D03F
    .el-progress-bar__inner
      background-image linear-gradient(270deg, #F7DC6F 0%, #F1C40F 100%)

</style>

<style lang="stylus" scoped>
.poll-detail
  background-color rgb(232,236,255)
  padding 20px 0
  text-align left
  color: #507DFE;
.header
  h1
    font-family: Roboto-Medium;
    font-size: 30px;
    letter-spacing: 0;
  h2
    font-family: Roboto-Medium;
    font-size: 26px;
  p
    font-family: Roboto-Regular;
    font-size: 18px;
    color: #8A8A8A;
    letter-spacing: 0;
.main
  font-family: Roboto-Regular;
  font-size: 18px;
  color: #8A8A8A;
  letter-spacing: 0;
  padding-left 0
  padding-top 0
  h1, h2, h3, h4, h5
    font-family: Roboto-Medium;
    color: #507DFE;
.prop-content
  overflow-wrap break-word
  >>> h2
    color: #507DFE;
  >>> p
    font-family: Roboto-Regular;
    font-size: 18px;
    color: #8A8A8A;
    letter-spacing: 0;
  >>> img
    max-width 100%
#back-button
  cursor pointer
  width 160px
  height 32px
  line-height 32px
  background: #507DFE;
  border-radius: 15.5px;
  font-family: PingFangSC-Medium;
  font-size: 18px;
  color: #FFFFFF;
  letter-spacing: 0;
  text-align: center;
.card
  padding 22px 34px
  background: #FCFDFF;
  box-shadow: 0 2px 4px 0 #B0D9FF;
  border-radius: 8px;
  margin-bottom 22px
.radio-button
  height 24px
  min-width 103px
  line-height 24px
  margin-right 10px
  display inline-block
  cursor pointer
  opacity: 0.5;
  background: #7599FF;
  border-radius: 13.29px;
  font-family: PingFangSC-Medium;
  font-size: 13.71px;
  color: #FFFFFF;
  letter-spacing: 0;
  text-align: center;
.radio-button-active
  opacity 1
.pie-chart
  width:800px
  height:500px
  margin:auto
#related-polls
  display flex
  flex-wrap wrap
  justify-content center
@media only screen and (max-width 840px)
  .main
    padding 0
  .radio-button
    margin-bottom 5px

</style>
