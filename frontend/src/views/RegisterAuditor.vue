<template>
  <el-container class="register-auditor">
    <el-main>
      <h1 style="line-height:30px;margin:15px 0px;">Register Candidate</h1>
      <div class="card" v-loading="actionLoading">
        <el-form ref="form" :rules="rules" :model="form" label-position="top" label-width="110px">
          <p>Responsibilities outlined in the. BOS Declaration. Please, do not register as a candidate unless you fully understand and can meet the responsibilites of an BOS custodian board member. And you need to transfer some BOS to auditor.bos as staked token.</p>
          <el-form-item prop="auditorName">
            <label slot="label">Candidate Name (Bos account)</label>
            <el-input style="max-width: 400px;" v-model="form.auditorName" ></el-input>
          </el-form-item>
          <el-form-item>
            <label slot="label">
              Stake Amount
              (Already staked
              <el-checkbox v-model="hasStaked"></el-checkbox>
              )
            </label>
            <el-input disabled style="max-width: 400px;" v-model="form.stakeAmount"></el-input>
          </el-form-item>
          <el-form-item prop="contact">
            <label slot="label">Email</label>
            <el-input style="max-width: 400px;" v-model="form.contact"></el-input>
          </el-form-item>
          <el-form-item>
            <label slot="label">Avatar image url (optinal)</label>
            <el-input style="max-width: 400px;" v-model="form.avatar" ></el-input>
          </el-form-item>
          <el-form-item prop="bio">
            <label slot="label">BIO (support markdown)</label>
            <el-input  v-model="form.bio" type="textarea" :rows="10" ></el-input>
          </el-form-item>
          <el-form-item prop="signDeclar">
            <el-checkbox style="margin-right:10px" size="medium" v-model="form.signDeclar"></el-checkbox>
            <span style="color:#606266">Sign the declaration.</span>
            <span style="cursor:pointer" @click="showDeclar = true">Read detail</span>
          </el-form-item>
          <el-form-item>
            <div @click="register" style="max-width: 400px; margin-top:30px;display: block" class="button square-button">Register</div>
          </el-form-item>
        </el-form>
      </div>
      <el-dialog
        :width="dialogWidth"
        title="Declaration"
        :visible.sync="showDeclar"
      >
      <h3>Auditor’s Declaration of Independence and Impartiality</h3>
        <p>
          I, the undersigned,<br>
          <br>
          accept to serve as Auditor, in accordance with the BOS Rules.<br>
          <br>
          I<br>
          <b>DECLARE</b> to be and to intend to remain independent and impartial during the auditing procedure.<br>
          <br>
          <b>DECLARE</b> that, to my knowledge, there are no facts, circumstances or relationships which may affect my independence and impartiality.<br>
          <br>
          <b>DECLARE</b> to treat all BOS members fairly, reward contributions appropriately and not seek unmerited profits. No member should have less or more information about an auditing decision than others.<br>
          <br>
          <b>DECLARE</b> not to seek any stake in, or exert undue influence over, other block producers and shall take appropriate measures to protect my own independence and impartiality. <br>
        </p>
        <span slot="footer">
          <el-button type="primary" @click="showDeclar=false;form.signDeclar=true">Agree</el-button>
        </span>
      </el-dialog>
    </el-main>
    <el-aside v-if="false">
      <div class="card" style="margin-top:80px">
        <p>You have not been registered as a Member yet. Please sign the declaration to use the Member Client.</p>
        <router-link :to="{path: '/auditor/constitution'}">
          <div style="width:100%" class="button square-button">SIGN THE DECLARATION</div>
        </router-link>
      </div>
    </el-aside>
  </el-container>
</template>

<script>
import Eos from 'eosjs'
import { NETWORK, NODE_ENDPOINT } from '@/assets/constants.js'
import { Message } from 'element-ui'
import { setInterval, clearInterval } from 'timers'
export default {
  name: 'RegisterAuditor',
  data () {
    var validateDeclar = (rule, value, callback) => {
      if (value === false) {
        callback(new Error('Please read and sign the declaration'))
      } else {
        callback()
      }
    }
    return {
      actionLoading: false,
      form: {
        stakeAmount: '100000.0000 BOS',
        contact: '',
        avatar: '',
        bio: '',
        signDeclar: false,
        auditorName: ''
      },
      hasStaked: false,
      rules: {
        auditorName: [
          { required: true, message: 'Please input the candidate account', trigger: 'blur' }
        ],
        contact: [
          { required: true, message: 'Please input candidate\'s public email', trigger: 'blur' }
        ],
        bio: [
          { required: true, message: 'Please input candidate\'s bio', trigger: 'blur' }
        ],
        signDeclar: [
          { validator: validateDeclar, trigger: 'blur' }
        ]
      },
      config: null,
      contract: 'auditor.bos',
      showDeclar: false
    }
  },
  computed: {
    dialogWidth () {
      if (this.$store.state.screenWidth < 450) {
        return '90%'
      }
      return '60%'
    },
    scatter () {
      return this.$store.state.scatter
    },
    eos () {
      if (this.scatter && this.scatter.identity) {
        const eosOptions = { expireInSeconds: 60 }
        const eos = this.scatter.eos(NETWORK, Eos, eosOptions)
        console.log(eos)
        return eos
      }
      return null
    }
  },
  mounted () {
    let interv = setInterval(() => {
      if (this.scatter && this.scatter.identity) {
        this.form.auditorName = this.scatter.identity.accounts.find(x => x.blockchain === 'eos').name
        clearInterval(interv)
      }
    }, 500)
    this.getConfig()
  },
  methods: {
    async stake () {
      const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      try {
        await this.eos.transfer(account.name, this.contract, this.form.stakeAmount, '')
      } catch (e) {
        this.actionLoading = false
        Message({
          showClose: true,
          type: 'error',
          message: 'Stake ERROR' + String(e)
        })
      }
    },
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
          this.form.stakeAmount = this.config.lockupasset
        }).catch(e => {
          console.log(e)
        })
    },
    async register () {
      this.$refs['form'].validate(async valid => {
        if (valid) {
          this.actionLoading = true
          if (this.hasStaked === false) {
            await this.stake()
          }
          if (this.actionLoading === false) {
            return
          }
          const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
          const transactionOptions = {
            actions: [
              {
                account: this.contract,
                name: 'nominatecand',
                authorization: [{
                  actor: account.name,
                  permission: account.authority
                }],
                data: {
                  cand: this.form.auditorName
                }
              },
              {
                account: this.contract,
                name: 'updatebio',
                authorization: [{
                  actor: account.name,
                  permission: account.authority
                }],
                data: {
                  cand: this.form.auditorName,
                  bio: JSON.stringify({
                    avatar: this.form.avatar,
                    bio: this.form.bio,
                    contact: this.form.contact
                  })
                }
              }]
          }
          this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
            .then(res => {
              this.actionLoading = false
              Message({
                showClose: true,
                type: 'success',
                message: 'You register as a new candidate successfully'
              })
              this.$router.replace('/auditor')
              // MessageBox.alert(`You register as a new candidate successfully`, '', {
              //   confirmButtonText: 'OK',
              //   callback: action => {
              //     if (action === 'confirm') {
              //       this.$router.replace('/auditor')
              //     }
              //   }
              // })
            }).catch(e => {
              this.actionLoading = false
              Message({
                showClose: true,
                type: 'error',
                message: 'Register ERROR' + String(e)
              })
            })
        } else {
          return false
        }
      })
    }
  }
}
</script>

<style lang="stylus" scoped>
.register-auditor
  text-align left
  color: #507DFE
</style>
