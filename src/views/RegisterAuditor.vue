<template>
  <el-container class="register-auditor">
    <el-main>
      <h1 style="line-height:30px;margin:15px 0px;">Register Auditor</h1>
      <div class="card">
        <p>Responsibiliies outlined in the. BOS Constitution. Please, do not register as a candidate unless you fully understand and can meet the responsibilites of an BOS custodian board member. Please review section 5 of the constitution carefully before registering.</p>
        <h3>Stake Amount</h3>
        <p>Please specify the amount of tokens you want to stake. The minimum amount is 35,000.0000 EOSDAC.</p>
        <el-input style="width: 400px;" v-model="stakeAmount" placeholder="Amount to stake BOS"></el-input>
        <h3>Request Pay</h3>
        <p>Please fill in The amount you want to receive for being a custodian during one period. The maximum amount is 50.0000 EOS.</p>
        <el-input style="width: 400px;" v-model="payAmount" placeholder="Request pay in EOS"></el-input>
        <div style="width: 400px; margin-top:30px;display: block" class="button square-button">Register</div>
      </div>
    </el-main>
    <el-aside>
      <div class="card" style="margin-top:80px">
        <p>You have not been registered as a Member yet. Please sign the constitution to use the Member Client.</p>
        <router-link :to="{path: '/auditor/constitution'}">
          <div style="width:100%" class="button square-button">SIGN THE CONSTITUTION</div>
        </router-link>
      </div>
    </el-aside>
  </el-container>
</template>

<script>
export default {
  name: 'RegisterAuditor',
  data () {
    return {
      stakeAmount: '',
      payAmount: ''
    }
  }
}
</script>

<style lang="stylus" scoped>
.register-auditor
  text-align left
  color: #507DFE
</style>
