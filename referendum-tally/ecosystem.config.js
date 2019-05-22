module.exports = {
  apps: [
    {
      name: "bos-referendum-tally",
      script: 'index.ts',
      autorestart: true,
      log_date_format : "YYYY-MM-DD HH:mm"
    }
  ]
};