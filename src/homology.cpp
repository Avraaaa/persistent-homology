#include <bits/stdc++.h>

using namespace std;

// for hashmap, C++ doesnt support hashing for vector<int> by default
struct VectorHash
{
    size_t operator()(const vector<int> &v) const
    {
        size_t hash = 0;
        for (int x : v)
        {
            hash ^= hash * 31 + std::hash<int>()(x);
        }
        return hash;
    }
};

class Point
{
public:
    double x, y, z;
    inline static double max_distance = 0.0;

    Point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    static double calcDistance(const Point &p1, const Point &p2)
    {
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        double dz = p2.z - p1.z;

        double distance = sqrt(dx * dx + dy * dy + dz * dz);
        return distance;
    }

    static void setMaxDistance(double distance)
    {
        max_distance = distance;
    }

    static double getMaxDistance()
    {
        return max_distance;
    }
};

class Simplex
{
public:
    double birth_time;
    int dimension;
    vector<int> vertices;

    Simplex(double birth_time, int dimension, vector<int> vertices)
    {
        this->birth_time = birth_time;
        this->dimension = dimension;
        this->vertices = vertices;
    }
};

class Interval
{
public:
    int dimension;
    double birth;
    double death;
    int birth_simplex;
    int death_simplex;

    Interval(int dimension, double birth, double death, int birth_simplex, int death_simplex)
    {
        this->dimension = dimension;
        this->birth = birth;
        this->death = death;
        this->birth_simplex = birth_simplex;
        this->death_simplex = death_simplex;
    }
};

class vietorisRips
{
public:
    static vector<Simplex> filter(vector<Point> points)
    {
        int n = points.size();
        vector<Simplex> filtration;

        // add 0-simplices(vertices)
        for (int i = 0; i < n; i++)
        {
            Simplex s = Simplex(0.0, 0, vector<int>{i});
            filtration.push_back(s);
        }

        // add 1-simplices(edges)
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                double distance_as_birthtime = Point::calcDistance(points[i], points[j]);
                Simplex s = Simplex(distance_as_birthtime, 1, vector<int>{i, j});
                filtration.push_back(s);
            }
        }

        // add 2-simplices(triangles)
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                for (int k = j + 1; k < n; k++)
                {
                    double d1 = Point::calcDistance(points[i], points[j]);
                    double d2 = Point::calcDistance(points[j], points[k]);
                    double d3 = Point::calcDistance(points[i], points[k]);
                    double distance_as_birthtime = max({d1, d2, d3});

                    Simplex s = Simplex(distance_as_birthtime, 2, vector<int>{i, j, k});
                    filtration.push_back(s);
                }
            }
        }

        // add 3-simplices(tetrahedrons)
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                for (int k = j + 1; k < n; k++)
                {
                    for (int l = k + 1; l < n; l++)
                    {
                        double d1 = Point::calcDistance(points[i], points[j]);
                        double d2 = Point::calcDistance(points[i], points[k]);
                        double d3 = Point::calcDistance(points[i], points[l]);
                        double d4 = Point::calcDistance(points[j], points[k]);
                        double d5 = Point::calcDistance(points[j], points[l]);
                        double d6 = Point::calcDistance(points[k], points[l]);
                        double distance_as_birthtime = max({d1, d2, d3, d4, d5, d6});

                        Simplex s = Simplex(distance_as_birthtime, 3, vector<int>{i, j, k, l});
                        filtration.push_back(s);
                    }
                }
            }
        }

        //  sort first by time then by dimension
        sort(filtration.begin(), filtration.end(), [](const Simplex &s1, const Simplex &s2)
             {
            if(fabs(s1.birth_time-s2.birth_time)>1e-9){
                return s1.birth_time<s2.birth_time;
            }
            if(s1.dimension!=s2.dimension){
                return s1.dimension<s2.dimension;
            }
            return s1.vertices<s2.vertices; });

        cout << "Filtration done, took " << filtration.size() << " simplices" << endl;

        return filtration;
    }
};

class BoundaryMatrix
{
public:
    // for now gonna use dense matrix ig
    // for future me sparse vector uwu
    static vector<vector<int>> getCombinations(vector<int> vertices)
    {
        vector<vector<int>> combinations;

        for (int i = 0; i < vertices.size(); i++)
        {
            vector<int> face;

            for (int j = 0; j < vertices.size(); j++)
            {
                if (j != i)
                {
                    face.push_back(vertices[j]);
                }
            }

            combinations.push_back(face);
        }

        return combinations;
    }

    static vector<vector<int>> getBoundaryMatrix(vector<Simplex> filtration)
    {

        int n = filtration.size();
        vector<vector<int>> boundaryMatrix(n, vector<int>(n, 0));
        unordered_map<vector<int>, int, VectorHash> lookup;

        for (int i = 0; i < n; i++)
        {
            lookup.insert({filtration[i].vertices, i});
        }

        for (int i = 0; i < n; i++)
        {
            vector<int> temp = filtration[i].vertices;
            vector<vector<int>> combinations = getCombinations(temp);
            // 0-dimensional simplices no have boundary
            if (temp.size() == 1)
            {
                continue;
            }
            for (int j = 0; j < combinations.size(); j++)
            {
                boundaryMatrix[lookup.at(combinations[j])][i] = 1;
            }
        }
        return boundaryMatrix;
    }
};

class Persistence
{

public:
    static vector<Interval> reduction(vector<vector<int>> &B, vector<Simplex> &filtration)
    {
        int n = B.size();
        vector<int> killed(n, 0);
        vector<int> lowOfColumn(n, -1);
        unordered_map<int, int> lowestRowToColumn;

        for (int i = 0; i < n; i++)
        {
            int temp_low = getLow(B, i);

            while (temp_low != -1 && lowestRowToColumn.count(temp_low) != 0)
            {
                xorColumn(B, lowestRowToColumn[temp_low], i);
                temp_low = getLow(B, i);
            }
            lowOfColumn[i] = temp_low;
            if (temp_low != -1)
            {
                lowestRowToColumn[temp_low] = i;
                killed[temp_low] = 1;
            }
        }
        vector<Interval> intervals;

        // finite intervals
        for (int death = 0; death < lowOfColumn.size(); death++)
        {
            int birth = lowOfColumn[death];
            if (birth != -1)
            {
                int dimension = filtration[birth].dimension;
                double birth_time = filtration[birth].birth_time;
                double death_time = filtration[death].birth_time;

                Interval interval = Interval(
                    dimension,
                    filtration[birth].birth_time,
                    filtration[death].birth_time,
                    birth,
                    death);
                intervals.push_back(interval);
            }
        }

        // infinite intervals
        for (int birth = 0; birth < n; birth++)
        {
            if (lowOfColumn[birth] == -1 && killed[birth] == 0)
            {
                int dimension = filtration[birth].dimension;

                Interval interval = Interval(
                    dimension,
                    filtration[birth].birth_time,
                    -1,
                    birth,
                    -1);

                intervals.push_back(interval);
            }
        }

        return intervals;
    }

    static void printIntervals(vector<Interval> intervals)
    {
        ofstream file;
        file.open("all_intervals");

        for (int i = 0; i < intervals.size(); i++)
        {
            file << "H" << intervals[i].dimension << " : ";

            if (intervals[i].death == -1)
            {
                file << "(" << intervals[i].birth << ", " << "inf" << ")" << endl;
            }
            else
            {
                file << "(" << intervals[i].birth << ", " << intervals[i].death << ")" << endl;
            }
        }

        file.close();

    }

    static void printImportantIntervals(vector<Interval> intervals)
    {
        ofstream file;
        file.open("useful_intervals");

        double eps = 1e-6;
        int maxDimension = 0;

        for (int i = 0; i < intervals.size(); i++)
        {
            maxDimension = max(maxDimension, intervals[i].dimension);
        }

        for (int i = 0; i < intervals.size(); i++)
        {
            if (intervals[i].dimension >= maxDimension)
            {
                continue;
            }

            if (intervals[i].death != -1)
            {
                double length = intervals[i].death - intervals[i].birth;

                if (length <= eps)
                {
                    continue;
                }
            }

            file << "H" << intervals[i].dimension << " : ";

            if (intervals[i].death == -1)
            {
                file << "(" << intervals[i].birth << ", " << "inf" << ")" << endl;
            }
            else
            {
                file << "(" << intervals[i].birth << ", " << intervals[i].death << ")" << endl;
            }
        }

        file.close();

    }

    static void printBettiNumbers(vector<Interval> intervals)
    {
        ofstream file;
        file.open("betti_numbers");

        int maxDimension = 0;

        for (int i = 0; i < intervals.size(); i++)
        {
            maxDimension = max(maxDimension, intervals[i].dimension);
        }

        vector<int> totalIntervals(maxDimension + 1, 0);
        vector<int> killedIntervals(maxDimension + 1, 0);
        vector<int> bettiNumbers(maxDimension + 1, 0);

        for (int i = 0; i < intervals.size(); i++)
        {
            int dimension = intervals[i].dimension;

            totalIntervals[dimension]++;

            if (intervals[i].death == -1)
            {
                bettiNumbers[dimension]++;
            }
            else
            {
                killedIntervals[dimension]++;
            }
        }

        file << "interval counts" << endl;

        for (int i = 0; i <= maxDimension; i++)
        {
            file << "H" << i << " total intervals created : " << totalIntervals[i] << endl;
            file << "H" << i << " intervals killed : " << killedIntervals[i] << endl;
        }

        file << endl;
        file << "betti numbers" << endl;

        for (int i = 0; i <= maxDimension; i++)
        {
            file << "B" << i << " : " << bettiNumbers[i] << endl;
            cout << "B" << i << " : " << bettiNumbers[i] << endl;
        }

        file.close();

    }

private:
    static int getLow(const vector<vector<int>> &B, int col)
    {
        int low = -1;

        for (int i = B.size() - 1; i >= 0; i--)
        {
            if (B[i][col] == 1)
            {
                low = i;
                return low;
            }
        }
        return low;
    }

    static void xorColumn(vector<vector<int>> &B, int src, int dest)
    {
        for (int row = B.size() - 1; row >= 0; row--)
        {
            B[row][dest] ^= B[row][src];
        }
    }
};

vector<Point> loadFromFile(string filename)
{
    vector<Point> points;

    ifstream file;
    file.open(filename);

    if (file.is_open() == false)
    {
        cout << "file could not be opened" << endl;
        return points;
    }

    double x;
    double y;
    double z;

    while (file >> x >> y >> z)
    {
        Point p(x, y, z);
        points.push_back(p);
    }

    file.close();

    cout << "number of points loaded: " << points.size() << endl;

    return points;
}

int main()
{
    vector<Point> points = loadFromFile("../point_generator/circle.txt");

    vector<Simplex> filtration = vietorisRips::filter(points);

    vector<vector<int>> B = BoundaryMatrix::getBoundaryMatrix(filtration);

    vector<Interval> intervals = Persistence::reduction(B, filtration);

    Persistence::printIntervals(intervals);
    Persistence::printImportantIntervals(intervals);
    Persistence::printBettiNumbers(intervals);

    return 0;
}