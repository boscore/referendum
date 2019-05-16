<template>
  <div class="create-proposal">
    <el-container>
      <el-main>
      <div id="back-button" @click="$router.push({path: '/'})">
        <i class="el-icon-arrow-left"></i>Back
      </div>
      <h2>Create Proposal</h2>
      <div class="card" v-loading="actionLoading">
        <el-form ref="form" :rules="rules" :model="form" label-position="top" label-width="110px">
          <el-form-item>
            <label slot="label">Proposer</label>
            <span>{{proposer}}</span>
          </el-form-item >
          <el-form-item prop="name">
            <label slot="label">Proposal Name</label>
            <el-input maxlength="12" v-model="form.name"></el-input>
          </el-form-item>
          <el-form-item prop="title">
            <label slot="label">Title</label>
            <el-input maxlength="1024" type="textarea" :autosize="{ minRows: 1, maxRows: 3}" v-model="form.title"></el-input>
          </el-form-item>
          <el-form-item prop="incentives">
            <label slot="label">Incentives</label>
            <el-input max="1000000" @change="form.incentives=form.incentives.replace(/[^\d]/g,'')" v-model="form.incentives"></el-input>
          </el-form-item>
          <el-form-item prop="receipt">
            <label slot="label">Receipt account</label>
            <el-input v-model="form.receipt"></el-input>
          </el-form-item>
          <el-form-item prop="content">
            <label slot="label">Content (support Markdown)</label>
            <el-input v-model="form.content" type="textarea" :rows="10"></el-input>
          </el-form-item>
          <el-form-item prop="expiry">
            <label slot="label">Expiry(UTC)</label>
            <el-date-picker
              type="datetime"
              value-format="yyyy-MM-ddThh:mm:ss"
              placeholder="expiry date"
              v-model="form.expiry"></el-date-picker>
          </el-form-item>
          <el-form-item prop="type">
            <label slot="label">Type</label>
            <el-select v-model="form.type">
              <el-option
                v-for="item in typeOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value"
              ></el-option>
            </el-select>
          </el-form-item>
          <el-form-item>
            <div class="button" @click="createProp">
              Propose
            </div>
          </el-form-item>
        </el-form>
      </div>
      </el-main>
    </el-container>
  </div>
</template>

<script>
import Eos from 'eosjs'
import { NETWORK } from '@/assets/constants.js'
import { Message } from 'element-ui'
export default {
  name: 'CreateProposal',
  data () {
    const checkExpiryDate = (rule, value, callback) => {
      if (!value) {
        return callback(new Error('Please choose proposal expiry date'))
      } else {
        let now = new Date().getTime() + (new Date().getTimezoneOffset() * 60 * 1000)
        let expiry = new Date(value).getTime()
        if (expiry < now) {
          return callback(new Error('Expiry date shouldn\'t be earlier than now(UTC)'))
        } else if ((expiry - now) > (1000 * 60 * 60 * 24 * 180)) {
          return callback(new Error('Expiry date shouldn\'t be later than 6 months in the future'))
        } else {
          callback()
        }
      }
    }
    const checkIncentives = (rule, value, callback) => {
      if (value === '') {
        return callback(new Error('Please input a number of incentives'))
      } else {
        if (Number(value) > 1000000) {
          return callback(new Error('No more than 1000,000 BOS'))
        } else {
          callback()
        }
      }
    }
    return {
      actionLoading: false,
      typeOptions: [
        {
          label: 'Referendum',
          value: 'referendum-v1'
        },
        {
          label: 'Poll',
          value: 'poll_default'
        }
      ],
      form: {
        name: '',
        title: '',
        content: '',
        expiry: '',
        receipt: this.proposer,
        incentives: 0,
        type: 'referendum-v1'
      },
      rules: {
        name: [
          { required: true, message: 'please input proposal name', trigger: 'blur' }
        ],
        title: [
          { required: true, message: 'please input proposal title', trigger: 'blur' }
        ],
        content: [
          { required: true, message: 'please input proposal content', trigger: 'blur' }
        ],
        expiry: [
          { validator: checkExpiryDate, trigger: 'blur' }
        ],
        type: [
          { required: true, message: 'please choose proposal type', trigger: 'blur' }
        ],
        incentives: [
          { validator: checkIncentives, trigger: 'blur' }
        ],
        receipt: [
          { required: true, message: 'please input receipt account of incentives', trigger: 'blur' }
        ]
      }
    }
  },
  computed: {
    proposer () {
      if (this.scatter && this.scatter.identity) {
        return this.scatter.identity.accounts.find(x => x.blockchain === 'eos').name
      } else {
        return ''
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
    }
  },
  methods: {
    createProp () {
      this.$refs['form'].validate(valid => {
        if (valid) {
          this.actionLoading = true
          const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
          let actionParams = {
            proposer: account.name,
            proposal_name: this.form.name,
            title: this.form.title,
            expires_at: this.form.expiry,
            proposal_json: JSON.stringify({
              content: this.form.content,
              type: this.form.type
            })
          }
          const transactionOptions = {
            actions: [{
              account: 'eosio.forum',
              name: 'propose',
              authorization: [{
                actor: account.name,
                permission: account.authority
              }],
              data: actionParams
            }]
          }
          this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
            .then(res => {
              this.actionLoading = false
              Message({
                showClose: true,
                type: 'success',
                message: 'You create a new proposal successfully'
              })
              this.$router.replace('/referendum')
              // MessageBox.alert(`You create a new proposal successfully`, '', {
              //   confirmButtonText: 'OK',
              //   callback: action => {
              //     if (action === 'confirm') {
              //       this.$router.replace('/referendum')
              //     }
              //   }
              // })
            }).catch(e => {
              // if (typeof e === 'string') {
              //   e = JSON.parse(e)
              // }
              this.actionLoading = false
              Message({
                showClose: true,
                type: 'error',
                message: 'Create ERROR' + String(e)
              })
              // MessageBox.alert(e, 'ERROR', {
              //   confirmButtonText: 'OK'
              // })
            })
        }
      })
    }
  }
}
</script>

<style lang="stylus" scoped>
.create-proposal
  h2
    color: #507DFE;
.card
  text-align left
  max-width 700px
  padding 22px 50px
  margin auto
  margin-bottom 70px
  label
    color: #507DFE;
.button
  display block
  border-radius: 4px;
  height 40px
  line-height 30px
  width 400px
  margin auto
  margin-top 20px
@media only screen and (max-width 600px)
  .button
    width 100%
#back-button
  cursor pointer
  width 100px
  height 32px
  line-height 32px
  background: #507DFE;
  border-radius: 15.5px;
  font-family: PingFangSC-Medium;
  font-size: 18px;
  color: #FFFFFF;
  letter-spacing: 0;
  text-align: center;
</style>
