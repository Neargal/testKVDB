#include <zmq.h>

#include "define.h"
#include "zmqServerBD.hpp"

int main(int argc, char const *argv[])
{
	zmqServerBD zmqServerBD;
	zmqServerBD.run();

	return 0;
}
