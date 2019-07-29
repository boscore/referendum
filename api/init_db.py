from peewee import *
from datetime import date, datetime
import os

DATABASE = "tally.db"

os.environ.get('DB_PWD')
db = SqliteDatabase(DATABASE)
# db = PostgresqlDatabase('d3cjkjmhbkt11i', user='dtzkpiirdsycdh', password=os.environ.get('DB_PWD'), host='ec2-54-247-178-166.eu-west-1.compute.amazonaws.com', port=5432)


class BaseModel(Model):
    class Meta:
        database = db


class Proposal(BaseModel):
    name = CharField(unique=True, index=True)
    json_info = TextField()
    vote_total = BigIntegerField()
    vote_yes = SmallIntegerField()
    vote_no = SmallIntegerField()
    staked_total = BigIntegerField()
    vote_bp_total = BigIntegerField()
    meet_conditions_days = IntegerField()
    approved_by_vote = BooleanField()
    approved_by_vote_date = DateTimeField(null=True)
    approved_by_BET = BooleanField()
    reviewed_by_BET_date = DateTimeField(null=True)
    approved_by_BPs = BooleanField()
    approved_by_BPs_date = DateTimeField(null=True)
    review = BooleanField()
    review_date = DateTimeField(null=True)
    finish = BooleanField()
    finish_date = DateTimeField(null=True)
    timestamp = DateTimeField(default=datetime.now)


class AuditorTally(BaseModel):
    name = CharField(unique=True, index=True)
    json_info = TextField()
    vote_total = BigIntegerField()
    vote_yes = SmallIntegerField()
    vote_no = SmallIntegerField()
    staked_total = BigIntegerField()
    vote_bp_total = BigIntegerField()
    meet_conditions_days = IntegerField()
    approved_by_vote = BooleanField()
    approved_by_vote_date = DateTimeField(null=True)
    timestamp = DateTimeField(default=datetime.now)


class ProposalTanslate(BaseModel):
    name = CharField(index=True)
    lang = CharField(index=True)
    title = TextField()
    content = TextField()
    timestamp = DateTimeField(default=datetime.now)


class AuditorTanslate(BaseModel):
    name = CharField(index=True)
    lang = CharField(index=True)
    bio = TextField()
    timestamp = DateTimeField(default=datetime.now)


class Config(BaseModel):
    TALLY_API = TextField()
    VOTE_TOTAL_API = TextField()
    AUDITOR_TOTAL_API = TextField()
    BP_TOTAL_VOTES = BigIntegerField()
    PROPOSAL_VOTE_RATIO = FloatField()
    AUDITOR_VOTE_RATIO = FloatField()
    PROPOSAL_YES_NO_RATIO = FloatField()
    AUDITOR_YES_NO_RATIO = FloatField()
    PROPOSAL_MEET_DAYS = IntegerField()
    AUDITOR_MEET_DAYS = IntegerField()


db.connect()
db.create_tables(
    [Proposal, AuditorTally, AuditorTanslate, ProposalTanslate, Config])
db.close()

dbconfig = Config.create(
    TALLY_API='https://s3.amazonaws.com/bos.referendum/referendum/tallies/latest.json',
    VOTE_TOTAL_API='https://s3.amazonaws.com/bos.referendum/referendum/summaries/latest.json',
    AUDITOR_TOTAL_API='https://s3.amazonaws.com/bos.referendum/referendum/auditor.tallies/latest.json',
    BP_TOTAL_VOTES=311193750652,
    PROPOSAL_VOTE_RATIO=0.4,
    AUDITOR_VOTE_RATIO=0.03,
    PROPOSAL_YES_NO_RATIO=1.5,
    AUDITOR_YES_NO_RATIO=1.5,
    PROPOSAL_MEET_DAYS=20,
    AUDITOR_MEET_DAYS=20
)

dbconfig.save()
cfg = Config.select().limit(1).get()
