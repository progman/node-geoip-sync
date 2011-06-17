//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// 0.0.1
// Alexey Potehin http://www.gnuplanet.ru/doc/cv
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// apt-get install geoip-database-contrib libgeoip-dev
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
#include <string>
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
// float -> std::string
std::string f2s(float x)
{
    char tmp[64];
    sprintf(tmp, "%f", x);
    return tmp;
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


    GeoIP           *gi;
    GeoIPRecord     *gir;
    const char      *time_zone = NULL;
    char            **ret;
    const char      *returnedCountry;


    if (args.Length() < 1)
    {
        return VException("One argument required - ip for geoip()");
    }


    v8::String::AsciiValue b64data(args[0]->ToString());
    std::string ip_str = std::string(*b64data, b64data.length());

    obj_inject->Set(v8::String::New("ip"), v8::String::New(ip_str.c_str()));


    gi = GeoIP_open("/usr/share/GeoIP/GeoIP.dat", GEOIP_STANDARD | GEOIP_MEMORY_CACHE);
    if (gi == NULL)
    {
	return VException("Error opening GeoIP database");
    }

    returnedCountry = GeoIP_country_name_by_addr(gi, ip_str.c_str());
    if (returnedCountry == NULL)
    {
	GeoIP_delete(gi);
	return VException("Invalid GeoIP Query test failed, got non NULL, expected NULL");
    }

    obj_inject->Set(v8::String::New("country_name"), v8::String::New(returnedCountry));

    GeoIP_delete(gi);


    gi = GeoIP_open("/usr/share/GeoIP/GeoLiteCity.dat", GEOIP_INDEX_CACHE);
    if (gi == NULL)
    {
        return VException("Error opening GeoIP database");
    }

    gir = GeoIP_record_by_name(gi, ip_str.c_str());
    if (gir != NULL)
    {
	ret = GeoIP_range_by_ip(gi, ip_str.c_str());
	time_zone = GeoIP_time_zone_by_country_and_region(gir->country_code, gir->region);


	obj_inject->Set(v8::String::New("country_code"), v8::String::New(unknown(gir->country_code).c_str()));
	obj_inject->Set(v8::String::New("region_name"),  v8::String::New(unknown(GeoIP_region_name_by_code(gir->country_code, gir->region)).c_str()));
	obj_inject->Set(v8::String::New("city_name"),    v8::String::New(unknown(gir->city).c_str()));
	obj_inject->Set(v8::String::New("time_zone"),    v8::String::New(unknown(time_zone).c_str()));
	obj_inject->Set(v8::String::New("latitude"),     v8::String::New(f2s(gir->latitude).c_str()));
	obj_inject->Set(v8::String::New("longitude"),    v8::String::New(f2s(gir->longitude).c_str()));


	GeoIP_range_by_ip_delete(ret);
	GeoIPRecord_delete(gir);
    }
    GeoIP_delete(gi);


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
