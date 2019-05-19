# OSX must be installed brew first
# Debian and Ubuntu repositories. Install using sudo apt-get install jq.
# Fedora repository. Install using sudo dnf install jq.
# openSUSE repository. Install using sudo zypper install jq.
# repository. Install using sudo pacman -Sy jq.

cleos='cleos -u https://bos.eoshenzhen.io:9443'
more=true
lower=0
while [ $more == true ]
do
 echo $lower"--------------"
 ret=`$cleos get table -l 1000 -L $lower eosio eosio voters `
 more=`echo $ret | jq .more`
 length=`echo $ret | jq '.rows | length'`
 lower=`echo $ret | jq ".rows[$(($length-1))].owner"`
 lower="${lower%\"}" && lower="${lower#\"}"

 if [ $more == true ];then
  ret_rows=`echo $ret | jq ".rows| .[:$(($length-1))]"`
 else
  ret_rows=`echo $ret | jq '.rows'`
 fi

 for row in $(echo "${ret_rows}" | jq -r '.[] | @base64'); do
     _jq() {
      echo ${row} | base64 --decode | jq -r ${1}
     }
     printf "%-15s %-10s\n" $(_jq '.owner') $(_jq '.staked')
 done
done