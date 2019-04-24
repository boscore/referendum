<template>
  <div class="create-proposal">
    <el-container>
      <el-main>
      <h2>Create Proposal</h2>
      <div class="card">
        <el-form ref="form" :model="form" label-position="top" label-width="110px">
          <el-form-item >
            <label slot="label">Proposer</label>
            <span>{{proposer}}</span>
          </el-form-item >
          <el-form-item>
            <label slot="label">Proposal Name</label>
            <el-input v-model="form.name"></el-input>
          </el-form-item>
          <el-form-item>
            <label slot="label">Title</label>
            <el-input v-model="form.title"></el-input>
          </el-form-item>
          <el-form-item>
            <label slot="label">Content</label>
            <el-input v-model="form.content" type="textarea" :rows="5"></el-input>
          </el-form-item>
          <el-form-item>
            <label slot="label">Expiry</label>
            <el-date-picker type="date" placeholder="expiry date" v-model="form.expiry"></el-date-picker>
          </el-form-item>
          <el-form-item>
            <label slot="label">Type</label>
            <el-select v-model="form.type">
              <el-option
                v-for="item in typeOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value"
              ></el-option>
            </el-select>
          </el-form-item>
          <el-form-item>
            <div class="button">
              Propose
            </div>
          </el-form-item>
        </el-form>
      </div>
      </el-main>
    </el-container>
  </div>
</template>

<script>
export default {
  name: 'CreateProposal',
  data () {
    return {
      proposer: 'eosuper',
      typeOptions: [
        {
          label: 'Referendum',
          value: 'referendum-v1'
        },
        {
          label: 'Poll',
          value: 'poll_default'
        }
      ],
      form: {
        name: '',
        title: '',
        content: '',
        expiry: '',
        type: 'referendum-v1'
      }
    }
  }
}
</script>

<style lang="stylus" scoped>
.create-proposal
  h2
    color: #507DFE;
.card
  text-align left
  max-width 700px
  padding 22px 50px
  margin auto
  margin-bottom 70px
  label
    color: #507DFE;
.button
  display block
  border-radius: 4px;
  height 40px
  line-height 30px
  width 400px
  margin auto
  margin-top 20px
</style>
