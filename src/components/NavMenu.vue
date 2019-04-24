<template>
  <div class="nav-menu">
    <el-menu :default-active="activeIndex" mode="horizontal" class="el-menu-demo clear-float">
      <el-menu-item index="home" @click="$router.push('/')">
        <img src="@/assets/logo.png">
      </el-menu-item>
      <el-menu-item index="referendum" @click="$router.push('/referendum')">{{$t('common.referendum')}}</el-menu-item>
      <el-menu-item index="auditor" @click="$router.push('/auditor')">{{$t('common.auditor')}}</el-menu-item>
    </el-menu>
  </div>
</template>

<script>
export default {
  name: 'NavMenu',
  data () {
    return {
    }
  },
  computed: {
    activeIndex () {
      return this.$route.name
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
