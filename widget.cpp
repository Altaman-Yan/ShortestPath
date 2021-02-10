#include "widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    //设置窗口标题
    this->setWindowTitle ("最短路径动画演示");
    //设置窗口大小
    this->setMinimumSize(800,600);
    //设置背景图片
    QPalette background=this->palette();
    QImage img(":/new/prefix1/C:/Users/HASEE/Desktop/p.jpg");
    QImage fitimgpic=img.scaled(this->width(),this->height(),Qt::IgnoreAspectRatio);
    background.setBrush(QPalette::Window,QBrush(fitimgpic));
    this->setPalette(background);

    this->setAutoFillBackground(true);//fill the background of the widget
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);//the default layout behavior of the widget
    this->setFocusPolicy(Qt::StrongFocus);//the widget accepts focus by both tabbing and clicking

    //初始化
    edge.setX(isNotClicked);
    edge.setY(isNotClicked);
    ShortestPath.setX(isNotClicked);
    ShortestPath.setY(isNotClicked);

    //创建按钮
    button1 = new QToolButton(this);
    button1->setText("Floyd最短路");
    button1->setCheckable(true);//点击按钮后，改变按钮颜色
    button1->setStyleSheet("background-color:rgb(250,250,250)");//设置按钮背景颜色
    button1->resize(150,50);//设置按钮的宽度和高度
    connect(button1,SIGNAL(toggled(bool)),this,SLOT(clearFloydShortestPath()));

    button2 = new QToolButton(this);
    button2->setText("清空");
    button2->setStyleSheet("background-color:rgb(248,248,255)");
    button2->resize(150,50);
    connect(button2,SIGNAL(clicked(bool)),this,SLOT(clearAll()));

    button3 = new QToolButton(this);
    button3->resize(60,50);
    button3->setStyleSheet("border-image:url(:/new/prefix1/C:/Users/HASEE/Desktop/quit.jpg)");
    connect(button3,SIGNAL(clicked(bool)),this,SLOT(closeWindow()));

    button4=new QToolButton(this);
    button4->setText("Dijskra最短路");
    button4->setStyleSheet("background-color:rgb(248,248,255)");
    button4->resize(150,50);
    button4->setCheckable(true);
    connect(button4,SIGNAL(toggled(bool)),this,SLOT(clearDijShortestPath()));

    button5=new QToolButton(this);
    button5->setText("删除顶点");
    button5->setStyleSheet("background-color:rgb(250,250,250)");
    button5->resize(150,50);
    button5->setCheckable(true);
    connect(button5,SIGNAL(clicked(bool)),this,SLOT(deleteVertex()));
}

//调用update()之后会调用paintEvent函数
void Widget::paintEvent(QPaintEvent *){
    QPainter Painter(this);
    Painter.setRenderHint(QPainter::Antialiasing);
    //设置线
    QPen pen( Qt::black );
    pen.setWidth(1);
    pen.setStyle( Qt::SolidLine );//实线
//    pen.setStyle(Qt::CustomDashLine);
    Painter.setPen( pen );
    //设置字体
    QFont font;
    font.setPixelSize(20);
    font.setWeight(QFont::ExtraLight);
    Painter.setFont(font);

    Painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));

    bool vertexFlag=false;

    for(int i = 0 ; i<points.size() ; i++)
    {
        if(points[i].x()==Erased){continue;}
        //如果用floyd方法寻求最短路径，将算法求出来的最短路径上的顶点填充成黄色
        if(button1->isChecked())
        {
            for(int j=0 ; j<path.size() ; j++)
            {
                if(path[j]==i)
                {
                    vertexFlag=true;
                    break;
                }
            }
            if(vertexFlag)
            {
                Painter.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));
            }
        }
        //如果用Dijkstra方法寻求最短路径，将算法求出来的最短路径上的顶点填充成黄色
        if(button4->isChecked())
        {
            for(int j=0 ; j<path.size() ; j++)
            {
                if(path[j]==i)
                {
                    vertexFlag=true;
                    break;
                }
            }
            if(vertexFlag)
            {
                Painter.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));
            }
        }

        //绘制顶点
        Painter.drawEllipse(points[i],Radius - Radius/5,Radius - Radius/5);
        Painter.drawText(points[i].x()-Radius, points[i].y()-Radius, 2*Radius, 2*Radius, Qt::AlignCenter, QString::number(i));//在顶点中心标上顶点号
        //brush恢复到白色，vertexFlag恢复到false，即都恢复到初始状态
        if(vertexFlag)
        {
            Painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));
            vertexFlag=false;
        }

    }

    //顶点被点击，变成亮灰色
    if(edge.x()==isClicked)
    {
        Painter.setBrush(Qt::lightGray);
        Painter.drawEllipse(points[edge.y()],Radius - Radius/5,Radius - Radius/5);
        Painter.drawText(points[edge.y()].x()-Radius, points[edge.y()].y()-Radius, 2*Radius,2*Radius, Qt::AlignCenter, QString::number(edge.y()));
    }

    //如果正在寻求最短路径，顶点变成绿色
    if((ShortestPath.x()!=isNotClicked)&&(ShortestPath.y()==isNotClicked))
    {
        Painter.setBrush(Qt::green);
        Painter.drawEllipse(points[ShortestPath.x()],Radius - Radius/5,Radius - Radius/5);
        Painter.drawText(points[ShortestPath.x()].x()-Radius, points[ShortestPath.x()].y()-Radius, 2*Radius,2*Radius, Qt::AlignCenter, QString::number(ShortestPath.x()));
    }

    //绘制边和箭头
    QLineF one,two,three,neoLine,arrowBase;
    bool bothVerticiesFlag=false,oneVertexFlag=false;
    int vertex1positionInPath,vertex2PositionInPath;

    //处理寻找完最短路径后，顶点及边的颜色变化
    for(int i = 0 ; i < points.size() ; i++)
    {
        if(points[i].x()==Erased)
        {
            continue;
        }//判断points中的顶点是否被删除

   //     qDebug()<<path.size();
        //如果顶点i在path中，将oneVertexFlag设置为true，并且记录在path数组中的位置
        for(int j=0 ; j<path.size() ; j++)
        {
            if(path[j]==i)
            {
                oneVertexFlag=true;
                vertex1positionInPath=j;
                break;
            }
        }

        for(int j=0 ; j< matrix[i].size() ; j++)
        {

            if(points[matrix[i][j]].x()==Erased)
            {
                continue;
            }
            //找到存在边的两个顶点并且都在数组path中后，将bothVerticiesFlag设置为true，并且记录顶点2在path中的位置
            for(int k=0 ; k<path.size() ; k++)
            {
                if((path[k]==matrix[i][j])&&(oneVertexFlag))
                {
                    bothVerticiesFlag=true;
                    vertex2PositionInPath=k;
                    break;
                }
            }
            //找到两个顶点并且这两个顶点是相邻的，就将边变为红色
            if((bothVerticiesFlag)&&((vertex1positionInPath==vertex2PositionInPath+1)||(vertex2PositionInPath==vertex1positionInPath+1)))
            {
                Painter.setPen(Qt::red);
            }

            one.setP1(points[i]);
            one.setP2(points[matrix[i][j]]);
            one.setLength(one.length() - Radius);

            two.setP2(points[i]);
            two.setP1(points[matrix[i][j]]);
            two.setLength(two.length() - Radius);

            neoLine.setP1(two.p2());
            neoLine.setP2(one.p2());
            neoLine.setLength(neoLine.length()-4);


            arrowBase = neoLine.normalVector();//一条与neoLine垂直的直线，拥有相同的起点和长度
            arrowBase.translate(neoLine.dx(),neoLine.dy());
            arrowBase.setLength(5);
            three.setP1(arrowBase.p2());
            three.setP2(neoLine.p2());
            three.setLength(10);

            Painter.drawLine(neoLine);//画的是箭头前边的那条线
  //          Painter.drawLine(three.p1(), three.p2());
            Painter.drawLine(three.p1(), one.p2());//绘制上半个箭头
            Painter.drawLine(three.p2(), one.p2());//绘制下半个箭头
            if(bothVerticiesFlag)
            {
                //恢复到原始状态
                Painter.setPen(Qt::black);
                bothVerticiesFlag=false;
            }


        }
        oneVertexFlag=false;
    }
    //绘制边权重
    for(int i = 0 ; i < points.size() ; i++)
    {
        if(points[i].x()==Erased){continue;}
        for(int j=0 ; j< weight[i].size() ; j++)
        {
            if(points[matrix[i][j]].x()==Erased){continue;}
            Painter.save();//保存painter state
            Painter.translate(points[i]);
            Painter.rotate(-angles[i][j]);
            Painter.drawText(Radius + 20, 10, QString::number(weight[i][j]));
            Painter.restore();//恢复painter state
        }
    }
}

void Widget::resizeEvent(QResizeEvent *){
    button4->move(10,height() - button1->height());
    button1->move(button4->width()+15,height() - button1->height());
    button3->move(this->width()-button3->width(),0);
    button2->move(button1->width()+button4->width()+20,height() - button1->height());
    button5->move(button1->width()+button4->width()+button2->width()+25,height()-button1->height());
    update();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    QRect rect(Margin+Radius, Margin+Radius, width() - 2 * Margin - 2 * Radius, height() - 2 * Margin - 2 * Radius);//建立一个矩形有效区域


    if (event->button() == Qt::LeftButton) //鼠标点击左键增加一个顶点或者选择一个顶点
    {
        if((ShortestPath.x()!=isNotClicked)&&(ShortestPath.y()!=isNotClicked))
        {
            button1->setFocus(); button4->setFocus();
            return ;
        }

        if (rect.contains(event->pos())) //在有效范围内
        {
            if(points.isEmpty())//points为空，说明还没有顶点加入进来，这是第一个顶点，用户这时不可能点击顶点
            {
                points.append(event->pos()); //从尾部加入
            }
            else
            {
                bool flag = false ;
                int theClickedVertex=0;
                QRect vertexSafeArea ;

                while(!flag) //检查是否有顶点被点击
                {
                    vertexSafeArea.setX( points[theClickedVertex].x()- 2 * Radius );
                    vertexSafeArea.setY( points[theClickedVertex].y()- 2 * Radius );
                    vertexSafeArea.setWidth(4 * Radius);
                    vertexSafeArea.setHeight(4 * Radius);
                    if(vertexSafeArea.contains(event->pos())){flag=true;break;}
                    theClickedVertex++;
                    if(theClickedVertex==points.size()){break;}
                }
                if(flag==false) //如果不是顶点被点击，则将鼠标点击位置的坐标保存到points中，并向matrix、weight、angles对应位置插入整型向量
                {
                    if(button1->isChecked())
                    {
                        QMessageBox::warning(this,"温馨提示","请点击顶点!",QMessageBox::Ok);
                        return;
                    }
                    if(button4->isChecked())
                    {
                        QMessageBox::warning(this,"温馨提示","请点击顶点!",QMessageBox::Ok);
                        return;
                    }
                    if(button5->isChecked())
                    {
                        QMessageBox::warning(this,"温馨提示","请点击顶点!",QMessageBox::Ok);
                        return;
                    }
                    if(edge.x()==isClicked){return;}
                    points.append(event->pos());
                    QVector<int> vect ;
                    QVector<qreal> vect2 ;
                    matrix.insert(points.size(),vect);
                    weight.insert(points.size(),vect);
                    angles.insert(points.size(),vect2);

                }
                else // 一些顶点被点击
                {
                    //有效区域变小
                    vertexSafeArea.setX( points[theClickedVertex].x()- Radius );
                    vertexSafeArea.setY( points[theClickedVertex].y()- Radius );
                    vertexSafeArea.setWidth(2 * Radius);
                    vertexSafeArea.setHeight(2 * Radius);
                    if(vertexSafeArea.contains(event->pos()))//在有效区域内
                    {
                        //如果要删除顶点
                        if(button5->isChecked()){
                            points[theClickedVertex].setX(Erased);
                            update();
                            QMessageBox::information(this,"温馨提示",tr("顶点%1已经被删除").arg(theClickedVertex),QMessageBox::Ok);
                            return ;
                        }
                        //如果正在寻找最短路径
                        else if(button1->isChecked())
                        {
                            if(ShortestPath.x()==isNotClicked)//如果还没有顶点被点击
                            {
                                ShortestPath.setX(theClickedVertex);
                            }
                            else if(ShortestPath.x()==theClickedVertex)//重复点击一个点
                            {
                                ShortestPath.setX(isNotClicked);
                                QMessageBox::warning(this,"警告",tr("不能重复点击顶点%1哟!").arg(theClickedVertex),QMessageBox::Ok);
                            }
                            else
                            {
                                ShortestPath.setY(theClickedVertex);

                                Floyd_shortestPath();//利用Floyd方法寻找最短路径

                            }
                        }else if(button4->isChecked()){
                            if(ShortestPath.x()==isNotClicked)
                            {
                                ShortestPath.setX(theClickedVertex);
                            }
                            else if(ShortestPath.x()==theClickedVertex)//重复点击一个点
                            {
                                ShortestPath.setX(isNotClicked);
                                QMessageBox::warning(this,"警告",tr("不能重复点击顶点%1哟!").arg(theClickedVertex),QMessageBox::Ok);
                            }
                            else
                            {
                                ShortestPath.setY(theClickedVertex);
                                Dijkstra_shortestPath();
                            }
                        }else //否则就是该顶点被点击或者完成边的添加
                        {
                            if(edge.x()==isNotClicked) //顶点被点击
                            {
                                edge.setX(isClicked);
                                edge.setY(theClickedVertex);
                            }
                            else if(edge.x()==isClicked)
                            {
                                if(edge.y()==theClickedVertex) //如果重复点击了某个顶点，则设置为未点击
                                {
                                    QMessageBox::warning(this,"警告","不能重复点击哟!",QMessageBox::Ok);
                                    edge.setX(isNotClicked);
                                }
                                else
                                {
                                    int k = 0 ;
                                    bool edgeFlag =false ;
                                    //判断是否两个顶点之间是否已经存在了边
                                    while(k<matrix[edge.y()].size())
                                    {
                                        if(matrix[edge.y()][k]==theClickedVertex){edgeFlag=true ; break ;}
                                        k++;
                                    }

                                    if(!edgeFlag)//在两个顶点之间不存在边
                                    {
                                        inputManager();
                                        QLineF line(points[edge.y()],points[theClickedVertex]);
                                        matrix[edge.y()].append(theClickedVertex);
                                        angles[edge.y()].append(line.angle());//把该边的角度保存到angle中
                                        edge.setX(isNotClicked);//最后要恢复到原来的状态，也就是没有顶点被点击过
                                    }
                                    else
                                    {
                                        QMessageBox::warning(this,"警告",tr("顶点%1与顶点%2已经存在了边哟!").arg(edge.y()).arg(theClickedVertex),QMessageBox::Ok);
                                        edge.setX(isNotClicked);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    update();

}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    QRect rect(Radius + Margin/2, Radius + Margin/2, width() - 2 * Radius - Margin, height() - 2 * Radius - Margin);//建立有效区域

    if (rect.contains(event->pos())) //鼠标所点击的点在有效区域内
    {
        bool ClickedFlag = false;
        int theClickedVertex=0;
        QRect vertexSafeArea ;

        while(!ClickedFlag) //检查是否有顶点被点击
        {
            vertexSafeArea.setX( points[theClickedVertex].x()- Radius );
            vertexSafeArea.setY( points[theClickedVertex].y()- Radius );
            vertexSafeArea.setWidth(2 * Radius);
            vertexSafeArea.setHeight(2 * Radius);
            if(vertexSafeArea.contains(event->pos())){ClickedFlag=true;break;}
            theClickedVertex++;
            if(theClickedVertex==points.size()){break;}
        }

        if(ClickedFlag)//如果确实有顶点被点击，需要改变points中的坐标，以及线的角度
        {
            points[theClickedVertex]=event->pos();
            for(int i =0 ; i<angles[theClickedVertex].size() ; i++)
            {
                QLineF li(points[theClickedVertex],points[matrix[theClickedVertex][i]]);
                angles[theClickedVertex][i]=li.angle();
            }
            edge.setX(isNotClicked);
            update();
        }
    }

}

//用dijskra算法来寻求最短路径
void Widget::Dijkstra_shortestPath()
{
    int a[points.size()][points.size()];//用来记录图中的边的权重
    bool flag[points.size()];//用来记录顶点是否被访问过
    int prev[points.size()];//用来记录前驱顶点
    int dist[points.size()];//用来记录从开始顶点到结束顶点的最短路径
    int min_dis;//最小距离临时变量
    int middle=0;//用来记录中间节点
    path.clear();

    //初始化数组
    for (int i = 0; i < points.size(); i++)
        for (int j = 0; j < points.size(); j++)
            a[i][j] = Infinity;

    for(int i = 0 ; i < points.size() ; i++)
    {
        if(points[i].x()==Erased){continue;}

        for(int j=0 ; j< matrix[i].size() ; j++)
        {
            if(points[matrix[i][j]].x()==Erased){continue;}
            a[i][matrix[i][j]]=weight[i][j];
        }
    }

    //初始化顶点信息
    for(int k=0;k<points.size();k++){
        flag[k]=false;
        dist[k]=a[ShortestPath.x()][k];//将dist数组初始化为最初图中的路径长度
        prev[k]=ShortestPath.x();
    }

    dist[ShortestPath.x()]=0;
    flag[ShortestPath.x()]=true;//标记起始顶点
    //主循环
    for(int i=0;i<points.size();i++){
        min_dis=Infinity;
        //遍历图中所有顶点，寻找当前未被访问的顶点中距离起始顶点最短距离的顶点编号
        for(int j=0;j<points.size();j++){
            if(!flag[j]&&dist[j]<min_dis){
                middle=j;
                min_dis=dist[j];
            }
        }
        flag[middle]=true;
        //以middle为中间节点，再循环遍历其他所有顶点
        for(int v=0;v<points.length();v++){
            //如果当前遍历的顶点v未曾作为过中间节点，且从起始顶点到v的距离大于从起始顶点
            //到middle与从middle到v的距离和
            if(!flag[v]&&(min_dis+a[middle][v])<dist[v]){
                dist[v]=min_dis+a[middle][v];
                prev[v]=middle;
            }
        }
    }
    if(dist[ShortestPath.y()]!=Infinity){//如果可以计算出两个顶点的最短路径，就将路径顶点保存到path中
        int e=ShortestPath.y();
        path.append(e);
        while(e!=ShortestPath.x()){
           path.append(prev[e]);
           e=prev[e];
        }
        QMessageBox::about(this,"",tr("顶点%1与顶点%2之间存在最短路径！\n,这两点间最短距离为：%3").arg(ShortestPath.x()).arg(ShortestPath.y()).arg(dist[ShortestPath.y()]));
    }else{
        QMessageBox::information(this,"温馨提示",tr("顶点%1与顶点%2之间不存在最短路径！").arg(ShortestPath.x()).arg(ShortestPath.y()),QMessageBox::Ok);
        path.append(Infinity);
    }
    update();
}

//用Floyd算法来寻求最短路径
void Widget::Floyd_shortestPath()
{
    int a[points.size()][points.size()];//用来记录图中的边的权重
    int p[points.size()][points.size()];//用来记录中间节点
    //首先要清空path
    path.clear();

    //初始化数组
    for (int i = 0; i < points.size(); i++)
        for (int j = 0; j < points.size(); j++)
            a[i][j] = Infinity;


    for(int i = 0 ; i < points.size() ; i++)
    {
        if(points[i].x()==Erased){continue;}

        for(int j=0 ; j< matrix[i].size() ; j++)
        {
            if(points[matrix[i][j]].x()==Erased){continue;}
            a[i][matrix[i][j]]=weight[i][j];
        }
    }

    //初始化p数组
    for (int i = 0; i < points.size(); i++)
        for (int j = 0; j < points.size(); j++)
            p[i][j] = i;

    //Floyd算法的核心语句，即如果a[i][j]>a[i][k]+a[k][j]，则将a[i][j]更新为a[i][k]+a[k][j]的值，并且将p[i][j]更新为顶点k
    for (int k = 0; k < points.size(); k++)
    {
        for (int i = 0; i < points.size(); i++)
        {
            for (int j = 0; j < points.size(); j++)
            {
                if (a[i][k] + a[k][j] < a[i][j])
                {
                    a[i][j] = a[i][k] + a[k][j];
                    p[i][j] = p[k][j]; // 把中间节点的值保存到数组p当中
                }
            }
        }
    }
    if(a[ShortestPath.x()][ShortestPath.y()]!=Infinity)//如果可以计算出两个顶点之间的最短路径，完成路径更新
    {
        int i=ShortestPath.y();
        while(i!=ShortestPath.x())
        {
            path.append(i);
            i=p[ShortestPath.x()][i];
        }
        path.append(ShortestPath.x());
        int distance=a[ShortestPath.x()][ShortestPath.y()];
        QMessageBox::about(this,"",tr("顶点%1与顶点%2之间存在最短路径！\n,这两点间最短距离为：%3").arg(ShortestPath.x()).arg(ShortestPath.y()).arg(distance));
    }
    else
    {
        QMessageBox::information(this,"温馨提示",tr("顶点%1与顶点%2之间不存在最短路径！").arg(ShortestPath.x()).arg(ShortestPath.y()),QMessageBox::Ok);
        path.append(Infinity);
    }

    update();
}

void Widget::inputManager()
{
    bool ok=false;
//    int i=QInputDialog::getInt(this,tr("权重输入"),tr("请输入0~400的整数"),10,0,400,1,&ok);
    int i=QInputDialog::getInt(this,tr("权重输入"),tr("请输入0~400的整数"),std::rand()%100,0,400,1,&ok);
    if(ok){
       weight[edge.y()].append(i);
       update();
    }
    else{
        QMessageBox::information(this,"温馨提示","请正确输入！",QMessageBox::Ok);
    }
}

void Widget::clearFloydShortestPath()
{
    ShortestPath.setX(isNotClicked);
    ShortestPath.setY(isNotClicked);
    path.clear();
    if(button5->isChecked()&&button1->isChecked())
    {
        button5->toggle();
    }

    if(button4->isChecked()&&button1->isChecked())
    {
        button4->toggle();
    }
    update();
}

void Widget::clearDijShortestPath()
{
    ShortestPath.setX(isNotClicked);
    ShortestPath.setY(isNotClicked);
    path.clear();
    if(button5->isChecked()&&button4->isChecked())
    {
        button5->toggle();
    }
    if(button1->isChecked()&&button4->isChecked())
    {
        button1->toggle();
    }
    update();
}

void Widget::closeWindow()
{
    if(QMessageBox::question(this,tr("close"),tr("确定关闭窗口嘛？"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes){
        this->close();
    }
}

void Widget::deleteVertex()
{
    if(button1->isChecked())
    {
        button1->toggle();
    }
    if(button4->isChecked())
    {
        button4->toggle();
    }
    update();
}

void Widget::clearAll()
{
    //初始化
    matrix.clear();
    weight.clear();
    angles.clear();
    path.clear();
    points.clear();
    edge.setX(isNotClicked);
    edge.setY(isNotClicked);
    ShortestPath.setX(isNotClicked);
    ShortestPath.setY(isNotClicked);
    if(button1->isChecked()){button1->toggle();}
    if(button4->isChecked()){button4->toggle();}
    if(button5->isChecked()){button5->toggle();}
    update();
}

Widget::~Widget()
{
}

