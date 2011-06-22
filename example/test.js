var geoip = require('node-geoip-sync').geoip;
var util  = require("util");


var geoip_obj = geoip('207.97.227.239');

if (geoip_obj.flag_valid == false)
{
    util.log('geoip(): ' + geoip_obj.error_string);
}

util.log('ip           : ' + geoip_obj.ip);
util.log('country_name : ' + geoip_obj.country_name);
util.log('country_code : ' + geoip_obj.country_code);
util.log('region_name  : ' + geoip_obj.region_name);
util.log('city_name    : ' + geoip_obj.city_name);
util.log('time_zone    : ' + geoip_obj.time_zone);
util.log('latitude     : ' + geoip_obj.latitude);
util.log('longitude    : ' + geoip_obj.longitude);
