<template>
  <div class="footer">
    <div class="footer-left">
      <img src="@/assets/logo-white.png"/>
    </div>
    <div class="footer-right">
      <div class="footer-links">
        <div>
          <h1>BOSCore</h1>
          <a>Resource</a>
        </div>
        <div>
          <h1>SOCIAL</h1>
          <div class="social-list clear-float">
              <a target="_blank" href="https://github.com/boscore">
                <img src="@/assets/github-logo.png" />
              </a>
              <a target="_blank" href="https://twitter.com/Boscore_BOS">
                <img src="@/assets/twitter-logo.png"/>
              </a>
              <a target="_blank" href="https://t.me/boscorecommunity">
                <img src="@/assets/telegram-logo.png"/>
              </a>
          </div>
        </div>
      </div>
      <div>
        <p>support@boscore.io | Terms of Use | Privacy Policy</p>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Footer'
}
</script>

<style lang="stylus" scoped>
.footer
  width 100%
  min-height 160px
  background-image linear-gradient(270deg, #5B87FF 0%, #4CB7FF 100%)
  display flex
  flex-wrap wrap
  justify-content space-between
  align-items center
  padding 34px
  box-sizing border-box
.footer-left
  img
    height 34px
    width 158px
.footer-right
  font-family: PingFangSC-Semibold;
  font-size: 14px;
  color: #FFFFFF;
  letter-spacing: 0;
  text-align: center;
  .footer-links
    display flex
    justify-content space-around
  h1
    font-size 16px
    text-align left
  .social-list
    width 100px
    display flex
    justify-content space-between
@media only screen and (max-width 450px)
  .footer
    justify-content center
</style>
