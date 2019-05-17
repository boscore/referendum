from peewee import *
from datetime import date,datetime
import os

DATABASE = "tally.db"

# os.environ.get('DB_PWD')
# db = SqliteDatabase(DATABASE)
db = PostgresqlDatabase('d3cjkjmhbkt11i', user='dtzkpiirdsycdh', password=os.environ.get('DB_PWD'), host='ec2-54-247-178-166.eu-west-1.compute.amazonaws.com', port=5432)

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
    approved_by_vote_date = DateTimeField(null = True)
    approved_by_BET = BooleanField()
    reviewed_by_BET_date = DateTimeField(null = True)
    approved_by_BPs = BooleanField()
    approved_by_BPs_date = DateTimeField(null = True)
    timestamp = DateTimeField(default=datetime.now)




db.connect()
db.create_tables([Proposal])
db.close()
