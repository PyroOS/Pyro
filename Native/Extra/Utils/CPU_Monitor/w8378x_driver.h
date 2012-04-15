#include <atheos/device.h>

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

enum
{
  W8378x_FIRST_OPCODE = IOCTL_USER,

	W8378x_READ_TEMP1 = W8378x_FIRST_OPCODE,
	W8378x_READ_TEMP2,
	W8378x_READ_TEMP3,

	W8378x_READ_FAN1 = W8378x_FIRST_OPCODE+100,
	W8378x_READ_FAN2,
	W8378x_READ_FAN3
};

//-----------------------------------------------------------------------------
