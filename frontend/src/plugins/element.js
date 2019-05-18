import Vue from 'vue'
import {
  Aside,
  Button,
  Checkbox,
  Container,
  Collapse,
  CollapseItem,
  DatePicker,
  Dialog,
  Footer,
  Form,
  FormItem,
  Header,
  Input,
  Loading,
  Main,
  Menu,
  MenuItem,
  Progress,
  Option,
  Radio,
  RadioGroup,
  Select,
  Submenu,
  Steps,
  Step,
  Tabs,
  TabPane,
  Table,
  TableColumn,
  Tag
} from 'element-ui'
import lang from 'element-ui/lib/locale/lang/en'
import locale from 'element-ui/lib/locale'

locale.use(lang)

Vue.use(Button)
Vue.use(Loading)

Vue.use(Collapse)
Vue.use(CollapseItem)

Vue.use(Progress)

Vue.use(Menu)
Vue.use(MenuItem)
Vue.use(Submenu)

Vue.use(Container)
Vue.use(Footer)
Vue.use(Header)
Vue.use(Aside)
Vue.use(Main)

Vue.use(Steps)
Vue.use(Step)

Vue.use(Tabs)
Vue.use(TabPane)

Vue.use(Table)
Vue.use(TableColumn)

Vue.use(Tag)

Vue.use(Form)
Vue.use(FormItem)
Vue.use(Select)
Vue.use(Option)
Vue.use(Input)
Vue.use(DatePicker)

Vue.use(Radio)
Vue.use(RadioGroup)

Vue.use(Checkbox)

Vue.use(Dialog)
