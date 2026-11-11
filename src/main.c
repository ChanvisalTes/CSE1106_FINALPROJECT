
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>

#define DEFAULT_MIN_TEMP 68.0
#define DEFAULT_MAX_TEMP 76.0
#define PIN 17

int checkTemperature(double temperature, double minTemp, double maxTemp);
void displayStatus(int warning);
void saveReading(FILE *file, double temperature, int warning);
int process();

int main() {
    gpioInitialise();
    gpioSetMode(PIN, PI_OUTPUT);
    int i=process();
    gpioTerminate();
    return i;
}

int process()
{

    double temperature;
    double totalTemperature = 0.0;
    double averageTemperature;

    double minTemp = DEFAULT_MIN_TEMP;
    double maxTemp = DEFAULT_MAX_TEMP;

    int readingCount = 0;
    int warningCount = 0;

    int warning;

    FILE *file = fopen("temperature_log.txt", "a");

    if (file == NULL)
    {
        printf("Error: Unable to open the file.\n");
        return 1;
    }

    printf("====================================\n");
    printf(" SMART CLASSROOM COMFORT MONITOR\n");
    printf("====================================\n");

    printf("\nDefault acceptable temperature range:\n");
    printf("%.1f F - %.1f F\n", minTemp, maxTemp);

    printf("\nWould you like to change the temperature range?\n");
    printf("Enter 1 for YES or 0 for NO: ");

    int choice;
    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input.\n");
        fclose(file);
        return 1;
    }

    if (choice == 1)
    {
        printf("Enter minimum acceptable temperature: ");
        if (scanf("%lf", &minTemp) != 1)
        {
            printf("Invalid temperature.\n");
            fclose(file);
            return 1;
        }

        printf("Enter maximum acceptable temperature: ");
        if (scanf("%lf", &maxTemp) != 1 || minTemp > maxTemp)
        {
            printf("Invalid temperature range.\n");
            fclose(file);
            return 1;
        }
    }

    printf("\nCurrent acceptable range: %.1f F - %.1f F\n",
        minTemp, maxTemp);

    printf("\nEnter classroom temperature readings.\n");
    printf("Enter -999 to finish entering readings.\n");

    while (1)
    {
        printf("\nEnter temperature (F): ");

        if (scanf("%lf", &temperature) != 1)
        {
            printf("Invalid input. Please enter a number.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);

            continue;
        }

        if (temperature == -999)
        {
            break;
        }

        warning = checkTemperature(
            temperature, minTemp, maxTemp);

        displayStatus(warning);

        saveReading(file, temperature, warning);

        totalTemperature += temperature;
        readingCount++;

        if (warning == 1)
        {
            warningCount++;
        }
    }

    printf("\n====================================\n");
    printf("         FINAL REPORT\n");
    printf("====================================\n");

    if (readingCount > 0)
    {
        averageTemperature = totalTemperature / readingCount;

        printf("Total readings: %d\n", readingCount);

        printf("Average temperature: %.2f F\n",
            averageTemperature);

        printf("Warning readings: %d\n", warningCount);

        printf("Normal readings: %d\n",
            readingCount - warningCount);
    }
    else
    {
        printf("No temperature readings were entered.\n");
    }

    fclose(file);

    printf("\nTemperature readings saved successfully.\n");
    printf("File name: temperature_log.txt\n");

    printf("\nThank you for using the Smart Classroom Comfort Monitor!\n");

    return 0;
}

int checkTemperature(double temperature, double minTemp, double maxTemp)
{
    if (temperature >= minTemp && temperature <= maxTemp)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void displayStatus(int warning)
{
    if (warning == 0)
    {
        printf("Temperature Status: NORMAL\n");
        printf("Warning LED: OFF\n");
        gpioWrite(PIN,0);
    }
    else
    {
        printf("Temperature Status: WARNING\n");
        printf("Warning LED: ON\n");
        gpioWrite(PIN,1);
    }
}

void saveReading(FILE *file, double temperature, int warning)
{
    if (warning == 0)
    {
        fprintf(file,
            "Temperature: %.2f F | Status: NORMAL | LED: OFF\n",
            temperature);
    }
    else
    {
        fprintf(file,
            "Temperature: %.2f F | Status: WARNING | LED: ON\n",
            temperature);
    }
}
