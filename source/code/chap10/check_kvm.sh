#!/usr/bin/env bash
# 检测 KVM 硬件虚拟化支持
set -euo pipefail

echo "=== KVM 检测 ==="

if [[ -e /dev/kvm ]]; then
    echo "[OK] /dev/kvm 存在"
    ls -l /dev/kvm
else
    echo "[--] /dev/kvm 不存在（可能未加载 kvm 模块或宿主机不支持）"
fi

if lsmod | grep -q '^kvm'; then
    echo "[OK] kvm 内核模块已加载"
    lsmod | grep '^kvm'
else
    echo "[--] kvm 模块未加载"
fi

if grep -qE '(vmx|svm)' /proc/cpuinfo; then
    echo "[OK] CPU 支持硬件虚拟化 (vmx/svm)"
    grep -m1 -E 'vmx|svm' /proc/cpuinfo
else
    echo "[--] /proc/cpuinfo 中未发现 vmx/svm 标志"
fi

if command -v qemu-system-x86_64 >/dev/null 2>&1; then
    echo "[OK] qemu-system-x86_64: $(command -v qemu-system-x86_64)"
else
    echo "[--] 未安装 qemu-system-x86_64"
fi
