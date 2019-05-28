<template>
  <div ref="auditor" class="auditor">
    <el-container v-loading="actionLoading">
      <el-main style="padding-top:0">
        <div class="main-panel">
          <h1>Auditor Board</h1>
          <div class="card board" v-loading="auditorLoading">
            <div v-for="auditor in auditorsList" :key="auditor.auditor_name" class="board-item">
              <Avatar :url="auditor.inform ? auditor.inform.avatar : ''" star></Avatar>
              <p>{{auditor.auditor_name}}</p>
            </div>
          </div>
          <div class="candidate-list">
            <h1>Candidate List <span style="color: #91ADFF;">- {{candidatesList.length}}</span></h1>
            <!-- <p>The Custodian Board manages the operations and affairs of the DAC, including but not limited to the governance and administration of the assets and liabilities of the DAC. The following DAC members have vested some of their tokens to submit themselves and candidates for a position on the custodian board which last for 7 days. Every 7 days, your votes are recalculated to select who will be part of the next custodian board. Voting is important! Please vote often and stay engaged within the DAC to know who is providing value</p> -->
            <div
              v-loading="candidateLoading"
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
      <el-aside :width="asideWidth">
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
        <div v-if="myCandidate && !myAuditor" class="card">
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
          <div @click="showUpdate" class="vote-button vote-button-active">Update info</div>
        </div>
        <div v-else-if="scatter">
          <div v-if="!scatter.identity" class="button square-button"
            @click="getIdentity"
          >
            Pair Scatter
          </div>
          <router-link v-else :to="{path: '/auditor/register'}">
            <div class="button square-button"
              v-if="!candidateLoading && !auditorLoading"
            >
              Register as Candidate
            </div>
          </router-link>
        </div>
        <div v-else>
          <!-- <p>Scatter is required!</p> -->
          <a target="blank" href="https://get-scatter.com/">
            <div class="button square-button">Get Scatter</div>
          </a>
        </div>
      </el-aside>
    </el-container>
  <el-dialog
    :width="dialogWidth"
    title="Update candidate information"
    :visible.sync="updateDialog"
    v-loading="actionLoading"
  >
    <span>
      <el-form ref="updateForm" :model="candInfo" label-width="210px" :label-position="screenWidth < 821 ? 'top' : 'left'" :rules="updateRules">
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
import { NETWORK, NODE_ENDPOINT } from '@/assets/constants.js'
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
      actionLoading: false,
      auditorLoading: true,
      bioInfo: [],
      candidateLoading: true,
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
      pendingStakeTable: [],
      screenWidth: document.body.clientWidth
    }
  },
  mounted () {
    window.onresize = () => {
      this.screenWidth = document.body.clientWidth
    }

    this.getAllInfo()
    this.getConfig()
    this.getPendingStake()
  },
  computed: {
    asideWidth () {
      if (this.screenWidth < 821) {
        return '100%'
      }
      return '320px'
    },
    account () {
      if (this.scatter && this.scatter.identity) {
        return this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      } else {
        return null
      }
    },
    dialogWidth () {
      if (this.$store.state.screenWidth < 450) {
        return '90%'
      }
      return '60%'
    },
    myCandidate () {
      if (this.account) {
        return this.allCandList.find(elm => elm.candidate_name === this.account.name)
      }
      return null
    },
    myAuditor () {
      if (this.account) {
        return this.auditorsList.find(elm => elm.auditor_name === this.account.name)
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
      fetch(NODE_ENDPOINT + '/v1/chain/get_table_rows', {
        method: 'POST',
        body: JSON.stringify(tableOptions)
      }).then(res => res.json())
        .then((res) => {
          this.config = res.rows[0]
        }).catch(e => {
          console.log(e)
        })
    },
    getPendingStake () {
      const tableOptions = {
        'scope': 'auditor.bos',
        'code': 'auditor.bos',
        'table': 'pendingstake',
        'json': true
      }
      fetch(NODE_ENDPOINT + '/v1/chain/get_table_rows', {
        method: 'POST',
        body: JSON.stringify(tableOptions)
      }).then(res => res.json()).then((res) => {
        this.pendingStakeTable = res.rows
      }).catch(e => { console.log(e) })
    },
    getAllInfo () {
      const tableOptions = {
        'scope': 'auditor.bos',
        'code': 'auditor.bos',
        'table': 'bios',
        'json': true
      }
      fetch(NODE_ENDPOINT + '/v1/chain/get_table_rows', {
        method: 'POST',
        body: JSON.stringify(tableOptions)
      }).then(res => res.json()).then(res => {
        this.bioInfo = res.rows
        this.getAuditors()
        this.getCandidates()
      }).catch(e => {
        this.candidateLoading = false
        console.log(e)
        // MessageBox.alert(e, 'ERROR.\n', {
        //   confirmButtonText: 'OK'
        // })
      })
    },
    getCandidates () {
      const tableOptions = {
        'scope': 'auditor.bos',
        'code': 'auditor.bos',
        'table': 'candidates',
        'json': true
      }
      fetch(NODE_ENDPOINT + '/v1/chain/get_table_rows', {
        method: 'POST',
        body: JSON.stringify(tableOptions)
      }).then(res => res.json()).then(res => {
        this.allCandList = []
        this.candidatesList = []
        this.candidateLoading = false
        res.rows.forEach(candidate => {
          candidate.isSelected = false
          let inform = this.bioInfo.find(element => {
            return element.candidate_name === candidate.candidate_name
          })
          if (inform) {
            try {
              inform.bio = JSON.parse(inform.bio)
            } catch (e) {
              console.log('json invalid')
            }
            candidate.inform = inform.bio
            candidate.isSelected = false
          }
          this.allCandList.push(candidate)
          if (candidate.is_active) {
            this.candidatesList.push(candidate)
            this.candidatesList.sort((a, b) => { return b.total_votes - a.total_votes })
          }
        })
      }).catch(e => {
        this.candidateLoading = false
        Message({
          showClose: true,
          message: 'Get candidates ERROR: ' + e.message,
          type: 'error'
        })
        console.log(e)
        // MessageBox.alert(e, 'ERROR.\n', {
        //   confirmButtonText: 'OK'
        // })
      })
    },
    getAuditors () {
      const tableOptions = {
        'scope': 'auditor.bos',
        'code': 'auditor.bos',
        'table': 'auditors',
        'json': true
      }
      fetch(NODE_ENDPOINT + '/v1/chain/get_table_rows', {
        method: 'POST',
        body: JSON.stringify(tableOptions)
      }).then(res => res.json()).then(res => {
        this.auditorLoading = false
        this.auditorsList = res.rows
        this.auditorsList.map(auditor => {
          let inform = this.bioInfo.find(element => {
            return element.candidate_name === auditor.auditor_name
          })
          if (inform) {
            try {
              inform.bio = JSON.parse(inform.bio)
            } catch (e) {
            }
            auditor.inform = inform.bio
            return auditor
          }
        })
      }).catch(e => {
        this.auditorLoading = false
        Message({
          showClose: true,
          message: 'Get auditors ERROR: ' + e.message,
          type: 'error'
        })
        console.log(e)
        // MessageBox.alert(e, 'ERROR.\n', {
        //   confirmButtonText: 'OK'
        // })
      })
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
      if (this.config && this.selectedCandidates.length <= this.config.maxvotes) {
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
          distinguishCancelAndClose: true,
          cancelButtonText: 'Later',
          callback: action => {
            if (action === 'confirm') {
              window.open('https://get-scatter.com/', '_blank')
            }
          }
        })
      } else if (!this.scatter.identity) {
        Message({
          showClose: true,
          type: 'warning',
          message: `Pair Scatter first`
        })
        // MessageBox.alert('Pair Scatter first', '', {
        //   confirmButtonText: 'OK'
        // })
        this.getIdentity()
      } else {
        this.actionLoading = true
        let newvotes = []
        this.selectedCandidates.forEach(item => {
          newvotes.push(item.candidate_name)
        })
        const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
        const transactionOptions = {
          actions: [
            {
              account: 'auditor.bos',
              name: 'voteauditor',
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
            this.actionLoading = false
            Message({
              showClose: true,
              type: 'success',
              message: 'Your vote has been cast on candidates'
            })
            this.getAllInfo()
            this.removeAllCand()
          }).catch(e => {
            this.actionLoading = false
            Message({
              showClose: true,
              type: 'error',
              message: 'Vote ERROR:' + e.message
            })
            console.log(e)
            // MessageBox.alert(e, 'ERROR.\n.\n', {
            //   confirmButtonText: 'OK'
            // })
          })
      }
    },
    async stake () {
      this.actionLoading = true
      this.eos.transfer(this.account.name, this.contract, this.config.lockupasset, '')
        .then(res => {
          this.getCandidates()
          this.getPendingStake()
          this.actionLoading = false
          Message({
            showClose: true,
            type: 'success',
            message: 'Stake successfully'
          })
        })
        .catch(e => {
          this.actionLoading = false
          Message({
            showClose: true,
            type: 'error',
            message: 'Stake ERROR:' + e.message
          })
          console.log(e)
          // MessageBox.alert(e, 'ERROR.\n.\n', {
          //   confirmButtonText: 'OK'
          // })
        })
    },
    unstake () {
      this.actionLoading = true
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
          this.actionLoading = false
          this.getCandidates()
          this.getPendingStake()
          Message({
            showClose: true,
            type: 'success',
            message: `Unstake successfully`
          })
        })
        .catch(e => {
          this.actionLoading = false
          Message({
            showClose: true,
            type: 'error',
            message: 'Unstake ERROR: ' + e.message
          })
          console.log(e)
          // MessageBox.alert(e, 'ERROR.\n.\n', {
          //   confirmButtonText: 'OK'
          // })
        })
    },
    active () {
      this.actionLoading = true
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
          this.actionLoading = false
          this.getCandidates()
          Message({
            showClose: true,
            type: 'success',
            message: 'You are active for auditor elections'
          })
        })
        .catch(e => {
          this.actionLoading = false
          Message({
            showClose: true,
            type: 'error',
            message: 'Be active ERROR: ' + e.message
          })
          console.log(e)
          // MessageBox.alert(e, 'ERROR.\n', {
          //   confirmButtonText: 'OK'
          // })
        })
    },
    inactive () {
      this.actionLoading = true
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
          this.actionLoading = false
          this.getCandidates()
          Message({
            showClose: true,
            type: 'success',
            message: 'You are inactive for auditor elections'
          })
        })
        .catch(e => {
          this.actionLoading = false
          Message({
            showClose: true,
            type: 'error',
            message: 'Be inactive ERROR: ' + e.message
          })
          console.log(e)
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
          this.actionLoading = true
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
              this.actionLoading = false
            })
            .catch(e => {
              this.actionLoading = false
              Message({
                showClose: true,
                type: 'error',
                message: 'Update BIO ERROR: ' + e.message
              })
              console.log(e)
              // MessageBox.alert(e, 'ERROR.\n', {
              //   confirmButtonText: 'OK'
              // })
            })
        }
      })
    }
  },
  watch: {
    $route () {
      this.getAllInfo()
      this.getConfig()
      this.getPendingStake()
    }
  }
}
</script>

<style lang="stylus">
@media only screen and (max-width 840px)
  .auditor
    .el-container
      flex-wrap wrap
      flex-direction column-reverse
    .el-aside
      width 100%
      padding 0 20px
</style>

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
  p
    font-family: Roboto-Regular;
    font-size: 12px;
    color: #507DFE;
    letter-spacing: 0;
.square-button
  width 100%
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
