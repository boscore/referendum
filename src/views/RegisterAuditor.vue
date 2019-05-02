<template>
  <el-container class="register-auditor">
    <el-main>
      <h1 style="line-height:30px;margin:15px 0px;">Register Auditor</h1>
      <div class="card">
        <el-form ref="form" :rules="rules" :model="form" label-position="top" label-width="110px">
        <p>Responsibilities outlined in the. BOS Declaration. Please, do not register as a candidate unless you fully understand and can meet the responsibilites of an BOS custodian board member. </p>
        <el-form-item>
          <label slot="label">Auditor (Bos account)</label>
          <el-input style="width: 400px;" v-model="form.auditorName" ></el-input>
        </el-form-item>
        <el-form-item>
          <label slot="label">
            Stake Amount
            (Already staked
            <el-checkbox v-model="hasStaked"></el-checkbox>
            )
          </label>
          <el-input disabled style="width: 400px;" v-model="form.stakeAmount"></el-input>
        </el-form-item>
        <el-form-item>
          <label slot="label">Email</label>
          <el-input style="width: 400px;" v-model="form.bio.contact"></el-input>
        </el-form-item>
        <el-form-item>
          <label slot="label">Avatar image url (optinal)</label>
          <el-input style="width: 400px;" v-model="form.bio.avatar" ></el-input>
        </el-form-item>
        <el-form-item>
          <label slot="label">BIO</label>
          <el-input  v-model="form.bio.bio" type="textarea" :rows="10" ></el-input>
        </el-form-item>
        <el-form-item>
         <div @click="register" style="width: 400px; margin-top:30px;display: block" class="button square-button">Register</div>
        </el-form-item>
        </el-form>
      </div>
    </el-main>
    <el-aside>
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
import { NETWORK } from '@/assets/constants.js'
import { setInterval, clearInterval } from 'timers'
export default {
  name: 'RegisterAuditor',
  data () {
    return {
      form: {
        stakeAmount: '100000.0000 BOS',
        bio: {
          contact: '',
          avator: '',
          bio: ''
        },
        auditorName: ''
      },
      hasStaked: false,
      rules: {},
      config: null,
      contract: 'auditor.bos'
    }
  },
  computed: {
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
    this.$nextTick(() => {
      this.getConfig()
    })
  },
  methods: {
    async stake () {
      const account = this.scatter.identity.accounts.find(x => x.blockchain === 'eos')
      const res = await this.eos.transfer(account.name, this.contract, this.form.stakeAmount, '')
      console.log(res)
    },
    getConfig () {
      const interval = setInterval(() => {
        const tableOptions = {
          'scope': 'auditor.bos',
          'code': 'auditor.bos',
          'table': 'config',
          'json': true
        }
        if (this.eos) {
          this.eos.getTableRows(tableOptions).then((res) => {
            this.config = res.rows[0]
            this.form.stakeAmount = this.config.lockupasset
          })
          clearInterval(interval)
        }
      }, 1000)
    },
    async register () {
      if (this.hasStaked === false) {
        await this.stake()
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
              bio: JSON.stringify(this.form.bio)
            }
          }]
      }
      this.eos.transaction(transactionOptions, { blocksBehind: 3, expireSeconds: 30 })
    }
  }
}
</script>

<style lang="stylus" scoped>
.register-auditor
  text-align left
  color: #507DFE
</style>
