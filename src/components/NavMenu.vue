<template>
  <div class="nav-menu">
    <el-menu :default-active="activeIndex" mode="horizontal" class="el-menu-demo clear-float">
      <el-menu-item index="home" @click="$router.push('/')">
        <img src="@/assets/logo.png">
      </el-menu-item>
      <el-menu-item index="referendum" @click="$router.push('/referendum')">{{$t('common.referendum')}}</el-menu-item>
      <el-menu-item index="auditor" @click="$router.push('/auditor')">{{$t('common.auditor')}}</el-menu-item>
      <el-submenu index="logout" style="float:right" v-if="account">
        <template slot="title">{{account}}</template>
        <el-menu-item @click="forgetIdentity" style="text-align:center">Remove Identity</el-menu-item>
      </el-submenu>
      <el-menu-item index="login" style="float:right"  v-else @click="getIdentity">Login</el-menu-item>
    </el-menu>
  </div>
</template>

<script>
import { NETWORK } from '@/assets/constants.js'
export default {
  name: 'NavMenu',
  data () {
    return {
    }
  },
  computed: {
    activeIndex () {
      return this.$route.name
    },
    account () {
      if (this.$store.state.scatter && this.$store.state.scatter.identity) {
        return this.$store.state.scatter.identity.accounts.find(x => x.blockchain === 'eos').name
      }
      return null
    },
    scatter () {
      return this.$store.state.scatter
    }
  },
  methods: {
    getIdentity () { // scatter认证
      const requiredFields = {
        accounts: [ NETWORK ]
      }
      this.scatter.getIdentity(requiredFields).then(() => {
        // console.log(this.scatter.identity)
        this.$store.dispatch('setScatter', { scatter: this.scatter })
      })
    },
    forgetIdentity () {
      this.scatter.forgetIdentity()
    }
  }
}
</script>

<style lang="stylus" scope>
// .nav-menu
// .el-menu
// .el-menu-item
  // float right
#logo-img
  float left
  height 100%
  img
    width 158px
    height 34px
    position absolute
    top 50%
    transform translate(0,-50%)

</style>
