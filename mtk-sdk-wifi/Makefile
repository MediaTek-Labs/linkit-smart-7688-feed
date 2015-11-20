#
# Copyright (C) 2015 John Crispin <blogic@openwrt.org>
#
# This is free software, licensed under the GPL 2 license.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mtk-sdk-wifi
PKG_VERSION:=2015-06-01
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=http://git.openwrt.org/project/iwinfo.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=ade8b1b299cbd5748db1acf80dd3e9f567938371
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_LICENSE:=GPL-2.0

PKG_CONFIG_DEPENDS := \
	CONFIG_PACKAGE_kmod-brcm-wl \
	CONFIG_PACKAGE_kmod-brcm-wl-mini \
	CONFIG_PACKAGE_kmod-brcm-wl-mimo \
	CONFIG_PACKAGE_kmod-madwifi \
	CONFIG_PACKAGE_kmod-cfg80211

include $(INCLUDE_DIR)/package.mk

define Package/mtk-sdk-wifi
  SECTION:=ralink
  CATEGORY:=Ralink SDK
  TITLE:=MT7628/88 SDK Wifi
  DEPENDS:=@TARGET_ramips_mt7688_LinkIt7688 +wireless-tools +libuci
  ABI_VERSION:=$(PKG_RELEASE)
endef

define Package/mtk-sdk-wifi/description
  Wireless information library with consistent interface for proprietary Broadcom,
  madwifi, nl80211 and wext driver interfaces.
  Command line frontend for the wireless information library.
endef

define Build/Configure
endef

IWINFO_BACKENDS := \
	$(if $(CONFIG_TARGET_ramips),rt) \

EXTRA_CFLAGS := \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include \
	-D_GNU_SOURCE

MAKE_FLAGS += \
	FPIC="$(FPIC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	BACKENDS="$(IWINFO_BACKENDS)"

define Build/Compile
	$(call Build/Compile/Default,)
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall -Werror -o $(PKG_BUILD_DIR)/ap_client src/ap_client.c
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/iwinfo
	$(CP) $(PKG_BUILD_DIR)/include/iwinfo.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/include/iwinfo/* $(1)/usr/include/iwinfo/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libiwinfo.so $(1)/usr/lib/libiwinfo.so
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwinfo.so $(1)/usr/lib/lua/iwinfo.so
endef

define Package/mtk-sdk-wifi/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/lib/modules/$(LINUX_VERSION)/
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwinfo $(1)/usr/bin/iwinfo.rt
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libiwinfo.so $(1)/lib/libiwinfo.so
	
	$(INSTALL_DIR) $(1)/sbin/
	$(CP) $(PKG_BUILD_DIR)/ap_client $(1)/sbin/
ifneq ($(CONFIG_ALL_KMODS),)
	$(CP) ./wifi_binary/mt_wifi.ko_$(LINUX_VERSION)_all $(1)/lib/modules/$(LINUX_VERSION)/mt_wifi.ko
else
	$(CP) ./wifi_binary/mt_wifi.ko_$(LINUX_VERSION) $(1)/lib/modules/$(LINUX_VERSION)/mt_wifi.ko
endif
	$(CP) ./files/* $(1)

endef

$(eval $(call BuildPackage,mtk-sdk-wifi))
