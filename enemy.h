#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QPixmap>

class WayPoint;
class QPainter;
class MainWindow;
class Tower;
// 这个类是敌方类（怪兽类）的定义


class Enemy : public QObject
{
    Q_OBJECT//所有应用QT槽的类都需要声明
public:
    Enemy(WayPoint *startWayPoint, MainWindow *game,
          const QPixmap &sprite = QPixmap(":/image/enemy.png"),
          int maxHp = 40,qreal walkingSpeed = 1.0);//血量和速度应该是可以更改的部分
	~Enemy();
    //设置为虚函数
    virtual void draw(QPainter *painter) const;
    virtual void move();
    virtual void getDamage(int damage);
    virtual void getRemoved();
    virtual void getAttacked(Tower *attacker);
    virtual void gotLostSight(Tower *attacker);
	QPoint pos() const;

public slots:
	void doActivate();

protected://为继承做准备
	bool			m_active;
	int				m_maxHp;
	int				m_currentHp;
	qreal			m_walkingSpeed;
	qreal			m_rotationSprite;

	QPoint			m_pos;
	WayPoint *		m_destinationWayPoint;
	MainWindow *	m_game;
	QList<Tower *>	m_attackedTowersList;

	const QPixmap	m_sprite;
	static const QSize ms_fixedSize;
};

//这个是一种特殊的怪，初次被攻击后加速,血量为20，是原来怪的一半
class Enemy2 : public Enemy
{
    Q_OBJECT//所有应用QT槽的类都需要声明
public:
    Enemy2(WayPoint *startWayPoint, MainWindow *game,
          const QPixmap &sprite = QPixmap(":/image/enemy_brightYellow.png"),
          int maxHp = 20,qreal walkingSpeed = 1.0);//血量和速度应该是可以更改的部分
    ~Enemy2();

    void getDamage(int damage);//受伤后有加速效果

protected:
    bool m_speedUp;//判断是否是第一次被攻击
};


#endif // ENEMY_H
