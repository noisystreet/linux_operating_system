// 最小 Netfilter 钩子：在 LOCAL_IN 记录 TCP 包（需内核头文件编译）
#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>

static unsigned int hello_nf_hook(void *priv, struct sk_buff *skb,
                                  const struct nf_hook_state *state)
{
    struct iphdr *iph;
    __be16 sport = 0, dport = 0;

    if (!skb)
        return NF_ACCEPT;

    iph = ip_hdr(skb);
    if (!iph || iph->protocol != IPPROTO_TCP)
        return NF_ACCEPT;

    if (!pskb_may_pull(skb, iph->ihl * 4 + sizeof(struct tcphdr)))
        return NF_ACCEPT;

    sport = tcp_hdr(skb)->source;
    dport = tcp_hdr(skb)->dest;

    pr_info("hello_netfilter: TCP %pI4:%u -> %pI4:%u (hook=%d)\n",
            &iph->saddr, ntohs(sport), &iph->daddr, ntohs(dport),
            state->hook);

    return NF_ACCEPT;
}

static struct nf_hook_ops nfho = {
    .hook     = hello_nf_hook,
    .pf       = NFPROTO_IPV4,
    .hooknum  = NF_INET_LOCAL_IN,
    .priority = NF_IP_PRI_FIRST,
};

static int __init hello_nf_init(void)
{
    int ret = nf_register_net_hook(&init_net, &nfho);
    if (ret < 0)
        pr_err("hello_netfilter: register failed %d\n", ret);
    else
        pr_info("hello_netfilter: loaded on NF_INET_LOCAL_IN\n");
    return ret;
}

static void __exit hello_nf_exit(void)
{
    nf_unregister_net_hook(&init_net, &nfho);
    pr_info("hello_netfilter: unloaded\n");
}

module_init(hello_nf_init);
module_exit(hello_nf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("os-tutorial");
MODULE_DESCRIPTION("Minimal Netfilter LOCAL_IN hook demo");
