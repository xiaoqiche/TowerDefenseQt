#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "towerposition.h"
#include "tower.h"

namespace Ui {
class MainWindow;
}

class WayPoint;
class Enemy;
class Bullet;
class AudioPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT//所有应用QT槽的类都需要声明
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

    void getHpDamage(int damage = 1);//鼠标点击触发事件
    void removedEnemy(Enemy *enemy);//敌方被攻击，消失
    void removedBullet(Bullet *bullet);//子弹击打后，消失(移除)
	void addBullet(Bullet *bullet);
	void awardGold(int gold);

	AudioPlayer* audioPlayer() const;
    QList<Enemy *> enemyList() const;//游戏中涉及到的怪物以列表形式存储

protected:
	void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);//鼠标点击触发事件

private slots://QT的private槽 当前类及其子类可以将信号与之相连接
	void updateMap();
	void gameStart();

private:
	void loadTowerPositions();
	void addWayPoints();
	bool loadWave();
	bool canBuyTower() const;
    bool canUpgradeTower() const;
	void drawWave(QPainter *painter);
	void drawHP(QPainter *painter);
	void drawPlayerGold(QPainter *painter);
	void doGameOver();
	void preLoadWavesInfo();

private:
	Ui::MainWindow *		ui;
	int						m_waves;
	int						m_playerHp;
	int						m_playrGold;
	bool					m_gameEnded;
	bool					m_gameWin;
	AudioPlayer *			m_audioPlayer;
	QList<QVariant>			m_wavesInfo;
	QList<TowerPosition>	m_towerPositionsList;
	QList<Tower *>			m_towersList;
	QList<WayPoint *>		m_wayPointsList;
	QList<Enemy *>			m_enemyList;
	QList<Bullet *>			m_bulletList;
};

#endif // MAINWINDOW_H
