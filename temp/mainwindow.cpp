#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>
#include <QXmlStreamReader>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
static const int TowerCost = 300;//设定每安置一个炮塔花费300金币

void MainWindow::mousePressEvent(QMouseEvent *event){
    QPoint pressPos = event->pos();
    auto it = m_towerPositionsList.begin();
    while (it != m_towerPositionsList.end())
    {
        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower())
        {
            m_audioPlayer->playSound(TowerPlaceSound);
            m_playrGold -= TowerCost;//购塔金币消耗
            it->setHasTower();//设定此处hasTower

            Tower *tower = new Tower(it->centerPos(), this);
            m_towersList.push_back(tower);//在塔的动态数组后增加这个新添加的塔
            update();
            break;
        }

        ++it;
    }

}
void MainWindow::paintEvent(QPaintEvent *){
     QPainter painter(this);
     painter.drawPixmap(0,0,QPixmap(""));//背景图片地址待填写


     //
     if (m_gameEnded || m_gameWin)
     {
         if(QString text=m_gameEnded){
             QPixmap pic_lost("");//加载游戏失败图片地址
             QPainter lostPainter(&pic_lost);
         }else{
             QPixmap pic_win("");//加载游戏胜利图片地址
             QPainter winPainter(&pic_win);
         }

         //QPainter painter(this);
         //painter.setPen(QPen(Qt::red));
         //painter.drawText(rect(), Qt::AlignCenter, text);
         return;
     }
     QPixmap towerPix("");//加载攻击塔的图片
     QPainter towerPainter(&towerPix);

     foreach (const TowerPosition &towerPos, m_towerPositionsList)
         towerPos.draw(&towerPainter);

     foreach (const Tower *tower, m_towersList)
         tower->draw(&towerPainter);

     /*foreach (const WayPoint *wayPoint, m_wayPointsList)
         wayPoint->draw(&towerPainter);*/

     foreach (const autoplayer *enemy, m_enemyList)
         enemy->draw(&towerPainter);
     foreach (const Bullet *bullet, m_bulletList)
         bullet->draw(&towerPainter);

     /*drawWave(&towerPainter);
     drawHP(&towerPainter);*/
     drawPlayerGold(&towerPainter);

     QPainter painter(this);
     painter.drawPixmap(0,0,towerPix);

     //

}
bool MainWindow::canBuyTower() const//判断钱币是否足够购买一个塔
{
    if (m_playrGold >= TowerCost)
        return true;
    return false;
}
void MainWindow::drawPlayerGold(QPainter *painter)
{
    painter->setPen(QPen(Qt::red));
    painter->drawText(QRect(200, 5, 200, 25), QString("GOLD : %1").arg(m_playrGold));
}


void MainWindow::updateMap()//怪兽的移动、炮塔的更新（动态数组实现）都会导致map的更新
{
    foreach (autoplayer *enemy, m_enemyList)
        enemy->move();//此处调用怪兽的运动函数
    foreach (Tower *tower, m_towersList)
        tower->checkEnemyInRange();
    update();
}
//以下是对一些private成员的修改更新
void MainWindow::awardGold(int gold)
{
    m_playrGold += gold;
    update();
}

void MainWindow::addBullet(Bullet *bullet)//
{
    Q_ASSERT(bullet);

    m_bulletList.push_back(bullet);
}
void MainWindow::removedBullet(Bullet *bullet)
{
    Q_ASSERT(bullet);

    m_bulletList.removeOne(bullet);
    delete bullet;
}

void MainWindow::loadTowerPositions()
{
    QFile file(":/config/TowersPosition.plist");//地址待修改，是一个记录炮塔位置的文件
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition.plist");
        return;
    }

    PListReader reader;
    reader.read(&file);

    QList<QVariant> array = reader.data();
    foreach (QVariant dict, array)
    {
        QMap<QString, QVariant> point = dict.toMap();
        int x = point.value("x").toInt();
        int y = point.value("y").toInt();
        m_towerPositionsList.push_back(QPoint(x, y));//在towerPositionList数组的后面增加新的Point
    }

    file.close();
}
AudioPlayer *MainWindow::audioPlayer() const//音乐播放函数
{
    return m_audioPlayer;
}
QList<autoplayer *> MainWindow::enemyList() const
{
    return m_enemyList;
}

/*void MainWindow::gameStart()
{
    loadWave();
}*/

void MainWindow::removedEnemy(Enemy *enemy)
{
    Q_ASSERT(enemy);

    m_enemyList.removeOne(enemy);
    delete enemy;

    if (m_enemyList.empty())// 即：全部敌人都已被消灭
    {
        ++m_waves;
        if (!loadWave())
        {
            m_gameWin = true;
            // 游戏胜利转到游戏胜利场景

        }
    }
}
void MainWindow::doGameOver()
{
    if (!m_gameEnded)
    {
        m_gameEnded = true;
        // 此处应该切换场景到结束场景

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
