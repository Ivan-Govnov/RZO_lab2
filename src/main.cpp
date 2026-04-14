#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace cv;
using namespace std;

int main() {

    // параметры сеток
    double a = 31;          //кол-во ячеек по ширине основной сетки
    double b = 19;          //кол-во ячеек по высоте основной сетки
    double alpha = 120;     //кол-во ячеек по ширине вложенной сетки
    double beta = 80;       //кол-во ячеек по высоте вложенной сетки

    //соотношения сторон у ячеек основной сетки r_a : r_b (ширина к высоте)
    double r_a = 1;
    double r_b = 1;
    //соотношения сторон у ячеек вложенной сетки r_alpha : r_beta (ширина к высоте)
    double r_alpha = 1;
    double r_beta = 1;

    //размер окна
    double w = 1280;
    double h = 720;

    double m = 24; //максимальное кол-во ячеек по ширине, которое может занять вложенная сетка 
    double n = 14; //максимальное кол-во ячеек по высоте, которое может занять вложенная сетка 

    //откуда примерно строить вложенную сетку
    int start_cell_x = 5;
    int start_cell_y = 3;

    //координаты фигур
    int star_cell_x = 29;
    int star_cell_y = 15;

    int circle_cell_x = 15;
    int circle_cell_y = 2;

    int ellipse_cell_x = 20;
    int ellipse_cell_y = 2;

    // чейка для текста
    int text_cell_x = 3;
    int text_cell_y = 10;

    //расчёт оптимальных размеров (x_star, y_star) для сеток
    double y_star1 = std::min(w / (a * r_a), h / (b * r_b));
    double k1 = (m * r_a) / (alpha * r_alpha);
    double k2 = (n * r_b) / (beta * r_beta);
    double k = std::min(k1, k2);
    double x_star1 = k * y_star1;
    int x_star = static_cast<int>(x_star1);

    double y_star;
    if (k1 <= k2) {
        y_star = x_star / k1;
    } else {
        y_star = x_star / k2;
    }

    //создание холста с небольшими полями по краям
    int canvas_w = static_cast<int>(w) + 4;
    int canvas_h = static_cast<int>(h) + 4;
    Mat base(canvas_h, canvas_w, CV_8UC3, Scalar(0, 0, 0));

    //построение основной сетки
    double cell_w1 = y_star * r_a;   // ширина одной ячейки первой сетки
    double cell_h1 = y_star * r_b;   // высота ячейки
    int grid1_w = static_cast<int>(std::round(a * cell_w1));
    int grid1_h = static_cast<int>(std::round(b * cell_h1));

    //центрируем основную сетку на холсте
    int rect1_x1 = (canvas_w - grid1_w) / 2;
    int rect1_y1 = (canvas_h - grid1_h) / 2;
    int rect1_x2 = rect1_x1 + grid1_w - 1;
    int rect1_y2 = rect1_y1 + grid1_h - 1;

    //заливка фона первой сетки белым
    rectangle(base, Point(rect1_x1, rect1_y1),
                    Point(rect1_x2, rect1_y2),
                    Scalar(255, 255, 255), FILLED);
    //внешняя рамка холста синим
    rectangle(base, Point(0, 0),
                    Point(canvas_w - 1, canvas_h - 1),
                    Scalar(255, 0, 0), 2);

    Scalar grid1_color(0, 255, 0);  //зелёный цвет линий

    //вертикальные линии основной сетки
    for (int i = 0; i <= a; ++i) {
        int x = rect1_x1 + static_cast<int>(std::round(i * cell_w1));
        if (x > rect1_x2) x = rect1_x2;
        line(base, Point(x, rect1_y1), Point(x, rect1_y2), grid1_color, 2);
    }

    //горизонтальные линии первой сетки
    for (int j = 0; j <= b; ++j) {
        int y = rect1_y1 + static_cast<int>(std::round(j * cell_h1));
        if (y > rect1_y2) y = rect1_y2;
        line(base, Point(rect1_x1, y), Point(rect1_x2, y), grid1_color, 2);
    }

    //построение вложенной сетки
    double cell_w2 = x_star * r_alpha;
    double cell_h2 = x_star * r_beta;

    //область внутри первой сетки, где будет вложенная сетка
    int area_x1 = rect1_x1 + start_cell_x * cell_w1;
    int area_y1 = rect1_y1 + start_cell_y * cell_h1;
    int area_w = m * cell_w1;
    int area_h = n * cell_h1;

    // азмер вложенной сетки в пикселях
    int width2  = alpha * cell_w2;
    int height2 = beta  * cell_h2;

    //центрируем вложенную сетку внутри выделенной области
    int rect2_x1 = area_x1 + (area_w - width2) / 2;
    int rect2_y1 = area_y1 + (area_h - height2) / 2;
    int rect2_x2 = rect2_x1 + width2 - 1;
    int rect2_y2 = rect2_y1 + height2 - 1;

    //заливка фона вложенной сетки светло-серым
    rectangle(base, Point(rect2_x1, rect2_y1),
                    Point(rect2_x2, rect2_y2),
                    Scalar(220, 220, 220), FILLED);
    //красная рамка вокруг вложенной сетки
    rectangle(base, Point(rect2_x1 - 2, rect2_y1 - 2),
                    Point(rect2_x2 + 2, rect2_y2 + 2),
                    Scalar(0, 0, 255), 2);

    Scalar grid2_color(0, 255, 255);    //жёлтый цвет линий

    //вертикальные линии вложенной сетки
    for (int i = 0; i <= alpha; ++i) {
        int x = rect2_x1 + static_cast<int>(std::round(i * cell_w2));
        if (x > rect2_x2) x = rect2_x2;
        line(base, Point(x, rect2_y1), Point(x, rect2_y2), grid2_color, 1);
    }

    //горизонтальные линии вложенной сетки
    for (int j = 0; j <= beta; ++j) {
        int y = rect2_y1 + static_cast<int>(std::round(j * cell_h2));
        if (y > rect2_y2) y = rect2_y2;
        line(base, Point(rect2_x1, y), Point(rect2_x2, y), grid2_color, 1);
    }

    // лямбда-функция для получения центра ячейки по её индексам
    auto cellCenter = [&](int cx, int cy) {
        return Point(
            rect1_x1 + static_cast<int>(std::round((cx + 0.5) * cell_w1)),
            rect1_y1 + static_cast<int>(std::round((cy + 0.5) * cell_h1))
        );
    };

    // звезда (5-конечная, рисуется 10 точками)
    Point star_center = cellCenter(star_cell_x, star_cell_y);
    vector<Point> pts;
    int R = static_cast<int>(0.4 * min(cell_w1, cell_h1));  // внешний радиус
    int r = R / 2;                                          // внутренний радиус

    for (int i = 0; i < 10; i++) {
        double angle = i * CV_PI / 5;
        int radius = (i % 2 == 0) ? R : r;
        pts.push_back(Point(
            star_center.x + static_cast<int>(radius * cos(angle)),
            star_center.y + static_cast<int>(radius * sin(angle))
        ));
    }
    polylines(base, pts, true, Scalar(0, 0, 255), 2);       //красный контур

    // круг
    circle(base, cellCenter(circle_cell_x, circle_cell_y),
           static_cast<int>(0.4 * min(cell_w1, cell_h1)),
           Scalar(255, 0, 255), 2);                         //розовый

    // эллипс
    ellipse(base, cellCenter(ellipse_cell_x, ellipse_cell_y),
            Size(static_cast<int>(cell_w1 * 0.4), static_cast<int>(cell_h1 * 0.25)),
            0, 0, 360,
            Scalar(0, 128, 255), 2);                        //оранжевый

    // анимация текста (движение вниз в пределах ячейки)
    int frame = 0;      //счётчик кадров для смещения текста

    bool save_image = false;

    // бесконечный цикл отображения (закрытие только по ESC или крестику окна)
    while (true) {
        //копия базового изображения, чтобы не рисовать всё заново
        Mat canvas = base.clone();

        //координаты верхней границы ячейки, в которой находится текст
        int cell_top = rect1_y1 + static_cast<int>(text_cell_y * cell_h1);
        //вертикальное смещение текста (увеличивается с каждым кадром)
        int text_y = cell_top + (frame % static_cast<int>(cell_h1));
        //координата X (левый край ячейки + небольшой отступ)
        int text_x = rect1_x1 + static_cast<int>(text_cell_x * cell_w1 + 5);

        // вывод движущегося текста
        putText(canvas, "Moving Text",
                Point(text_x, text_y),
                FONT_HERSHEY_SIMPLEX,
                0.6,
                Scalar(0, 0, 0), 2);

        // показать изображение в окне
        imshow("Grids", canvas);

        if (save_image == false) {
            cv::imwrite("result.jpg", canvas);
            save_image = true;
        }

        // ожидание 30 мс и получение кода нажатой клавиши
        int key = waitKey(30);

        // условие выхода (код 27 у ESC)
        if (key == 27) {
            break;
        }

        // проверка, не закрыто ли окно крестиком (свойство VISIBLE становится 0)
        if (getWindowProperty("Grids", WND_PROP_VISIBLE) == 0) {
            break;
        }

        // увеличиваем счётчик кадров для следующего положения текста
        frame++;
    }

    return 0;
}