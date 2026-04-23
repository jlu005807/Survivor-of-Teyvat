# Survivor of Teyvat

`Survivor of Teyvat` 是一个基于 C++、EasyX 和 WinMM 音频接口实现的 2D 生存小游戏。玩家在角色选择界面选择角色后进入战斗场景，通过方向键移动，利用环绕子弹击败不同敌人并累积分数。

## 功能概览

- 主菜单、角色选择、战斗场景等多场景切换。
- 两个可选角色：派蒙和战士。
- 环绕子弹攻击系统，得分达到阈值后自动增加子弹数量。
- 三类敌人：野猪、蜜蜂、蜗牛，分别带有受击、眩晕、冻结等效果。
- 图片资源位于 `img/`，音乐和音效资源位于 `mus/`。

## 开发环境

- Windows
- Visual Studio 2022
- MSVC v143 工具集
- EasyX 图形库

项目文件已经为 Debug/Release 和 Win32/x64 配置了 `/utf-8` 编译选项，源码中的中文注释按 UTF-8 保存。

## 项目结构

```text
Survivor of Teyvat.sln
Survivor of Teyvat/
  include/              # 头文件：类声明、接口和内联逻辑
  src/                  # 源文件：入口、场景初始化、核心实现
  img/                  # 图片资源
  mus/                  # 音频资源
  Survivor of Teyvat.vcxproj
  Survivor of Teyvat.vcxproj.filters
Survivor-of-Teyvat_Setup/
  Release/              # 已生成的安装包
```

## 模块说明

- `game`：管理全局窗口、场景列表、图集资源、音频资源和主循环。
- `Widget`：表示单个场景，负责输入分发和对象绘制。
- `Object`：所有可更新、可绘制游戏对象的抽象基类。
- `Button`：主菜单与角色选择界面的图片按钮。
- `Atlas` / `Animation`：负责加载图集、生成特效图集和播放动画。
- `Player` / `Bullet`：玩家、移动输入、得分、状态特效和环绕子弹。
- `Enemy` / `EnemyEffect` / `EnemyPool`：敌人行为、不同敌人效果和对象池复用。

## 构建方式

使用 Visual Studio 打开 `Survivor of Teyvat.sln`，选择 `Debug|x64` 或 `Release|x64` 后生成解决方案即可。

也可以在开发者命令行中运行：

```powershell
MSBuild "Survivor of Teyvat.sln" /t:Build /p:Configuration=Debug /p:Platform=x64 /m
```

构建产物默认输出到仓库根目录下的 `x64/Debug/` 或 `x64/Release/`。

## 运行方式

开发调试时建议直接从 Visual Studio 启动项目，工作目录保持为 `Survivor of Teyvat/`，这样程序可以通过相对路径读取 `img/` 和 `mus/` 资源。

如果只想安装体验：

1. 进入 `Survivor-of-Teyvat_Setup/Release/`。
2. 双击 `setup.exe` 或 `Survivor-of-Teyvat_Setup.msi` 安装。
3. 从开始菜单或安装目录运行游戏。

## 安装包发布说明

安装包项目位于 `Survivor-of-Teyvat_Setup/`，已生成的安装文件位于 `Survivor-of-Teyvat_Setup/Release/`。发布新版本时建议按以下顺序检查：

1. 先生成 `Survivor of Teyvat` C++ 游戏项目的 `Release|x64` 配置。
2. 确认安装包包含最新的可执行文件，以及 `img/`、`mus/` 两个资源目录。
3. 通过 Visual Studio 的安装项目工具生成 `.vdproj` 安装包。注意：`.vdproj` 不支持直接用 MSBuild 构建，命令行构建解决方案时可能会跳过或报出安装项目不受支持的警告。
4. 生成后在 `Survivor-of-Teyvat_Setup/Release/` 检查 `setup.exe` 和 `Survivor-of-Teyvat_Setup.msi`，并在干净目录中安装运行一次。

如果调整了资源目录或可执行文件输出路径，需要同步更新安装项目中的文件系统视图，确保安装后的程序仍能从运行目录读取 `img/` 和 `mus/`。

## 操作说明

- 方向键：移动玩家。
- 鼠标：点击菜单按钮和角色头像。
- 分数提升后会自动增加环绕子弹，敌人类型越多，状态效果越复杂。

## 本次整理要点

- 将头文件统一移动到 `Survivor of Teyvat/include/`。
- 将 `.cpp` 源文件统一移动到 `Survivor of Teyvat/src/`。
- 更新 Visual Studio 工程文件和过滤器，使解决方案可以直接编译。
- 修正图集复制固定 6 帧、输入状态被非键盘消息清空、升级计分状态跨角色残留等问题。
- 清理源码乱码注释，为各源码文件补充中文说明。
