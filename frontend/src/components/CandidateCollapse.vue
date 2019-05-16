<template>
  <div class="candidate-collapse" :style="{border: selected ? '2px solid #21ba45':'2px solid #ffffff',height: isActive ? '446px' : '96px'}">
        <div class="collapse-header" >
          <div class="title-bar">
            <div class="choose-button" @click="select()" :class="{'is-selected' : selected}">
              <i class="el-icon-plus" v-if="!selected"></i>
              <i class="el-icon-check" v-if="selected"></i>
            </div>
            <Avatar style="margin: 0 18px" :url="inform.avatar" size="48px"></Avatar>
            <div class="candidate-info">
              <p class="name">
                {{id}}
                <span v-if="(givenName || familyName)" style="font-family: Roboto-Regular">{{`(${givenName} ${familyName})`}}</span>
              </p>
              <p class="vote">
                Votes: {{formVotes}} &nbsp;&nbsp; Staked:{{staked}}
              </p>
            </div>
            <i
              @click="isActive = !isActive"
              class="collapse-item__arrow el-icon-arrow-right"
              :class="{'is-active': isActive}">
            </i>
          </div>
        </div>
        <div class="collapse-wrap" :style="{display: isActive ? 'block' : 'none'}">
          <div class="title">
            <h1>BIO</h1>
            <p v-if="contact" style="float:right">Email: {{contact}}</p>
          </div>
          <div v-html="desc" class="content"></div>
        </div>
  </div>
</template>

<script>
import Avatar from '@/components/Avatar.vue'
import marked from 'marked'
export default {
  name: 'CandidateCollapse',
  props: {
    inform: {
      type: Object,
      default () {
        return {}
      }
    },
    votes: {
      type: [String, Number],
      default: ''
    },
    staked: {
      type: [String, Number],
      default: ''
    },
    familyName: {
      type: String,
      default: ''
    },
    givenName: {
      type: String,
      default: ''
    },
    id: {
      type: String,
      default: ''
    },
    isSelected: {
      type: Boolean,
      default: false
    }
  },
  data () {
    return {
      isActive: false,
      isSelectedInner: false
    }
  },
  components: {
    Avatar
  },
  computed: {
    contact () {
      if (this.inform) {
        return this.inform.contact
      }
      return null
    },
    formVotes () {
      if (this.votes) {
        return (this.votes / 10000).toFixed(4)
      }
      return '0.0000'
    },
    selected () {
      return this.isSelected
    },
    desc () {
      if (this.inform.bio) {
        return marked(this.inform.bio.replace(/↵/g, '\n'), { sanitize: true })
      } else {
        return ''
      }
    }
  },
  methods: {
    select () {
      this.$emit('select', {
        isSelected: !this.isSelected,
        id: this.id
      })
    }
  }
}
</script>

<style lang="stylus" scoped>
.candidate-collapse
  padding 10px
  width 100%
  background: #FCFDFF;
  box-shadow: 0 2px 4px 0 #B0D9FF;
  border-radius: 8px;
  margin-bottom 22px
  box-sizing border-box
  transition .3s height
.choose-button
  height 48px
  min-width 48px
  border-radius 50%
  background #507DFE
  text-align center
  transition .3s
  &:hover
    opacity 0.8
  i
    line-height 48px
    color #ffffff
    font-size 16px
.is-selected
  background #21ba45
.title-bar
  padding 10px
  display flex
.candidate-info
  color: #507DFE;
  display flex
  flex-direction column
  justify-content space-around
  p
    margin 0
  .name
    font-family: Roboto-Medium;
    font-size: 14px;
    line-height 16px
    letter-spacing: 0;
  .vote
    font-family: Roboto-Regular;
    font-size: 12px;
    line-height 14px
.collapse-item__arrow
  font-size 24px
  color #737373
  margin auto 8px auto auto
  transition transform 0.3s
  font-weight 300
  cursor pointer
.is-active
  transform rotate(90deg)
.collapse-header
  height 72px
.collapse-wrap
  border-top 1px solid #507DFE
  height 350px
  .title
    height 50px
    display flex
    color #507DFE
    justify-content space-between
    align-items center
    h1
      font-family: Roboto-Medium;
      font-size: 18px;
    p
      font-family: Roboto-Regular;
      font-size: 11px;
  .content
    overflow auto
    max-height 300px
    >>> img
     max-width 100%
</style>
