#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "raylib.h"

using namespace std;

struct Node {
    double x;
    double y;
};

double getFunction(double x) {
    return cos(x)*sin(x);
}

vector<Node> chebish(double xmin, double xmax, int n) {
    vector<Node> dots(n);

    for (int i = 0; i < n; i++)
    {
        double t_i = cos((2.0 * i + 1.0) * PI / (2.0 * n));

        dots[i].x = 0.5 * (xmin + xmax) + 0.5 * (xmax - xmin) * t_i;
        dots[i].y = getFunction(dots[i].x);
    }
    return dots;
}

double polimom(double x, vector<Node>dots) {
    double sum = 0;
    for (int i = 0; i < dots.size(); i++) {
        double l = dots[i].y;
        for (int j = 0; j < dots.size(); j++) {
            if (i != j) {
                l *= ((x - dots[j].x) / (dots[i].x - dots[j].x));
            }
        }
        sum += l;
    }
    return sum;
}
/* для эксель
int main() {
    vector<Node> a = chebish(0, PI, 5);

    int M = 10;
    double step = (PI-0) / (M - 1);

    for (int i = 0; i < 10; i++) {
        double xi = 0 + i * step;
        double y = getFunction(xi);
        double y_approx = polimom(xi, a);

        cout << xi << " " << y << " " << y_approx<<endl;
    }
    */

    Vector2 MathToScreen(double x, double y, double xmin, double xmax, int screenWidth, int screenHeight) {
        double margin = 50.0;
        double plotWidth = screenWidth - 2 * margin;
        double plotHeight = screenHeight - 2 * margin;

        // Масштабируем x от [xmin, xmax] к [margin, screenWidth - margin]
        float px = margin + (float)((x - xmin) / (xmax - xmin) * plotWidth);

        // Масштабируем y от [-1.5, 1.5] к [screenHeight - margin, margin] (ось Y в окне перевернута)
        float py = (screenHeight - margin) - (float)((y - (-1.5)) / (1.5 - (-1.5)) * plotHeight);

        return { px, py };
    }

    int main() {
        double xmin = 0.0;
        double xmax = PI;
        int N = 5;

        // Вычисляем узлы
        vector<Node> nodes = chebish(xmin, xmax, N);

        // Параметры окна
        const int screenWidth = 800;
        const int screenHeight = 600;
        InitWindow(screenWidth, screenHeight, "Chebyshev Interpolation");

        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);

            // 1. Рисуем сетку и оси
            DrawRectangleLines(50, 50, screenWidth - 100, screenHeight - 100, LIGHTGRAY);

            // Ось X (y = 0)
            Vector2 axisX_start = MathToScreen(xmin, 0, xmin, xmax, screenWidth, screenHeight);
            Vector2 axisX_end = MathToScreen(xmax, 0, xmin, xmax, screenWidth, screenHeight);
            DrawLineV(axisX_start, axisX_end, GRAY);

            // 2. Рисуем точный график y(x) = cos(x) (СИНИЙ)
            int steps = 300;
            double step = (xmax - xmin) / steps;
            for (int i = 0; i < steps; i++) {
                double x1 = xmin + i * step;
                double x2 = xmin + (i + 1) * step;

                Vector2 p1 = MathToScreen(x1, getFunction(x1), xmin, xmax, screenWidth, screenHeight);
                Vector2 p2 = MathToScreen(x2, getFunction(x2), xmin, xmax, screenWidth, screenHeight);
                DrawLineEx(p1, p2, 2.0f, BLUE);
            }

            // 3. Рисуем интерполяционный полином P(x) (КРАСНЫЙ ПУНКТИР/ЛИНИЯ)
            for (int i = 0; i < steps; i++) {
                double x1 = xmin + i * step;
                double x2 = xmin + (i + 1) * step;

                Vector2 p1 = MathToScreen(x1, polimom(x1, nodes), xmin, xmax, screenWidth, screenHeight);
                Vector2 p2 = MathToScreen(x2, polimom(x2, nodes), xmin, xmax, screenWidth, screenHeight);
                DrawLineEx(p1, p2, 2.0f, RED);
            }

            // 4. Рисуем узлы Чебышева (ЗЕЛЕНЫЕ ТОЧКИ)
            for (const auto& node : nodes) {
                Vector2 p = MathToScreen(node.x, node.y, xmin, xmax, screenWidth, screenHeight);
                DrawCircleV(p, 6.0f, DARKGREEN);
            }

            // Легенда
            DrawText("BLUE - Exact y(x)", 60, 60, 16, BLUE);
            DrawText("RED - Approx P(x)", 60, 80, 16, RED);
            DrawText("GREEN - Chebyshev Nodes", 60, 100, 16, DARKGREEN);

            EndDrawing();
        }

        CloseWindow();
        return 0;
    
}
