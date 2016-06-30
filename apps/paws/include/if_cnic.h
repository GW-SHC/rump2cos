#ifndef _NET_IF_CNIC_H_
#define _NET_IF_CNIC_H_

#ifdef _KERNEL
struct cnic_softc {
	struct	ifnet cnic_if;		/* the interface */

	u_short	cnic_flags;		/* misc flags */
#define	CNIC_OPEN	0x0001
#define	CNIC_INITED	0x0002
#define	CNIC_RCOLL	0x0004
#define	CNIC_IASET	0x0008
#define	CNIC_DSTADDR	0x0010
#define	CNIC_RWAIT	0x0040
#define	CNIC_ASYNC	0x0080
#define	CNIC_NBIO	0x0100
#define	CNIC_PREPADDR	0x0200
#define	CNIC_IFHEAD	0x0400

#define	CNIC_READY	(CNIC_OPEN | CNIC_INITED | CNIC_IASET)

	pid_t	cnic_pgid;		/* PID or process group ID */
	struct	selinfo	cnic_rsel;	/* read select */
	struct	selinfo	cnic_wsel;	/* write select (not used) */
	int	cnic_unit;		/* the tunnel unit number */
	kmutex_t cnic_lock;		/* lock for this tunnel */
	LIST_ENTRY(cnic_softc) cnic_list;	/* list of all tuns */
	void	*cnic_osih;		/* soft interrupt handle */
	void	*cnic_isih;		/* soft interrupt handle */
};
#endif	/* _KERNEL */

/* Maximum packet size */
#define	CNICMTU		1500

/* ioctl's for get/set debug */
#define	CNICSDEBUG	_IOW('t', 90, int)
#define	CNICGDEBUG	_IOR('t', 89, int)
#define	CNICSIFMODE	_IOW('t', 88, int)
#define	CNICSLMODE	_IOW('t', 87, int)
#define	CNICSIFHEAD	_IOW('t', 66, int)
#define	CNICGIFHEAD	_IOR('t', 65, int)

#endif /* !_NET_IF_TUN_H_ */
