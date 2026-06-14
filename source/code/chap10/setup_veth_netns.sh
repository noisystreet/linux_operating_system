#!/usr/bin/env bash
# 创建一对 veth，将一端移入独立网络命名空间并配置 IP（需 root）
set -euo pipefail

NS="${1:-ns1}"
VETH_HOST="veth0"
VETH_NS="veth1"
HOST_IP="192.168.100.1/24"
NS_IP="192.168.100.2/24"

if [[ $EUID -ne 0 ]]; then
    echo "请使用 sudo 运行"
    exit 1
fi

cleanup() {
    ip netns del "$NS" 2>/dev/null || true
    ip link del "$VETH_HOST" 2>/dev/null || true
}
trap cleanup EXIT

ip netns add "$NS"
ip link add "$VETH_HOST" type veth peer name "$VETH_NS"
ip link set "$VETH_NS" netns "$NS"

ip addr add "$HOST_IP" dev "$VETH_HOST"
ip link set "$VETH_HOST" up

ip netns exec "$NS" ip addr add "$NS_IP" dev "$VETH_NS"
ip netns exec "$NS" ip link set "$VETH_NS" up
ip netns exec "$NS" ip link set lo up

echo "=== 宿主机 ==="
ip -br addr show "$VETH_HOST"
echo "=== 命名空间 $NS ==="
ip netns exec "$NS" ip -br addr
echo "=== ping 测试 ==="
ip netns exec "$NS" ping -c 2 "${HOST_IP%/*}"

echo "进入命名空间: ip netns exec $NS bash"
echo "清理: ip netns del $NS && ip link del $VETH_HOST"
