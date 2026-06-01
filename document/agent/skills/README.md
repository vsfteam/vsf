# VSF Agent Skills

两个 AI agent skill：`vsf-hal-driver`（驱动移植）和 `vsf-bench`（硬件测试）。Cover 从寄存器级驱动开发到自动化硬件验证的完整流程。

## 安装

对 code agent 说：

```
安装 document/agent/skills/ 下的 vsf-bench 和 vsf-hal-driver 到项目 skills
```

Agent 会自动识别 `SKILL.md` 入口，默认安装为项目级 skill（仅当前项目生效）。

如果需要在所有项目中使用，告诉 agent "安装到用户 skills" 即可。

## 依赖

`vsf-bench` 需要 Python 包 `vsf_bench`（含串口通信、逻辑分析仪驱动等）。Agent 会在首次使用时引导安装。

硬件测试需要：目标板、串口、烧录器（SWD 或 UF2），可选逻辑分析仪。Agent 会引导配置 `hardware-map.yml`。

## 两个 skill 的分工

| Skill | 做什么 | 什么时候用 |
|-------|--------|------------|
| `vsf-hal-driver` | 移植驱动、写寄存器代码、调试 LV0 bug | 新外设、新芯片、驱动不工作 |
| `vsf-bench` | 构建、烧录、跑硬件测试 | 验证驱动是否正确、回归测试 |

`vsf-hal-driver` 写完驱动后会自动委托 `vsf-bench` 验证，不需要手动切换。

## 背后有什么

每个 skill 都包含：

- **文档** — 移植流程、编码规范、故障排除
- **确定性脚本** — 骨架检查、结构检查、质量检查、跨文件审计
- **测试模板** — Python 硬件测试脚本模板

详细内容见 `vsf-hal-driver/modules/` 和 `vsf-bench/modules/`。
