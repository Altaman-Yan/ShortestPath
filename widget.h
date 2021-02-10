#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QPalette>
#include <QImage>
#include <QMap>
#include <QVector>
//#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QInputDialog>
#include <QPainter>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void deleteVertex();
    void clearFloydShortestPath() ;
    void clearDijShortestPath();
    void clearAll() ;
    void closeWindow();

public slots:
    void Floyd_shortestPath();
    void Dijkstra_shortestPath();

private:
//    bool boolean=false;
    const int Infinity=999999999;
    const int Radius=30;//半径
    const int Margin=5;//边界
    const int isClicked=-1;//被点击
    const int isNotClicked=-2;//没有被点击
    const int Erased=-99;//被删除
    QPoint edge ;//x坐标作为删除或者被点击的标志，y坐标可以存储顶点号
    QPoint ShortestPath ;//x坐标保存第一个被点击的顶点号，y坐标保存第二个被点击的顶点号
    QToolButton* button1;//Floyd shortestpath button
    QToolButton* button2;//clear button
    QToolButton* button3;//close button
    QToolButton* button4;//Dijskra
    QToolButton* button5;//delete vertex
    QMap< int,QVector<int> > matrix ;//保存的是哪个顶点与哪个顶点之间存在边
    QMap< int,QVector<int> > weight ;//边的权重
    QMap< int,QVector<qreal> > angles ;//边的角度
    QVector<QPointF> points ;//顶点坐标
    QVector<int> path ;//存储最短路径
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void inputManager() ;
};
#endif // WIDGET_H
