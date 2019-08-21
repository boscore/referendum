# 定时下载任务


tmp=`wget https://api.boswps.com/eosio.forum/proposal/latest.json`
find latest.json -exec mv {} /root/referendum/api/proposal_tally.json \;



tmp=`wget https://s3.amazonaws.com/bos.referendum/referendum/summaries/latest.json`
find latest.json -exec mv {} /root/referendum/api/vote_total_tally.json \;


tmp=`wget https://api.boswps.com/referendum/auditor.tallies/latest.json`
find latest.json -exec mv {} /root/referendum/api/auditor_tally.json \;
