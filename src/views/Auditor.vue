<template>
  <div class="auditor">
    <el-container>
      <el-main style="padding-top:0">
        <div class="main-panel">
          <h1>Candidate Board</h1>
          <div class="card board">
            <div v-for="cand in candidatesList" :key="cand.id" class="board-item">
              <Avatar :url="cand.url" star></Avatar>
              <p>{{cand.id}}</p>
            </div>
          </div>
          <div class="candidate-list">
            <h1>Candidate List <span style="color: #91ADFF;">- {{candidatesList.length}}</span></h1>
            <p>The Custodian Board manages the operations and affairs of the DAC, including but not limited to the governance and administration of the assets and liabilities of the DAC. The following DAC members have vested some of their tokens to submit themselves and candidates for a position on the custodian board which last for 7 days. Every 7 days, your votes are recalculated to select who will be part of the next custodian board. Voting is important! Please vote often and stay engaged within the DAC to know who is providing value</p>
            <CandidateCollapse v-for="candidate in candidatesList" :key="candidate.id" @select="handleSelect" v-bind="candidate"></CandidateCollapse>
          </div>
        </div>
      </el-main>
      <el-aside width="320px">
        <div class="vote-panel">
          <h1>
            My Vote <span style="color: #91ADFF;">{{selectedCandidates.length}}/5</span>
          </h1>
          <p>
            You can vote for up to 5 custodian candidates at a time. Please select candidates who you think will bring value to the DAC.
          </p>
          <div class="selected-candidates card">
            <div class="vote-button" :class="{'vote-button-active': selectedCandidates.length}">
              {{selectedCandidates.length ? 'SUBMIT MY VOTES' : 'MY VOTES'}}
            </div>
            <div v-for="candidate in selectedCandidates" :key="candidate.id" class="selected-candidate-card">
              <Avatar size="35px" :url="candidate.image"></Avatar>
              <p>{{candidate.givenName}} {{candidate.familyName}}</p>
              <div class="remove-button" @click="removeCandidate(candidate.id)">
                <i class="el-icon-close"></i>
              </div>
            </div>
          </div>
        </div>
        <router-link :to="{path: '/auditor/register'}">
          <div class="button"
            style="border-radius:6px;width:90%;height:40px;line-height:30px"
          >
            Register as Candidate
          </div>
        </router-link>
      </el-aside>

    </el-container>

  </div>
</template>

<script>
import Avatar from '@/components/Avatar.vue'
import CandidateCollapse from '@/components/CandidateCollapse.vue'
export default {
  name: 'Auditor',
  components: {
    Avatar,
    CandidateCollapse
  },
  data () {
    return {
      selectedCandidates: [],
      candidatesList: []
    }
  },
  created () {
    this.getCandidates()
  },
  methods: {
    getCandidates () {
      for (let i = 0; i < 10; i++) {
        this.candidatesList.push({
          description: '**DACs are the future, lets help build them!**↵↵I am part of the launch team of eosDAC and have been serving as an interim custodian since launch.↵↵**미래는 DAC입니다. DAC을 만들고 돕는 데 함께 합시다!**↵↵저는 eosDAC 런칭팀 일원으로 런칭 후 임시 관리인으로 일해 왔습니다.↵↵### Block Production↵↵Since before the launch of the chain I have been involved in EOS testnets going right back to the days of the Superhero Network. Was heavily involved in the launch of the mainnet and I now have over 1 years experience running EOSIO software. Since launch I have been leading the block production team at eosDAC and we have consistently been within the top 21.↵↵We currently have over 17 machines running our infrastructure and are running on 5 networks (including 2 live blockchains). I have been working closely with members of Block One to find and fix bugs in the EOSIO software and I earned the bug bounty for finding a bug in EOSIO which could crash any node.↵↵### 블록 생산↵↵체인 런칭 전부터 수퍼히어로 네트워크 시절까지 포함하여 EOS 테스트 넷에 참여해 왔습니다. 저는 메인넷 런칭에 아주 깊게 관여했으며, 이제 1년 이상의 EOSIO 소프트웨어 실행 경험을 보유하고 있습니다. 런칭 이래로 저는 eosDAC에서 블록 생산 팀을 이끌어 왔으며 eosDAC은 계속적으로 상위 21에 머물렀습니다.↵↵eosDAC의 인프라는 현재 17대 이상의 머신이 돌아가고 있으며 총 5개 네트워크 (메인 네트워크 둘 포함)에서 구동 중입니다. 저는 EOSIO 소프트웨어의 버그를 찾고 고치기 위해 Block One 멤버들과 긴밀히 협력해 왔으며, 어떠한 노드도 충돌하게 만드는 EOSIO 상의 버그를 발견하여 버그 바운티를 받은 적도 있습니다.↵↵### Software Architect↵↵I have been the Senior Developer working on the eosDAC suite of contracts, including the voting, token and supporting contracts. We have built the major contracts which support election, as well as designing the permissions layout and a nicely designed front-end to make it possible for non-technical people to be involved in running a DAC.↵↵### 소프트웨어 아키텍트↵↵저는 투표, 토큰 및 지원 계약 등 eosDAC의 여러 계약을 쓴 시니어 개발자입니다. 우리는 투표를 가능하게 하는 주요 계약을 만들었으며 퍼미션 레이아웃을 디자인하고 기술과 관련 없는 사람들도 DAC을 운영할 수 있도록 프론트 엔드 역시 멋지게 디자인했습니다.↵↵### Community support↵↵As part of our work in enabling DACs - I have been involved in supporting other DAPPs within the EOS ecosystem as a technical advisor. I was a mentor at the London EOS Hackathon in 2018, and regularly support and engage with the EOS community. I will always fight for the interests of the token holders when representing eosDAC and hope that we can be a strong voice in the future of the chain.↵↵### 커뮤니티 서포트↵↵DAC 활성화에 대한 노력의 일환으로 저는 EOS 생태계 내의 다른 DAPP들을 기술 자문으로서 지원해 왔습니다. 2018년 런던 EOS 해커톤에 멘터로 참여했고, 정기적으로 EOS 커뮤니티를 지원하고 참여했습니다. 제가 eosDAC을 대표한다면 언제나 토큰 홀더의 이익을 위해 싸울 것이며 미래 블록체인에서 우리가 함께 강력한 목소리를 낼 수 있기를 바랍니다.↵↵### The Future↵↵We have achieved a lot so far at eosDAC and it has been much harder than any of us imagined, but there is still a long way to go. I hope that we can continue the work done so far and prove that the concept of a DAC can work, even across international and cultural boundaries.↵↵### 미래↵↵eosDAC은 지금까지 많은 성과를 거두었으며, 많은 것들이 우리가 상상했던 것보다 훨씬 어려웠지만, 아직도 갈 길이 멉니다. 지금까지 달성한 일들을 계속 진행하고 DAC의 개념이 국제적 및 문화적 경계를 넘어서도 작동할 수 있기를 바라는 바입니다.',
          email: '',
          familyName: 'Yeates',
          gender: '',
          givenName: 'Michael',
          image: 'https://i.imgur.com/fF721W7.jpg',
          sameAs: [],
          timezone: 0,
          url: '',
          id: 'mryeatesher' + i,
          isSelected: false
        })
      }
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
      for (let i = 0; i < this.candidatesList.length; i++) {
        if (this.candidatesList[i].id === id) {
          this.candidatesList[i].isSelected = true
          this.selectedCandidates.push(this.candidatesList[i])
          break
        }
      }
    },
    removeCandidate (id) {
      for (let i = 0; i < this.selectedCandidates.length; i++) {
        if (this.selectedCandidates[i].id === id) {
          this.selectedCandidates.splice(i, 1)
          break
        }
      }
      for (let i = 0; i < this.candidatesList.length; i++) {
        if (this.candidatesList[i].id === id) {
          this.candidatesList[i].isSelected = false
          break
        }
      }
    }
  }
}
</script>

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
  width 90%
  p
    font-family: Roboto-Regular;
    font-size: 12px;
    color: #507DFE;
    letter-spacing: 0;
  .vote-button
    height 36px
    width auto
    line-height 36px
    background: #7599FF;
    border-radius: 6px;
    font-family: Roboto-Medium;
    font-size: 16px;
    color: #FFFFFF;
    letter-spacing: 0;
    text-align center
  .vote-button-active
    background: #527FFF;
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
