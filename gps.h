#ifndef __GPS_H
#define __GPS_H

struct gps_data_t{
	double llh_itow;
	bool valid_ll;
	bool valid_h;
	bool valid_velned;
	double llh[3];
	double heading;
	double acc_v, acc_h;
	double vel_ned[3];
	double acc_vel;

	unsigned long itow;
	unsigned char fix_type;
};

struct gps_time_t{
	unsigned int gps_year;
	unsigned int gps_month;
	unsigned int gps_day;
	unsigned int gps_hour;
	unsigned int gps_min;
	unsigned int gps_sec;
	char* gps_time_zone;
};

#endif