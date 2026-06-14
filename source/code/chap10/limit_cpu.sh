#!/usr/bin/env bash
# cgroup v2 CPU 限制演示（需 root 或 cgroup 写权限）
set -euo pipefail

CGROUP="/sys/fs/cgroup/os_tutorial_demo"
DURATION="${1:-3}"

if [[ ! -w /sys/fs/cgroup/cgroup.procs ]]; then
    echo "需要 root 或对 cgroup 的写权限"
    exit 1
fi

cleanup() {
    if [[ -d "$CGROUP" ]]; then
        # 将进程移回根 cgroup 再删除
        while read -r pid; do
            echo "$pid" > /sys/fs/cgroup/cgroup.procs 2>/dev/null || true
        done < "$CGROUP/cgroup.procs" 2>/dev/null || true
        rmdir "$CGROUP" 2>/dev/null || true
    fi
}
trap cleanup EXIT

mkdir -p "$CGROUP"
echo "50000 100000" > "$CGROUP/cpu.max"   # 50% 单核

(
    echo $$ > "$CGROUP/cgroup.procs"
    echo "在受限 cgroup 中运行 stress  ${DURATION}s ..."
    if command -v stress-ng >/dev/null 2>&1; then
        stress-ng --cpu 2 --timeout "${DURATION}s" --metrics-brief
    else
        # 无 stress-ng 时用 bash 忙等
        end=$((SECONDS + DURATION))
        while (( SECONDS < end )); do :; done
        echo "（未安装 stress-ng，已用忙等代替）"
    fi
)

echo "完成。可用以下命令查看 cgroup 统计:"
echo "  cat $CGROUP/cpu.stat"
