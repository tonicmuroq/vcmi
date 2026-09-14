# 下游补丁集

这个 fork 在上游 VCMI 之上维护一组上游不会接受的补丁，基本都是"揭示隐藏信息"性质的，等于作弊。
补丁集以 rebase 方式维护：分支 = 上游某个固定点 + 下面这几个补丁，永远线性，不合并。

## 当前基点

固定在 `upstream/develop` 的一个快照上，不追 develop 的 HEAD。

上游 1.8.0 还没发布（最新 tag 是 1.7.5，落后一千多个提交，回退过去会丢掉 GPU 渲染、敌方回合回放、Lua 脚本等等）。
**等 1.8.0 打上 tag，就改成跟 tag 走**，一年同步两三次即可。

## 补丁

"上游接触面"指这个补丁改到的上游已有文件——rebase 冲突只会发生在这些地方。

### 1. CI: add a mac-arm only build for local use

手动触发的 workflow，只构建 arm64 macOS dmg，本机装不了依赖时靠它出包。
产物打成 zip，因为 `gh run download` 只认这个。

上游接触面：`osx/CMakeLists.txt`。（workflow 文件是新增的。）

### 2. Fix quick load rejecting saves on random maps

纯 bug 修复，跟 reveal 功能无关。**可以提给上游**，提了就能从补丁集里删掉。

上游接触面：`client/CServerHandler.cpp`。

### 3. Let Pandora's Box and event spell points exceed the hero's mana limit

H3 里事件/魔盒给的魔法值是直接加上去的，可以超过英雄上限（上限只约束百分比型的魔法泉、魔力漩涡）。
VCMI 的 `manaOverflowFactor` 默认 0，H3M 读取器又从来不设，于是满蓝英雄踩 +25 魔法的事件一点都拿不到。

**已提上游：vcmi/vcmi#7837。** 合了就删掉这个补丁。

上游接触面：`lib/mapping/MapFormatH3M.cpp`。

### 4. Add lobby options to reveal hidden information

四个大厅开关，默认全关，只有房主能改。写在 `StartInfo` 里所以各客户端一致，开局系统消息会公布开了哪些。

- **Reveal Mage Guild Spells** — 城镇弹窗显示各级魔法行会的法术，含未建造的等级。
- **Reveal Hidden Rewards** — 右键看潘多拉魔盒、野外兵营的内容，以及先知小屋、任务守卫的任务要求和奖励，不用先侦察。
- **Reveal Monster Info** — 选中英雄后右键野怪，显示开战分组以及会加入/索要金币/逃跑/战斗。分组和决策直接调服务端那套代码，预览跟实际一致。
- **Reveal Hidden Events** — 地图事件本来没有贴图、还被 `getVisitableObjs` 过滤掉，开了之后在地图上标出来、右键看内容。只标会对当前玩家触发的；`showInvisible` 作弊开关不受影响，仍然全标。
- **Reveal Enemy Heroes** — 右键敌方英雄看确切部队、主属性、魔法值；Alt+右键开完整英雄界面；战斗中左键敌方英雄或右键其信息面板同样打开。非己方英雄的解散/战术/编队按钮禁用，法术书只读。

几处实现要点：

- 事件标记用自带的 `debug/event.png`，不用编辑器的 `AVZevnt0`——那张图比一格大、带 margin，阴影存在 `COLORKEY` 不处理的调色板索引里，画出来是个品红角块还会溢出地图边界。
- 被揭示的对象走单独的 `RevealedContentsPopup`，守卫和奖励各占一段各带标题；为此把 `getPopupComponents` 拆成 `getPopupGuards` / `getPopupRewards`，合并后的返回值不变。
- 揭示未侦察过的对象时不传英雄上下文，显示地图里的原始数值——否则满蓝英雄看 +25 魔法的事件会显示成 +0。

上游接触面：`lib/ExtraOptionsInfo.h`、`lib/serializer/ESerializationVersion.h`、
`lib/mapObjects/{CGObjectInstance,CRewardableObject,CGPandoraBox,CGCreature,Quest}.{h,cpp}`、
`client/mapView/MapRenderer.cpp`、`client/adventureMap/AdventureMapInterface.{h,cpp}`、
`client/windows/{InfoWindows,CHeroWindow,CSpellWindow,CWindowWithArtifacts}.{h,cpp}`、
`client/battle/{BattleHero,BattleWindow,HeroInfoWindow}.{h,cpp}`、`client/widgets/CComponent.h`、
`client/lobby/{OptionsTabBase,CLobbyScreen}.cpp`、`config/widgets/extraOptionsTab.json`、
`server/CVCMIServer.cpp`、三份翻译。

## 同步上游

`rerere` 已开（`git config rerere.enabled true`），解过一次的冲突下次自动重放。

```sh
git fetch upstream --tags
git branch -f backup/$(date +%F) reveal          # 先留快照
git rebase <目标 ref> reveal                     # 1.8.0 发布后就填 tag 名
git push -f origin reveal:reveal-rebase
gh workflow run mac-only.yml -R tonicmuroq/vcmi --ref reveal-rebase
gh run view <run-id> -R tonicmuroq/vcmi --json headSha   # 核对 checkout 的是不是分支头
```

CI 绿了再把 `reveal` 推上去。两个坑：文本无冲突不代表能编译，本机装不了依赖，编译必须靠 CI 验证；
`gh workflow run` 紧跟在 `git push` 后面时，GitHub 可能解析到推送前的 ref，务必核对 headSha。

## 维护约定

- **一个功能一个提交**，别让补丁互相穿插。修 bug 就压回对应补丁，不要在队列尾巴上堆修补提交。
- **少碰上游文件**。能新建文件就新建，在上游文件里只留一行钩子。补丁 4 的 `RevealedContentsPopup` 是新类（好），
  `CRewardableObject::getPopupComponentsImpl` 是原地改的（上游一动那个函数就冲突）。
- **可提上游的补丁单独放**（补丁 2、3），合并后直接删掉，队列自然变短。
- **勤 rebase**。冲突成本随落后程度超线性增长。
- 补丁 4 会推进 `ESerializationVersion::CURRENT`，所以本 fork 存的档原版读不了。
