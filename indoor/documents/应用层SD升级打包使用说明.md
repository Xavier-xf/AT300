# 应用层SD升级打包使用说明

## 适用范围

本说明适用于 `indoor/` 应用层工程。流程是先编译应用，再把生成的程序同步到 `AnyCloud37E_SDK_V1.10_EXT/`，最后调用 EXT 自带的 `make_image.sh` 生成 `SAT_ANYKA.IMG`。

## 目录来源

- 应用源码：`indoor/sources/`
- 编译产物：`indoor/build/DOORBELL_anyka_release.BIN`
- SDK 升级输入：`AnyCloud37E_SDK_V1.10_EXT/rootfs/` 和 `AnyCloud37E_SDK_V1.10_EXT/upgrade/platform/`
- 一键脚本：`indoor/package_sd_upgrade.sh`

## 使用步骤

1. 编译应用：

```sh
cd /home/xiaoxiao/workspace/QT300/indoor
make
```

2. 生成升级包：

```sh
./package_sd_upgrade.sh
```

默认只打 `app` 分区。脚本会自动把 `DOORBELL_anyka_release.BIN` 复制到 SDK 的 `SAT_ANYKA.BIN`，再生成 `app.sqsh4`，最后调用 EXT 的打包脚本。

3. 常用分区示例：

```sh
./package_sd_upgrade.sh app
./package_sd_upgrade.sh app dtb
./package_sd_upgrade.sh all
make sd PARTS="app dtb"
```

输出文件：

```text
/home/xiaoxiao/workspace/QT300/AnyCloud37E_SDK_V1.10_EXT/upgrade/SAT_ANYKA.IMG
```

## 分区说明

- `app`：当前应用层程序，最常用。
- `dtb`、`kernel`、`uboot`：仅在底层配置变更时使用。
- `rootfs`、`usr`、`config`、`data`、`tuya`、`home`：对应 SDK 里的现有升级平台文件。
- 多分区组合时，脚本会自动按 SDK 原有顺序填写 `make_image.sh` 的交互选项。

## 维护原则

- 只改动实际变更的分区，不要每次都打全包。
- 应用层改完先确认 `build/DOORBELL_anyka_release.BIN` 已更新，再打包。
- 如果只改 UI 或业务逻辑，通常只需要 `app` 分区。
