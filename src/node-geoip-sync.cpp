//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// 0.0.3
// Alexey Potehin http://www.gnuplanet.ru/doc/cv
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// apt-get install geoip-database-contrib libgeoip-dev
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <v8.h>
#include <node.h>
#include <node_buffer.h>

#include "GeoIP.h"
#include "GeoIPCity.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// char* -> std::string
std::string unknown(const char *p)
{
    if (p == NULL)
    {
	return "unknown";
    }

    return p;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// exception
static v8::Handle<v8::Value>
VException(const char *msg)
{
    v8::HandleScope scope;
    return ThrowException(v8::Exception::Error(v8::String::New(msg)));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// main function
v8::Handle<v8::Value>
geoip_binding(const v8::Arguments &args)
{
    v8::HandleScope scope;
    v8::Handle<v8::Object> obj_inject = v8::Object::New();

    obj_inject->Set(v8::String::New("flag_valid"),   v8::Boolean::New(false));
    obj_inject->Set(v8::String::New("error_string"), v8::String::New(""));
    obj_inject->Set(v8::String::New("ip"),           v8::String::New("127.0.0.1"));
    obj_inject->Set(v8::String::New("country_name"), v8::String::New("unknown"));
    obj_inject->Set(v8::String::New("country_code"), v8::String::New("??"));
    obj_inject->Set(v8::String::New("region_name"),  v8::String::New("unknown"));
    obj_inject->Set(v8::String::New("city_name"),    v8::String::New("unknown"));
    obj_inject->Set(v8::String::New("time_zone"),    v8::String::New("unknown/unknown"));
    obj_inject->Set(v8::String::New("latitude"),     v8::String::New("0.0"));
    obj_inject->Set(v8::String::New("longitude"),    v8::String::New("0.0"));


    if (args.Length() != 1)
    {
	return VException("One argument required - ip for geoip()");
    }


    v8::String::AsciiValue b64data(args[0]->ToString());
    std::string ip_str(*b64data, b64data.length());


    boost::shared_ptr<GeoIP> gi(GeoIP_open("/usr/share/GeoIP/GeoIP.dat", GEOIP_STANDARD | GEOIP_MEMORY_CACHE), GeoIP_delete);
    if (!gi)
    {
	obj_inject->Set(v8::String::New("error_string"), v8::String::New("Error opening GeoIP database"));
	return scope.Close(obj_inject);
    }


    std::string returnedCountry(GeoIP_country_name_by_addr(gi.get(), ip_str.c_str()));
    if (returnedCountry.empty())
    {
	obj_inject->Set(v8::String::New("error_string"), v8::String::New("Invalid GeoIP Query"));
	return scope.Close(obj_inject);
    }


    gi = boost::shared_ptr<GeoIP>(GeoIP_open("/usr/share/GeoIP/GeoIPCity.dat", GEOIP_INDEX_CACHE), GeoIP_delete);
    if (!gi)
    {
	obj_inject->Set(v8::String::New("error_string"), v8::String::New("Error opening GeoIP database"));
	return scope.Close(obj_inject);
    }


    boost::shared_ptr<GeoIPRecord> gir(GeoIP_record_by_name(gi.get(), ip_str.c_str()), GeoIPRecord_delete);
    if (!gir)
    {
	obj_inject->Set(v8::String::New("error_string"), v8::String::New("Invalid GeoIP Query"));
	return scope.Close(obj_inject);
    }


    boost::shared_ptr<char*> ret(GeoIP_range_by_ip(gi.get(), ip_str.c_str()), GeoIP_range_by_ip_delete);
    std::string time_zone(GeoIP_time_zone_by_country_and_region(gir->country_code, gir->region));


    obj_inject->Set(v8::String::New("flag_valid"),   v8::Boolean::New(true));
    obj_inject->Set(v8::String::New("ip"),           v8::String::New(ip_str.c_str()));
    obj_inject->Set(v8::String::New("country_name"), v8::String::New(returnedCountry.c_str()));
    obj_inject->Set(v8::String::New("country_code"), v8::String::New(unknown(gir->country_code).c_str()));
    obj_inject->Set(v8::String::New("region_name"),  v8::String::New(unknown(GeoIP_region_name_by_code(gir->country_code, gir->region)).c_str()));
    obj_inject->Set(v8::String::New("city_name"),    v8::String::New(unknown(gir->city).c_str()));
    obj_inject->Set(v8::String::New("time_zone"),    v8::String::New(unknown(time_zone.c_str()).c_str()));
    obj_inject->Set(v8::String::New("latitude"),     v8::String::New(boost::lexical_cast<std::string>(gir->latitude).c_str()));
    obj_inject->Set(v8::String::New("longitude"),    v8::String::New(boost::lexical_cast<std::string>(gir->longitude).c_str()));

    return scope.Close(obj_inject);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// init and bind function
extern "C" void init (v8::Handle<v8::Object> target)
{
    v8::HandleScope scope;
    target->Set(v8::String::New("geoip"), v8::FunctionTemplate::New(geoip_binding)->GetFunction());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
