export PATH=/usr/bin:$PATH

# Uncomment for the hardware that your EPICS IOC will use.
# The hardware connot be shared amongst different IOCs

# ===========================================================================
# set kernel thread priority for the SLAC EVR using evrma
# ===========================================================================
### The current properties of the thread can be viewed with this:
###    ps  -Leo pid,tid,rtprio,comm | grep vevr11

### set kernel read thread priority for the vevr11

/usr/bin/chrt -pf 95  `ps  -Leo tid,comm | /usr/bin/awk '/evrma_vevr*/'`
/usr/bin/chrt -pf 96  `ps  -Leo pid,comm | /usr/bin/awk '/irq.*pci-evrm/'`
ps -Leo pid,ppid,tid,rtprio,comm
