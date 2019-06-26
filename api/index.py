from flask import Flask
import flask
from eospy.cleos import Cleos
from init_db import *
from utils import *
import json
from urllib.request import urlopen


# init global config
cfg = Config.select().limit(1).get()

# constants
BOS_URLS = [
		'https://api.boscore.io'
		,'https://bos.eoshenzhen.io:9443'
		,'https://api-bos.eospacex.com'
		]

TALLY_API = cfg.TALLY_API
VOTE_TOTAL_API = cfg.VOTE_TOTAL_API
AUDITOR_TOTAL_API = cfg.AUDITOR_TOTAL_API
BP_TOTAL_VOTES = cfg.BP_TOTAL_VOTES
PROPOSAL_MEET_DAYS = cfg.PROPOSAL_MEET_DAYS
AUDITOR_MEET_DAYS = cfg.PROPOSAL_MEET_DAYS

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
# caculate propsal condition meet
@app.route('/getJson')
def jsoninfo():
	logger = reactive_log('json')
	url_index = test_net_is_working(BOS_URLS)
	logger.info('BOS Node ['+ str(url_index) + '] is working:' + BOS_URLS[url_index])
	ce = Cleos(url=BOS_URLS[url_index])
	BP_TOTAL_VOTES = cfg.BP_TOTAL_VOTES
	try:
		rawtext = urlopen(VOTE_TOTAL_API, timeout=15).read()
		NEW_BP_TOTAL_VOTES = json.loads(rawtext.decode('utf8'))['bp_votes']
		BP_TOTAL_VOTES = int(NEW_BP_TOTAL_VOTES) if int(NEW_BP_TOTAL_VOTES) > 0 else BP_TOTAL_VOTES
	except Exception as err:
		logger.error(err)
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
	#iterater the current proposals
	for proposal in proposals:
		proposal_item = proposals[proposal]
		vote_key = proposal_item['stats']['votes']
		stake_key = proposal_item['stats']['staked']
		staked_total = float(int(proposal_item['stats']['staked']['total'])) if 'total' in stake_key else 0
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
					_approved_by_vote = 1 if int(meet) >= PROPOSAL_MEET_DAYS else 0
					_approved_by_vote_date = datetime.now() if abvd == None else abvd
					# if meet >= 0 and abv != False and abvd != None:
					nrow = (Proposal.update(
						json_info = proposal_item
						, vote_total = vote_total
						, staked_total = staked_total
						, vote_bp_total = BP_TOTAL_VOTES
						, vote_yes = vote_yes
						, vote_no = vote_no
						, meet_conditions_days = int(meet) + 1 if int(meet) < PROPOSAL_MEET_DAYS + 1 else int(meet)						, approved_by_vote = _approved_by_vote						, approved_by_vote_date = _approved_by_vote_date
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

# caculate auditor condition meet
@app.route('/getAuditorJson')
def aujsoninfo():
	logger = reactive_log('auditorjson')
	url_index = test_net_is_working(BOS_URLS)
	logger.info('BOS Node ['+ str(url_index) + '] is working:' + BOS_URLS[url_index])
	ce = Cleos(url=BOS_URLS[url_index])
	BP_TOTAL_VOTES = cfg.BP_TOTAL_VOTES
	try:
		rawtext = urlopen(VOTE_TOTAL_API, timeout=15).read()
		NEW_BP_TOTAL_VOTES = json.loads(rawtext.decode('utf8'))['bp_votes']
		BP_TOTAL_VOTES = int(NEW_BP_TOTAL_VOTES) if int(NEW_BP_TOTAL_VOTES) > 0 else BP_TOTAL_VOTES
	except Exception as err:
		logger.error(err)
	print(BP_TOTAL_VOTES)
	# try:
		# get tally json file
	auditors = {}
	try:
		rawtext = urlopen(AUDITOR_TOTAL_API, timeout=15).read()
		auditors = json.loads(rawtext.decode('utf8'))
	except Exception as err:
		logger.error(err)
	# get Voted Total
	#iterater the current auditors
	for auditor in auditors:
		auditor_item = auditors[auditor]
		vote_key = auditor_item['stats']['votes']
		stake_key = auditor_item['stats']['staked']
		staked_total = float(int(auditor_item['stats']['staked']['total'])) if 'total' in stake_key else 0
		vote_total = auditor_item['stats']['votes']['total'] if  'total' in vote_key else 0
		vote_yes = auditor_item['stats']['votes']['1'] if  '1' in vote_key else 0
		vote_no = auditor_item['stats']['votes']['0'] if '0' in vote_key else 0
		_meet_conditions_days =  0
		_approved_by_vote =  0
		_approved_by_vote_date =  None
		
		# check if the auditor exists
		try:
			audits = AuditorTally.select().where(AuditorTally.name == auditor).count()
			logger.info("result"+str(audits))				
			# the auditor exists
			if audits > 0:
				# check if the auditor is passed
				if auditor_base_condition_ckeck(BP_TOTAL_VOTES, staked_total, vote_yes, vote_no):
					audits = AuditorTally.get(AuditorTally.name == auditor)
					meet = audits.meet_conditions_days
					# abv = audits.approved_by_vote 
					abvd = audits.approved_by_vote_date
					_approved_by_vote = 1 if int(meet) >= AUDITOR_MEET_DAYS else 0
					_approved_by_vote_date = datetime.now() if abvd == None else abvd
					# if meet >= 0 and abv != False and abvd != None:
					nrow = (AuditorTally.update(
						json_info = auditor_item
						, vote_total = vote_total
						, staked_total = staked_total
						, vote_bp_total = BP_TOTAL_VOTES
						, vote_yes = vote_yes
						, vote_no = vote_no
						, meet_conditions_days = int(meet) + 1 if int(meet) < AUDITOR_MEET_DAYS + 1 else int(meet)						, approved_by_vote = _approved_by_vote						, approved_by_vote_date = _approved_by_vote_date
						).where(AuditorTally.name == auditor).execute())
				else:
					nrow = (AuditorTally.update(
						json_info = auditor_item
						, vote_total = vote_total
						, staked_total = staked_total
						, vote_bp_total = BP_TOTAL_VOTES
						, vote_yes = vote_yes
						, vote_no = vote_no
						, meet_conditions_days = 0
						, approved_by_vote = 0
						, approved_by_vote_date = None
						).where(AuditorTally.name == auditor).execute())
			else:
				if auditor_base_condition_ckeck(BP_TOTAL_VOTES, staked_total, vote_yes, vote_no):
					_approved_by_vote = 1
					_approved_by_vote_date = datetime.now()
				else:
					pass
				new_auditor = AuditorTally(
				  name = auditor
                , json_info = auditor_item
				, vote_total = vote_total
				, staked_total = staked_total
				, vote_bp_total = BP_TOTAL_VOTES
				, vote_yes = vote_yes
				, vote_no = vote_no
				, meet_conditions_days = _meet_conditions_days
				, approved_by_vote = _approved_by_vote
				, approved_by_vote_date = _approved_by_vote_date
				)
				new_auditor.save()  
		except Exception as err:
			logger.error(err)
		logger.info("auditor name: " + auditor)
		logger.info("vote total: " + str(vote_total))
		logger.info("staked total: " + str(staked_total))
		logger.info("vote yes: " + str(vote_yes))
		logger.info("vote no: " + str(vote_no))
		logger.info("___________________")
	resp = flask.Response(auditors)
	resp.headers['Access-Control-Allow-Origin'] = '*'
	return resp
	# except Exception as err:
	# 		return err

@app.route('/getAllAuditors')
def auditorsinfo():
	logger = reactive_log('getAllAuditors')
	try:
		rawtext = urlopen(AUDITOR_TOTAL_API, timeout=15).read()
		auditors = json.loads(rawtext.decode('utf8'))
	except Exception as err:
		logger.error(err)
	for auditor in auditors:
		try:
			propos = AuditorTally.select().where(AuditorTally.name == auditor).count()
			logger.info("result"+str(propos))
			# the auditor exists
			if propos > 0:
				propos = AuditorTally.get(AuditorTally.name == auditor)
				auditors[auditor]['meet_conditions_days'] = propos.meet_conditions_days
				auditors[auditor]['approved_by_vote'] = propos.approved_by_vote
				auditors[auditor]['approved_by_vote_date'] = str(propos.approved_by_vote_date)
			else:
				auditors[auditor]['meet_conditions_days'] = 0
				auditors[auditor]['approved_by_vote'] = 0
				auditors[auditor]['approved_by_vote_date'] = ""
		except Exception as err:
				logger.error(err)
	resp = flask.Response(json.dumps(auditors), mimetype='application/json')
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
			resp = flask.Response(json.dumps(
				{"result": "not existed"}), mimetype='application/json')
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
			if propos2.approved_by_vote == 1 and propos2.review == 1:
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
