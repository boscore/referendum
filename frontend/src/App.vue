<template>
  <div id="app">
    <NavMenu></NavMenu>
    <div class="app-main">
      <router-view></router-view>
    </div>
    <Footer></Footer>
  </div>
</template>

<script>
import Footer from '@/components/Footer.vue'
import NavMenu from '@/components/NavMenu.vue'
import ScatterJS from 'scatterjs-core'
import ScatterEOS from 'scatterjs-plugin-eosjs'
// import { API_URL } from '@/assets/constants.js'
// import { connect } from 'http2'

ScatterJS.plugins(new ScatterEOS())
export default {
  name: 'app',
  components: {
    Footer,
    NavMenu
  },
  data () {
    return {
      interval: null
    }
  },
  created () {
    this.interval = setInterval((() => {
      this.$store.dispatch('getAccounts')
      this.$store.dispatch('getVotes')
      this.$store.dispatch('getProxies')
      console.log('getinfo')
    })(), 60000)
    this.$store.dispatch('getProposals')
    if (this.timer) clearTimeout(this.timer)
    this.timer = setTimeout(() => {
      ScatterJS.scatter.connect('BOSCore-Referendum').then(connected => {
        if (!connected) return false
        // 有scatter
        this.$store.dispatch('setScatter', { scatter: ScatterJS.scatter })
      })
    }, 1000)
  },
  mounted () {
    this.$store.dispatch('setScreenWidth', { screenWidth: document.body.clientWidth })
    window.onresize = () => {
      this.$store.dispatch('setScreenWidth', { screenWidth: document.body.clientWidth })
    }
  },
  beforeDestroy () {
    clearInterval(this.interval)
  },
  methods: {
  }
}
</script>

<style lang="stylus" scope>
body {
  margin: 0;
  background-color: rgb(232,236,255);
}

.app-main {
  min-height: calc(100vh - 160px - 60px);
  background-color: rgb(232,236,255);
  max-width: 1420px;
  margin: auto;
}
@media only screen and (min-width 450px)
  .app-main
    padding-left: 50px;
    padding-right: 50px;
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
}
</style>
