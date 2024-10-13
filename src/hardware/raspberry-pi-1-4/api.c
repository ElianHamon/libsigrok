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
#define _GNU_SOURCE

#include <config.h>
#include "protocol.h"

#include <stdio.h>

static const uint32_t scanopts[] = {
};

static const uint32_t drvopts[] = {
	SR_CONF_LOGIC_ANALYZER,
};

static const uint32_t devopts[] = {
	SR_CONF_SAMPLERATE | SR_CONF_GET | SR_CONF_SET | SR_CONF_LIST,
};

/* Sample rate can either provide a std_gvar_samplerates_steps or a
 * std_gvar_samplerates. The latter is just a long list of every supported rate.
 * For the steps, pulseview/pv/toolbars/mainbar.cpp will do a min,max,step. If
 * step is 1 then it provides a 1,2,5,10 select otherwise it allows a spin box.
 * Going with the full list because while the spin box is more flexible, it is
 * harder to read */
static const uint64_t samplerates[] = {
	SR_MHZ(6),
};

static struct sr_dev_driver raspberry_pi_1_4_driver_info;

/** @brief Checks that the hardware has a BCM283* CPU and /dev/gpiomem.
 * @return -1 on error, 0 on success.
 *
 * Here you need to fill a new struct sr_dev_inst for every device discovered, and make a GSList of them.
 * Then you add them to the drv_context->instances from di->context.
 * You can use the std_scan_complete helper.
 */
static GSList *scan(struct sr_dev_driver *di, GSList *options)
{
	GSList* device = NULL;
	struct sr_dev_inst* sdi;
	struct dev_context* devc;

	(void)di;
	(void)options;

	FILE* dev_gpiomem = fopen("/dev/gpiomem", "r+b");
	if (dev_gpiomem == NULL) {
		sr_err("/dev/gpiomem does not exists");
		return NULL;
	}
	fclose(dev_gpiomem);

	FILE* cpuinfo = fopen("/proc/cpuinfo", "rb");
	if(cpuinfo == NULL) {
		sr_err("Can't read /proc/cpuinfo");
		return NULL;
	}

	char* arg = NULL;
	char* model = NULL;
	size_t size = 0;
	// Parse cpuinfo to get hardware model.
	while(getline(&arg, &size, cpuinfo) >= 0)
	{
//		sr_dbg("%s", arg);
// 		if(strstr(arg, "Hardware") != NULL)
// 		{
// 			const char* bcm = strstr(arg, "BCM283");
// 			if(bcm == NULL) {
// 				sr_err("Not a BCM283X CPU");
// 				goto end;
// 			}
//
// //			size_t index = bcm - arg;
// 			sr_info("CPU: %s", bcm);
// 		}

		if(strstr(arg, "Model") != NULL)
		{
			const char* rpi = strstr(arg, "Raspberry Pi");
			if(rpi == NULL) {
				sr_err("Not a Raspberry Pi");
				goto end;
			}

			model = g_strdup(rpi);
			sr_info("Model: %s", rpi);
		}
	}

	sdi = g_malloc0(sizeof(struct sr_dev_inst));
	sdi->status = SR_ST_INACTIVE;
	sdi->vendor = g_strdup("Raspberry Pi");
	sdi->model = model;
	sdi->version = g_strdup("00");
	sdi->conn = NULL; // No connection since it's the same machine.
	sdi->driver = &raspberry_pi_1_4_driver_info;
	sdi->inst_type = SR_INST_USER;
	sdi->serial_num = g_strdup("N/A");

	devc = g_malloc0(sizeof(struct dev_context));
	devc->sample_rate = samplerates[0];

	sdi->priv = devc;

	device = std_scan_complete(di, g_slist_append(NULL, sdi));

end:
	free(arg);
	fclose(cpuinfo);

	return device;
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
	(void)cg;

	struct dev_context* devc = sdi->priv;

	sr_dbg("config_get");

	switch (key) {
	case SR_CONF_SAMPLERATE:
		*data = g_variant_new_uint64(devc->sample_rate);
		break;

	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

static int config_set(uint32_t key, GVariant *data,
	const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	(void)cg;

	struct dev_context* devc = sdi->priv;

	sr_dbg("config_set");

	switch (key) {
	case SR_CONF_SAMPLERATE:
		devc->sample_rate = g_variant_get_uint64(data);
		sr_dbg("config_set sr %"PRIu64"\n", devc->sample_rate);
		break;

	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

static int config_list(uint32_t key, GVariant **data,
	const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	/* Scan or device options are the only ones that can be called without a
	 * defined instance */
	if ((key == SR_CONF_SCAN_OPTIONS) || (key == SR_CONF_DEVICE_OPTIONS)) {
		return STD_CONFIG_LIST(key, data, sdi, cg, scanopts, drvopts, devopts);
	}

	sr_dbg("config_list");

	switch (key) {
	case SR_CONF_SAMPLERATE:
		*data = std_gvar_samplerates(ARRAY_AND_SIZE(samplerates));
		break;

	default:
		return SR_ERR_NA;
	}

	return SR_OK;
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
