VPBUS_TEST_VERSION = 1.0
VPBUS_TEST_SITE = $(TOPDIR)/package/vpbus_test
VPBUS_TEST_SITE_METHOD = local
VPBUS_TEST_LICENSE = GPLv3+


define VPBUS_TEST_INSTALL_TARGET_CMDS
	
endef

$(eval $(cmake-package))
