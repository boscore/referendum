import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import './plugins/element.js'
import VueI18n from 'vue-i18n'
import messages from '@/language'
import '@/assets/common.styl'
import axios from 'axios'
import util from '@/util.js'

Vue.config.productionTip = false
Vue.prototype.$axios = axios
Vue.prototype.$util = util

Vue.use(VueI18n)

const i18n = new VueI18n({
  locale: 'en',
  fallbackLocale: 'en',
  messages
})

new Vue({
  router,
  store,
  i18n,
  render: h => h(App)
}).$mount('#app')
