import { MessageBox as MbMessageBox } from 'mint-ui'
import { MessageBox } from 'element-ui'

export default {
  alert,
  dateConvert,
  errorFormat,
  toThousands,
  isExpired,
  isPC,
  transSpecialChar,
  unTransSpecialChar
}

function dateConvert (date) {
  // convert date to current time zone
  const diff = new Date().getTimezoneOffset()
  const newDateTime = new Date(date).getTime() + (diff * 60 * 1000)
  const newDate = new Date(newDateTime)
  function formatNumber (n) {
    if (n < 10) {
      return '0' + n
    }
    return n
  }
  return `${newDate.getFullYear()}-${formatNumber(newDate.getMonth() + 1)}-${formatNumber(newDate.getDate())} ${formatNumber(newDate.getHours())}:${formatNumber(newDate.getMinutes())}`
}

function toThousands (num) {
  return (num || 0).toString().replace(/(\d)(?=(?:\d{3})+$)/g, '$1,')
}

function isExpired (exporiesAt) {
  let now = new Date().getTime() + (new Date().getTimezoneOffset() * 60 * 1000)
  let expiry = new Date(exporiesAt).getTime()
  if (expiry < now) {
    return true
  }
  return false
}

function transSpecialChar (json) {
  if (json !== undefined && json !== '' && json !== 'null') {
    json = json.replace(/\r/g, '\\r')
    json = json.replace(/\n/g, '\\n')
    json = json.replace(/\t/g, '\\t')
    json = json.replace(/\\/g, '\\\\')
    json = json.replace(/\\"/g, '\\\\"')
  }
  return json
}

function unTransSpecialChar (json) {
  if (json !== undefined && json !== '' && json !== 'null') {
    json = json.replace(/\\r/g, '\r')
    json = json.replace(/\\n/g, '\n')
    json = json.replace(/\\t/g, '\t')
    json = json.replace(/\\\\/g, '\\')
    json = json.replace(/\\\\"/g, '\\"')
  }
  return json
}

function isPC () {
  const userAgentInfo = navigator.userAgent
  const Agents = ['Android', 'iPhone', 'SymbianOS', 'Windows Phone', 'iPad', 'iPod']
  let flag = true
  for (let i = 0; i < Agents.length; i++) {
    if (userAgentInfo.indexOf(Agents[i]) > 0) {
      flag = false
      break
    }
  }
  return flag
}

function errorFormat (e) {
  let error = e
  if (typeof e === 'string') {
    try {
      error = JSON.parse(e)
    } catch (jsonError) {
      error = {
        message: e
      }
    }
  }
  return error
}

function alert (title, msg) {
  if (isPC()) {
    MessageBox.alert(msg, title, {
      confirmButtonText: 'OK'
    })
  } else {
    MbMessageBox.alert(msg, title, {
      confirmButtonText: 'OK'
    })
  }
}
