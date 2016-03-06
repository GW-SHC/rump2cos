/* RG:
 * This is a stub file so we only need to have the FS and DISK rump kernel components loaded in to run paws.
 * None of these functions should ever end up being called, so it should be ok to just stub them out
 */

int
rump_pub_netconfig_ifcreate(const char *arg1)
{
	while(1);
	return 0;
}

int
rump_pub_netconfig_ipv4_ifaddr_cidr(const char *arg1, const char *arg2, int arg3)
{
	while(1);
	return 0;
}

int
rump_pub_netconfig_ipv6_ifaddr(const char *arg1, const char *arg2, int arg3)
{
	while(1);
	return 0;
}

int
rump_pub_netconfig_ipv6_gw(const char *arg1)
{
	while(1);
	return 0;
}

int
rump_pub_netconfig_ipv4_gw(const char *arg1)
{
	while(1);
	return 0;
}

int
rump_pub_netconfig_auto_ipv6(const char *arg1)
{
	while(1);
	return 0;
}

int
rump_pub_netconfig_dhcp_ipv4_oneshot(const char *arg1)
{
	while(1);
	return 0;
}
