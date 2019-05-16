<template>
  <div class="nav-menu clear-float">
    <div id="logo" @click="$router.push('/')">
      <img src="@/assets/bos-logo.png" id="logo-img" />
    </div>
    <div class="mobile-nav" @click="showMenu = !showMenu">
      <i class="el-icon-menu mobile-nav-icon"></i>
    </div>
    <div class="search-mask" v-if="showMenu" @click="showMenu=false"></div>
    <div class="el-menu-main">
    <el-menu :class="[{'hidden-menu': !showMenu}]" style="float:right" :default-active="activeIndex" mode="horizontal">
      <el-menu-item index="referendum" @click="$router.push('/referendum')">{{$t('common.referendum')}}</el-menu-item>
      <el-menu-item index="auditor" @click="$router.push('/auditor')">{{$t('common.auditor')}}</el-menu-item>
      <el-submenu index="logout" style="float:right" v-if="account">
        <template slot="title">{{account}}</template>
        <el-menu-item @click="forgetIdentity" style="text-align:center">Remove Identity</el-menu-item>
      </el-submenu>
      <el-menu-item index="login"  v-else @click="getIdentity">Login</el-menu-item>
    </el-menu>
    </div>
  </div>
</template>

<script>
import { NETWORK } from '@/assets/constants.js'
export default {
  name: 'NavMenu',
  data () {
    return {
      showMenu: false
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

<style lang="stylus">
@media only screen and (max-width 450px)
  .nav-menu
    .el-menu-item, .el-submenu
      width 150px
    .el-menu
      position absolute
      right 0
      top 60px
      z-index 1000
      width 150px
</style>

<style lang="stylus" scope>
.nav-menu
  background-color #fff
  position relative
// .el-menu
// .el-menu-item
  // float right
.search-mask
  width 100%
  height 100%
  position fixed
  top 0
  left 0
  z-index 999
  overflow hidden
.mobile-nav
  display none
  height 60px
  width 60px
  float right
  .mobile-nav-icon
    color #507dfe
    line-height 60px
    font-size 24px
    margin auto
    cursor pointer
@media only screen and (max-width 450px)
  .hidden-menu
    display none
  .mobile-nav
    display block
#logo
  height 60px
  float left
  #logo-img
    width: 160px;
    margin 13px
</style>
