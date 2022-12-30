#include <cstdio>
#include <cstring>
#include <cinttypes>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cerrno>

int serialOpen(const char *device)
{
    int serialPort = open(device, O_RDWR);

    if (serialPort < 0)
    {
        printf("Error %i from open: %s\n", errno, strerror(errno));
    }

    // Configure port
    struct termios tty;
    // Initialise with existing attributes
    if (tcgetattr(serialPort, &tty) != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 0;
    }
    tty.c_cflag &= ~PARENB;                                                      // No parity
    tty.c_cflag &= ~CSTOPB;                                                      // 1 stop bit
    tty.c_cflag |= CS8;                                                          // 8 bits per data frame
    tty.c_cflag &= ~HUPCL;                                                       // Disable drop DTR on close
    tty.c_cflag &= ~CRTSCTS;                                                     // No hardware flow control
    tty.c_cflag |= CLOCAL;                                                       // Ignore modem control lines
    tty.c_cflag |= CREAD;                                                        // Enable read
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);                     // Disable local special handling bytes
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                      // Disable software flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable special handling of received bytes
    tty.c_oflag &= ~(OPOST | ONLCR);                                             // Disable special handling of sent bytes

    // Set timeout
    tty.c_cc[VTIME] = 1;   // Inter-character timeout in deciseconds
    tty.c_cc[VMIN] = 0xff; // Read until timeout is met
    // Set baud rate
    cfsetspeed(&tty, B9600);
    // Set attributes
    if (tcsetattr(serialPort, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 0;
    }

    return serialPort;
}

/**
 * Writes data to the serial port.
 * Returns the number of bytes written, or -1 if an error ocurred.
 */
ssize_t serialWrite(int serialPort, const uint8_t *data, size_t size)
{
    return write(serialPort, data, size);
}

/**
 * Read data from the serial port.
 * Returns the number of bytes read, or -1 if an error ocurred.
 */
ssize_t serialRead(int serialPort, uint8_t *data, size_t size)
{
    return read(serialPort, data, size);
}

void serialFlush(int serialPort)
{
    tcflush(serialPort, TCIOFLUSH);
}

void serialClose(int serialPort)
{
    close(serialPort);
}
