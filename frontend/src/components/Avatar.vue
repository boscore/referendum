<template>
  <div :style="avatarSize" class="avatar">
    <img class="avatar-img" :src="url" onerror="this.style.display='none'"/>
    <img class="default-avatar" :src="defaultAvatar">
    <img class="star" v-if="star" :src="starImage"/>
  </div>
</template>

<script>
import defaultAvatar from '@/assets/avatar.png'
import starImage from '@/assets/star.png'
export default {
  name: 'Avatar',
  props: {
    url: {
      type: String,
      default: defaultAvatar
    },
    star: {
      type: Boolean,
      default: false
    },
    size: {
      type: [String, Number],
      default: 52
    }
  },
  data () {
    return {
      starImage: starImage,
      defaultAvatar: defaultAvatar
    }
  },
  computed: {
    avatarSize () {
      if (typeof this.size === 'number') {
        return {
          width: `${this.size}px`,
          height: `${this.size}px`
        }
      } else {
        return {
          'min-width': this.size,
          height: this.size
        }
      }
    }
  }
}
</script>

<style lang="stylus" scoped>
.avatar
  position relative
  border-radius 50%
  border: 2px solid #88A7FF;
.star
  width 18px
  height 18px
  position absolute
  right 0
  bottom 0
  z-index 999
.avatar-img
  height 100%
  width 100%
  border-radius 50%
  position absolute
  left 0
  z-index 998
.default-avatar
  height 100%
  width 100%
  border-radius 50%
  position absolute
  left 0
  z-index 996
</style>
