// date String
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
  return `${newDate.getFullYear()}-${formatNumber(newDate.getMonth())}-${formatNumber(newDate.getDate())} ${formatNumber(newDate.getHours())}:${formatNumber(newDate.getMinutes())}`
}

function toThousands (num) {
  return (num || 0).toString().replace(/(\d)(?=(?:\d{3})+$)/g, '$1,')
}

export default {
  dateConvert,
  toThousands
}
