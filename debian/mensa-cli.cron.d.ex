#
# Regular cron jobs for the mensa-cli package
#
0 4	* * *	root	[ -x /usr/bin/mensa-cli_maintenance ] && /usr/bin/mensa-cli_maintenance
