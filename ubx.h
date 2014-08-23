#ifndef __UBX_H
#define __UBX_H

#define ID_AID_ALM	0x30
#define ID_AID_EPH	0x31

#define ID_MON_VER	0x04

#define CLASS_NAV           0x01
#define CLASS_RXM           0x02
#define CLASS_ACK           0x05
#define CLASS_CFG           0x06
#define CLASS_MON           0x0A
#define CLASS_AID           0x0B

#define ID_NAV_POSLLH       0x02
#define ID_NAV_STATUS       0x03
#define ID_NAV_VELNED       0x12
#define ID_NAV_TIMEUTC      0x21
#define ID_NAV_SVINFO       0x30

#define ID_ACK_NAK          0x00
#define ID_ACK_ACK          0x01

#define ID_CFG_PRT          0x00
#define ID_CFG_MSG          0x01
#define ID_CFG_RST          0x04
#define ID_CFG_RATE         0x08
#define ID_CFG_SBAS         0x16
#define ID_CFG_RXM          0x11
#define ID_CFG_NAV2         0x1A

#define ID_AID_INI          0x01
#define ID_AID_HUI          0x02
#define ID_AID_ALM          0x30
#define ID_AID_EPH          0x31

#define ID_MON_VER          0x04


struct _ubx_msg_type{
	unsigned char msg_class;
	unsigned char msg_id;
	unsigned char* payload_addr;
	unsigned short payload_length;
	unsigned char chksum[2];
};

typedef _ubx_msg_type ubx_msg_t;

ubx_msg_t nav_posllh_t = { CLASS_NAV, ID_NAV_POSLLH};
ubx_msg_t nav_status_t = { CLASS_NAV, ID_NAV_STATUS};
ubx_msg_t nav_velned = { CLASS_NAV, ID_NAV_VELNED};
ubx_msg_t nav_timeutc = { CLASS_NAV, ID_NAV_TIMEUTC};
ubx_msg_t nav_svinfo = { CLASS_NAV, ID_NAV_SVINFO};

ubx_msg_t cfg_prt = { CLASS_CFG, ID_CFG_PRT};
ubx_msg_t cfg_msg = { CLASS_CFG, ID_CFG_MSG};
ubx_msg_t cfg_rate = { CLASS_CFG, ID_CFG_RATE};
ubx_msg_t cfg_sbas = { CLASS_CFG, ID_CFG_SBAS};
ubx_msg_t cfg_rst = { CLASS_CFG, ID_CFG_RST};
ubx_msg_t cfg_rxm = { CLASS_CFG, ID_CFG_RXM};

ubx_msg_t mon_ver = { CLASS_MON, ID_MON_VER };

#include "gps.h"
#include "ubx_util.h"

gps_data_t gps_contain;
gps_time_t gps_time;

void chksum(unsigned char* packet, int size){
	unsigned long a(0x00);
	unsigned long b(0x00);
	int i(0);

	while (i < size){
		a += packet[i++];
		b += a;
	}
	packet[size] = a & 0xFF;
	packet[size + 1] = b & 0xFF;
}

void ubx_read_next(unsigned char* packet, ubx_msg_t* msg){
	msg->msg_class = packet[0];
	msg->msg_id = packet[1];
	msg->payload_length = (packet[3] << 8 | packet[2]);
	msg->payload_addr = packet + 4;
	msg->chksum[0] = packet[msg->payload_length];
	msg->chksum[1] = packet[msg->payload_length + 1];
}

bool parse_nav_posllh(ubx_msg_t* msg){
	unsigned char* buf = msg->payload_addr;

	if (msg->payload_length != 28){
		return false;
	}
#define POS_SCALE 10000000
#define ACC_SCALE 1000
#define VALID_HACC 1000000

	unsigned long acc_h = (unsigned long)READ_ULONG(buf + 20);
	if (acc_h > VALID_HACC){
		gps_contain.valid_ll = false;
		gps_contain.valid_h = false;
		return true;
	}

	gps_contain.llh_itow = (unsigned long)READ_ULONG(buf);
	gps_contain.acc_h = (float)acc_h / ACC_SCALE;
	gps_contain.acc_v = (float)READ_ULONG(buf + 24) / ACC_SCALE;
	gps_contain.llh[0] = (double)READ_DOUBLE(buf + 4) / ACC_SCALE;
	gps_contain.llh[1] = (double)READ_DOUBLE(buf + 8) / POS_SCALE;
	gps_contain.llh[2] = (double)READ_DOUBLE(buf + 12) / POS_SCALE;
	gps_contain.valid_ll = true;
	gps_contain.valid_h = true;

	return true;
}

bool parse_nav_velned(ubx_msg_t* msg){
	unsigned char* buf = msg->payload_addr;

	if (msg->payload_length != 36){
		return false;
	}
#define HEADING_SCALE	100000
#define VEL_SCALE	100
#define VALID_SPEED_ACC	300

	unsigned long acc = (unsigned long)READ_ULONG(buf + 28);
	if (acc > VALID_SPEED_ACC){
		gps_contain.valid_velned = false;
	}

	gps_contain.vel_ned[0] = (float)READ_DOUBLE(buf + 4) / VEL_SCALE;
	gps_contain.vel_ned[1] = (float)READ_DOUBLE(buf + 8) / VEL_SCALE;
	gps_contain.vel_ned[2] = (float)READ_DOUBLE(buf + 12) / VEL_SCALE;
	gps_contain.heading = (float)READ_DOUBLE(buf + 24) / HEADING_SCALE;
	gps_contain.valid_velned = true;

	return true;
}

bool parse_nav_time(ubx_msg_t* msg){
	unsigned char* buf = msg->payload_addr;

	gps_contain.gps_year = (U2)READ_USHORT(buf + 12);
	gps_contain.gps_month = (U1)READ_UCHAR(buf + 14);
	gps_contain.gps_day = (U1)READ_UCHAR(buf + 15);
	gps_contain.gps_hour = (U1)READ_UCHAR(buf + 16);
	gps_contain.gps_min = (U1)READ_UCHAR(buf + 17);
	gps_contain.gps_sec = (U1)READ_UCHAR(buf + 18);
}

bool parse_nav_status(ubx_msg_t* msg){
	unsigned char* buf = msg->payload_addr;

	gps_contain.itow = (unsigned long)READ_ULONG(buf);
	gps_contain.fix_type = (unsigned char)READ_UCHAR(buf + 4);

	return true;
}

bool parse_unit(ubx_msg_t* msg){
	if (msg->msg_class == CLASS_NAV){
		if (msg->msg_id == ID_NAV_POSLLH){
			return parse_nav_posllh(msg);
		}
		else if (msg->msg_id == ID_NAV_VELNED){
			return parse_nav_velned(msg);
		}
		else if (msg->msg_id == ID_NAV_TIMEUTC){
			return parse_nav_time(msg);
		}
		else if (msg->msg_id == ID_NAV_STATUS){
			return parse_nav_status(msg);
		}

	}
	return false;
}

void make_packet(){}



#endif