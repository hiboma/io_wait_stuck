#include <linux/module.h>
#include <linux/rtnetlink.h>
#include <linux/sysctl.h>
#include <linux/sched.h>
#include <linux/mutex.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("hiboma <hiroyan@gmail.com>");
MODULE_DESCRIPTION("This module is PoC");

static struct ctl_table_header *iowait_stuck_sysctl_header;

DEFINE_MUTEX(iowait_stuck_mutex);

static int min = 1;
static int max = INT_MAX;
static int iowait_stuck_msleep_seconds = 1;

int io_schedule_stuck_sysctl_handler(struct ctl_table *table, int write,
	void __user *buffer, size_t *length, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec_minmax(table, write, buffer, length, ppos);
	if (ret)
		return ret;

	if (write) {
		pr_info("I will I/O wait for %d seconds", iowait_stuck_msleep_seconds);
		set_current_state(TASK_UNINTERRUPTIBLE);
		io_schedule_timeout(iowait_stuck_msleep_seconds * HZ);
	}

	return 0;
}

int mutex_lock_io_stuck_sysctl_handler(struct ctl_table *table, int write,
	void __user *buffer, size_t *length, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec_minmax(table, write, buffer, length, ppos);
	if (ret)
		return ret;

	if (write) {
		pr_info("I will I/O wait for %d seconds", iowait_stuck_msleep_seconds);
		mutex_lock_io(&iowait_stuck_mutex);
		msleep( iowait_stuck_msleep_seconds * 1000 );
		mutex_unlock(&iowait_stuck_mutex);
	}

	return 0;
}

static struct ctl_table iowait_stuck_ctl_table[] = {
	{
		.procname	= "io_schedule_stuck",
		.data		= &iowait_stuck_msleep_seconds,
		.maxlen		= sizeof(int),
		.mode		= 0666,
		.proc_handler	= &io_schedule_stuck_sysctl_handler,
		.extra1		= &min,
		.extra2		= &max,
	},
	{
		.procname	= "mutex_lock_io_stuck",
		.data		= &iowait_stuck_msleep_seconds,
		.maxlen		= sizeof(int),
		.mode		= 0666,
		.proc_handler	= &mutex_lock_io_stuck_sysctl_handler,
		.extra1		= &min,
		.extra2		= &max,
	},
	{},
};

static int __init iowait_stuck_init(void)
{
	iowait_stuck_sysctl_header = register_sysctl_table(iowait_stuck_ctl_table);
	if (!iowait_stuck_sysctl_header)
		return -ENOMEM;

	return 0;
}

static void __exit iowait_stuck_exit(void)
{
	unregister_sysctl_table(iowait_stuck_sysctl_header);
}

module_init(iowait_stuck_init);
module_exit(iowait_stuck_exit);
