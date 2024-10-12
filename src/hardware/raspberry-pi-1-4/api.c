/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2024 Elian Hamon <156338926+ElianHamon@users.noreply.github.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include "protocol.h"

#define _GNU_SOURCE
#include <stdio.h>

static struct sr_dev_driver raspberry_pi_1_4_driver_info;

/** @brief Checks that the hardware has a BCM283* CPU and /dev/gpiomem.
 * @return -1 on error, 0 on success.
 */
static int is_raspberry_pi_1_4()
{
	int ret = -1;
	FILE* cpuinfo = fopen("/proc/cpuinfo", "rb");
	if(cpuinfo == NULL)
		return -1;

	char* arg = NULL;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, cpuinfo) != -1)
	{
//		sr_dbg("%s", arg);
		if(strstr(arg, "Hardware") != NULL)
		{
			const char* bcm = strstr(arg, "BCM283");
			if(bcm == NULL)
				goto end; // Not a raspberry pi CPU.

//			size_t index = bcm - arg;
			sr_dbg("CPU: %s", bcm);
		}

		if(strstr(arg, "Model") != NULL)
		{
			const char* rpi = strstr(arg, "Raspberry Pi");
			if(rpi == NULL)
				goto end; // Not a raspberry pi.

			sr_dbg("Model: %s", rpi);
		}
	}
	ret = 0;

end:
	free(arg);
	fclose(cpuinfo);
	return ret;
}

static GSList *scan(struct sr_dev_driver *di, GSList *options)
{
	GSList *devices = NULL;

	(void)options;

	struct drv_context *drvc = di->context;
	drvc->instances = NULL;

	sr_dbg("scan");

	if(is_raspberry_pi_1_4() < 0)
		return NULL;

	/* TODO: scan for devices, either based on a SR_CONF_CONN option
	 * or on a USB scan. */

	return devices;
}

static int dev_open(struct sr_dev_inst *sdi)
{
	(void)sdi;

	sr_dbg("dev_open");

	/* TODO: get handle from sdi->conn and open it. */

	return SR_OK;
}

static int dev_close(struct sr_dev_inst *sdi)
{
	(void)sdi;

	sr_dbg("dev_close");

	/* TODO: get handle from sdi->conn and close it. */

	return SR_OK;
}

static int config_get(uint32_t key, GVariant **data,
	const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	int ret;

	(void)sdi;
	(void)data;
	(void)cg;

	sr_dbg("config_get");

	ret = SR_OK;
	switch (key) {
	/* TODO */
	default:
		return SR_ERR_NA;
	}

	return ret;
}

static int config_set(uint32_t key, GVariant *data,
	const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	int ret;

	(void)sdi;
	(void)data;
	(void)cg;

	sr_dbg("config_set");

	ret = SR_OK;
	switch (key) {
	/* TODO */
	default:
		ret = SR_ERR_NA;
	}

	return ret;
}

static int config_list(uint32_t key, GVariant **data,
	const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	int ret;

	(void)sdi;
	(void)data;
	(void)cg;

	sr_dbg("config_list");

	ret = SR_OK;
	switch (key) {
	/* TODO */
	default:
		return SR_ERR_NA;
	}

	return ret;
}

static int dev_acquisition_start(const struct sr_dev_inst *sdi)
{
	/* TODO: configure hardware, reset acquisition state, set up
	 * callbacks and send header packet. */

	sr_dbg("dev_acquisition_start");

	(void)sdi;

	return SR_OK;
}

static int dev_acquisition_stop(struct sr_dev_inst *sdi)
{
	/* TODO: stop acquisition. */

	sr_dbg("dev_acquisition_stop");

	(void)sdi;

	return SR_OK;
}

static struct sr_dev_driver raspberry_pi_1_4_driver_info = {
	.name = "raspberry-pi-1-4",
	.longname = "Raspberry Pi 1-4",
	.api_version = 1,
	.init = std_init,
	.cleanup = std_cleanup,
	.scan = scan,
	.dev_list = std_dev_list,
	.dev_clear = std_dev_clear,
	.config_get = config_get,
	.config_set = config_set,
	.config_list = config_list,
	.dev_open = dev_open,
	.dev_close = dev_close,
	.dev_acquisition_start = dev_acquisition_start,
	.dev_acquisition_stop = dev_acquisition_stop,
	.context = NULL,
};
SR_REGISTER_DEV_DRIVER(raspberry_pi_1_4_driver_info);
