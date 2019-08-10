from logging.handlers import TimedRotatingFileHandler
from threading import Thread
import re,logging
import threading
import time 
from init_db import *

# Check if condition match
# proposal votes > 40% staked_total all net
# Yes / No >= 1.5
def proposal_base_condition_ckeck(bp_votes = 0, staked_total = 10, yes = 0, no = 0):
    VOTE_RATIO = cfg.PROPOSAL_VOTE_RATIO
    YES_NO_RATIO = cfg.PROPOSAL_YES_NO_RATIO
    try:
        if bp_votes * VOTE_RATIO <= staked_total and (no == 0 or yes / no > YES_NO_RATIO):
            return True
        else:
            return False
    except ZeroDivisionError as err:
        print(err)

def auditor_base_condition_ckeck(bp_votes = 0, staked_total = 10, yes = 0, no = 0):
    VOTE_RATIO = cfg.AUDITOR_VOTE_RATIO
    YES_NO_RATIO = cfg.AUDITOR_YES_NO_RATIO
    print(VOTE_RATIO)
    print(YES_NO_RATIO)
    try:
        if bp_votes * VOTE_RATIO <= staked_total and (no == 0 or yes / no > YES_NO_RATIO):
            return True
        else:
            return False
    except ZeroDivisionError as err:
        print(err)



# Create log default dir './quant/log/'  7days
def init_log(log_name, path = './quant/log/', backupCount=7):

    # create the logger
    # create the handler，for writing logfile
    path = path + log_name + '.log'  
    fh = TimedRotatingFileHandler(filename=path, when="D", interval=1, backupCount=backupCount)
    fh.suffix = "%Y-%m-%d"
    fh.extMatch = re.compile(r"^\d{4}-\d{2}-\d{2}$")
    fh.setLevel(logging.DEBUG)      

    # create the handler，output to terminal
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)        
    #  define handler output parttern
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)
    
    # 给logger add handler
    logger = logging.getLogger(log_name)
    logger.setLevel(logging.DEBUG)        
    logger.addHandler(fh)
    logger.addHandler(ch)
    return logger

# rewrite log
def reactive_log(log_name):

    logger = logging.getLogger(log_name)

    return logger



# thread check
def check_eos_to_bos_Thread(eos_to_bos, thread_name_prefix, sleeptimes=180, initThreadsName=[],):
    while True:# loop
        nowThreadsName=[]#save the thread name
        now=threading.enumerate()#get thread name
        for i in now:
            nowThreadsName.append(i.getName())# save the current threads name

        for task in initThreadsName:
            if  task in nowThreadsName:
                pass #if the name contain means it runs correctly
            else:
                print('=== %s stopped，now restart' % task)
                if (task == thread_name_prefix+'eos_to_bos'):
                    t = threading.Thread(target=eos_to_bos,args=(True,))#restart 
                    t.setName(task)#reset thread name
                    t.start()          
                elif (task == 'MainThread'):
                    pass
        time.sleep(sleeptimes)#tick for checking the thread  

# 线程检查
def check_bos_to_eos_Thread(bos_to_eos, thread_name_prefix, sleeptimes=180, initThreadsName=[],):
    while True:#循环运行
        nowThreadsName=[]#用来保存当前线程名称
        now=threading.enumerate()#获取当前线程名
        for i in now:
            nowThreadsName.append(i.getName())#保存当前线程名称

        for task in initThreadsName:
            if  task in nowThreadsName:
                pass #当前某线程名包含在初始化线程组中，可以认为线程仍在运行
            else:
                print('=== %s stopped，now restart' % task)
                if(task == thread_name_prefix+'bos_to_eos'):
                    t = threading.Thread(target=bos_to_eos,args=(True,))#重启线程
                    t.setName(task)#重设name
                    t.start()          
                elif (task == 'MainThread'):
                    pass
        time.sleep(sleeptimes)#隔一段时间重新运行，检测有没有线程down


# 将小数转为1位小数
def round1(num):
    return  round(float(num), 1)

# 校正sleep运行时间
def correct_loop_time(next_loop_time, start_running_seconds, real_running_seconds):
    return next_loop_time + (float(start_running_seconds) - float(real_running_seconds))

# 获取秒位 传入time.time()
def get_time_seconds_str(timestamp):
    return time.strftime('%S',time.localtime(timestamp))

# 找出与现在时间最接近的粒度时间差
def get_approach_granularity_ts(granularity):
    t = time.localtime(time.time())
    # 获取当天0点时间戳
    # 转整数秒 
    nowtime = time.mktime(time.strptime(time.strftime('%Y-%m-%d %H:%M:%S', t),'%Y-%m-%d %H:%M:%S'))
    nowtime = int(nowtime)
    
    zerotime = time.mktime(time.strptime(time.strftime('%Y-%m-%d 00:00:00', t),'%Y-%m-%d %H:%M:%S'))
    zerotime = int(zerotime)
 
    time_range = 1000000
    approach_ts = 0
    for i in range(time_range):
        if zerotime + granularity * i > nowtime:
            approach_ts = zerotime + granularity * i
            break
    negitive_time = approach_ts - nowtime
    return negitive_time+1

    

def running_in_time(granularity, logger):
    negitive_time = get_approach_granularity_ts(granularity)
    print('start after %smin...' % str(round(negitive_time/60)))
    logger.info('start after %smin...' % str(round(negitive_time/60)))
    time.sleep(round(abs(negitive_time),1))

# multi process start
def processing_start(worker_list):
    print('Parent process %s.' % os.getpid())
    for i in range(len(worker_list)):
        print('---Start Process%d---'%i)
        p = Process(target=worker_list[i])
        print('Process will start.')
        p.start()

# Thread with return val
class ThreadWithReturnValue(Thread):
    def __init__(self, group=None, target=None, name=None, args=(), kwargs=None, *, daemon=None):
        Thread.__init__(self, group, target, name, args, kwargs, daemon=daemon)
 
        self._return = None
 
    def run(self):
        if self._target is not None:
            self._return = self._target(*self._args, **self._kwargs)
 
    def join(self):
        Thread.join(self)
        return self._return


def round4(num):
    return  round(float(num), 4)

def round3(num):
    return  round(float(num), 3)


if __name__ == '__main__':
    result = proposal_base_condition_ckeck(311193750652, 129469948578, 26, 0)
    print(result)

    result = auditor_base_condition_ckeck(100, 10, 7, 3)
    print(result)

