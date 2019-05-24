module.exports = {
  apps: [
    {
      name: "bos-referendum-vote-tally",
      script: 'index.ts',
      autorestart: true,
      log_date_format : "YYYY-MM-DD HH:mm"
    }
  ]
};