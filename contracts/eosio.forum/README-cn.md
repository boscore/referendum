一个简易的EOS论坛，有通信和投票系统
===================================================

该合同的目的是支持EOS公投系统，将提案及其相关投票存储在区块链状态的RAM下。

它也可以用来发帖子和状态，但它们就不会存存储在区块链的RAM中。
它允许发布通过了身份验证的消息，你在链的历史记录中可以看到它。
它需要链外工具来对帖子和状态命令的输出进行排序，显示，汇总和报告。

### 生命周期

首先调用 `propose` 命令，给出提议者的帐户、提案的名称（提案ID，用来区别它与其他提案）、
提案标题、额外元数据的JSON字符串（规范尚未定义，可以留空）、
以及有效期（必须在将来的6个月之前）。

一旦提案被创建，人们就可以通过 `vote` 命令开始投票。
调用投票命令需要选民的帐户、提案的名称、投票的值（`0` 表示反对，`1`表示同意）、
以及包含额外元数据的JSON字符串（规范尚未定义，可以留空）。

如果出现新的投票，它将覆盖任何先前的值。这意味着，
如果你最初投了一个值为 `0` 的票（反对票），然后对同一提案进行第二次 `vote`，值为 `1`（同意票），
你对该提案的当前投票就是 `1`。

与EOS主网BP竞选不同，在这，投票之后是没有强度衰退的。
票投出去后，它不会改变，也不会被删除，
除非你用了 `unvote`命令，或者提议被 `clnproposal` 命令清理了。

用户可以通过 `unvote` 命令删除其投票。
使用选民的帐户和提案的名称来调用 `unvote` 命令。
一个 `unvote` 命令就可以从提案中完全删除你的投票，并清除与该投票相关的RAM用量。

### 开发

需要的软件：
- [Docker 17+](https://www.docker.com/get-started)
- [eosc 1.1+](https://github.com/eoscanada/eosc/releases)
- [eos-bios 1.2+](https://github.com/eoscanada/eos-bios/releases)

确保 `docker` 二进制文件、 `eosc` 还有 `eos-bios` 在 你的`PATH` 环境中是可用的。
需要`eos-bios` 和 `eosc` 二进制文件才能正确启动本地开发&测试节点以及运行自动化测试套件。

#### 构建

直接调用 `build.sh` 脚本即可启动 Docker 容器编写合约：

```
./build.sh
```

#### 运行

使用 `run.sh` 脚本轻松启动开发节点
它会用 [eos-bios](https://github.com/eoscanada/eos-bios) 和 Docker 启动一个配置完全的沙盒 `nodeos` 开发节点：

```
./run.sh
```

这会创建以下帐户：
- `eosio.forum`
- `proposer1`
- `proposer2`
- `poster1`
- `poster2`
- `voter1`
- `voter2`
- `zzzzzzzzzzzz`

上面开发节点所创建的账户都用下面这对公钥和私钥：

- 公钥：`EOS5MHPYyhjBjnQZejzZHqHewPWhGTfQWSVTWYEhDmJu4SXkzgweP`
- 私钥：`5JpjqdhVCQTegTjrLtCSXHce7c9M8w7EXYZS7xC13jVFF4Phcrx`

你可以通过调用 `./ tests / data.sh` 脚本直接预填写一些提案和投票到你的环境中。

```
./tests/data.sh
```

结束使用 `nodeos` 开发节点后，只需调用 `stop.sh` 即可停止正在运行的实例：

```
./stop.sh
```

##### 环境

只需导出以下环境变量，即可在你的终端上用 `eosc` 轻松与开发节点进行交互：

```
export EOSC_GLOBAL_INSECURE_VAULT_PASSPHRASE ="secure"
export EOSC_GLOBAL_API_URL ="http：// localhost：9898"
export EOSC_GLOBAL_VAULT_FILE ="`pwd` / tests / eosc-vault.json"
```

当你的 `cd` 在项目根目录中时，可用 [direnv](https://direnv.net/) 工具自动导入这些变量


#### 测试

运行全自动测试套件很简单：

```
./tests.sh
```

这将启动 `nodeos` 开发节点（通过 `./run.sh`），
然后执行 `tests` 文件夹中的所有集成测试（想知道拾取的具体文件，请参见 [all.sh](./tests/all.sh)）。

想正确运行测试，你需要要把提案冻结期切换为2秒
（不然你等3天有点太长了！）。`tests.sh` 脚本也会自动将这个冻结周期自动更改为2秒，
所以当你测试运行时它是这样的：

```
constexpr static uint32_t FREEZE_PERIOD_IN_SECONDS = 2; // NEVER MERGE LIKE THIS
```

**注意** 一旦测试脚本完成，不管是结果报错还是成功，`tests.sh` 脚本会自动恢复更改。
在发送更改之前，以防万一，你还是应该检查更改是否被有效地还原了，别让一个只有2秒的冻结期的推送到库中！

-------------------------

### 合约部署

此代码的最新版本在EOS主网的 `eosforumrcpp` 帐户上
和EOS Kylin测试网的 `cancancan345` 帐户上。

--------------------------

### 操作参考

以下列表是可有的操作：

- [propose](#action-propose)（提案）
- [cancel](#action-cancel)
- [vote](#action-vote)（投票）
- [unvote](#action-unvote)（撤销投票）
- [post](#action-post)（发布帖子）
- [unpost](#action-unpost)（撤销发布了的帖子）
- [status](#action-status)（状态）

-------------------------

#### Action `propose`

向社区提出一个提案。

##### 参数

- `proposer` (type `name`) - 提案者的账户名
- `proposal_name` (type `name`) - 提案的名称，把它与其他提案区别开来的ID
- `title` (type `string`) - 提案的标题 (必须少于1024个字符)
- `proposal_json` (type `string`) - 提案的 JSON 元数据，还没有具体规格，请见[JSON结构指南](#json-structure-guidelines)

##### 拒绝情况

- 当缺少 `proposer` 的签名时
- 当 `proposal_name` 已经存在时
- 当 `title` 超过1024个字符时
- 当 `proposal_json` JSON无效或太大时（必须是JSON对象且小于32768个字符）

##### 例如

```
eosc tx create eosio.forum propose '{"proposer": "proposer1", "proposal_name": "example", "title": "The title, for list views", "proposal_json": ""}' -p proposer1@active
```
或

```
eosc forum propose proposer1 example "The title, for list views" 2019-01-30T17:03:20 --json "[JSON object]"
```

-------------------------


#### Action `vote`

使用你的帐户给某个提案投票。

##### 参数

- `voter` (type `name`) - 投票者账户
- `proposal_name` (type `name`) - 被投票的提案名称
- `vote` (type `uint8`) - 你对该提案的投票，“0”表示反对，“1”表示同意
- `vote_json` (type `string`) - 投票的 JSON 元数据，还没有具体规格，请见[JSON结构指南](#json-structure-guidelines)

##### 拒绝情况


- 当缺少 `voter` 的签名时
- 当 `proposal_name` 不存在时
- 当 `proposal_name` 已经过期时
- 当 `vote_json` JSON无效或太大时（必须是JSON对象且小于8192个字符）

##### 例如

```
eosc tx create eosio.forum vote '{"voter": "voter1", "proposal_name": "example", "vote": 0, "vote_json": ""}' -p voter1@active
```
或
```
eosc forum vote voter1 example 0
```

-------------------------

#### Action `unvote`

删除当前的有效投票，有效地赎回存储投票的RAM。
当然，你的的投票将不再被统计到当前提案中（无论是同意还是反对票）。

对于已过期且在其3天的冻结期的提案，是不可能 `unvote` 的。
如果提案已过期且冻结期已过，则可以对提案进行 `unvote`。


##### 参数

- `voter` (type `name`) - 投票者账户
- `proposal_name` (type `name`) - 想从中删除你的投票的提案名称

##### 拒绝情况

- 当缺少 `voter` 的签名时
- 当`proposal_name`不存在时
- 当`proposal_name`过期但在其3天冻结期内时

##### 例如

```
eosc tx create eosio.forum unvote '{"voter": "voter1", "proposal_name": "example"}' -p voter1@active
```
或
```
eosc forum unvote voter1 example
```

-------------------------

#### Action `cancel`

Is used to cancel a `proposal_name` authorized by the `proposer`.

##### Parameters

- `proposer` (type `name`) - The original proposer
- `proposal_name` (type `name`) - The proposal's name to cancel

##### Rejections

- When missing signatures of proposal's `proposer`
- When `proposal_name` does not exist

##### Example

```
eosc tx create eosio.forum cancel '{"proposer": "proposer1", "proposal_name": "example"}' -p proposer1@active
```

**注意** `proposal1` 必须与当初 `example` 提案的创建者相同。

-------------------------

#### Action `post`

发布一个帖子。

##### 参数

- `poster` (type `name`) - 发帖的账户
- `post_uuid` (type `string`) - 帖子的 `UUID` (用来回复)
- `content`（type`string`） - 帖子的实际内容
- `reply_to_poster`（type` name`） - 你的帖子所回复的原帖的版主
- `reply_to_post_uuid`（type`string`） - 你的帖子所回复的原帖的 UUID
- `certify`（type` bool`） - 供将来使用
- `json_metadata`（type`string`） - 帖子的JSON元数据，还没有规范，请见[JSON结构指南](#json-structure-guidelines)

##### 拒绝情况

- 当缺少 `poster` 的签名时
- 当 `content` 是一个空字符串时
- 当 `content` 大于10240个字符时
- 当 `post_uuid` 为空字符串时
- 当 `post_uuid` 大于128个字符时
- 当没有设置 `reply_to_poster` 但设置了 `reply_to_post_uuid` 时
- 当 `reply_to_poster` 帐户不存在时
- 当设置了 `reply_to_poster` 并且 `reply_to_post_uuid` 为空字符串时
- 当设置了 `reply_to_poster` 并且 `reply_to_post_uuid` 大于128个字符时
- 当 `json_metadata` JSON无效或太大（必须是JSON对象且小于8192个字符）

##### 例如

```
eosc tx create eosio.forum post '{"poster": "poster1", "post_uuid":"examplepost_id", "content": "hello world", "reply_to_poster": "", "reply_to_post_uuid": "", "certify": false, "json_metadata": "{\"type\": \"chat\"}"}' -p poster1@active
```
或
```
eosc forum post poster1 "hello world"
```

-------------------------

#### Action `unpost`

撤销发帖。

##### 参数

- `poster` (type `name`) - 删除你之前发布的帖子
- `post_uuid` (type `string`) - 要删除帖子的 `UUID`

##### 拒绝情况

- 当缺少 `poster` 的签名时
- 当 `post_uuid` 为空字符串时
- 当 `post_uuid` 大于128个字符时

##### 例如

```
eosc tx create eosio.forum unpost '{"poster": "poster1", "post_uuid":"examplepost_id"}' -p poster1@active
```
或
```
eosc forum unpost poster1 [UUID_of_example]
```

-------------------------

#### Action `status`

记录关联帐户的状态。 如果 `content` 为空，则操作将删除之前的状态。 否则，它将使用 `content` 的内容为 `account` 添加一条状态。

##### 参数

- `account` (type `name`) - 添加状态的账户
- `content` (type `string`) - 状态的内容

##### 拒绝情况

- 当缺少 `account` 的签名时
- 当 `post_uuid` 大于256个字符时
- 当 `content` 是空字符串时，`account` 不存在以前的 `status`

例如（添加状态）：

```
eosc tx create eosio.forum status '{"account": "voter2", "content":"status of something"}' -p voter2@active
```

例如（移除之前的状态）：

```
eosc tx create eosio.forum status '{"account": "voter2", "content":""}' -p voter2@active
```
或
```
eosc forum status voter2 "status of something"
```

例如（移除之前的状态）：

```
eosc forum status voter2 ""
```

-------------------------

#### Table `proposals`

##### 行

- `proposal_name`（type`name`） - 提案的名称，它的ID
- `proposer`（type` name`） - 发出提案的帐户
- `title`（type`string`） - 提案的标题，提案的简要说明
- `proposal_json`（type`tring`） - 元数据的JSON提议，尚无规范，请见[JSON结构指南](#json-structure-guidelines)
- `created_at`（type` time_point_sec`） - 创建提案的日期，ISO 8601字符串格式（UTC）**不含**时区修饰符。

##### 指数
- 一号（`1` type `name`) - 按 `proposal_name` 字段索引
- 二号(`2` type `name`) - 按 `proposer` 索引

##### 示例 (读取所有的提案):

```
eosc get table eosio.forum eosio.forum proposal
```
或
```
eosc forum list
```

##### 示例 （读取某个提案者的所有提案）：

**警告**现在，`eosc` 不支持只用一个直接的key搜索。
相反，它需要一个下限和上限。 上限是独有的，
要正确获取上限，请把帐户名称的最后一个字符更改为EOS名称字母表中的下一个字符（顺序为`a-z1-5`）

比如，想要查找 `testusertest` 提出的所有提议，下限key将是 `testusertest`，
上限key是 `testusertesu`（最后一个字符`t`的下一个字母是`u`）。

```
eosc get table eosio.forum eosio.forum proposal --index 2 --key-type name --lower-bound testusertest --upper-bound testusertesu
```
或
```
eosc forum list --from-proposer testusertest
```

-------------------------

#### Table `status`

##### 行

- `account`（type `name`） - 状态的发布者
- `content`（type `tring`） - 状态的内容
- `updated_at`（type `time_point_sec`） - 状态上次更新的日期，ISO 8601字符串格式（UTC）**不含**时区修饰符。

##### 示例

```
eosc get table eosio.forum eosio.forum status
```

-------------------------

#### Table `vote`

##### 行

- `id`（type `uint64`） - 一对 `voter` 和 `proposal_name` 相对应的特殊ID
- `proposal_name`（type`name`） - 投票相对应的 `proposal_name`
- “voter”（type`name`） - 投票的 `voter`
- `vote`（type`uint8`） - `voter` 的投票值（“0”表示反对票，“1”表示票）
- `vote_json`（type`字符串`） - 投票的JSON元数据，还没有规范，请见[JSON结构指南](#json-structure-guidelines)
- `updated_at`（type`time_point_sec`） - 上次更新投票的日期，ISO 8601字符串格式（UTC）**不含**时区修饰符。

##### 指数
- 一号（`1` type`i64`） - 按 `id` 字段索引
- 二号（`2` 类型 `i128`，以十六进制小端格式输入的） - 按提议名称索引，key是用 `proposal_name`  组成的高字节， `voter` 组成的低字节。
- 三号（以十六进制小端格式输入的`3`类型`i128`） - 按 `voter` 索引，键使用 `voter` 以高字节组成，低字节是`proposal_name`。

##### 示例（读取所有的投票）：

```
eosc get table eosio.forum eosio.forum vote
```

##### 示例（读取某个提案下的所有的投票）:

原理是把 `proposal_name` 转换为整数，将其转换为十六进制，并计算 `voter`（下限）的最低值key和`voter`（上限）的最高键key。

**注意**下面的十六进制值都是小端格式，高字节在右侧和低字节在左侧。

以下是计算表查询的下限/上限的步骤：


1.使用 `eosc tools name` 将 `ramusetest` EOS名称转换为十六进制。

   ```
   eosc tools names ramusetest

   from \ to  hex           	hex_be        	name    	uint64
   ---------  ---           	------        	----    	------
   name   	0040c62a2baca5b9  b9a5ac2b2ac64000  ramusetest  13377287569575133184
   ```

1.通过将 `0000000000000000` 添加到上面的 `hex` 值前面来创建 `lower_bound` key：`0x00000000000000000040c62a2baca5b9`

1.通过将 `ffffffffffffffff` 添加到上面 `hex` 值前面来创建 `upper_bound` key：`0xffffffffffffffff0040c62a2baca5b9`。

现在我们有了下限和上限key，只需执行查询：


```
eosc get table eosio.forum eosio.forum vote --index 2 --key-type i128 --lower-bound 0x00000000000000000040c62a2baca5b9 --upper-bound 0xffffffffffffffff0040c62a2baca5b9
```

你只会看到针对提议 `ramusetest` 的投票。

##### 示例（读取某个选民的所投过的所有提案）：

原理是把 `voter` 转换为整数，将其转换为十六进制，并计算 `proposal_name`（下限）的最低值key和 `proposal_name`（上限）的最高值key。

**注意**下面的十六进制值都是小端格式，高字节在右侧和低字节在左侧。

这里是查询Table的下限/上限的计算步骤：

1.使用 `eosc tools name` 将 `testusertest` EOS名称转换为十六进制。

   ```
   eosc tools names testusertest

   from \ to  hex           	hex_be        	name      	uint64
   ---------  ---           	------        	----      	------
   name   	90b1ca57619db1ca  cab19d6157cab190  testusertest  14605628107949519248
   ```

1.通过将 `0000000000000000` 添加到上面的 `hex` 值的前面来创建 `lower_bound` key：`0x000000000000000090b1ca57619db1ca`

1.通过将 `ffffffffffffffff` 添加到上面 `hex` 值的前面来创建 `upper_bound` key：`0xffffffffffffffff90b1ca57619db1ca`

现在我们有了下限和上限key，只需执行查询：


```
eosc get table eosio.forum eosio.forum vote --index 2 --key-type i128 --lower-bound 0x00000000000000000040c62a2baca5b9 --upper-bound 0xffffffffffffffff0040c62a2baca5b9
```

这样你就能只看到选民 `testusertest` 所投过的所有提案。

JSON结构指南
==============================
#### JSON Structure Guidelines


目前JSON还没有规范，创建帖子，提案和投票时，你可以使用任何词汇。
但是，遵守下面的这个指南可以为你和任何构建这些消息的UI的人省很多的事。

对于 `suggest`、`vote` 和 `post` 中的所有 `json` 前缀或后缀字段，`type` 字段应确定更高阶的协议，并确定将需要哪些兄弟字段。

##### 在一个 `propose` 的 `proposal_json` 字段中

* `type` 是区分各个协议的必填字段。 请参阅下面的 type。

* `question` 表示一个提案的参考语言问题。

* `content` 是Markdown文档，详细说明了有关该提案的所有信息
    （计数方法，时间范围，参考，要求等……）

##### 在一个 `vote` 的 `vote_json` 字段

* `type` 是可选的。 如果不存在，则默认为 `simple` 。

###### `type` 值

* `simple` 就是没有 `type` 一样。 投票的值是这_action_的布尔 `vote` 字段。

##### 在一个 `post` 的 `json_metadata` 的字段中

* `type` 是区分协议的必填字段。 见下文中的 type 例子

以下字段是为了标准化某些 key 的含义。 如果你指定自己的`type`，你可以按需自由定义。

* `title` 是一个标题，将显示在消息上方，通常可点击。 类似于Reddit帖子的标题。

* `tags` 是一个字符串列表，前缀可有可无，带一个 `#`。


###### `type` 值

* `chat` 简单，推出一条消息。

* `eos-bps-roll-call`，这在 EOS BP 电话会议时使用，表明他们在场。

* `eos-bps-emergency`，一旦**3个** BP在一小时内都发出这个消息，所有的BP都可以在1小时内触发唤醒警报。 请勿滥用此消息以避免警报疲劳。 **一些警报的例子：严重漏洞需要缓解，严重的网络问题，需要立即采取行动等。

* `eos-bps-notify`，一旦**7个**BP在一小时内发出此类消息，其他BP可以在接下来的**24小时**内触发通知以引起大家的注意。 **比如：新的ECAF指令需要注意。

* `eos-arbitration-order`，BP们可以监视已知的仲裁论坛帐户，并提醒自己所需的操作。 剩下的字段可以被定义为PDF格式顺序的链接; 对现有的 `eosio.msig` 交易提案的引用; 等等……


证书
=======

MIT [查看证书文件](./LICENSE.md)


致谢
=======

原始代码和灵感来自于 Daniel Larimer
