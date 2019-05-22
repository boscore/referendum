from eospy.cleos import Cleos
from utils import *
import numpy as np
import pandas as pd  
import csv

BOS_URLS = [
		'https://api.boscore.io'
		,'https://bos.eoshenzhen.io:9443'
		,'https://api-bos.eospacex.com'
		]


# util func
def test_net_is_working(urls):
	logger = reactive_log('test_net_is_working')
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
			logger.error(err)
	return url_index

Tstart =time.process_time()

url_index = test_net_is_working(BOS_URLS)
ce = Cleos(BOS_URLS[url_index])
result = []
logger = reactive_log('getStake')


current_total = 655507
each_record = 1000
thread_count = 10
top06w = ['.bos.c','.bos.k','.c.c','.c.c.c','.k.k.k','1.c','1.k','1.money','1.rose','1.xmas']
top13w = ['fctqjkkthbje','fctranxfrykx','fctrqssvcelj','fctryxmqbfep','fctsfnnrpoun','fctsnmnmimsn','fctsoirivlfe','fctswfrqjriw','fctsyexoqrgv','fctsynhwiapk']
top19w = ['hruznieecdmr','hrvalobudwcj','hrvatapqeodp','hrvavanmebfv','hrvavovxfyvj','hrvbkfbrfhgk','hrvcqtzcrtke','hrvcwpexvcvv','hrvebnfzvcld','hrvehfadvyco']
top26w = ['khgdotbkhpuo','khgdpvvzvgif','khgdtwdfnhxo','khggwseeicqy','khghackxphaa','khghezffdywj','khghtxxcyomx','khgichqisndx','khgjiyvjiguq','khgkdhcwhxvh']
top32w = ['mwlpyllrotcz','mwlqylfbykak','mwlracqoqtyv','mwlsexoqmdca','mwlsivwcppji','mwltpvmyixxo','mwluoldbyola','mwlvcfmwrrpq','mwlvlobvjofu','mwlvxgvdeulv']
top39w = ['pmbicalqvyqu','pmbigfsofflc','pmbkhwcxpbtw','pmbkkdyvdonj','pmbnlwaiktcf','pmbnmdxcdacr','pmbnyzfrsuod','pmbpqhhyhxsw','pmbpxbjrzvxq','pmbpzaezcful']
top45w = ['saqvglwddreh','saqzlckzfhbu','sar1i2p15y15','sarafamjcbfo','sarapmgkfodg','sarboxytykju','sarbxwomicfq','sarcktfxgwrp','sarcqdqxnbcg','sarfhklqerdn']
top52w = ['upujhhqacxgq','upumocwvtngu','upumwnjxnikp','upuodvygdoav','upuoriwbykbl','upupusnbtbop','upusdwpkcnkv','upusnqlvceyo','uputdeallouh','uputemxxzvur']
top58w = ['xevhewsesqrl','xevicemdlmzl','xevigpxjdoac','xevjexvnuixv','xevjhtvquzmj','xevkbiptkubv','xevknqnbjwsk','xevldnjzvvjc','xevlgkbpbqsc','xevloihsgjsn']
top65w = ['zudlvkwgtlam','zudlwirrrwoq','zudmfaqqlcdc','zudmifgxinoi','zudmxppkzbqk','zudocoubmusc','zudphsqslvrg','zudrrfykijss','zudsgdhouxqw','zudsmyscdcbk']

#init

def check_lo_up(top):
    _i = 0
    _j = 0

    try:
        for i in (0, len(top)-1):
            # print(top[i])
            result = ce.get_table(code='eosio',scope='eosio',table='voters',lower_bound =top[i],  limit=1, timeout=30)
            if len(result) > 0:
                _i = i
                if(i < len(top)-1):
                    for j in (i, len(top)-1):
                        # print(top[j])
                        result2 = ce.get_table(code='eosio',scope='eosio',table='voters',lower_bound =top[j],  limit=1, timeout=30)
                        if len(result2) > 0:
                            _j = j
                            break
                else:
                    _j = _i
                    break
    except Exception as es:
        logger.error(es)
    return _i, _j

#thread1
i1, i11 = check_lo_up(top06w)
time.sleep(1)
print(top06w[i1])
i2, i22 = check_lo_up(top13w)
time.sleep(1)
print(top13w[i2])
i3, i33 = check_lo_up(top19w)
time.sleep(1)
print(top19w[i3])
i4, i44 = check_lo_up(top26w)
time.sleep(1)
print(top26w[i4])
i5, i55 = check_lo_up(top32w)
time.sleep(1)
print(top32w[i5])
i6, i66 = check_lo_up(top39w)
time.sleep(1)
print(top39w[i6])
i7, i77 = check_lo_up(top45w)
time.sleep(1)
print(top45w[i7])
i8, i88 = check_lo_up(top52w)
time.sleep(1)
print(top52w[i8])
i9, i99 = check_lo_up(top58w)
time.sleep(1)
print(top58w[i9])
i10, i1010 = check_lo_up(top65w)
time.sleep(1)
print(top65w[i10])
time.sleep(1)

def get_all_stake(lower, upper):
    more = True
    _result = []
    try:
        while more == True:
            result = ce.get_table(code='eosio',scope='eosio',table='voters',lower_bound = lower, upper_bound = upper, limit=2000, timeout=30)
            print(result)
            if len(result) > 0:
                _result = _result + result['rows']
                if(result['rows'][-1]['owner'] != upper): 
                    more = result['more']
                    lower = result['rows'][-1]['owner']
                    print(lower)
                    print('\n')
                else:
                    break
            else:
                break
            time.sleep(5)
    except Exception as ex:
        logger.error(ex)
        get_all_stake(lower, upper)
    finally:
        return _result


# multi thread require

threads = [] 
tkl1 = ThreadWithReturnValue(target=get_all_stake,args=(top06w[i1], top13w[i2]))
tkl2 = ThreadWithReturnValue(target=get_all_stake,args=(top13w[i22], top19w[i3]))
tkl3 = ThreadWithReturnValue(target=get_all_stake,args=(top19w[i33], top26w[i4]))
tkl4 = ThreadWithReturnValue(target=get_all_stake,args=(top26w[i44], top32w[i5]))
tkl5 = ThreadWithReturnValue(target=get_all_stake,args=(top32w[i55], top39w[i6]))
tkl5 = ThreadWithReturnValue(target=get_all_stake,args=(top39w[i66], top45w[i7]))
tkl6 = ThreadWithReturnValue(target=get_all_stake,args=(top45w[i77], top52w[i8]))
tkl7 = ThreadWithReturnValue(target=get_all_stake,args=(top52w[i88], top58w[i9]))
tkl8 = ThreadWithReturnValue(target=get_all_stake,args=(top58w[i99], top65w[i10]))

tkl1.setName('tkl1')                
tkl2.setName('tkl2')                
tkl3.setName('tkl3')                
tkl4.setName('tkl4')                
tkl5.setName('tkl5')                
tkl6.setName('tkl6')                
tkl7.setName('tkl7')                
tkl8.setName('tkl8')                

tkl1.setDaemon(True)
tkl2.setDaemon(True)
tkl3.setDaemon(True)
tkl4.setDaemon(True)
tkl5.setDaemon(True)
tkl6.setDaemon(True)
tkl7.setDaemon(True)
tkl8.setDaemon(True)

tkl1.start()
tkl2.start()
tkl3.start()
tkl4.start()
tkl5.start()
tkl6.start()
tkl7.start()
tkl8.start()


threads.append(tkl1)        
threads.append(tkl2)        
threads.append(tkl3)        
threads.append(tkl4)        
threads.append(tkl5)        
threads.append(tkl6)        
threads.append(tkl7)        
threads.append(tkl8)

for t in threads:
    lock = threading.Lock()
    if lock.acquire():
        result = t.join() + result
        lock.release()
Tend =time.process_time()

# result = get_all_stake(top06w[i1], top13w[i2])
df = pd.DataFrame.from_records(result)



def df_to_csv(df, logger):
        start = time.process_time()
        filename =  "total_stake.csv"
        try:
            df.to_csv('./' + filename,index=False,header=False)
        except TypeError as err:
            logger.info("save csv err：%s" % err)
        end = time.process_time()
        logger.info("csv in %ss" % str(round3(end - start)))
print(len(result))

def df_summary(df, logger):
        sum = 0
        start = time.process_time()
        filename =  "total_stake.csv"
        try:
            df = pd.read_csv('./' + filename)
            df2 =  df[df.iloc[:,4].str.contains(',')]
            sum = df2.iloc[:,9].sum()
            print(df2)
        except TypeError as err:
            logger.info("read csv err：%s" % err)
            end = time.process_time()
            logger.info("csv in %ss" % str(round3(end - start)))
        finally:
            return sum

df_to_csv(df,logger)
sum = df_summary(df, logger)

out = open('sum.csv', 'a', newline='')
csv_write = csv.writer(out, dialect='excel')
csv_write.writerow(sum)


logger.info('finished. during %ss' % round4(Tend-Tstart))
print(round4(Tend-Tstart))
print('sum:'+str(sum))

