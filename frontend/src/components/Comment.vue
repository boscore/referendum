<template>
  <div class="comment">
    <div class="comment-header clear-float">
      <div class="comment-avatar">
        <img :src="avatar" onerror="this.style.display='none'" />
      </div>
      <div class="comment-info">
        <p class="comment-commenter">{{name}}</p>
        <p class="comment-time">{{$util.dateConvert(time)}}</p>
      </div>
    </div>
    <div class="comment-content">{{comment}}</div>
  </div>
</template>

<script>
export default {
  name: 'Comment',
  props: {
    avatar: [String],
    name: {
      type: String,
      default: 'unknown'
    },
    time: {
      type: String,
      default: '1 hour ago'
    },
    comment: {
      type: String,
      default: 'no comment'
    }
  }
}
</script>

<style lang="stylus" scoped>
.comment
  padding 20px 0
  border-bottom 1px solid #D8D8D8
  .comment-avatar
    float left
    height 38px
    width 38px
    border-radius 50%
    background: #F5F3F3;
    border: 2px solid #88A7FF
  .comment-info
    float left
    margin-left 10px
    p
      margin 0
    .comment-commenter
      margin-bottom 8px
      font-family: Roboto-Regular;
      font-size: 14px;
      line-height 16px
      color: #507DFE;
      letter-spacing: 0;
    .comment-time
      font-family: Roboto-Regular;
      font-size: 12px;
      line-height 14px
      color: #C1C1C1;
      letter-spacing: 0;
  .comment-content
    overflow-wrap break-word
    margin-top  20px
    font-family: Roboto-Regular;
    font-size: 14px;
    color: #707070;
    letter-spacing: 0;
</style>
