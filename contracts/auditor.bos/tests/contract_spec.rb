require 'rspec'
require 'rspec_command'
require "json"

# 1. A recent version of Ruby is required
# 2. Ensure the required gems are installed with `gem install rspec json rspec-command`
# 3. Run this from the command line with rspec contract_spec.rb

# Optionally output the test results with -f [p|d|h] for required views of the test results.

RSpec.configure do |config|
  config.include RSpecCommand
end

EOSIO_PUB = 'EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV'
EOSIO_PVT = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'

CONTRACT_OWNER_PRIVATE_KEY = '5K86iZz9h8jwgGDttMPcHqFHHru5ueqnfDs5fVSHfm8bJt8PjK6'
CONTRACT_OWNER_PUBLIC_KEY = 'EOS6Y1fKGLVr2zEFKKfAmRUoH1LzM7crJEBi4dL5ikYeGYqiJr6SS'

CONTRACT_ACTIVE_PRIVATE_KEY = '5Jbf3f26fz4HNWXVAd3TMYHnC68uu4PtkMnbgUa5mdCWmgu47sR'
CONTRACT_ACTIVE_PUBLIC_KEY = 'EOS7rjn3r52PYd2ppkVEKYvy6oRDP9MZsJUPB2MStrak8LS36pnTZ'

TEST_OWNER_PRIVATE_KEY = '5K86iZz9h8jwgGDttMPcHqFHHru5ueqnfDs5fVSHfm8bJt8PjK6'
TEST_OWNER_PUBLIC_KEY = 'EOS6Y1fKGLVr2zEFKKfAmRUoH1LzM7crJEBi4dL5ikYeGYqiJr6SS'

TEST_ACTIVE_PRIVATE_KEY = '5Jbf3f26fz4HNWXVAd3TMYHnC68uu4PtkMnbgUa5mdCWmgu47sR'
TEST_ACTIVE_PUBLIC_KEY = 'EOS7rjn3r52PYd2ppkVEKYvy6oRDP9MZsJUPB2MStrak8LS36pnTZ'

CONTRACT_NAME = 'daccustodian'
ACCOUNT_NAME = 'daccustodian'

TOKENCONTRACT = 'eosio.token'
AUTHACCOUNT = 'auditor.bos'

CONTRACTS_DIR = 'dependencies'

def configure_wallet
  beforescript = <<~SHELL

  cleos wallet unlock --password `cat ~/eosio-wallet/.pass`
  cleos wallet import --private-key #{CONTRACT_ACTIVE_PRIVATE_KEY}
  cleos wallet import --private-key #{TEST_ACTIVE_PRIVATE_KEY}
  cleos wallet import --private-key #{TEST_OWNER_PRIVATE_KEY}
  cleos wallet import --private-key #{EOSIO_PVT}
  SHELL

  `#{beforescript}`
end

# @param [eos account name for the new account] name
# @param [if not nil amount of BOS to issue to the new account] issue
# @param [if not nil register the account with the agreed terms as this value] memberreg
# @param [if not nil transfer this amount to the elections contract so they can register as an election candidate] stake
# @param [if not nil register as a candidate with this amount as the requested pay] requestedpay
def seed_account(name, issue: nil, memberreg: nil, stake: nil, requestedpay: nil)
  `cleos system newaccount --stake-cpu "10.0000 EOS" --stake-net "10.0000 EOS" --transfer --buy-ram-kbytes 1024 eosio #{name} #{TEST_OWNER_PUBLIC_KEY} #{TEST_ACTIVE_PUBLIC_KEY}`

  unless issue.nil?
    `cleos push action eosio.token issue '{ "to": "#{name}", "quantity": "#{issue}", "memo": "Initial amount."}' -p eosio`
    `cleos system delegatebw #{name} #{name} "0.0000 EOS" "#{issue}" -p #{name}`
  end

  # unless memberreg.nil?
  #   `cleos push action eosio.token memberreg '{ "sender": "#{name}", "agreedterms": "#{memberreg}"}' -p #{name}`
  # end

  unless stake.nil?
    `cleos push action eosio.token issue '{ "to": "#{name}", "quantity": "#{stake}", "memo": "Initial amount."}' -p eosio`
    `cleos push action eosio.token transfer '{ "from": "#{name}", "to": "daccustodian", "quantity": "#{stake}","memo":"daccustodian"}' -p #{name}`
  end

  unless requestedpay.nil?
    `cleos push action daccustodian nominatecand '{ "cand": "#{name}" }' -p #{name}`
  end
end

def reset_chain
  `kill -INT \`pgrep nodeos\``

  # Launch nodeos in a new tab so the output can be observed.
  # ttab is a nodejs module but this could be easily achieved manually without ttab.
  `ttab 'nodeos --delete-all-blocks --verbose-http-errors'`

  # nodeos --delete-all-blocks --verbose-http-errors &>/dev/null & # Alternative without ttab installed

  puts "Give the chain a chance to settle."
  sleep 4

end

def seed_system_contracts

  beforescript = <<~SHELL
   set -x

  cleos set contract eosio #{CONTRACTS_DIR}/eosio.bios -p eosio
  echo `pwd`
  cleos create account eosio eosio.msig #{EOSIO_PUB}
  cleos get code eosio.msig
  echo "eosio.msig"
  cleos create account eosio eosio.token #{EOSIO_PUB}
  cleos create account eosio eosio.ram #{EOSIO_PUB}
  cleos create account eosio eosio.ramfee #{EOSIO_PUB}
  cleos create account eosio eosio.names #{EOSIO_PUB}
  cleos create account eosio eosio.stake #{EOSIO_PUB}
  cleos create account eosio eosio.saving #{EOSIO_PUB}
  cleos create account eosio eosio.bpay #{EOSIO_PUB}
  cleos create account eosio eosio.vpay #{EOSIO_PUB}
  cleos create account eosio #{ACCOUNT_NAME} #{CONTRACT_OWNER_PUBLIC_KEY} #{CONTRACT_ACTIVE_PUBLIC_KEY}
   cleos create account eosio dacauthority #{CONTRACT_OWNER_PUBLIC_KEY} #{CONTRACT_ACTIVE_PUBLIC_KEY}
   cleos create account eosio bosauditfund #{CONTRACT_OWNER_PUBLIC_KEY} #{CONTRACT_ACTIVE_PUBLIC_KEY}
   cleos create account eosio dacocoiogmbh #{CONTRACT_OWNER_PUBLIC_KEY} #{CONTRACT_ACTIVE_PUBLIC_KEY}
  cleos push action eosio setpriv  '["eosio.msig",1]' -p eosio
  cleos set contract eosio.msig #{CONTRACTS_DIR}/eosio.msig -p eosio.msig
  cleos set contract eosio.token #{CONTRACTS_DIR}/eosio.token -p eosio.token
  cleos push action eosio.token create '["eosio","10000000000.0000 EOS"]' -p eosio.token
  cleos push action eosio.token issue '["eosio", "100000.0000 EOS", "Initial EOS amount."]' -p eosio
  cleos set contract eosio #{CONTRACTS_DIR}/eosio.system -p eosio
  SHELL

  `#{beforescript}`
  exit() unless $? == 0
end

def install_contracts

  beforescript = <<~SHELL
   # set -x

   # Setup the inital permissions.
   cleos set account permission dacauthority owner '{"threshold": 1,"keys": [{"key": "#{CONTRACT_ACTIVE_PUBLIC_KEY}","weight": 1}],"accounts": [{"permission":{"actor":"daccustodian","permission":"eosio.code"},"weight":1}]}' '' -p dacauthority@owner
   # cleos set account permission bosauditfund active '{"threshold": 1,"keys": [{"key": "#{CONTRACT_ACTIVE_PUBLIC_KEY}","weight": 1}],"accounts": [{"permission":{"actor":"daccustodian","permission":"eosio.code"},"weight":1}]}' owner -p bosauditfund@owner
   cleos set account permission bosauditfund xfer '{"threshold": 1,"keys": [{"key": "#{CONTRACT_ACTIVE_PUBLIC_KEY}","weight": 1}],"accounts": [{"permission":{"actor":"daccustodian","permission":"eosio.code"},"weight":1}]}' active -p bosauditfund@active
   cleos set account permission daccustodian xfer '{"threshold": 1,"keys": [{"key": "#{CONTRACT_ACTIVE_PUBLIC_KEY}","weight": 1}],"accounts": [{"permission":{"actor":"daccustodian","permission":"eosio.code"},"weight":1}]}' active -p daccustodian@active
   cleos push action eosio.token issue '["bosauditfund", "1000.0000 EOS", "Initial EOS amount."]' -p eosio

   cleos set action permission bosauditfund eosio.token transfer xfer
   cleos set action permission daccustodian eosio.token transfer xfer  
 
   cleos set account permission dacauthority high #{CONTRACT_OWNER_PUBLIC_KEY} owner -p dacauthority@owner 
   cleos set account permission dacauthority med #{CONTRACT_OWNER_PUBLIC_KEY} high -p dacauthority@owner 
   cleos set account permission dacauthority low #{CONTRACT_OWNER_PUBLIC_KEY} med -p dacauthority@owner 
   cleos set account permission dacauthority one #{CONTRACT_OWNER_PUBLIC_KEY} low -p dacauthority@owner   

   cleos set account permission #{ACCOUNT_NAME} active '{"threshold": 1,"keys": [{"key": "#{CONTRACT_ACTIVE_PUBLIC_KEY}","weight": 1}],"accounts": [{"permission":{"actor":"daccustodian","permission":"eosio.code"},"weight":1}]}' owner -p #{ACCOUNT_NAME}

  #  source output/unit_tests/compile.sh
  #  if [[ $? != 0 ]] 
  #    then 
  #    echo "failed to compile contract" 
  #    exit 1
  #  fi
   # cd ..
   cleos set contract #{ACCOUNT_NAME} .. auditor.wasm auditor.abi
   

  SHELL

  `#{beforescript}`
  exit() unless $? == 0
end


# Configure the initial state for the contracts for elements that are assumed to work from other contracts already.
def configure_contracts
  # configure accounts for eosio.token
#  `cleos push action eosio.token issue '{ "to": "bosauditfund", "quantity": "100000.0000 EOS", "memo": "Initial EOS amount."}' -p eosio`

  #create users
  # Ensure terms are registered in the token contract
  #`cleos push action eosio.token newmemterms '{ "terms": "normallegalterms", "hash": "New Latest terms"}' -p eosio`

  #create users
  seed_account("testreguser1", issue: "100.0000 EOS", memberreg: "New Latest terms")
  seed_account("testreguser2", issue: "100.0000 EOS")
  seed_account("testreguser3", issue: "100.0000 EOS", memberreg: "")
  seed_account("testreguser4", issue: "100.0000 EOS", memberreg: "old terms")
  seed_account("testreguser5", issue: "100.0000 EOS", memberreg: "New Latest terms")
  seed_account("testregusera", issue: "100.0000 EOS", memberreg: "New Latest terms")
end

describe "eosdacelect" do
  before(:all) do
    configure_wallet
    seed_system_contracts
    install_contracts
    configure_contracts
  end

  describe "updateconfig" do
    context "before being called with token contract will prevent other actions from working" do
      context "with valid and registered member" do
        command %(cleos push action daccustodian nominatecand '{ "cand": "testreguser1", "bio": "any bio", "authaccount": "dacauthority", "auththresh": 3}' -p testreguser1), allow_error: true
        its(:stderr) {is_expected.to include('Error 3050003')}
      end
    end

    context "with invalid auth" do
      command %(cleos push action daccustodian updateconfig '{"newconfig": {"lockupasset": "13.0000 EOS", "maxvotes": 4, "auditor_tenure": 604800 , "numelected": 12, "authaccount": "dacauthority", "auththresh": 3, "initial_vote_quorum_percent": 15, "vote_quorum_percent": 10, "auth_threshold_high": 11, "auth_threshold_mid": 7, "auth_threshold_low": 3, "lockup_release_time_delay": 10 }}' -p testreguser1), allow_error: true
      its(:stderr) {is_expected.to include('Error 3090004')}
    end

    context "with valid auth" do
      command %(cleos push action daccustodian updateconfig '{"newconfig": {"lockupasset": "10.0000 EOS", "maxvotes": 5, "auditor_tenure": 604800 , "numelected": 12, "authaccount": "dacauthority",  "auththresh": 3, "initial_vote_quorum_percent": 15, "vote_quorum_percent": 10, "auth_threshold_high": 11, "auth_threshold_mid": 7, "auth_threshold_low": 3, "lockup_release_time_delay": 10 }}' -p daccustodian), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::updateconfig')}
    end
  end

  describe "nominatecand" do

    context "with valid and registered member after transferring insufficient staked tokens" do
      before(:all) do
        `cleos push action eosio.token issue '{ "to": "testreguser1", "quantity": "10.0000 EOS", "memo": "Initial amount."}' -p eosio`
        `cleos push action eosio.token transfer '{ "from": "testreguser1", "to": "daccustodian", "quantity": "5.0000 EOS","memo":"daccustodian"}' -p testreguser1 -f`
        # TODO: Removed memo requirement... do we want it back?
        # Verify that a transaction with an invalid account memo still is insufficient funds.
        # `cleos push action eosio.token transfer '{ "from": "testreguser1", "to": "daccustodian", "quantity": "25.0000 EOS","memo":"noncaccount"}' -p testreguser1 -f`
      end
      command %(cleos push action daccustodian nominatecand '{ "cand": "testreguser1", "bio": "any bio"}' -p testreguser1), allow_error: true
      its(:stderr) {is_expected.to include('A registering candidate must transfer sufficient tokens to the contract for staking')}
    end

    context "with valid and registered member after transferring sufficient staked tokens in multiple transfers" do
      before(:all) do
        `cleos push action eosio.token transfer '{ "from": "testreguser1", "to": "daccustodian", "quantity": "5.0000 EOS","memo":"daccustodian"}' -p testreguser1 -f`
      end
      command %(cleos push action daccustodian nominatecand '{ "cand": "testreguser1", "bio": "any bio"}' -p testreguser1), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::nominatecand')}
    end

    context "without first staking" do
      command %(cleos push action daccustodian nominatecand '{ "cand": "testreguser5", "bio": "any bio"}' -p testreguser5), allow_error: true
      its(:stderr) {is_expected.to include("A registering candidate must transfer sufficient tokens to the contract for staking")}
    end


    context "with user is already registered" do
      command %(cleos push action daccustodian nominatecand '{ "cand": "testreguser1", "bio": "any bio"}' -p testreguser1), allow_error: true
      its(:stderr) {is_expected.to include('Error 3050003')}
    end

    context "Read the candidates table after nominatecand" do
      command %(cleos get table daccustodian daccustodian candidates), allow_error: true
      it do
        expect(JSON.parse(subject.stdout)).to eq JSON.parse <<~JSON
            {
              "rows": [{
                "candidate_name": "testreguser1",
                "requestedpay": "10.0000 EOS",
                "locked_tokens": "10.0000 EOS",
                "total_votes": 0,
                "is_active": 1,
                "custodian_end_time_stamp": "1970-01-01T00:00:00"
              }
            ],
            "more": false
          }
        JSON
      end
    end

    context "Read the pendingstake table after nominatecand and it should be empty" do
      command %(cleos get table daccustodian daccustodian pendingstake), allow_error: true
      it do
        expect(JSON.parse(subject.stdout)).to eq JSON.parse <<~JSON
            {
              "rows": [],
            "more": false
          }
        JSON
      end
    end
  end

  context "To ensure behaviours change after updateconfig" do
    context "updateconfigs with valid auth" do
      command %(cleos push action daccustodian updateconfig '{"newconfig": {"lockupasset": "23.0000 EOS", "maxvotes": 5, "auditor_tenure": 604800 , "numelected": 12, "authaccount": "dacauthority", "auththresh": 3, "initial_vote_quorum_percent": 15, "vote_quorum_percent": 10, "auth_threshold_high": 11, "auth_threshold_mid": 7, "auth_threshold_low": 3, "lockup_release_time_delay": 10 }}' -p dacauthority), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::updateconfig')}
    end
  end

  describe "withdrawcand" do
    before(:all) do
      seed_account("unreguser1", issue: "100.0000 EOS", memberreg: "New Latest terms")
      seed_account("unreguser2", issue: "100.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "11.5000 EOS")
    end

    context "with invalid auth" do
      command %(cleos push action daccustodian withdrawcand '{ "cand": "unreguser3"}' -p testreguser3), allow_error: true
      its(:stderr) {is_expected.to include('Error 3090004')}
    end

    context "with valid auth but not registered" do
      command %(cleos push action daccustodian withdrawcand '{ "cand": "unreguser1"}' -p unreguser1), allow_error: true
      its(:stderr) {is_expected.to include('Error 3050003')}
    end

    context "with valid auth" do

      command %(cleos push action daccustodian withdrawcand '{ "cand": "unreguser2"}' -p unreguser2), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::withdrawcand')}
    end
  end

  describe "update bio" do
    before(:all) do
      seed_account("updatebio1", issue: "100.0000 EOS", memberreg: "New Latest terms")
      seed_account("updatebio2", issue: "100.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "11.5000 EOS")
    end

    context "with invalid auth" do
      command %(cleos push action daccustodian updatebio '{ "cand": "updatebio1", "bio": "new bio"}' -p testreguser3), allow_error: true
      its(:stderr) {is_expected.to include('Error 3090004')}
    end

    context "with valid auth but not registered" do
      command %(cleos push action daccustodian updatebio '{ "cand": "updatebio1", "bio": "new bio"}' -p updatebio1), allow_error: true
      its(:stderr) {is_expected.to include('Error 3050003')}
    end

    context "with valid auth" do
      command %(cleos push action daccustodian updatebio '{ "cand": "updatebio2", "bio": "new bio"}' -p updatebio2), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::updatebio')}
    end
  end

  describe "votedcust" do
    before(:all) do

      #create users

      seed_account("votedcust1", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "11.0000 EOS")
      seed_account("votedcust2", issue: "102.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "12.0000 EOS")
      seed_account("votedcust3", issue: "103.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "13.0000 EOS")
      seed_account("votedcust4", issue: "104.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "14.0000 EOS")
      seed_account("votedcust5", issue: "105.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "15.0000 EOS")
      seed_account("votedcust11", issue: "106.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "16.0000 EOS")
      seed_account("voter1", issue: "3000.0000 EOS", memberreg: "New Latest terms")
      seed_account("voter2", issue: "108.0000 EOS", memberreg: "New Latest terms")
      seed_account("unregvoter", issue: "109.0000 EOS")
    end

    context "Read the candidates table after _change_ vote" do
      command %(cleos get table daccustodian daccustodian candidates), allow_error: true
      it do

        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 9

        candidate = json["rows"][3]

        expect(candidate["candidate_name"]).to eq 'votedcust1'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 0
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][5]

        expect(candidate["candidate_name"]).to eq 'votedcust2'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 0
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][6]

        expect(candidate["candidate_name"]).to eq 'votedcust3'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 0
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][7]

        expect(candidate["candidate_name"]).to eq 'votedcust4'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 0
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"
      end
    end

    context "with invalid auth" do
      command %(cleos push action daccustodian votecust '{ "voter": "voter1", "newvotes": ["votedcust1","votedcust2","votedcust3","votedcust4","votedcust5"]}' -p testreguser3), allow_error: true
      its(:stderr) {is_expected.to include('Error 3090004')}
    end

    context "exceeded allowed number of votes" do
      command %(cleos push action daccustodian votecust '{ "voter": "voter1", "newvotes": ["voter1","votedcust2","votedcust3","votedcust4","votedcust5", "votedcust11"]}' -p voter1), allow_error: true
      its(:stderr) {is_expected.to include('Error 3050003')}
    end

    context "Voted for the same candidate multiple times" do
      command %(cleos push action daccustodian votecust '{ "voter": "voter1", "newvotes": ["votedcust2","votedcust3","votedcust2","votedcust5", "votedcust11"]}' -p voter1), allow_error: true
      its(:stderr) {is_expected.to include('Added duplicate votes for the same candidate')}
    end

    context "Voted for an inactive candidate" do
      command %(cleos push action daccustodian votecust '{ "voter": "voter1", "newvotes": ["votedcust1","unreguser2","votedcust2","votedcust5", "votedcust11"]}' -p voter1), allow_error: true
      its(:stderr) {is_expected.to include('Attempting to vote for an inactive candidate.')}
    end

    context "Voted for an candidate not in the list of candidates" do
      command %(cleos push action daccustodian votecust '{ "voter": "voter1", "newvotes": ["votedcust1","testreguser5","votedcust2","votedcust5", "votedcust11"]}' -p voter1), allow_error: true
      its(:stderr) {is_expected.to include('ERR::VOTECUST_CANDIDATE_NOT_FOUND::')}
    end

    context "with valid auth create new vote" do
      command %(cleos push action daccustodian votecust '{ "voter": "voter1", "newvotes": ["votedcust1","votedcust2","votedcust3"]}' -p voter1), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::votecust')}
    end

    context "Read the votes table after _create_ vote" do
      before(:all) do
        `cleos push action daccustodian votecust '{ "voter": "voter2", "newvotes": ["votedcust1","votedcust2","votedcust3"]}' -p voter2`
      end
      command %(cleos get table daccustodian daccustodian votes), allow_error: true
      it do
        expect(JSON.parse(subject.stdout)).to eq JSON.parse <<~JSON
            {
              "rows": [{
                "voter": "voter1",
                "weight": 30200000,
                "candidates": [
                  "votedcust1",
                  "votedcust2",
                  "votedcust3"
                ]
              }, {
                "voter": "voter2",
                "weight": 1280000,
                "candidates": [
                  "votedcust1",
                  "votedcust2",
                  "votedcust3"
                ]
              }
            ],
            "more": false
          }
        JSON
      end
    end

    context "Read the state table after placed votes" do
      before(:all) do
        # `cleos push action daccustodian votecust '{ "voter": "voter2", "newvotes": ["votedcust1","votedcust2","votedcust3"]}' -p voter2`
      end
      command %(cleos get table daccustodian daccustodian state), allow_error: true
      it do
        expect(JSON.parse(subject.stdout)).to eq JSON.parse <<~JSON
            {
              "rows": [
  {
    "lastperiodtime": 0,
    "total_weight_of_votes": 31480000,
    "total_votes_on_candidates": 94440000,
    "number_active_candidates": 8,
    "met_initial_votes_threshold": 0
  }
],
            "more": false
          }
        JSON
      end
    end

    context "with valid auth to clear a vote" do
      command %(cleos push action daccustodian votecust '{ "voter": "voter2", "newvotes": []}' -p voter2), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::votecust')}
    end

    context "Read the votes table after clearing a vote" do

      command %(cleos get table daccustodian daccustodian votes), allow_error: true
      it do
        expect(JSON.parse(subject.stdout)).to eq JSON.parse <<~JSON
            {
              "rows": [{
                "voter": "voter1",
                "weight": 30200000,
                "candidates": [
                  "votedcust1",
                  "votedcust2",
                  "votedcust3"
                ]
              }
            ],
            "more": false
          }
        JSON
      end
    end

    context "Read the candidates table after _create_ vote" do
      command %(cleos get table daccustodian daccustodian candidates), allow_error: true
      it do

        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 9

        candidate = json["rows"][3]

        expect(candidate["candidate_name"]).to eq 'votedcust1'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 30200000
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][5]

        expect(candidate["candidate_name"]).to eq 'votedcust2'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 30200000
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][6]

        expect(candidate["candidate_name"]).to eq 'votedcust3'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 30200000
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][7]

        expect(candidate["candidate_name"]).to eq 'votedcust4'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 0
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"
      end
    end

    context "with valid auth change existing vote" do
      command %(cleos push action daccustodian votecust '{ "voter": "voter1", "newvotes": ["votedcust1","votedcust2","votedcust4"]}' -p voter1), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::votecust')}
    end

    context "Read the votes table after _change_ vote" do
      command %(cleos get table daccustodian daccustodian votes), allow_error: true
      it do
        expect(JSON.parse(subject.stdout)).to eq JSON.parse <<~JSON
            {
              "rows": [{
                "voter": "voter1",
                "weight": 30200000,
                "candidates": [
                  "votedcust1",
                  "votedcust2",
                  "votedcust4"
                ]
              }
            ],
            "more": false
          }
        JSON
      end
    end

    context "Read the candidates table after _change_ vote" do
      command %(cleos get table daccustodian daccustodian candidates), allow_error: true
      it do

        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 9

        candidate = json["rows"][3]

        expect(candidate["candidate_name"]).to eq 'votedcust1'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 30200000
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][5]

        expect(candidate["candidate_name"]).to eq 'votedcust2'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 30200000
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][6]

        expect(candidate["candidate_name"]).to eq 'votedcust3'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 0
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        candidate = json["rows"][7]

        expect(candidate["candidate_name"]).to eq 'votedcust4'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["total_votes"]).to eq 30200000
        expect(candidate["is_active"]).to eq 1
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"
      end
    end

    ###TODO: Transfers dont update values. Need to test refresh vote command.

    # context "After token transfer vote weight should move to different candidates" do
    #   before(:all) do
    #     `cleos push action daccustodian votecust '{ "voter": "voter2", "newvotes": ["votedcust3"]}' -p voter2`
    #   end
    #   command %(cleos push action eosio.token transfer '{ "from": "voter1", "to": "voter2", "quantity": "1300.0000 EOS","memo":"random transfer"}' -p voter1), allow_error: true
    #   its(:stdout) {is_expected.to include('eosio.token::transfer')}
    # end

    # context "Read the candidates table after transfer for voter" do
    #   command %(cleos get table daccustodian daccustodian candidates), allow_error: true
    #   it do

    #     json = JSON.parse(subject.stdout)
    #     expect(json["rows"].count).to eq 9

    #     candidate = json["rows"][3]

    #     expect(candidate["candidate_name"]).to eq 'votedcust1'
    #     expect(candidate["requestedpay"]).to eq '10.0000 EOS'
    #     expect(candidate["total_votes"]).to eq 17000000 # was 3000,0000 now subtract 1300,0000 = 1700,0000
    #     expect(candidate["is_active"]).to eq 1
    #     expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

    #     candidate = json["rows"][5]

    #     expect(candidate["candidate_name"]).to eq 'votedcust2'
    #     expect(candidate["requestedpay"]).to eq '10.0000 EOS'
    #     expect(candidate["total_votes"]).to eq 17000000 # was 3000,0000 now subtract 1300,0000 = 1700,0000
    #     expect(candidate["is_active"]).to eq 1
    #     expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

    #     candidate = json["rows"][6]

    #     expect(candidate["candidate_name"]).to eq 'votedcust3'
    #     expect(candidate["requestedpay"]).to eq '10.0000 EOS'
    #     expect(candidate["total_votes"]).to eq 14080000 # initial balance of 108,0000 + 1300,0000 = 1408,0000
    #     expect(candidate["is_active"]).to eq 1
    #     expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

    #     candidate = json["rows"][7]

    #     expect(candidate["candidate_name"]).to eq 'votedcust4'
    #     expect(candidate["requestedpay"]).to eq '10.0000 EOS'
    #     expect(candidate["total_votes"]).to eq 17000000 # was 3000,0000 now subtract 1300,0000 = 1700,0000
    #     expect(candidate["is_active"]).to eq 1
    #     expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"
    #   end
    # end

    context "Before new period has been called the custodians table should be empty" do
      command %(cleos get table daccustodian daccustodian custodians), allow_error: true
      it do
        expect(JSON.parse(subject.stdout)).to eq JSON.parse <<~JSON
          {
            "rows": [],
          "more": false
        }
        JSON
      end
    end
  end

  describe "newtenure" do
    before(:all) do
      seed_account("voter3", issue: "110.0000 EOS", memberreg: "New Latest terms")
      seed_account("whale1", issue: "1500000.0000 EOS", memberreg: "New Latest terms")
    end

    describe "with insufficient votes to trigger the dac should fail" do
      before(:all) do
        `cleos push action daccustodian updateconfig '{"newconfig": {"lockupasset": "10.0000 EOS", "maxvotes": 5, "auditor_tenure": 5, "numelected": 12, "authaccount": "dacauthority", "auththresh": 3, "initial_vote_quorum_percent": 15, "vote_quorum_percent": 10, "auth_threshold_high": 3, "auth_threshold_mid": 2, "auth_threshold_low": 1, "lockup_release_time_delay": 10 }}' -p dacauthority`
      end
      command %(cleos push action daccustodian newtenure '{ "message": "log message", "earlyelect": false}' -p daccustodian), allow_error: true
      its(:stderr) {is_expected.to include('Voter engagement is insufficient to activate the Audit Cycle')}
    end

    describe "allocateCust" do
      before(:all) do
        # add cands
        `cleos push action daccustodian updateconfig '{"newconfig": {"lockupasset": "10.0000 EOS", "maxvotes": 5, "auditor_tenure": 1 , "numelected": 12, "authaccount": "dacauthority", "auththresh": 3, "initial_vote_quorum_percent": 15, "vote_quorum_percent": 10, "auth_threshold_high": 4, "auth_threshold_mid": 4, "auth_threshold_low": 2, "lockup_release_time_delay": 10 }}' -p dacauthority`
      end

      context "given there are not enough candidates to fill the custodians" do
        command %(cleos push action daccustodian newtenure '{ "message": "log message", "earlyelect": false}' -p daccustodian), allow_error: true
        its(:stderr) {is_expected.to include('Voter engagement is insufficient to activate the Audit Cycle')}
      end

      context "given there are enough candidates to fill the custodians but not enough have votes greater than 0" do
        before(:all) do
          seed_account("allocate1", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "11.0000 EOS")
          seed_account("allocate2", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "12.0000 EOS")
          seed_account("allocate3", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "13.0000 EOS")
          seed_account("allocate4", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "14.0000 EOS")
          seed_account("allocate5", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "15.0000 EOS")
          seed_account("allocate11", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "16.0000 EOS")
          seed_account("allocate21", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "17.0000 EOS")
          seed_account("allocate31", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "18.0000 EOS")
          seed_account("allocate41", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "19.0000 EOS")
          seed_account("allocate51", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "20.0000 EOS")
          seed_account("allocate12", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "21.0000 EOS")
          seed_account("allocate22", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "22.0000 EOS")
          seed_account("allocate32", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "23.0000 EOS")
          seed_account("allocate42", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "24.0000 EOS")
          seed_account("allocate52", issue: "101.0000 EOS", memberreg: "New Latest terms", stake: "23.0000 EOS", requestedpay: "25.0000 EOS")
        end

        command %(cleos push action daccustodian newtenure '{ "message": "log message", "earlyelect": false}' -p daccustodian), allow_error: true
        its(:stderr) {is_expected.to include('Voter engagement is insufficient to activate the Audit Cycle')}
      end

      context "given there are enough votes with total_votes over 0" do
        before(:all) do
          `cleos push action daccustodian votecust '{ "voter": "voter1", "newvotes": ["allocate1","allocate2","allocate3","allocate4","allocate5"]}' -p voter1`
          `cleos push action daccustodian votecust '{ "voter": "voter2", "newvotes": ["allocate11","allocate21","allocate31","allocate41","allocate51"]}' -p voter2`
          `cleos push action daccustodian votecust '{ "voter": "voter3", "newvotes": ["allocate12","allocate22","allocate32","allocate4","allocate5"]}' -p voter3`
        end
        context "But not enough engagement to active the DAC" do
          command %(cleos push action daccustodian newtenure '{ "message": "log message", "earlyelect": false}' -p daccustodian), allow_error: true
          its(:stderr) {is_expected.to include('Voter engagement is insufficient to activate the Audit Cycle')}
        end

        context "And enough voter weight to activate the DAC" do
          before(:all) {`cleos push action daccustodian votecust '{ "voter": "whale1", "newvotes": ["allocate12","allocate22","allocate32","allocate4","allocate5"]}' -p whale1`}

          command %(cleos push action daccustodian newtenure '{ "message": "log message"}' -p daccustodian), allow_error: true
            its(:stdout) {is_expected.to include('daccustodian::newtenure')}
        end
      end

      context "Read the votes table after adding enough votes for a valid election" do
        command %(cleos get table daccustodian daccustodian votes), allow_error: true
        it do

          json = JSON.parse(subject.stdout)
          expect(json["rows"].count).to eq 4

          vote = json["rows"].detect {|v| v["voter"] == 'voter2'}

          expect(vote["candidates"].count).to eq 5
          expect(vote["candidates"][0]).to eq 'allocate11'

          vote = json["rows"].detect {|v| v["voter"] == 'voter3'}

          expect(vote["candidates"].count).to eq 5
          expect(vote["candidates"][0]).to eq 'allocate12'

          vote = json["rows"].detect {|v| v["voter"] == 'whale1'}
          expect(vote["candidates"].count).to eq 5
          expect(vote["candidates"][0]).to eq 'allocate12'
        end
      end

      context "Read the custodians table after adding enough votes for election" do
        command %(cleos get table daccustodian daccustodian custodians --limit 20), allow_error: true
        it do
          json = JSON.parse(subject.stdout)
          expect(json["rows"].count).to eq 12

          custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate11'}
          expect(custodian["total_votes"]).to eq 1280000

          custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate32'}
          expect(custodian["total_votes"]).to eq 15001500000

          custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate4'}
          expect(custodian["total_votes"]).to eq 168100000

          custnames = json["rows"].map {|c| c["auditor_name"]}
          expect(custnames).to eq ["allocate1", "allocate11", "allocate12", "allocate2", "allocate21", "allocate22", "allocate3", "allocate31", "allocate32", "allocate4", "allocate41", "allocate5"]
        end
      end
    end

    # TODO: Figure out why this succeeded
    describe "called too early in the period should fail after recent newtenure call" do
      before(:all) do
        `cleos push action daccustodian votecust '{ "voter": "whale1", "newvotes": ["allocate1","allocate2","allocate3","allocate4","allocate5"]}' -p whale1`
      end

      command %(cleos push action daccustodian newtenure '{ "message": "called too early", "earlyelect": false}' -p daccustodian), allow_error: true
      its(:stderr) {is_expected.to include('New period is being called too soon. Wait until the period has complete')}
    end

    describe "called after period time has passed" do
      before(:all) do
        `cleos push action daccustodian updateconfig '{"newconfig": { "lockupasset": "10.0000 EOS", "maxvotes": 5, "auditor_tenure": 1, "numelected": 12, "authaccount": "dacauthority", "auththresh": 3, "initial_vote_quorum_percent": 15, "vote_quorum_percent": 10, "auth_threshold_high": 3, "auth_threshold_mid": 2, "auth_threshold_low": 1, "lockup_release_time_delay": 10 }}' -p dacauthority`
        sleep 2
      end
      command %(cleos push action daccustodian newtenure '{ "message": "Good new period call after config change", "earlyelect": false}' -p daccustodian), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::newtenure')}
    end

    describe "called after voter engagement has dropped to too low" do
      before(:all) do
        # Remove the whale vote to drop backs
        `cleos push action daccustodian votecust '{ "voter": "whale1", "newvotes": []}' -p whale1`
        `cleos push action daccustodian updateconfig '{"newconfig": {"lockupasset": "10.0000 EOS", "maxvotes": 5, "auditor_tenure": 1, "numelected": 12, "authaccount": "dacauthority", "auththresh": 3, "initial_vote_quorum_percent": 15, "vote_quorum_percent": 4, "auth_threshold_high": 3, "auth_threshold_mid": 2, "auth_threshold_low": 1, "lockup_release_time_delay": 10 }}' -p dacauthority`
        sleep 2
      end
      command %(cleos push action daccustodian newtenure '{ "message": "Good new period call after config change", "earlyelect": false}' -p daccustodian), allow_error: true
      its(:stderr) {is_expected.to include('Voter engagement is insufficient to process a new period')}
    end

    describe "called after voter engagement has risen to above the continuing threshold" do
      before(:all) do
        `cleos push action daccustodian updateconfig '{"newconfig": {"lockupasset": "10.0000 EOS", "maxvotes": 5, "auditor_tenure": 1, "numelected": 12, "authaccount": "dacauthority", "auththresh": 3, "initial_vote_quorum_percent": 15, "vote_quorum_percent": 4, "auth_threshold_high": 3, "auth_threshold_mid": 2, "auth_threshold_low": 1, "lockup_release_time_delay": 10 }}' -p dacauthority`
        `cleos push action eosio.token transfer '{ "from": "whale1", "to": "voter1", "quantity": "1300.0000 EOS","memo":"random transfer"}' -p whale1`

        sleep 2
      end
      command %(cleos push action daccustodian newtenure '{ "message": "Good new period call after config change", "earlyelect": false}' -p daccustodian), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::newtenure')}
    end

    context "the pending_pay table" do
      command %(cleos get table daccustodian daccustodian pendingpay --limit 50), allow_error: true
      it do

        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 24

        custodian = json["rows"].detect {|v| v["receiver"] == 'allocate5'}
        expect(custodian["quantity"]).to eq '10.0000 EOS'
        expect(custodian["memo"]).to eq 'Custodian pay. Thank you.'

        custodian = json["rows"].detect {|v| v["receiver"] == 'allocate3'}
        expect(custodian["quantity"]).to eq '10.0000 EOS'
      end
    end

    context "the votes table" do
      command %(cleos get table daccustodian daccustodian votes), allow_error: true
      it do

        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 3

        vote = json["rows"].detect {|v| v["voter"] == 'voter2'}

        expect(vote["candidates"].count).to eq 5
        expect(vote["candidates"][0]).to eq 'allocate11'

        vote = json["rows"].detect {|v| v["voter"] == 'voter3'}

        expect(vote["candidates"].count).to eq 5
        expect(vote["candidates"][0]).to eq 'allocate12'

        vote = json["rows"].detect {|v| v["voter"] == 'voter1'}
        expect(vote["candidates"].count).to eq 5
        expect(vote["candidates"][0]).to eq 'allocate1'
      end
    end

    context "the candidates table" do
      subject {command %(cleos get table daccustodian daccustodian candidates --limit 40), allow_error: true}
      it do
        json = JSON.parse(subject.stdout)

        expect(json["rows"].count).to eq 12

        delayedcandidates = json["rows"].select {|v| v["custodian_end_time_stamp"] > "1970-01-01T00:00:00"}
        expect(delayedcandidates.count).to eq(12)

        # custnames = json["rows"].map { |c| c["candidate_name"] }
        # puts custnames
        # expect(custnames).to eq ["allocate1", "allocate11", "allocate12", "allocate2", "allocate21", "allocate22", "allocate3", "allocate31", "allocate32", "allocate4", "allocate41", "allocate5"]
      end
    end

    context "the custodians table" do
      command %(cleos get table daccustodian daccustodian custodians --limit 20), allow_error: true
      it do
        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 12

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate11'}
        expect(custodian["total_votes"]).to eq 1280000

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate1'}
        expect(custodian["total_votes"]).to eq '15030400000'

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate4'}
        expect(custodian["total_votes"]).to eq '15030400000'

        custnames = json["rows"].map {|c| c["auditor_name"]}

        # allocate32 was dropped and then allocate51 took the spot
        expect(custnames).to eq ["allocate1", "allocate11", "allocate12", "allocate2", "allocate21", "allocate22", "allocate3", "allocate31", "allocate4", "allocate41", "allocate5", "allocate51"]
      end
    end
  end

  describe "claimpay" do
    context "with invalid payId should fail" do
      command %(cleos push action daccustodian claimpay '{ "payid": 100}' -p votedcust4), allow_error: true
      its(:stderr) {is_expected.to include('Invalid pay claim id')}
    end

    context "claiming for a different acount should fail" do
      command %(cleos push action daccustodian claimpay '{ "payid": 10}' -p votedcust4), allow_error: true
      its(:stderr) {is_expected.to include('missing authority of allocate41')}
    end

    context "Before claiming pay the balance should be 0" do
      command %(cleos get currency balance eosio.token dacocoiogmbh EOS), allow_error: true
      its(:stdout) {is_expected.to eq('')}
    end

    context "claiming for the correct account with matching auth should succeed" do
      command %(cleos push action daccustodian claimpay '{ "payid": 1 }' -p allocate11), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::claimpay')}
      # exit
    end

    context "After claiming for the correct should be added to the claimer" do
      command %(cleos get currency balance eosio.token dacocoiogmbh EOS), allow_error: true
      its(:stdout) {is_expected.not_to include('17.0000 EOS')} # eventually this would pass but now it's time delayed I cannot assert.
    end

    context "After claiming for the correct should be added to the claimer" do
      before(:each) { sleep 12 }
      command %(cleos get currency balance eosio.token dacocoiogmbh EOS), allow_error: true
      its(:stdout) {is_expected.to include('10.0000 EOS')} # eventually this would pass but now it's time delayed I cannot assert.
    end
  end

  describe "withdrawcand" do
    context "when the auth is wrong" do
      command %(cleos push action daccustodian withdrawcand '{ "cand": "allocate41"}' -p allocate4), allow_error: true
      its(:stderr) {is_expected.to include('missing authority of allocate41')}
    end

    context "when the auth is correct" do
      command %(cleos push action daccustodian withdrawcand '{ "cand": "allocate41"}' -p allocate41), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::withdrawcand')}
    end

    context "the candidates table" do
      subject {command %(cleos get table daccustodian daccustodian candidates --limit 40), allow_error: true}
      it do
        json = JSON.parse(subject.stdout)

        expect(json["rows"].count).to eq 24

        delayedcandidatescount = json["rows"].count {|v| v["custodian_end_time_stamp"] > "1970-01-01T00:00:00"}
        expect(delayedcandidatescount).to eq(12)

        inactiveCandidatesCount = json["rows"].count {|v| v["is_active"] == 0}
        expect(inactiveCandidatesCount).to eq(2)

        inactiveCand = json["rows"].detect {|v| v["candidate_name"] == 'allocate41'}
        expect(inactiveCand["is_active"]).to eq(0)
      end
    end
  end

  describe "rereg custodian candidate" do
    context "with valid and registered member after transferring sufficient staked tokens in multiple transfers" do
      before(:all) do
        `cleos push action eosio.token transfer '{ "from": "allocate41", "to": "daccustodian", "quantity": "10.0000 EOS","memo":"daccustodian"}' -p allocate41 -f`
      end
      command %(cleos push action daccustodian nominatecand '{ "cand": "allocate41" "requestedpay": "11.5000 EOS"}' -p allocate41), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::nominatecand')}
    end

    context "Read the custodians table after unreg custodian and a single vote will be replaced" do
      command %(cleos get table daccustodian daccustodian candidates --limit 40), allow_error: true
      it do
        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 24

        candidate = json["rows"].detect {|v| v["candidate_name"] == 'allocate41'}
        expect(candidate["total_votes"]).to eq 1280000
        expect(candidate["candidate_name"]).to eq 'allocate41'
        expect(candidate["requestedpay"]).to eq '10.0000 EOS'
        expect(candidate["locked_tokens"]).to eq "33.0000 EOS"
        expect(candidate["custodian_end_time_stamp"]).to be > "1970-01-01T00:00:00"
      end
    end
  end

  describe "resign cust" do
    context "with invalid auth" do
      command %(cleos push action daccustodian resigncust '{ "auditor": "allocate31"}' -p allocate3), allow_error: true
      its(:stderr) {is_expected.to include('missing authority of allocate31')}
    end

    context "with a candidate who is not an elected custodian" do
      command %(cleos push action daccustodian resigncust '{ "auditor": "votedcust3"}' -p votedcust3), allow_error: true
      its(:stderr) {is_expected.to include('The entered account name is not for a current custodian.')}
    end

    context "when the auth is correct" do
      command %(cleos push action daccustodian resigncust '{ "auditor": "allocate31"}' -p allocate31), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::resigncust')}
    end

    context "Read the state" do
      command %(cleos get table daccustodian daccustodian state), allow_error: true
      it do

        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 1

        state = json["rows"][0]
        expect(state["total_weight_of_votes"]).to eq 32780000
        expect(state["total_votes_on_candidates"]).to eq 163900000
        expect(state["number_active_candidates"]).to eq 23
        expect(state["met_initial_votes_threshold"]).to eq 1
      end
    end

    context "the custodians table" do
      command %(cleos get table daccustodian daccustodian custodians --limit 20), allow_error: true
      it do
        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 12

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate11'}
        expect(custodian["total_votes"]).to eq 1280000

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate1'}
        expect(custodian["total_votes"]).to eq '15030400000'

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate4'}
        expect(custodian["total_votes"]).to eq '15030400000'

      end
    end

    context "Read the candidates table after resign cust" do
      command %(cleos get table daccustodian daccustodian candidates --limit 40), allow_error: true
      it do
        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 24

        candidate = json["rows"].detect {|v| v["candidate_name"] == 'allocate31'}

        expect(candidate["candidate_name"]).to eq 'allocate31'
        expect(candidate["requestedpay"]).to eq "10.0000 EOS"
        expect(candidate["locked_tokens"]).to eq "23.0000 EOS"
        expect(candidate["custodian_end_time_stamp"]).to be > "1970-01-01T00:00:00"
        expect(candidate["is_active"]).to eq(0)
      end
    end
  end

  describe "unstake" do
    context "for an elected custodian" do
      command %(cleos push action daccustodian unstake '{ "cand": "allocate41"}' -p allocate41), allow_error: true
      its(:stderr) {is_expected.to include('Cannot unstake tokens for an active candidate. Call withdrawcand first.')}
    end

    context "for a unelected custodian" do
      context "who has not withdrawn as a candidate" do
        command %(cleos push action daccustodian unstake '{ "cand": "votedcust2"}' -p votedcust2), allow_error: true
        its(:stderr) {is_expected.to include('Cannot unstake tokens for an active candidate. Call withdrawcand first.')}
      end

      context "who has withdrawn as a candidate" do
        command %(cleos push action daccustodian unstake '{ "cand": "allocate31"}' -p allocate31), allow_error: true
        its(:stderr) {is_expected.to include('Cannot unstake tokens before they are unlocked from the time delay.')}
      end
    end

    context "Before unstaking the token should note have been transferred back" do
      command %(cleos get currency balance eosio.token unreguser2 EOS), allow_error: true
      its(:stdout) {is_expected.to include('77.0000 EOS')}
    end

    context "for a resigned custodian after time expired" do
      command %(cleos push action daccustodian unstake '{ "cand": "unreguser2"}' -p unreguser2), allow_error: true
      its(:stdout) {is_expected.to include('daccustodian::unstake')}
    end

    context "After successful unstaking the token should have been transferred back" do
      command %(cleos get currency balance eosio.token unreguser2 EOS), allow_error: true
      its(:stdout) {is_expected.to include('77.0000 EOS')}
    end

    context "After successful unstaking the token should have been transferred back" do
      before(:each) { sleep 12 }
      command %(cleos get currency balance eosio.token unreguser2 EOS), allow_error: true
      its(:stdout) {is_expected.to include('100.0000 EOS')}
    end
  end

  describe "fire cand" do
    context "with invalid auth" do
      command %(cleos push action daccustodian firecand '{ "cand": "votedcust4", "lockupStake": true}' -p votedcust4), allow_error: true
      its(:stderr) {is_expected.to include('missing authority of dacauthority')}
    end

    xcontext "with valid auth" do # Needs further work to understand how this could be tested.
      context "without locked up stake" do
        command %(cleos multisig propose fireproposal '[{"actor": "dacauthority", "permission": "med"}]' '[{"actor": "allocate2", "permission": "active"}, {"actor": "allocate3", "permission": "active"}]' daccustodian firecand '{ "cand": "votedcust4", "lockupStake": false}' -p dacauthority@active -sdj), allow_error: true
        its(:stderr) {is_expected.to include('Cannot unstake tokens before they are unlocked from the time delay.')}
      end
      context "with locked up stake" do
        command %(cleos push action daccustodian firecand '{ "cand": "votedcust5", "lockupStake": true}' -p dacauthority), allow_error: true
        its(:stderr) {is_expected.to include('Cannot unstake tokens before they are unlocked from the time delay.')}
      end
    end

    context "Read the candidates table after fire candidate" do
      command %(cleos get table daccustodian daccustodian candidates --limit 40), allow_error: true
      it do
        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 24

        candidate = json["rows"].detect {|v| v["candidate_name"] == 'votedcust4'}

        expect(candidate["candidate_name"]).to eq 'votedcust4'
        expect(candidate["requestedpay"]).to eq "10.0000 EOS"
        expect(candidate["locked_tokens"]).to eq "23.0000 EOS"
        expect(candidate["custodian_end_time_stamp"]).to eq "1970-01-01T00:00:00"

        #expect(candidate["is_active"]).to eq(0) # Since the multisig is not yet working in the tests this will fail.

        candidate = json["rows"].detect {|v| v["candidate_name"] == 'votedcust5'}

        expect(candidate["candidate_name"]).to eq 'votedcust5'
        expect(candidate["requestedpay"]).to eq "10.0000 EOS"
        expect(candidate["locked_tokens"]).to eq "23.0000 EOS"
        # expect(candidate["custodian_end_time_stamp"]).to be > "2018-01-01T00:00:00" # Will fail due to the multisig not being testable at the moment.
        # expect(candidate["is_active"]).to eq(0) # Will fail due to the multisig not being testable at the moment.
      end
    end
  end

  xdescribe "fire custodian" do
    context "with invalid auth" do
      command %(cleos push action daccustodian fireauditor '{ "auditor": "allocate1"}' -p allocate31), allow_error: true
      its(:stderr) {is_expected.to include('missing authority of dacauthority')}
    end

    context "with valid auth" do
      command %(cleos push action daccustodian fireauditor '{ "auditor": "allocate1"}' -p dacauthority), allow_error: true
      its(:stderr) {is_expected.to include('Cannot unstake tokens before they are unlocked from the time delay.')}
    end

    context "Read the candidates table after fire candidate" do
      command %(cleos get table daccustodian daccustodian candidates --limit 40), allow_error: true
      it do
        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 25

        candidate = json["rows"].detect {|v| v["candidate_name"] == 'allocate1'}

        expect(candidate["candidate_name"]).to eq 'allocate1'
        expect(candidate["requestedpay"]).to eq "10.0000 EOS"
        expect(candidate["locked_tokens"]).to eq "23.0000 EOS"
        expect(candidate["custodian_end_time_stamp"]).to be eq "1970-01-01T00:00:00"
        expect(candidate["is_active"]).to eq(0)

        candidate = json["rows"].detect {|v| v["candidate_name"] == 'allocate11'}

        expect(candidate["candidate_name"]).to eq 'allocate11'
        expect(candidate["requestedpay"]).to eq "10.0000 EOS"
        expect(candidate["locked_tokens"]).to eq "23.0000 EOS"
        expect(candidate["custodian_end_time_stamp"]).to be > "2018-01-01T00:00:00"
        expect(candidate["is_active"]).to eq(0)
      end
    end

    context "Read the custodians table" do
      command %(cleos get table daccustodian daccustodian custodians --limit 20), allow_error: true
      it do
        json = JSON.parse(subject.stdout)
        expect(json["rows"].count).to eq 12

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate11'}
        expect(custodian["total_votes"]).to eq 1280000

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate1'}
        expect(custodian["total_votes"]).to eq 30000000

        custodian = json["rows"].detect {|v| v["auditor_name"] == 'allocate4'}
        expect(custodian["total_votes"]).to eq '15030400000'

      end
    end
  end
end

