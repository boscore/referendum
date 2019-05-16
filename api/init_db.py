from peewee import *
from datetime import date,datetime

DATABASE = "tally.db"


db = SqliteDatabase(DATABASE)

class BaseModel(Model):
    class Meta:
        database = db

class Proposal(BaseModel):
    name = CharField(unique=True, index=True)
    json_info = TextField()
    vote_total = BigIntegerField()
    vote_yes = SmallIntegerField()
    vote_no = SmallIntegerField()
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
# {
#  "1person1vote": {
#         "id": "1person1vote_20190113",
#         "proposal": {
#             "expires_at": "2019-05-13T02:00:48",
#             "created_at": "2019-01-13T08:31:00",
#             "proposal_json": "{\"content\":\"One person one vote. One person can cast Max 30 votes like real democratic system.\",\"type\":\"referendum-v1\"}",
#             "title": "one person one vote Max 30 votes.",
#             "proposer": "eosforfuture",
#             "proposal_name": "1person1vote"
#         },
#         "stats": {
#             "votes": {
#                 "0": 74,
#                 "1": 77,
#                 "total": 151,
#                 "proxies": 12,
#                 "accounts": 139
#             },
#             "accounts": {
#                 "0": 4801860596,
#                 "1": 2474681866,
#                 "total": 7276542462
#             },
#             "proxies": {
#                 "0": 13504486021,
#                 "1": 136998897,
#                 "total": 13641484918
#             },
#             "staked": {
#                 "0": 18306346617,
#                 "1": 2611680763,
#                 "total": 20918027380
#             },
#             "vote_participation": false,
#             "more_yes": false,
#             "sustained_days": 0,
#             "block_num": 57783000,
#             "currency_supply": 1011316379.0174,
#             
#             
#             "meet_conditions_days": 10;                  new
#             "approved_by_vote":true,                     new
#             "approved_by_vote_date":2019-05-13T02:00:48  new
#             "approved_by_BET":yes                        new
#             "reviewed_by_BET_date":2019-05-13T02:00:48   new
#             "key":111                                    new
#             "ext_key": 123123                            new
#         }
#     },
#     }