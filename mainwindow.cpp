#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waypoint.h"
#include "enemy.h"
#include "bullet.h"
#include "audioplayer.h"
#include "plistreader.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>//时间
#include <QXmlStreamReader>
#include <QtDebug>

static const int TowerCost = 300;//设定每安置一个炮塔花费300金币
static const int UpdateTowerCost = 800;//设定升级炮塔花费600金币
static const int TowerSlowingAttackCost=500;//设定减速炮塔花费500金币

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_waves(0)
    , m_playerHp(50)//调多点以免死掉
    , m_playrGold(100000)//先开挂
	, m_gameEnded(false)
    , m_gameWin_first(false)
    , m_gameWin_final(false)
    , m_stopgame(false)
{
	ui->setupUi(this);

    preLoadWavesInfo();//从plist读取预先设定的怪物数据
	loadTowerPositions();
	addWayPoints();

	m_audioPlayer = new AudioPlayer(this);
	m_audioPlayer->startBGM();

    //一个循环结构的时间检定，30ms做一次
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
	timer->start(30);

    //手动实现按钮，暂时没有成功
    //QTimer::singleShot(300,this,SLOT(drawButton()));

    // 设置300ms后游戏启动,singleShot是在该时间间隔之后运行一次
	QTimer::singleShot(300, this, SLOT(gameStart()));
}

MainWindow::~MainWindow()
{
	delete ui;
}



void MainWindow::loadTowerPositions()
{
	QFile file(":/config/TowersPosition.plist");
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
		m_towerPositionsList.push_back(QPoint(x, y));
	}

	file.close();
}

void MainWindow::loadTowerPositions2()
{
    QFile file(":/config/TowersPosition02.plist");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "TowerDefense", "Cannot Open TowersPosition02.plist");
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
        m_towerPositionsList.push_back(QPoint(x, y));
    }

    file.close();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if (m_gameEnded || m_gameWin_final)
	{
		QString text = m_gameEnded ? "YOU LOST!!!" : "YOU WIN!!!";
		QPainter painter(this);
		painter.setPen(QPen(Qt::red));
		painter.drawText(rect(), Qt::AlignCenter, text);
		return;
	}



    if (m_gameWin_first&&(!m_gameWin_final)){
        //换图之后


        preLoadWavesInfo2();
        loadTowerPositions2();
        addWayPoints2();
        QPixmap cachePix2(":/image/Bg2.png");
        QPainter cachePainter2(&cachePix2);

        foreach (const TowerPosition &towerPos, m_towerPositionsList)
            towerPos.draw(&cachePainter2);

        foreach (const Tower *tower, m_towersList)
            tower->draw(&cachePainter2);

        foreach (const WayPoint *wayPoint, m_wayPointsList)
            wayPoint->draw(&cachePainter2);

        foreach (const Enemy *enemy, m_enemyList)
            enemy->draw(&cachePainter2);

        foreach (const Bullet *bullet, m_bulletList)
            bullet->draw(&cachePainter2);

        drawWave(&cachePainter2);
        drawHP(&cachePainter2);
        drawPlayerGold(&cachePainter2);

        QPainter painter(this);
        painter.drawPixmap(0, 0, cachePix2);
    }
    if (!m_gameEnded && !m_gameWin_first){
        QPixmap cachePix(":/image/Bg.png");
        QPainter cachePainter(&cachePix);
        foreach (const TowerPosition &towerPos, m_towerPositionsList)
            towerPos.draw(&cachePainter);

        foreach (const Tower *tower, m_towersList)
            tower->draw(&cachePainter);

        foreach (const WayPoint *wayPoint, m_wayPointsList)
            wayPoint->draw(&cachePainter);

        foreach (const Enemy *enemy, m_enemyList)
            enemy->draw(&cachePainter);

        foreach (const Bullet *bullet, m_bulletList)
            bullet->draw(&cachePainter);

        drawWave(&cachePainter);
        drawHP(&cachePainter);
        drawPlayerGold(&cachePainter);

        QPainter painter(this);
        painter.drawPixmap(0, 0, cachePix);
    }

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	QPoint pressPos = event->pos();
	auto it = m_towerPositionsList.begin();
    while (it != m_towerPositionsList.end())
	{
        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower()  && 1==TowerMode)
		{
			m_audioPlayer->playSound(TowerPlaceSound);
			m_playrGold -= TowerCost;
            it->setHasTower();

            Tower *tower = new Tower(it->centerPos(), this);
            tower->setTowerLevel(1);
			m_towersList.push_back(tower);
			update();
			break;
		}
        if (canUpgradeTower() && it->containPoint(pressPos) && 1==TowerMode)
        {


            foreach(Tower *attacker, m_towersList)
            {
                if(it->centerPos()==attacker->getPosition() && 1==attacker->getTowerLevel())
                {
                    attacker->setTowerLevel(2);
                    m_audioPlayer->playSound(TowerPlaceSound);
                    m_playrGold -= UpdateTowerCost;
                }

            }
            update();
            break;
        }
        if (canBuyTower() && it->containPoint(pressPos) && !it->hasTower() && 2==TowerMode)
        {//为第二种形式时建立第二种塔
            m_audioPlayer->playSound(TowerPlaceSound);
            m_playrGold -= TowerSlowingAttackCost;
            it->setHasTower();

            Tower *tower = new TowerSlowingAttack(it->centerPos(), this);
            tower->setTowerLevel(1);
            m_towersList.push_back(tower);
            update();
            break;
        }
        if (canUpgradeTower() && it->containPoint(pressPos) && 2==TowerMode)
        {


            foreach(Tower *attacker, m_towersList)
            {
                if(it->centerPos()==attacker->getPosition())
                {
                    attacker->setTowerLevel(2);
                    m_audioPlayer->playSound(TowerPlaceSound);
                    m_playrGold -= UpdateTowerCost;
                }

            }
            update();
            break;
        }
		++it;
	}
}

bool MainWindow::canBuyTower() const
{
    if (m_playrGold >= TowerCost)
		return true;
	return false;
}

bool MainWindow::canBuySlowingAttackTower()const
{
    if(m_playrGold >= TowerSlowingAttackCost)
        return true;
    return false;
}

bool MainWindow::canUpgradeTower() const
{
    if(m_playrGold >= UpdateTowerCost)
        return true;
    return false;
}

void MainWindow::drawWave(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(400, 5, 100, 25), QString("WAVE : %1").arg(m_waves + 1));
}

void MainWindow::drawHP(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(30, 5, 100, 25), QString("HP : %1").arg(m_playerHp));
}

void MainWindow::drawPlayerGold(QPainter *painter)
{
	painter->setPen(QPen(Qt::red));
	painter->drawText(QRect(200, 5, 200, 25), QString("GOLD : %1").arg(m_playrGold));
}


void MainWindow::doGameOver1(){
    if(!m_gameWin_first){
        m_gameWin_first = true;

    }
}

void MainWindow::doGameOver2()
{
    if (!m_gameEnded)
    {
        m_gameEnded = true;
        // 此处应该切换场景到结束场景
        // 暂时以打印替代,见paintEvent处理
    }
}
void MainWindow::awardGold(int gold)
{
	m_playrGold += gold;
	update();
}

AudioPlayer *MainWindow::audioPlayer() const
{
	return m_audioPlayer;
}

void MainWindow::addWayPoints()
{
	WayPoint *wayPoint1 = new WayPoint(QPoint(420, 285));
	m_wayPointsList.push_back(wayPoint1);

	WayPoint *wayPoint2 = new WayPoint(QPoint(35, 285));
	m_wayPointsList.push_back(wayPoint2);
	wayPoint2->setNextWayPoint(wayPoint1);

	WayPoint *wayPoint3 = new WayPoint(QPoint(35, 195));
	m_wayPointsList.push_back(wayPoint3);
	wayPoint3->setNextWayPoint(wayPoint2);

	WayPoint *wayPoint4 = new WayPoint(QPoint(445, 195));
	m_wayPointsList.push_back(wayPoint4);
	wayPoint4->setNextWayPoint(wayPoint3);

	WayPoint *wayPoint5 = new WayPoint(QPoint(445, 100));
	m_wayPointsList.push_back(wayPoint5);
	wayPoint5->setNextWayPoint(wayPoint4);

	WayPoint *wayPoint6 = new WayPoint(QPoint(35, 100));
	m_wayPointsList.push_back(wayPoint6);
	wayPoint6->setNextWayPoint(wayPoint5);
}

void MainWindow::addWayPoints2(){
    //m_towerPositionsList.clear();
    WayPoint *wayPoint1 = new WayPoint(QPoint(0, 100));
    m_wayPointsList.push_back(wayPoint1);

    WayPoint *wayPoint2 = new WayPoint(QPoint(70,100));
    m_wayPointsList.push_back(wayPoint2);
    wayPoint2->setNextWayPoint(wayPoint1);

    WayPoint *wayPoint3 = new WayPoint(QPoint(70,290));
    m_wayPointsList.push_back(wayPoint3);
    wayPoint3->setNextWayPoint(wayPoint2);

    WayPoint *wayPoint4 = new WayPoint(QPoint(180, 290));
    m_wayPointsList.push_back(wayPoint4);
    wayPoint4->setNextWayPoint(wayPoint3);

    WayPoint *wayPoint5 = new WayPoint(QPoint(180, 40));
    m_wayPointsList.push_back(wayPoint5);
    wayPoint5->setNextWayPoint(wayPoint4);

    WayPoint *wayPoint6 = new WayPoint(QPoint(320,40));
    m_wayPointsList.push_back(wayPoint6);
    wayPoint6->setNextWayPoint(wayPoint5);

    WayPoint *wayPoint7 = new WayPoint(QPoint(320,280));
    m_wayPointsList.push_back(wayPoint7);
    wayPoint6->setNextWayPoint(wayPoint6);

    WayPoint *wayPoint8 = new WayPoint(QPoint(380,280));
    m_wayPointsList.push_back(wayPoint8);
    wayPoint6->setNextWayPoint(wayPoint7);
}


void MainWindow::getHpDamage(int damage/* = 1*/)
{
	m_audioPlayer->playSound(LifeLoseSound);
	m_playerHp -= damage;
	if (m_playerHp <= 0)
        doGameOver2();
}

void MainWindow::removedEnemy(Enemy *enemy)
{
	Q_ASSERT(enemy);

	m_enemyList.removeOne(enemy);
	delete enemy;


    if (m_enemyList.empty())//没怪了就加一波
	{
		++m_waves;
        if (!loadWave())//所有波用完就结束,这里是标志一个关结束的最初位置
		{
            if(!m_gameWin_first){
                m_gameWin_first = true;
                // 游戏第一关胜利，切换到第二关
                m_towersList.clear();
                m_towerPositionsList.clear();
                m_wayPointsList.clear();
            }
            else{
                m_gameWin_final = true;
            }
            // 这里暂时以打印处理
		}
	}

}

void MainWindow::removedBullet(Bullet *bullet)
{
	Q_ASSERT(bullet);

	m_bulletList.removeOne(bullet);
	delete bullet;
}

void MainWindow::addBullet(Bullet *bullet)
{
	Q_ASSERT(bullet);

	m_bulletList.push_back(bullet);
}

void MainWindow::updateMap()
{
    if(!m_stopgame){//如果暂停就不更新了
        foreach (Enemy *enemy, m_enemyList)
            enemy->move();
        foreach (Tower *tower, m_towersList)
            tower->checkEnemyInRange();
        update();
    }
}



void MainWindow::preLoadWavesInfo()
{
	QFile file(":/config/Waves.plist");
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
        QMessageBox::warning(this, "TowerDefense", "Cannot Open wave.plist");
		return;
	}

	PListReader reader;
	reader.read(&file);

	// 获取波数信息
	m_wavesInfo = reader.data();

	file.close();
}

void MainWindow::preLoadWavesInfo2()
{
    QFile file(":/config/Waves02.plist");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "TowerDefense", "Cannot Open Waves02.plist");
        return;
    }

    PListReader reader;
    reader.read(&file);

    // 获取波数信息
    m_wavesInfo = reader.data();

    file.close();
}

bool MainWindow::loadWave()//这个是每波加载怪物的函数
{
	if (m_waves >= m_wavesInfo.size())
		return false;

	WayPoint *startWayPoint = m_wayPointsList.back();
	QList<QVariant> curWavesInfo = m_wavesInfo[m_waves].toList();

	for (int i = 0; i < curWavesInfo.size(); ++i)
	{
		QMap<QString, QVariant> dict = curWavesInfo[i].toMap();
		int spawnTime = dict.value("spawnTime").toInt();

        if(i%3 == 1){
            Enemy *enemy = new Enemy(startWayPoint, this);
            m_enemyList.push_back(enemy);
            QTimer::singleShot(spawnTime, enemy, SLOT(doActivate()));
        }
        else if(i%3 == 2){
            Enemy *enemy2 = new Enemy2(startWayPoint, this);//另一类怪物也加点进去
            m_enemyList.push_back(enemy2);
            QTimer::singleShot(spawnTime, enemy2, SLOT(doActivate()));
        }else{
            Enemy *enemy3 = new Enemy3(startWayPoint, this);//另一类怪物也加点进去
            m_enemyList.push_back(enemy3);
            QTimer::singleShot(spawnTime, enemy3, SLOT(doActivate()));
        }

    }

	return true;
}

QList<Enemy *> MainWindow::enemyList() const//调取怪物表
{
	return m_enemyList;
}

void MainWindow::gameStart()
{
	loadWave();
}

void MainWindow::on_stop_clicked()
{
    m_stopgame = true;
}

void MainWindow::on_continue_2_clicked()
{
    m_stopgame = false;
}

void MainWindow::on_ChangeType_clicked()
{
    TowerMode=1;
}

void MainWindow::on_ChangeType_2_clicked()
{
    TowerMode=2;
}

