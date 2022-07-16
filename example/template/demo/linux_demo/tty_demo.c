#include "shell/sys/linux/vsf_linux_cfg.h"
#if VSF_USE_LINUX == ENABLED && APP_USE_LINUX_TTY_DEMO == ENABLED

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int tty_main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("format: %s tty_path\r\n", argv[0]);
        return -1;
    }

    struct termios termios;
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("fail to open terminal %s\r\n", argv[1]);
        return -1;
    }

    tcgetattr(fd, &termios);
    termios.c_cflag &= ~(CSIZE | PARENB | PARODD | CSTOPB);
    termios.c_cflag |= CS8;     // 8N1
    cfsetispeed(&termios, B115200);
    tcsetattr(fd, TCSANOW, &termios);

    write(fd, "echo\r\n", 6);

    char ch;
    while (read(fd, &ch, 1) == 1) {
        printf("%s: %c\r\n", argv[1], ch);
        write(fd, &ch, 1);
    }
    close(fd);
    return 0;
}
#endif
