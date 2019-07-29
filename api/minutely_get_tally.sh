# 定时下载任务
wget https://s3.amazonaws.com/bos.referendum/referendum/tallies/latest.json | mv latest.json proposal_tally.json

wget https://s3.amazonaws.com/bos.referendum/referendum/summaries/latest.json && mv latest.json vote_total_tally.json

wget https://s3.amazonaws.com/bos.referendum/referendum/auditor.tallies/latest.json && mv latest.json auditor_tally.json
