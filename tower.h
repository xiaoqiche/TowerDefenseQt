#ifndef TOWER_H
#define TOWER_H

#include <QPoint>
#include <QSize>
#include <QPixmap>
#include <QObject>

class QPainter;
class Enemy;
class MainWindow;
class QTimer;

class Tower : public QObject
{
    Q_OBJECT//所有应用QT槽的类都需要声明
public:
    Tower(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/tower.png"));
	~Tower();

	void draw(QPainter *painter) const;
	void checkEnemyInRange();
	void targetKilled();
    virtual void attackEnemy();
    virtual void chooseEnemyForAttack(Enemy *enemy);
	void removeBullet();
	void damageEnemy();
	void lostSightOfEnemy();
    void setTowerLevel(int level);
    int getTowerLevel();

public slots:
    virtual void shootWeapon();

protected:
    int             m_towerType;
    int             m_towerLevel;
	bool			m_attacking;
	int				m_attackRange;	// 代表塔可以攻击到敌人的距离
	int				m_damage;		// 代表攻击敌人时造成的伤害
	int				m_fireRate;		// 代表再次攻击敌人的时间间隔
	qreal			m_rotationSprite;

	Enemy *			m_chooseEnemy;
	MainWindow *	m_game;
	QTimer *		m_fireRateTimer;

    const QPoint	m_pos;
	const QPixmap	m_sprite;

	static const QSize ms_fixedSize;
};

class Tower2:public Tower
{
    Q_OBJECT//所有应用QT槽的类都需要声明
public:
    Tower2(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/towerUpgrade1.png"));
    ~Tower2();
};

class TowerSlowingAttack:public Tower
{
    Q_OBJECT//所有应用QT槽的类都需要声明
public:
    TowerSlowingAttack(QPoint pos, MainWindow *game, const QPixmap &sprite = QPixmap(":/image/tower2.png"));
    ~TowerSlowingAttack();
    void chooseEnemyForAttack(Enemy *enemy);

public slots:
    void shootWeapon();

};

#endif // TOWER_H
