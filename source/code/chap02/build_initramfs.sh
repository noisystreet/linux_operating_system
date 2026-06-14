#!/usr/bin/env bash
# 将 mini_init 打包为最小 initramfs 镜像（供 QEMU 实验使用）
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if [[ ! -x ./mini_init ]]; then
    echo "请先编译: make"
    exit 1
fi

ROOT="${TMPDIR:-/tmp}/initramfs_root_$$"
OUT="${SCRIPT_DIR}/initramfs.cpio.gz"

rm -rf "$ROOT"
mkdir -p "$ROOT"/{dev,proc,sys,tmp}

cp ./mini_init "$ROOT/init"
chmod +x "$ROOT/init"

# 静态链接的 busybox 可放入 $ROOT/bin 以提供基本 shell 命令（可选）
# cp /bin/busybox "$ROOT/bin/busybox" && chroot "$ROOT" /bin/busybox --install -s

(
    cd "$ROOT"
    find . -print0 | cpio --null --create --format=newc | gzip -9 >"$OUT"
)

rm -rf "$ROOT"
echo "已生成: $OUT"
echo "配合内核 bzImage 启动示例:"
echo "  qemu-system-x86_64 -kernel /path/to/bzImage -initrd $OUT -append 'console=ttyS0' -nographic"
