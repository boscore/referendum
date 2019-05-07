<template>
  <div class="auditor">
    <el-container>
      <el-main style="padding-top:0">
        <div class="main-panel">
          <h1>Auditor Board</h1>
          <div class="card board">
            <div v-for="auditor in auditorsList" :key="auditor.cust_name" class="board-item">
              <Avatar :url="cand.url" star></Avatar>
              <p>{{cand.cust_name}}</p>
            </div>
          </div>
          <div class="candidate-list">
            <h1>Candidate List <span style="color: #91ADFF;">- {{candidatesList.length}}</span></h1>
            <!-- <p>The Custodian Board manages the operations and affairs of the DAC, including but not limited to the governance and administration of the assets and liabilities of the DAC. The following DAC members have vested some of their tokens to submit themselves and candidates for a position on the custodian board which last for 7 days. Every 7 days, your votes are recalculated to select who will be part of the next custodian board. Voting is important! Please vote often and stay engaged within the DAC to know who is providing value</p> -->
            <div
              v-for="candidate in candidatesList"
              :key="candidate.candidate_name">
              <CandidateCollapse
                v-if="candidate.is_active"
                @select="handleSelect"
                :id="candidate.candidate_name"
                :votes="candidate.total_votes"
                :inform="candidate.inform"
                :isSelected="candidate.isSelected"
                :staked="candidate.locked_tokens"
                ></CandidateCollapse>
            </div>
          </div>
        </div>
      </el-main>
      <el-aside width="320px">
        <div class="vote-panel">
          <h1>
            My Vote <span style="color: #91ADFF;">{{selectedCandidates.length}}/{{config ? config.maxvotes : 5}}</span>
          </h1>
          <p>
            You can vote for up to 5 auditor candidates at a time. Please select candidates who you think will bring value to the BOS.
          </p>
          <div class="selected-candidates card">
            <div @click="sendVotes" class="vote-button" :class="{'vote-button-active': selectedCandidates.length}">
              {{selectedCandidates.length ? 'SUBMIT MY VOTES' : 'MY VOTES'}}
            </div>
            <div v-for="candidate in selectedCandidates" :key="candidate.id" class="selected-candidate-card">
              <Avatar size="35px" :url="candidate.image"></Avatar>
              <p>{{candidate.candidate_name}}</p>
              <div class="remove-button" @click="removeCandidate(candidate.candidate_name)">
                <i class="el-icon-close"></i>
              </div>
            </div>
          </div>
        </div>
        <div v-if="myCandidate" class="card">
          <h1>You are a candidate</h1>
          <p>Votes: {{(myCandidate.total_votes / 10000).toFixed(4)}}</p>
          <p>Staked: {{myCandidate.locked_tokens}}</p>
          <div v-if="myCandidate.is_active">
            <p>you are active for elections</p>
            <div @click="inactive" class="vote-button vote-button-active">Be inactive</div>
          </div>
          <div v-else >
            <p>you are inactive for elections</p>
            <div v-if="myCandidate.locked_tokens !== '0.0000 BOS' || pendingStake">
              <div @click="active" class="vote-button vote-button-active">Be active</div>
              <div @click="unstake" class="vote-button vote-button-active">Unstake</div>
            </div>
            <div v-else @click="stake" class="vote-button vote-button-active">Stake</div>
          </div>
          <div @click="showUpdate" class="vote-button vote-button-active">Update info</div>
        </div>
        <div v-else-if="myAuditor" class="card">
          <h1>You are a auditor</h1>
          <p>Votes: {{(myAuditor.total_votes / 10000).toFixed(4)}}</p>
        </div>
        <div v-else-if="scatter">
          <div v-if="!scatter.identity" class="button"
            @click="getIdentity"
            style="border-radius:6px;width:90%;height:40px;line-height:30px"
          >
            Pair Scatter
          </div>
          <router-link v-else :to="{path: '/auditor/register'}">
            <div class="button"
              style="border-radius:6px;width:90%;height:40px;line-height:30px"
            >
              Register as Candidate
            </div>
          </router-link>
        </div>
        <div v-else>
          <!-- <p>Scatter is required!</p> -->
          <a target="blank" href="https://get-scatter.com/">
            <div class="button" style="border-radius:6px;width:90%;height:40px;line-height:30px">Get Scatter</div>
          </a>
        </div>
      </el-aside>
    </el-container>
  <el-dialog
    title="Update candidate information"
    :visible.sync="updateDialog"
  >
    <span>
      <el-form ref="updateForm" :model="candInfo" label-width="210px" label-position="left" :rules="updateRules">
        <el-form-item prop="contact">
            <label slot="label">Email</label>
            <el-input style="max-width: 400px;" v-model="candInfo.contact"></el-input>
          </el-form-item>
          <el-form-item>
            <label slot="label">Avatar image url</label>
            <el-input style="max-width: 400px;" v-model="candInfo.avatar" ></el-input>
          </el-form-item>
          <el-form-item prop="bio">
            <label slot="label">BIO</label>
            <el-input  v-model="candInfo.bio" type="textarea" :rows="10" ></el-input>
          </el-form-item>
      </el-form>
    </span>
    <span slot="footer">
      <el-button @click="updateDialog=false">Cancel</el-button>
      <el-button type="primary" @click="updateBio">Update</el-button>
    </span>
  </el-dialog>
  </div>
</template>

<script>
import Eos from 'eosjs'
import { NETWORK } from '@/assets/constants.js'
import Avatar from '@/components/Avatar.vue'
import { Message, MessageBox } from 'element-ui'
import CandidateCollapse from '@/components/CandidateCollapse.vue'
export default {
  name: 'Auditor',
  components: {
    Avatar,
    CandidateCollapse
  },
  data () {
    return {
      selectedCandidates: [],
      auditorsList: [],
      allCandList: [],
      candidatesList: [],
      config: null,
      contract: 'auditor.bos',
      updateDialog: false,
      candInfo: {
        avatar: '',
        bio: '',
        contact: ''
      },
      updateRules: {
        bio: [
          { required: true, message: 'Bio can\'t be empty', trigger: 'blur' }
        ],
        contact: [
          { required: true, message: 'Contact way can\'t be empty', trigger: 'blur' }
        ]
      },
      pendingStakeTable: []
    }
  },
  created () {
    const interval = setInterval(() => {
      if (this.eos) {
        this.getConfig()
        this.getAuditors()
        this.getCandidates()
        this.getPendingStake()
        clearInterval(interval)
      }
    }, 1000)
  },
  computed: {
    account () {
      if (this.scatter && this.scatter.identity) {
        return this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      } else {
        return null
      }
    },
    myCandidate () {
      if (this.account) {
        return this.allCandList.find(elm => elm.candidate_name === this.account.name)
      }
      return null
    },
    myAuditor () {
      if (this.account) {
        return this.auditorsList.find(elm => elm.cust_name === this.account.name)
      }
      return null
    },
    scatter () {
      return this.$store.state.scatter
    },
    eos () {
      if (this.scatter) {
        const eosOptions = { expireInSeconds: 60 }
        const eos = this.scatter.eos(NETWORK, Eos, eosOptions)
        return eos
      }
      return null
    },
    pendingStake () {
      let flag = false
      this.pendingStakeTable.forEach(item => {
        if (item.sender === this.account.name && item.quantity >= this.config.lockupasset) {
          flag = true
        }
      })
      return flag
    }
  },
  methods: {
    getConfig () {
      const tableOptions = {
        'scope': 'auditor.bos',
        'code': 'auditor.bos',
        'table': 'config',
        'json': true
      }
      if (this.eos) {
        this.eos.getTableRows(tableOptions).then((res) => {
          this.config = res.rows[0]
        })
      }
    },
    getPendingStake () {
      const tableOptions = {
        'scope': 'auditor.bos',
        'code': 'auditor.bos',
        'table': 'pendingstake',
        'json': true
      }
      if (this.eos) {
        this.eos.getTableRows(tableOptions).then((res) => {
          this.pendingStakeTable = res.rows
        })
      }
    },
    async getInforms () {
      if (this.eos) {
        const tableOptions = {
          'scope': 'auditor.bos',
          'code': 'auditor.bos',
          'table': 'bios',
          'json': true
        }
        const res = await this.eos.getTableRows(tableOptions)
        return res.rows
      }
    },
    async getCandidates () {
      if (this.eos) {
        const tableOptions = {
          'scope': 'auditor.bos',
          'code': 'auditor.bos',
          'table': 'candidates',
          'json': true
        }
        const informs = await this.getInforms()
        this.eos.getTableRows(tableOptions).then(res => {
          this.allCandList = []
          this.candidatesList = []
          res.rows.forEach(candidate => {
            candidate.isSelected = false
            let inform = informs.find(element => {
              return element.candidate_name === candidate.candidate_name
            })
            if (inform) {
              try {
                inform.bio = JSON.parse(inform.bio)
              } catch (e) {
              }
              candidate.inform = inform.bio
              candidate.isSelected = false
            }
            this.allCandList.push(candidate)
            if (candidate.is_active) {
              this.candidatesList.push(candidate)
            }
          })
        })
      }
    },
    getAuditors () {
      if (this.eos) {
        const tableOptions = {
          'scope': 'auditor.bos',
          'code': 'auditor.bos',
          'table': 'custodians',
          'json': true
        }
        this.eos.getTableRows(tableOptions).then(res => {
          this.auditorsList = res.rows
        })
      }
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
    handleSelect (msg, err) {
      if (err) {
        console.log(err)
      } else {
        if (msg.isSelected) {
          this.pushCandidate(msg.id)
        } else {
          this.removeCandidate(msg.id)
        }
      }
    },
    pushCandidate (id) {
      if (this.selectedCandidates.length <= this.config.maxvotes) {
        for (let i = 0; i < this.candidatesList.length; i++) {
          if (this.candidatesList[i].candidate_name === id) {
            this.candidatesList[i].isSelected = true
            this.selectedCandidates.push(this.candidatesList[i])
            break
          }
        }
      } else {
        //  提示
        Message({
          showClose: true,
          type: 'warning',
          message: `You only can vote to ${this.config.maxvotes} candidates`
        })
      }
    },
    removeCandidate (id) {
      for (let i = 0; i < this.selectedCandidates.length; i++) {
        if (this.selectedCandidates[i].candidate_name === id) {
          this.selectedCandidates.splice(i, 1)
          break
        }
      }
      for (let i = 0; i < this.candidatesList.length; i++) {
        if (this.candidatesList[i].candidate_name === id) {
          this.candidatesList[i].isSelected = false
          break
        }
      }
    },
    removeAllCand () {
      this.selectedCandidates = []
      this.candidatesList.map(cand => {
        cand.isSelected = false
        return cand
      })
    },
    sendVotes () {
      if (!this.scatter) {
        MessageBox.alert('Get Scatter first', '', {
          confirmButtonText: 'Get it',
          cancelButtonText: 'Later',
          callback: action => {
            if (action === 'confirm') {
              window.open('https://get-scatter.com/', '_blank')
            }
          }
        })
      } else if (!this.scatter.identity) {
        MessageBox.alert('Pair Scatter first', '', {
          confirmButtonText: 'OK'
        })
        this.getIdentity()
      } else {
        let newvotes = []
        this.selectedCandidates.forEach(item => {
          newvotes.push(item.candidate_name)
        })
        const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
        const transactionOptions = {
          actions: [
            {
              account: 'auditor.bos',
              name: 'votecust',
              authorization: [{
                actor: account.name,
                permission: account.authority
              }],
              data: {
                voter: account.name,
                newvotes: newvotes
              }
            }]
        }
        this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
          .then(() => {
            Message({
              showClose: true,
              type: 'success',
              message: 'Your vote has been cast on candidates'
            })
            this.removeAllCand()
          })
      }
    },
    async stake () {
      this.eos.transfer(this.account.name, this.contract, this.config.lockupasset, '')
        .then(res => {
          this.getCandidates()
          this.getPendingStake()
          Message({
            showClose: true,
            type: 'success',
            message: 'Stake successfully'
          })
        })
        .catch(e => {})
    },
    unstake () {
      const transactionOptions = {
        actions: [
          {
            account: this.contract,
            name: 'unstake',
            authorization: [{
              actor: this.account.name,
              permission: this.account.authority
            }],
            data: {
              cand: this.account.name
            }
          }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
        .then(() => {
          this.getCandidates()
          this.getPendingStake()
          Message({
            showClose: true,
            type: 'success',
            message: `Unstake successfully, stake will be released back ${this.config.lockup_release_time_delay}s later`
          })
        })
    },
    active () {
      const transactionOptions = {
        actions: [
          {
            account: this.contract,
            name: 'nominatecand',
            authorization: [{
              actor: this.account.name,
              permission: this.account.authority
            }],
            data: {
              cand: this.account.name
            }
          }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
        .then(() => {
          this.getCandidates()
          Message({
            showClose: true,
            type: 'success',
            message: 'You are active for auditor elections'
          })
        })
    },
    inactive () {
      const transactionOptions = {
        actions: [
          {
            account: this.contract,
            name: 'withdrawcand',
            authorization: [{
              actor: this.account.name,
              permission: this.account.authority
            }],
            data: {
              cand: this.account.name
            }
          }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
        .then(() => {
          this.getCandidates()
          Message({
            showClose: true,
            type: 'success',
            message: 'You are inactive for auditor elections'
          })
        })
    },
    showUpdate () {
      // this.candInfo = { ...this.myCandidate.inform }
      this.candInfo = {
        avatar: this.myCandidate.inform.avatar || '',
        bio: this.myCandidate.inform.bio,
        contact: this.myCandidate.inform.contact
      }
      this.updateDialog = true
    },
    updateBio () {
      this.$refs['updateForm'].validate(valid => {
        if (valid) {
          const transactionOptions = {
            actions: [
              {
                account: this.contract,
                name: 'updatebio',
                authorization: [{
                  actor: this.account.name,
                  permission: this.account.authority
                }],
                data: {
                  cand: this.account.name,
                  bio: JSON.stringify(this.candInfo)
                }
              }]
          }
          this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
            .then(res => {
              this.updateDialog = false
            })
        }
      })
    }
  }
}
</script>

<style lang="stylus" scoped>
.auditor
  text-align left
h1
  font-family: Roboto-Medium;
  font-size: 20px;
  color: #507DFE;
  letter-spacing: 0;
.main-panel
  p
    font-family: Roboto-Regular;
    font-size: 12px;
    color: #507DFE;
    letter-spacing: 0;
.board
  display flex
  justify-content flex-start
  flex-wrap wrap
.board-item
  display flex
  flex-direction column
  align-items center
  text-align center
  margin-right 20px
  p
    margin 10px
    font-family: Roboto-Medium;
    font-size: 12px;
    color: #507DFE;
    letter-spacing: 0;

.vote-panel
  width 90%
  p
    font-family: Roboto-Regular;
    font-size: 12px;
    color: #507DFE;
    letter-spacing: 0;
.vote-button
  height 36px
  width auto
  line-height 36px
  background: #7599FF;
  border-radius: 6px;
  font-family: Roboto-Medium;
  font-size: 16px;
  color: #FFFFFF;
  cursor pointer
  letter-spacing: 0;
  text-align center
  margin 5px 0
.vote-button-active
  background: #527FFF;
  &:hover
    opacity 0.8
.card
  padding 22px 34px
  background: #FCFDFF;
  box-shadow: 0 2px 4px 0 #B0D9FF;
  border-radius: 8px;
  margin-bottom 22px
.selected-candidate-card
  background: #FCFDFF;
  box-shadow: 0 2px 4px 0 #B0D9FF;
  border-radius: 8px;
  padding 5px 10px
  margin-top 15px
  display flex
  justify-content space-between
  align-items center
  p
    font-family: Roboto-Medium;
    font-size: 14px;
    line-height 16px
    letter-spacing: 0;
.remove-button
  width 30px
  height 30px
  border-radius 50%
  text-align center
  background  #507DFE
  &:hover
    opacity 0.8
  i
    font-size 20px
    line-height 30px
    color #ffffff
</style>
