/*
 * Atheros PHY drivers
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * Copyright 2011, 2013 Freescale Semiconductor, Inc.
 * author Andy Fleming
 */
#include <phy.h>

static int ar8021_config(struct phy_device *phydev)
{
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x05);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, 0x3D47);

	phydev->supported = phydev->drv->features;
	return 0;
}

static int ar8035_config(struct phy_device *phydev)
{
	int regval;

#ifdef ATHEROS_QUIRK_SMARTEEE
	/* Ar803x phy SmartEEE feature cause link status generates glitch,
	 * which cause ethernet link down/up issue, so disable SmartEEE
	 */
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x0003);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, 0x805d);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x4003);
	regval = phy_read(phydev, MDIO_DEVAD_NONE, 0xe);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, (regval&~0x0100));
#endif

	/* To enable AR8035 ouput a 125MHz clk from CLK_25M */
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x0007);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, 0x8016);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x4007);
	regval = phy_read(phydev, MDIO_DEVAD_NONE, 0xe);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, (regval|0x0018));

	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x05);
	regval = phy_read(phydev, MDIO_DEVAD_NONE, 0x1e);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, (regval|0x0100));

	genphy_config_aneg(phydev);

	phy_reset(phydev);

	return 0;
}

static struct phy_driver AR8021_driver =  {
	.name = "AR8021",
	.uid = 0x4dd040,
	.mask = 0x4fffff,
	.features = PHY_GBIT_FEATURES,
	.config = ar8021_config,
	.startup = genphy_startup,
	.shutdown = genphy_shutdown,
};

static struct phy_driver AR8031_driver =  {
	.name = "AR8031/AR8033",
	.uid = 0x4dd074,
	.mask = 0xffffffef,
	.features = PHY_GBIT_FEATURES,
	.config = genphy_config,
	.startup = genphy_startup,
	.shutdown = genphy_shutdown,
};

static struct phy_driver AR8035_driver =  {
	.name = "AR8035",
	.uid = 0x4dd072,
	.mask = 0xffffffef,
	.features = PHY_GBIT_FEATURES,
	.config = ar8035_config,
	.startup = genphy_startup,
	.shutdown = genphy_shutdown,
};

int phy_atheros_init(void)
{
	phy_register(&AR8021_driver);
	phy_register(&AR8031_driver);
	phy_register(&AR8035_driver);

	return 0;
}
