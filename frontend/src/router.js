import Vue from 'vue'
import Router from 'vue-router'
import Referendum from './views/Referendum.vue'
import Auditor from './views/Auditor.vue'
import CreateProposal from './views/CreateProposal.vue'
import PollDetail from './views/PollDetail.vue'
import RegisterAuditor from './views/RegisterAuditor.vue'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: '',
      component: Referendum
    },
    {
      path: '/referendum',
      name: 'referendum',
      component: Referendum
    },
    {
      path: '/auditor',
      name: 'auditor',
      component: Auditor
    },
    {
      path: '/auditor/register',
      name: 'register',
      component: RegisterAuditor
    },
    {
      path: '/poll_detail',
      name: 'pull_detail',
      component: PollDetail
    },
    {
      path: '/create_proposal',
      name: 'create_proposal',
      component: CreateProposal
    }
  ]
})
