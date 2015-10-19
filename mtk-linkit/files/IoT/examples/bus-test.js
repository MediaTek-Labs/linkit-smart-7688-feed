#!/usr/bin/node

var m = require('mraa');

var i2c = new m.I2c(0)
i2c.address(0x62)
i2c.writeReg(0, 0)
i2c.writeReg(1, 0)

function char(x) { return parseInt(x, 16); }
var spi = new m.Spi(0)
var buf = new Buffer(4)
buf[0] = char('0xf4')
buf[1] = char('0x2e')
buf[2] = char('0x3e')
buf[3] = char('0x4e')
spi.write(buf)

var pwm = new m.Pwm(18);
pwm.enable(true);
pwm.write(0.5);
