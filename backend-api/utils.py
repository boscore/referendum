from logging.handlers import TimedRotatingFileHandler
from threading import Thread
import re,logging
import threading
import time 
 

# 检查数据是否满足提案条件
# 票数大于10% bp参与投票
# 因为有可能投弃权票，所以需要total_vote参数
def proposal_base_condition_ckeck(bp_votes = 0, total_votes = 10, yes = 0, no = 0):
    VOTE_RATIO = 0.1
    YES_NO_RATIO = 1.5
    try:
        if bp_votes * VOTE_RATIO <= total_votes and yes / no > YES_NO_RATIO:
            return True
        else:
            return False
    except ZeroDivisionError as err:
        print(err)

def auditor_base_condition_ckeck(bp_votes = 0, total_votes = 10, yes = 0, no = 0):
    VOTE_RATIO = 0.03
    YES_NO_RATIO = 1.5
    try:
        if bp_votes * VOTE_RATIO <= total_votes and yes / no > YES_NO_RATIO:
            return True
        else:
            return False
    except ZeroDivisionError as err:
        print(err)



# 创建log 默认位置'./quant/log/'  默认保存7天
def init_log(log_name, path = './quant/log/', backupCount=7):

    # 创建一个logger
    # 创建一个handler，用于写入日志文件
    path = path + log_name + '.log'  
    fh = TimedRotatingFileHandler(filename=path, when="D", interval=1, backupCount=backupCount)
    fh.suffix = "%Y-%m-%d"
    fh.extMatch = re.compile(r"^\d{4}-\d{2}-\d{2}$")
    fh.setLevel(logging.DEBUG)      

    # 再创建一个handler，用于输出到控制台
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)        
    # 定义handler的输出格式
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)
    
    # 给logger添加handler
    logger = logging.getLogger(log_name)
    logger.setLevel(logging.DEBUG)        
    logger.addHandler(fh)
    logger.addHandler(ch)
    return logger

# 断线重新打log
def reactive_log(log_name):

    logger = logging.getLogger(log_name)

    return logger


# 多线程发IBC失败信息
# def send_signal_multi(content, logger, msg_send=True):
#     if msg_send:
#         ddRobot = dingRobot()
#         try:
#             # 开启邮件线程
#             t_send_msg = threading.Thread(target=ddRobot.dingStart, args=(content,))
#             t_send_msg.setName('_dingding_signal_sending')
#             t_send_msg.setDaemon(True)
#             t_send_msg.start()
#             t_send_msg.join()
#         except Exception as err:
#             logger.error("DingDing Signal error %s" % str(err))
#         finally:
#             logger.info('DingDing msg sent!')



# 线程检查
def check_eos_to_bos_Thread(eos_to_bos, thread_name_prefix, sleeptimes=180, initThreadsName=[],):
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
                if (task == thread_name_prefix+'eos_to_bos'):
                    t = threading.Thread(target=eos_to_bos,args=(True,))#重启线程
                    t.setName(task)#重设name
                    t.start()          
                elif (task == 'MainThread'):
                    pass
        time.sleep(sleeptimes)#隔一段时间重新运行，检测有没有线程down

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

# 启动多进程
def processing_start(worker_list):
    print('Parent process %s.' % os.getpid())
    for i in range(len(worker_list)):
        print('---启动进程%d---'%i)
        p = Process(target=worker_list[i])
        print('Process will start.')
        p.start()



if __name__ == '__main__':
    result = proposal_base_condition_ckeck(100, 10, 7, 3)
    print(result)

    result = auditor_base_condition_ckeck(100, 10, 7, 3)
    print(result)

