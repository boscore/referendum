pwd=`pwd`
env="export GOOGLE_APPLICATION_CREDENTIALS='${pwd}/GOOGLE_APPLICATION_CREDENTIALS.json'"
env >> /etc/profile
source /etc/profile