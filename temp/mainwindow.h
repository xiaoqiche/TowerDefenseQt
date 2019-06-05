#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <fstream>
#include <QMainWindow>
#include <QTimer>
#include <iostream>
#include <QList>
#include "tower.h"
#include "towerposition.h"


namespace Ui {
class MainWindow;
}
class autoplayer;
class Tower;
class Audioplayer;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);//鼠标点击触发事件
    ~MainWindow();

    void removedEnemy(autoplayer *enemy);//敌方被攻击，消失
    void removedBullet(Bullet *bullet);//子弹击打后，消失(移除)
    void addBullet(Bullet *bullet);
    void awardGold(int gold);



    AudioPlayer* audioPlayer() const;
    QList<autoplayer *> autoplayerList() const;

protected slots:
    void automan();//怪物的行进，待编写
    void add();//自动产生敌方人物，待编写


private:
    void loadTowerPositions();
    void addWayPoints();
    bool loadWave();
    bool canBuyTower() const;//判断是否可以购买新的塔
    //void drawWave(QPainter *painter);//敌人进攻的
    /*void drawHP(QPainter *painter);*///己方的血量
    void drawPlayerGold(QPainter *painter);//展现出玩家的金币（分数）
    void doGameOver();//游戏结束
    //void preLoadWavesInfo();????????????????????????????????????????


private slots:
    void updateMap();//加载地图函数
    void gameStart();//游戏开始

private:
    Ui::MainWindow *ui;
    //QTimer *time1,*time2;//设定两个计时器
    bool m_gameEnded;
    bool m_gameWin;
    int m_playrGold;
    int m_waves;

    AudioPlayer * m_audioPlayer;
    QList<autoplayer *> m_autoplayerList;
    QList<Bullet *> m_bulletList;
    QList<Tower *> m_towersList;//用于管理攻击塔的信息
    QList<TowerPosition *> m_towerPositionsList;
    QList<WayPoint *>m_wayPointsList;



 /*
    int						m_playerHp;
    QList<QVariant>			m_wavesInfo;
*/





};

#endif // MAINWINDOW_H
