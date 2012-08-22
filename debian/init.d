#! /bin/sh
#
# /etc/init.d/propagated
#
# System startup script for the propagate daemon
#
### BEGIN INIT INFO
# Provides:       propagated
# Required-Start: $network
# Should-Start:   
# Required-Stop:  
# Should-Stop:	  $network
# Default-Start:  2 3 5
# Default-Stop:   0 1 6
# Short-Description: Propagate service
# Description:    Propagate service
### END INIT INFO

PROP_BIN=/usr/sbin/propagated
test -x $PROP_BIN || exit 5
PIDFILE=/var/run/propagated.pid

if [ -f /etc/default/propagated ]; then
	. /etc/default/propagated
else
	echo "/etc/default/propagated does not exists";
	exit -1;
fi

if [ -z "${INTERFACE_ADDR}" -o -z "${MCAST_ADDR}" -o -z "${GATE_ADDR}" ]; then
	echo "Some of: INTERFACE_ADDR,MCAST_ADDR,GATE_ADDR is not set in /etc/default/propagated";
	exit -1;
fi


# Return values acc. to LSB for all commands but status:
# 0 - success
# 1 - generic or unspecified error
# 2 - invalid or excess argument(s)
# 3 - unimplemented feature (e.g. "reload")
# 4 - insufficient privilege
# 5 - program is not installed
# 6 - program is not configured
# 7 - program is not running
# 
# Note that starting an already running service, stopping
# or restarting a not-running service as well as the restart
# with force-reload (in case signaling is not supported) are
# considered a success.

case "$1" in
    start)
	echo -n "Starting Propagate daemon"
	start-stop-daemon -p $PIDFILE --start --exec $PROP_BIN -- ${MCAST_ADDR} ${GATE_ADDR} ${INTERFACE_ADDR} -d
	/usr/bin/propd_get ${MCAST_ADDR} ${INTERFACE_ADDR}
	;;
    stop)
	echo -n "Shutting down Propagate daemon"
	start-stop-daemon -p $PIDFILE --stop $PROP_BIN
	;;
    restart)
	## Stop the service and regardless of whether it was
	## running or not, start it again.
	$0 stop
	$0 start
	;;
    force-reload)
	echo -n "Reload service Prapagate"
	/usr/bin/propd_get ${MCAST_ADDR} ${INTERFACE_ADDR}
	;;
    reload)
	/usr/bin/propd_get ${MCAST_ADDR} ${INTERFACE_ADDR}
	;;
    *)
	echo "Usage: $0 {start|stop|status|try-restart|restart|force-reload|reload|probe}"
	exit 1
	;;
esac

