from flask import Flask
import flask
from eospy.cleos import Cleos
from utils import *
import json
from init_db import *
from urllib.request import urlopen
import requests
from lxml import etree

# constants
BOS_URLS = [
		'https://bostest.api.blockgo.vip'
		,'http://bos-test.eoshenzhen.io:8888'
		,'https://bos-testnet.eosphere.io'
		,'https://api.bostest.alohaeos.com'
		,'https://boscore.eosrio.io'
		]

TALLY_API = "https://s3.amazonaws.com/bostest.referendum/referendum/tallies/latest.json"
BOS_TEST_NET_EX = "https://bos-test.eosx.io/"
BOS_NET_EX = "https://bos.eospark.com/"
EOSX_VOTE_TOTAL_XPATH = u'//*[@id="__layout"]/div/div[2]/div[2]/div/div/div[5]/div/span/span/text()'

BP_TOTAL_VOTES = 14620687

# util func
def test_net_is_working(urls):
	url_index = -1
	for i in (0, len(urls)-1):
		ce = Cleos(url=urls[i])
		try:
			info = ce.get_info()
			# print(info)
			if info != None:
				url_index = i
				break
		except Exception as err:
			print(err)
	return url_index


app = Flask(__name__)
app.url_map.strict_slashes = False

@app.route('/')
def hi():
    return '🐛'

@app.route('/getBPs')
def bpinfos():
	logger = reactive_log('getBPs')
	url_index = test_net_is_working(BOS_URLS)
	logger.info('BOS Node ['+ str(url_index) + '] is working:' + BOS_URLS[url_index])
	ce = Cleos(url=BOS_URLS[url_index])

	result = json.dumps({'producer':ce.get_producers()['rows']})
	resp = flask.Response(result)
	resp.headers['Access-Control-Allow-Origin'] = '*'
	return resp

@app.route('/getJson')
def jsoninfo():
	logger = reactive_log('json')
	url_index = test_net_is_working(BOS_URLS)
	logger.info('BOS Node ['+ str(url_index) + '] is working:' + BOS_URLS[url_index])
	ce = Cleos(url=BOS_URLS[url_index])

	BP_TOTAL_VOTES = float(int(ce.get_table('eosio','eosio','global')['rows'][0]['total_activated_stake'])/10000)
	print(BP_TOTAL_VOTES)
	# try:
		# get tally json file
	proposals = {}
	try:
		rawtext=urlopen(TALLY_API,timeout=15).read()
		proposals = json.loads(rawtext.decode('utf8'))
	except Exception as err:
		logger.error(err)
	# get Voted Total
	# page = requests.get(BOS_TEST_NET_EX)
	# try:
	# 	page = requests.get(BOS_NET_EX)
	# 	tree =  etree.HTML(page.content.lower().decode('utf-8'))
	# 	vote_total_from_web = tree.xpath(EOSX_VOTE_TOTAL_XPATH)
	# 	logger.info(str(vote_total_from_web))
	# except Exception as err:
	# 	logger.error(err)
	#iterater the current proposals
	for proposal in proposals:
		proposal_item = proposals[proposal]
		vote_key = proposal_item['stats']['votes']
		stake_key = proposal_item['stats']['staked']
		staked_total = float(int(proposal_item['stats']['staked']['total'])/10000) if 'total' in stake_key else 0
		vote_total = proposal_item['stats']['votes']['total'] if  'total' in vote_key else 0
		vote_yes = proposal_item['stats']['votes']['1'] if  '1' in vote_key else 0
		vote_no = proposal_item['stats']['votes']['0'] if '0' in vote_key else 0
		_meet_conditions_days =  0
		_approved_by_vote =  0
		_approved_by_vote_date =  None
		_approved_by_BET =  0
		_reviewed_by_BET_date =  None
		_approved_by_BPs =  0
		_approved_by_BPs_date =  None
		_review = 0
		_review_date = None
		_finish =  0
		_finish_date =  None
		# check if the proposal exists
		try:
			propos = Proposal.select().where(Proposal.name == proposal).count()
			logger.info("result"+str(propos))				
			# the proposal exists
			if propos > 0:
				# check if the proposal is passed
				if proposal_base_condition_ckeck(BP_TOTAL_VOTES, staked_total, vote_yes, vote_no):
					propos = Proposal.get(Proposal.name == proposal)
					meet = propos.meet_conditions_days
					# abv = propos.approved_by_vote 
					abvd = propos.approved_by_vote_date
					_approved_by_vote = 1 if int(meet) >= 20 else 0
					_approved_by_vote_date = datetime.now() if abvd == None else abvd
					# if meet >= 0 and abv != False and abvd != None:
					nrow = (Proposal.update(
						json_info = proposal_item
						, vote_total = vote_total
						, staked_total = staked_total
						, vote_bp_total = BP_TOTAL_VOTES
						, vote_yes = vote_yes
						, vote_no = vote_no
						, meet_conditions_days = int(meet) + 1
						, approved_by_vote = _approved_by_vote
						, approved_by_vote_date = _approved_by_vote_date
						).where(Proposal.name == proposal).execute())
				else:
					nrow = (Proposal.update(
						json_info = proposal_item
						, vote_total = vote_total
						, staked_total = staked_total
						, vote_bp_total = BP_TOTAL_VOTES
						, vote_yes = vote_yes
						, vote_no = vote_no
						, meet_conditions_days = 0
						, approved_by_vote = 0
						, approved_by_vote_date = None
						, approved_by_BET = 0
						, reviewed_by_BET_date = None
						, approved_by_BPs = 0
						, approved_by_BPs_date = None
						, review = 0
						, review_date = None
						, finish = 0
						, finish_date = None
						).where(Proposal.name == proposal).execute())
			else:
				if proposal_base_condition_ckeck(BP_TOTAL_VOTES, staked_total, vote_yes, vote_no):
					_approved_by_vote = 1
					_approved_by_vote_date = datetime.now()
				else:
					pass
				new_proposal = Proposal(name = proposal
				, json_info = proposal_item
				, vote_total = vote_total
				, staked_total = staked_total
				, vote_bp_total = BP_TOTAL_VOTES
				, vote_yes = vote_yes
				, vote_no = vote_no
				, meet_conditions_days = _meet_conditions_days
				, approved_by_vote = _approved_by_vote
				, approved_by_vote_date = _approved_by_vote_date
				, approved_by_BET = _approved_by_BET
				, reviewed_by_BET_date = _reviewed_by_BET_date
				, approved_by_BPs = _approved_by_BPs
				, approved_by_BPs_date = _approved_by_BPs_date
				, review = 0
				, review_date = None
				, finish = _finish
				, finish_date = _finish_date)
				new_proposal.save()  
		except Exception as err:
			logger.error(err)
		logger.info("proposal name: " + proposal)
		logger.info("vote total: " + str(vote_total))
		logger.info("staked total: " + str(staked_total))
		logger.info("vote yes: " + str(vote_yes))
		logger.info("vote no: " + str(vote_no))
		logger.info("___________________")
	resp = flask.Response(proposals)
	resp.headers['Access-Control-Allow-Origin'] = '*'
	return resp
	# except Exception as err:
	# 		return err



@app.route('/getAllProposals')
def proposals():
	logger = reactive_log('getAllProposals')
	try:
		rawtext=urlopen(TALLY_API,timeout=15).read()
		proposals = json.loads(rawtext.decode('utf8'))
	except Exception as err:
		logger.error(err)
	for proposal in proposals:
		# print("@@@@"+str(proposals[proposal]))
		try:
			propos = Proposal.select().where(Proposal.name == proposal).count()
			logger.info("result"+str(propos))
			# the proposal exists
			if propos > 0:
				propos = Proposal.get(Proposal.name == proposal)
				proposals[proposal]['meet_conditions_days'] = propos.meet_conditions_days
				proposals[proposal]['approved_by_vote'] = propos.approved_by_vote
				proposals[proposal]['approved_by_vote_date'] = str(propos.approved_by_vote_date)
				proposals[proposal]['approved_by_BET'] = propos.approved_by_BET
				proposals[proposal]['reviewed_by_BET_date'] = str(propos.reviewed_by_BET_date)
				proposals[proposal]['approved_by_BPs'] = propos.approved_by_BPs
				proposals[proposal]['approved_by_BPs_date'] = str(propos.approved_by_BPs_date)
				proposals[proposal]['review'] = propos.review
				proposals[proposal]['review_date'] = str(propos.review_date)
				proposals[proposal]['finish'] = propos.finish
				proposals[proposal]['finish_date'] = str(propos.finish_date)
			else:
				proposals[proposal]['meet_conditions_days'] = 0
				proposals[proposal]['approved_by_vote'] = 0
				proposals[proposal]['approved_by_vote_date'] = ""
				proposals[proposal]['approved_by_BET'] = 0
				proposals[proposal]['reviewed_by_BET_date'] = ""
				proposals[proposal]['approved_by_BPs'] = 0
				proposals[proposal]['approved_by_BPs_date'] = ""
				proposals[proposal]['review'] = 0
				proposals[proposal]['review_date'] = ""
				proposals[proposal]['finish'] = ""
				proposals[proposal]['finish_date'] = 0
		except Exception as err:
				logger.error(err)
	resp = flask.Response(json.dumps(proposals), mimetype='application/json')
	resp.headers['Access-Control-Allow-Origin'] = '*'
	return resp


@app.route('/getProposal/<proposal_name>')
def proposal(proposal_name):
	logger = reactive_log('getProposal')
	try:
		rawtext=urlopen(TALLY_API,timeout=15).read()
		proposals = json.loads(rawtext.decode('utf8'))
	except Exception as err:
		logger.error(err)

	try:
		propos = Proposal.select().where(Proposal.name == proposal_name).count()
		logger.info("result"+str(propos))
		# the proposal exists
		if propos > 0:

			propos = Proposal.get(Proposal.name == proposal_name)
			proposals[proposal_name]['meet_conditions_days'] = propos.meet_conditions_days
			proposals[proposal_name]['approved_by_vote'] = propos.approved_by_vote
			proposals[proposal_name]['approved_by_vote_date'] = str(propos.approved_by_vote_date)
			proposals[proposal_name]['approved_by_BET'] = propos.approved_by_BET
			proposals[proposal_name]['reviewed_by_BET_date'] = str(propos.reviewed_by_BET_date)
			proposals[proposal_name]['approved_by_BPs'] = propos.approved_by_BPs
			proposals[proposal_name]['approved_by_BPs_date'] = str(propos.approved_by_BPs_date)
			proposals[proposal_name]['review'] = propos.review
			proposals[proposal_name]['review_date'] = str(propos.review_date)
			proposals[proposal_name]['finish'] = propos.finish
			proposals[proposal_name]['finish_date'] = str(propos.finish_date)
		else:
			proposals[proposal_name]['meet_conditions_days'] = 0
			proposals[proposal_name]['approved_by_vote'] = 0
			proposals[proposal_name]['approved_by_vote_date'] = ""
			proposals[proposal_name]['approved_by_BET'] = 0
			proposals[proposal_name]['reviewed_by_BET_date'] = ""
			proposals[proposal_name]['approved_by_BPs'] = 0
			proposals[proposal_name]['approved_by_BPs_date'] = ""
			proposals[proposal_name]['review'] = 0
			proposals[proposal_name]['review_date'] = ""
			proposals[proposal_name]['finish'] = ""
			proposals[proposal_name]['finish_date'] = 0
	except Exception as err:
			logger.error(err)
	resp = flask.Response(json.dumps(proposals[proposal_name]), mimetype='application/json')
	resp.headers['Access-Control-Allow-Origin'] = '*'
	return resp


@app.route('/review/<proposal_name>')
def review(proposal_name):
	logger = reactive_log('review')
	try:
		propos = Proposal.select().where(Proposal.name == proposal_name).count()
		logger.info("result"+str(proposal_name))
		#the proposal exists
		if propos > 0:
			propos2 = Proposal.get(Proposal.name == proposal_name)
			print(propos2)
			if propos2.approved_by_vote == 1:
				nrow=(Proposal.update(review = 1
					,review_date = datetime.now()
					).where(Proposal.name == proposal_name).execute())
				resp = flask.Response(json.dumps({"result":"ok"}), mimetype='application/json')
				resp.headers['Access-Control-Allow-Origin'] = '*'
				return resp
			else:
				resp = flask.Response(json.dumps({"result":"not approved by vote"}), mimetype='application/json')
				resp.headers['Access-Control-Allow-Origin'] = '*'
				return resp
		else:
			resp = flask.Response(json.dumps({"result":"not exited"}), mimetype='application/json')
			resp.headers['Access-Control-Allow-Origin'] = '*'
			return resp
	except Exception as err:
			logger.error(err)
			resp = flask.Response(json.dumps({"result":"ERROR"}), mimetype='application/json')
			resp.headers['Access-Control-Allow-Origin'] = '*'
			return resp

@app.route('/finish/<proposal_name>')
def finish(proposal_name):
	logger = reactive_log('review')
	try:
		propos = Proposal.select().where(Proposal.name == proposal_name).count()
		logger.info("result"+str(propos))
		# the proposal exists
		if propos > 0:
			propos2 = Proposal.get(Proposal.name == proposal_name)
			if propos2.approved_by_vote == 1 and propos2.approved_by_BET == 1:
				nrow = (Proposal.update(
							finish = 1
							, finish_date = datetime.now()
							).where(Proposal.name == proposal_name).execute())
				resp = flask.Response(json.dumps({"result":"ok"}), mimetype='application/json')
				resp.headers['Access-Control-Allow-Origin'] = '*'
				return resp
			else:
				resp = flask.Response(json.dumps({"result":"not approved by vote and not review"}), mimetype='application/json')
				resp.headers['Access-Control-Allow-Origin'] = '*'
				return resp
		else:
			resp = flask.Response(json.dumps({"result":"not exited"}), mimetype='application/json')
			resp.headers['Access-Control-Allow-Origin'] = '*'
			return resp
	except Exception as err:
			logger.error(err)
			resp = flask.Response(json.dumps({"result":"ERROR"}), mimetype='application/json')
			resp.headers['Access-Control-Allow-Origin'] = '*'
			return resp
