# linkit-smart-feed
This feeds holds the SDK wifi driver and the config/meta package for the LinkIt Smart 7688 (Duo)

# Installation

Start by downloading the OpenWrt CC source code

`git clone git://git.openwrt.org/15.05/openwrt.git`


Copy the feeds.conf.default file

`cp feeds.conf.default feeds.conf`


Add the linkit-smart-feed by calling

`echo src-git linkit https://github.com/MediaTek-Labs/linkit-smart-feed.git >> feeds.conf`

# Setting up the feeds

To get access to all of the packages availbale we must first update the feeds

`./scripts/feeds update`

And then install all packages

`./scripts/feeds install -a`

# Configure and Build

Next we need to tell OpenWrt tat we want to build an image for the LinkIt Smart 7688

`make menuconfig`

Then select these option

- Target System (Ralink RT288x/RT3xxx)  --->
- Subtarget (MT7688 based boards)  --->
- Target Profile (LinkIt7688)  --->

finally start the compile job by calling

`make V=99`

