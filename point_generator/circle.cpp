#include <bits/stdc++.h>

using namespace std;

double PI = 3.141592653589793;

int randomInt(double min, double max)
{
    int range = (int)(max - min);
    return rand() % range + min;
}

int main()
{
    srand(time(0));

    int numberOfCicles = 1;
    int numberOfPointsPerCircle = 50;
    double radiusMin = 1, radiusMax = 2;

    ofstream fout("circle.txt");

    for (int circle = 0; circle < numberOfCicles; circle++)
    {
        double r = randomInt(radiusMin, radiusMax);
        double k = randomInt(0, 15);

        double tiltAngleX = randomInt(0, 360) * (PI / 180);
        double tiltAngleY = randomInt(0, 360) * (PI / 180);
        double tiltAngleZ = randomInt(0, 360) * (PI / 180);

        for (int point = 0; point < numberOfPointsPerCircle; point++)
        {
            double t = randomInt(0, 360) * (PI / 180);

            double x = r * cos(t);
            double y = r * sin(t);
            double z = k;

            // https://www.geeksforgeeks.org/maths/rotation-matrix/
            // https://www.youtube.com/watch?v=EZufiIwwqFA&t=255s

            double tx, ty, tz;

            // x rotation
            tx = x;
            ty = y;
            tz = z;

            ty = y * cos(tiltAngleX) - z * sin(tiltAngleX);
            tz = y * sin(tiltAngleX) + z * cos(tiltAngleX);

            y = ty;
            z = tz;

            // y rotation
            tx = x;
            ty = y;
            tz = z;

            tx = x * cos(tiltAngleY) + z * sin(tiltAngleY);
            tz = -x * sin(tiltAngleY) + z * cos(tiltAngleY);

            x = tx;
            z = tz;

            // z rotation
            tx = x;
            ty = y;
            tz = z;

            tx = x * cos(tiltAngleZ) - y * sin(tiltAngleZ);
            ty = x * sin(tiltAngleZ) + y * cos(tiltAngleZ);

            x = tx;
            y = ty;

            fout << x << " " << y << " " << z << endl;
        }
    }

    fout.close();

    cout << "points.txt created" << endl;

    return 0;
}