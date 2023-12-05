#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

double f(double x)
{
    return 4.0 / (x * x + 1.0);
}

double integrate(double a, double b, double width)
{
    double integral_value = 0.0;
    double x;

    for (x = a + 0.5 * width; x < b; x += width)
    {
        integral_value += f(x) * width;
    }

    return integral_value;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        return 1;
    }

    // printf("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);

    double a = atof(argv[2]);
    double b = atof(argv[3]);
    double width = atof(argv[4]);

    double partial_integral = integrate(a, b, width);

    int fd = open(argv[1], O_WRONLY);
    write(fd, &partial_integral, sizeof(double));
    close(fd);

    return 0;
}